//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  kai zuo kai.zuo@united-imaging.com
//
/// \file    tps_rl_dosegridboundaryrenderstrategy.cpp
///
/// \brief   DoseGridBoundaryRenderStrategy
/// 
/// \version 1.0
/// 
/// \date    2014/04/12
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundaryrenderstrategy.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
//#include "McsfMedViewer3DArithmetic/arithmetic.h"

#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"

#include "RtTpsAlgorithmProxy/tps_pl_collimator.h"
#include "RtTpsFramework/tps_fw_common_enums.h"


using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

DoseGridBoundaryRenderStrategy::DoseGridBoundaryRenderStrategy() :
mDoseGridBoundaryGO(nullptr), mVolumeGraphicObject(nullptr),
      mMprGraphicObject(nullptr){
}

DoseGridBoundaryRenderStrategy::~DoseGridBoundaryRenderStrategy() {

}

void DoseGridBoundaryRenderStrategy::Initialize() {

}

//这个绘制方法存在问题，对于外部导入的RtDose可能会显示错误。
//如果DoseGrid的各轴方向与Volume不平行，那么就会出错。
//需要将DoseGrid2Pat矩阵传进来根据具体的grid信息进行绘制。---GML
void DoseGridBoundaryRenderStrategy::RenderForTest() {
    using namespace Mcsf::MedViewer3D;
    CalcScreenToWorld_i();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glViewport(0, 0, mWidth, mHeight);
//    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
//    glEnable(GL_STENCIL_TEST);
//    glStencilMask(0x01);
//    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    Matrix4x4 matWorld2Patient = mVolumeGraphicObject->GetWorld2Patient();
    Matrix4x4 matModel2Patient = mVolumeGraphicObject->GetModel2PatientMat();
    size_t dim[3];
    mVolumeGraphicObject->GetDimension(dim);

    Matrix4x4 matPatient2Volume(dim[0], 0, 0, 0, /*|*/ 0, dim[1], 0, 0, 
        0, 0, dim[2], 0, /*|*/ -0.5, -0.5, -0.5, 1);

    matPatient2Volume.Append(matModel2Patient.Inverse());
    Matrix4x4 matPatient2World = matWorld2Patient.Inverse();
    Matrix4x4 matWorld2Volume = matPatient2Volume;
    matWorld2Volume.Append(matWorld2Patient);
    Point3D aVertices[4];
    Point3D vertex[2];
    Point3D original;
    original.x = 0.0;
    original.y = 0.0;
    original.z = 0.0;
    Point3D spacing;
    mMprGraphicObject->GetVertices(aVertices);
    vertex[0] = mDoseGridBoundaryGO->GetLeftBottomPoint();
    vertex[1] = mDoseGridBoundaryGO->GetRightTopPoint();
    spacing = mDoseGridBoundaryGO->GetSpacing();
    if (vertex[0] == vertex[1]) {
        return;
    }
    vertex[0] = matPatient2Volume.Transform(vertex[0]);
    vertex[1] = matPatient2Volume.Transform(vertex[1]);
    int sign[3] = 
    {
        TPS_FLOAT_SIGN(vertex[1].x - vertex[0].x),
        TPS_FLOAT_SIGN(vertex[1].y - vertex[0].y),
        TPS_FLOAT_SIGN(vertex[1].z - vertex[0].z)
    };

    original = matPatient2Volume.Transform(original);
    spacing = matPatient2Volume.Transform(spacing);
    spacing.x = abs(spacing.x - original.x);
    spacing.y = abs(spacing.y - original.y);
    spacing.z = abs(spacing.z - original.z);

    Matrix4x4 matWorld2Screen = mScreenToWorldMat.Inverse();
    for (int i = 0; i < 4; ++i){
        aVertices[i] = matWorld2Volume.Transform(aVertices[i]);
        aVertices[i].x = sign[0] * (aVertices[i].x - vertex[0].x);
        aVertices[i].y = sign[1] * (aVertices[i].y - vertex[0].y);
        aVertices[i].z = sign[2] * (aVertices[i].z - vertex[0].z);
    }
    double dim1[3];
    dim1[0] = abs(vertex[1].x - vertex[0].x);
    dim1[1] = abs(vertex[1].y - vertex[0].y);
    dim1[2] = abs(vertex[1].z - vertex[0].z);

    int NetCount[3];
    NetCount[0] = dim1[0] / spacing.x + 1e-6;
    NetCount[1] = dim1[1] / spacing.y + 1e-6;
    NetCount[2] = dim1[2] / spacing.z + 1e-6;

    std::vector<Mcsf::MedViewer3D::Point3D> intersections;
    Matrix4x4 matPatient2Screen = matWorld2Screen;
    matPatient2Screen.Append(matPatient2World);
    Matrix4x4 matVolume2Screen = matPatient2Screen;
    matVolume2Screen.Append(matPatient2Volume.Inverse());
    if (!GetIntersectionsOfDoseGridWithMpr(dim1, aVertices, intersections)) {
        return;
    }
    for (int i = 0; i < intersections.size(); ++i) {
        intersections[i].x /= sign[0];
        intersections[i].y /= sign[1];
        intersections[i].z /= sign[2];
        intersections[i].x += vertex[0].x;
        intersections[i].y += vertex[0].y;
        intersections[i].z += vertex[0].z;
        intersections[i] = matVolume2Screen.Transform(intersections[i]);
    }
    if (intersections.size() >= 4) {
        MakeVertexOrdered(intersections, intersections.size());
    }

    //Following code : calculate the net
    if (mDoseGridBoundaryGO->GeType() == "DOSEGRID") {
        std::vector<Mcsf::MedViewer3D::Point3D> netIntersections;
        std::vector<Mcsf::MedViewer3D::Point3D> tempIntersections;
        for (int i = 0; i < intersections.size(); ++i) {
            tempIntersections.push_back((matVolume2Screen.Inverse()).Transform(intersections[i]));
        }
        double D = 0;
        for(int i = 1; i <= NetCount[0]; ++i) {
            D = -(sign[0] * i * spacing.x + vertex[0].x);
            GetIntersectionOfBoundaryWithPlane(tempIntersections, netIntersections, 1, 0, 0, D);
        }
        for(int i = 1; i <= NetCount[1]; ++i) {
            D = -(sign[1] * i * spacing.y + vertex[0].y);
            GetIntersectionOfBoundaryWithPlane(tempIntersections, netIntersections, 0, 1, 0, D);
        }
        for(int i = 1; i <= NetCount[2]; ++i) {
            D = -(sign[2] * i * spacing.z + vertex[0].z);
            GetIntersectionOfBoundaryWithPlane(tempIntersections, netIntersections, 0, 0, 1, D);
        }
        for (int i = 0; i < netIntersections.size(); ++i) {
            netIntersections[i] = matVolume2Screen.Transform(netIntersections[i]);
        }

        glMatrixMode(GL_PROJECTION);
        glOrtho(-1.0, 1.0, -1.0, 1.0, -0.001, +0.001);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
        glLineWidth(0.5f);
        glColor4f(1.0f, 0.0f, 0.0f, 0.8f);
        glBegin(GL_LINES);
        for (int i = 0; i < netIntersections.size(); ++i) {
            glVertex3f(2 * netIntersections[i].x / mWidth - 1, 2 * netIntersections[i].y / mHeight - 1, netIntersections[i].z);
        }
        glEnd();
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
        glLineWidth(1.0f);
    }

    const float* color;
    mDoseGridBoundaryGO->GetColor(color);
    glColor4f(color[0], color[1], color[2], 1.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < intersections.size(); ++i) {
        glVertex3f(2 * intersections[i].x / mWidth - 1, 2 * intersections[i].y / mHeight - 1, intersections[i].z);
        mHitTestData.push_back(Point2D(intersections[i].x, intersections[i].y));
    }
    //glVertex3f(-0.8f,-0.8f,0.0f);
    //glVertex3f(-0.8f, 0.8f,0.0f);
    //glVertex3f( 0.8f, 0.8f,0.0f);
    //glVertex3f( 0.5f,-0.5f,0.0f);
    glEnd();

    glFlush();
//    glDisable(GL_STENCIL_TEST);
//    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void DoseGridBoundaryRenderStrategy::Render_i() {
    mHitTestData.clear();
    if (mDoseGridBoundaryGO == nullptr || mVolumeGraphicObject == nullptr ||
        mMprGraphicObject == nullptr) {
        return;
    }

    if (!mDoseGridBoundaryGO->GetVisible()){
        TPS_LOG_DEV_INFO << "dose grid boundary is not visible !";
        return;
    }

    //TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;
    RenderForTest();
    //TEST_PERFORMANCE_END("TotalRender");
}

int DoseGridBoundaryRenderStrategy::CacheChanges() {
    if (mDoseGridBoundaryGO == nullptr || mVolumeGraphicObject == nullptr ||
       mMprGraphicObject == nullptr) {
      return -1;
    }
    if (mDoseGridBoundaryGO->IsDirty() || mVolumeGraphicObject->IsDirty() ||
       mMprGraphicObject->IsDirty()) {
      mDirty = true;
    }
    return 0;
}

void DoseGridBoundaryRenderStrategy::Resize(int w, int h) {
    mWidth = w;
    mHeight = h;
}

void DoseGridBoundaryRenderStrategy::Finalize() {

}

void DoseGridBoundaryRenderStrategy::SetDoseGridBoundaryGO(std::shared_ptr<TpsDoseGridBoundaryGraphicObject> go) {
    if (mDoseGridBoundaryGO != go){
        mDoseGridBoundaryGO = go;
        mDirty = true;
    }
}

void DoseGridBoundaryRenderStrategy::SetMprGraphicObject(std::shared_ptr<MPRGraphicObject> mprGo) {
    if (mMprGraphicObject != mprGo){
        mMprGraphicObject = mprGo;
        mDirty = true;
    }
}

void DoseGridBoundaryRenderStrategy::SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume) {
    if (mVolumeGraphicObject != volume){
        mVolumeGraphicObject = volume;
        mDirty = true;
    }
}

bool DoseGridBoundaryRenderStrategy::GetIntersectionsOfDoseGridWithMpr(
                double* dim, const Mcsf::MedViewer3D::Point3D* pVertices, 
                std::vector<Mcsf::MedViewer3D::Point3D>& intersections) {
    double x0 = pVertices[0].x;
    double y0 = pVertices[0].y;
    double z0 = pVertices[0].z;
    double x1 = pVertices[1].x;
    double y1 = pVertices[1].y;
    double z1 = pVertices[1].z;
    double x2 = pVertices[2].x;
    double y2 = pVertices[2].y;
    double z2 = pVertices[2].z;
    double x3 = pVertices[3].x;
    double y3 = pVertices[3].y;
    double z3 = pVertices[3].z;
    double xPara = (y2 - y1)*(z3 - z1) - (z2 - z1)*(y3 - y1);
    double yPara = (x2 - x1)*(z3 - z1) - (x3 - x1)*(z2 - z1);
    double zPara = (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1);
    double planeFunc = 0.0;
    planeFunc = (x0 - x1)*xPara - (y0 - y1)*yPara + (z0 - z1)*zPara;
    //planeFunc = (x1 - x0) * (y2 - y0) * (z3 - z0) + (y1 - y0) * (z2 - z0) * (x3 - x0) +
    //    (z1 - z0) * (x2 - x0) * (y3 - y0) - (z1 - z0) * (y2 - y0) * (x3 - x0) - 
    //    (x1 - x0) * (z2 - z0) * (y3 - y0) - (y1 - y0) * (x2 - x0) * (z3 - z0);

    if ((planeFunc > 1e-6) || (planeFunc < -(1e-6))) return false;

    Mcsf::MedViewer3D::Point3D intersection;
    double x = 0;
    double y = 0;
    double z = 0;
    bool flag = false;

    if ((xPara > 1e-6) || (xPara < -1e-6)) {

        flag = true;

        x = ((0 - y1) * yPara - (dim[2] - z1) * zPara) / xPara + x1;

        if (x >= 0 && x <= dim[0]) {
            intersection.x = x;
            intersection.y = 0;
            intersection.z = dim[2];
            intersections.push_back(intersection);
        }

        x = ((dim[1] - y1) * yPara - (dim[2] - z1) * zPara) / xPara + x1;

        if (x >= 0 && x <= dim[0]) {
            intersection.x = x;
            intersection.y = dim[1];
            intersection.z = dim[2];
            intersections.push_back(intersection);
        }

        x = ((0 - y1) * yPara - (0 - z1) * zPara) / xPara + x1;

        if (x >= 0 && x <= dim[0]) {
            intersection.x = x;
            intersection.y = 0;
            intersection.z = 0;
            intersections.push_back(intersection);
        }

        x = ((dim[1] - y1) * yPara - (0 - z1) * zPara) / xPara + x1;

        if (x >= 0 && x <= dim[0]) {
            intersection.x = x;
            intersection.y = dim[1];
            intersection.z = 0;
            intersections.push_back(intersection);
        }
    }
    if ((yPara > 1e-6) || (yPara < -1e-6)) {

        y = ((0 - x1) * xPara + (0 - z1) * zPara) / yPara + y1;

        if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
            intersection.x = 0;
            intersection.y = y;
            intersection.z = 0;
            intersections.push_back(intersection);
        }

        y = ((dim[0] - x1) * xPara + (dim[2] - z1) * zPara) / yPara + y1;

        if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
            intersection.x = dim[0];
            intersection.y = y;
            intersection.z = dim[2];
            intersections.push_back(intersection);
        }

        y = ((0 - x1) * xPara + (dim[2] - z1) * zPara) / yPara + y1;

        if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
            intersection.x = 0;
            intersection.y = y;
            intersection.z = dim[2];
            intersections.push_back(intersection);
        }

        y = ((dim[0] - x1) * xPara + (0 - z1) * zPara) / yPara + y1;

        if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
            intersection.x = dim[0];
            intersection.y = y;
            intersection.z = 0;
            intersections.push_back(intersection);
        }

        flag = true;
    }

    if ((zPara > 1e-6) || (zPara < -1e-6)){

        z = ((0 - y1) * yPara - (0 - x1) * xPara) / zPara + z1;

        if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
            intersection.x = 0;
            intersection.y = 0;
            intersection.z = z;
            intersections.push_back(intersection);
        }

        z = ((0 - y1) * yPara - (dim[0] - x1) * xPara) / zPara + z1;

        if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
            intersection.x = dim[0];
            intersection.y = 0;
            intersection.z = z;
            intersections.push_back(intersection);
        }

        z = ((dim[1] - y1) * yPara - (dim[0] - x1) * xPara) / zPara + z1;

        if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
            intersection.x = dim[0];
            intersection.y = dim[1];
            intersection.z = z;
            intersections.push_back(intersection);
        }

        z = ((dim[1] - y1) * yPara - (0 - x1) * xPara) / zPara + z1;

        if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
            intersection.x = 0;
            intersection.y = dim[1];
            intersection.z = z;
            intersections.push_back(intersection);
        }
    }
    if (intersections.size() < 3) {
        return false;
    }
    return true;
}

void DoseGridBoundaryRenderStrategy::CalcScreenToWorld_i(){
    Mcsf::MedViewer3D::Point3D vertices[4];
    mMprGraphicObject->GetVertices(vertices);

    Viewer3D::Vector3D vecY = vertices[3] - vertices[0];
    vecY.Normalize();
    Viewer3D::Vector3D vecZ; 
    mMprGraphicObject->GetNormal(&vecZ); vecZ *= -1;
    Viewer3D::Vector3D vecX = vecZ.CrossProduct(vecY);
    vecY = vecX.CrossProduct(vecZ);

    double scale, ratio;
    double planeWidth = (vertices[1] - vertices[0]).Magnitude();
    double planeHeight = (vertices[3] - vertices[0]).Magnitude();
    Viewer3D::Point3D ptOrigin = vertices[0];
    double ratioPlane = planeWidth / planeHeight;
    double ratioView = (double)mWidth / mHeight;
    if (ratioPlane <= ratioView){
        scale = mHeight / planeHeight;
        ratio = ratioView / ratioPlane;
        ptOrigin += (vertices[0] - vertices[1]) * (ratio - 1) * 0.5;
    }
    else{
        scale = mWidth / planeWidth;
        ratio = ratioPlane / ratioView;
        ptOrigin += (vertices[0] - vertices[3]) * (ratio - 1) * 0.5;
    }

    Viewer3D::Vector3D temp(ptOrigin.x, ptOrigin.y, ptOrigin.z);
    Viewer3D::Matrix4x4 matWorldToScreen(vecX.x, vecY.x, vecZ.x, 0.0, 
        vecX.y, vecY.y, vecZ.y, 0.0, 
        vecX.z, vecY.z, vecZ.z, 0.0, 
        -1 * temp.DotProduct(vecX), 
        -1 * temp.DotProduct(vecY), 
        -1 * temp.DotProduct(vecZ), 1.0);
    matWorldToScreen.Prepend(MakeScale(Viewer3D::Vector3D(scale, scale, scale)));
    mScreenToWorldMat = matWorldToScreen.Inverse();
}
bool DoseGridBoundaryRenderStrategy::MakeVertexOrdered(std::vector<Mcsf::MedViewer3D::Point3D>& pVertices, int vertexCount) {
    //apply to convex polygon,do not use this function when the polygon is concave
    Mcsf::MedViewer3D::Point3D orderedVertices[6];
    double maxY = pVertices[0].y;
    double maxCorresX = pVertices[0].x;
    double minY = pVertices[0].y;
    double minCorresX = pVertices[0].x;
    for (int i = 1; i < vertexCount; ++i) {
        if ((pVertices[i].y > maxY) || ((pVertices[i].y == maxY) && (pVertices[i].x > maxCorresX))) {
            maxY = pVertices[i].y;
            maxCorresX = pVertices[i].x;
        }
        if ((pVertices[i].y < minY) || ((pVertices[i].y == minY) && (pVertices[i].x < minCorresX))) {
            minY = pVertices[i].y;
            minCorresX = pVertices[i].x;
        }
    }
    orderedVertices[0].x = maxCorresX;
    orderedVertices[0].y = maxY;
    Mcsf::MedViewer3D::Point3D temp;

    for (int i = 0; i < vertexCount - 1; ++i) {
        for (int j = 0; j < vertexCount - i - 1; ++j) {
            if ((pVertices[j].y < pVertices[j + 1].y) || ((pVertices[j].y == pVertices[j + 1].y) && (pVertices[j].x < pVertices[j + 1].y)))
            {
                temp = pVertices[j + 1];
                pVertices[j + 1] = pVertices[j];
                pVertices[j] = temp;
            }
        }
    }

    int t = 1;
    for (int i = 0; i < vertexCount; ++i) {
        if (pVertices[i].x * (maxY - minY) - minCorresX * maxY + minY * maxCorresX + pVertices[i].y * (minCorresX - maxCorresX) < -1e-6) {
            orderedVertices[t++] = pVertices[i];
        }
    }
    orderedVertices[t].x = minCorresX;
    orderedVertices[t].y = minY;

    int r = 1;
    for (int i = 0; i < vertexCount; ++i) {
        if (pVertices[i].x * (maxY - minY) - minCorresX * maxY + minY * maxCorresX + pVertices[i].y * (minCorresX - maxCorresX) > 1e-6) {
            orderedVertices[vertexCount - r++] = pVertices[i];
        }
    }
    for (int i = 0; i < vertexCount; ++i) {
        pVertices[i] = orderedVertices[i];
    }

    return true;
}

bool DoseGridBoundaryRenderStrategy::GetIntersectionOfBoundaryWithPlane(std::vector<Mcsf::MedViewer3D::Point3D>& pVertices, 
               std::vector<Mcsf::MedViewer3D::Point3D>& intersections, double A, double B, double C, double D) {
    int next = 0;
    Mcsf::MedViewer3D::Point3D currentPoint;
    Mcsf::MedViewer3D::Point3D nextPoint;
    Mcsf::MedViewer3D::Point3D intersection;
    Mcsf::MedViewer3D::Point3D intersection1;
    double x0, y0, z0;
    double v0, v1, v2;
    double p0, p1, p2;
    double vt = 0.0;
    double t = 0.0;
    if (abs(A) > 1e-6) {
        p0 = -D / A;
        p1 = 0.0;
        p2 = 0.0;
    }
    else if (abs(B) > 1e-6) {
        p0 = 0.0;
        p1 = -D / B;
        p2 = 0.0;
    }
    else {
        p0 = 0.0;
        p1 = 0.0;
        p2 = -D / C;
    }
    int count = 0;
    for (int current = 0; current < pVertices.size(); ++current) {
        next = current + 1;
        if (next >= pVertices.size()) {
            next = 0;
        }
        currentPoint = pVertices[current];
        nextPoint = pVertices[next];
        x0 = currentPoint.x;
        y0 = currentPoint.y;
        z0 = currentPoint.z;
        v0 = nextPoint.x - x0;
        v1 = nextPoint.y - y0;
        v2 = nextPoint.z - z0;
        vt = v0 * A + v1 * B + v2 * C;
        if (abs(vt) < 1e-6) {
            continue;
        }
        t = ((p0 - x0) * A + (p1 - y0) * B + (p2 - z0) * C) / vt;
        if (t < 0 || t >= 1) continue;
        if (count == 2) break;
        count++;
        if (count == 1) {
            intersection.x = x0 + v0 * t;
            intersection.y = y0 + v1 * t;
            intersection.z = z0 + v2 * t;
        }
        else if (count == 2) {
            intersection1.x = x0 + v0 * t;
            intersection1.y = y0 + v1 * t;
            intersection1.z = z0 + v2 * t;
        }
    }
    if (count == 2) {
        intersections.push_back(intersection);
        intersections.push_back(intersection1);
        return true;
    }
    return false;
}

void DoseGridBoundaryRenderStrategy::HitTest(double x, double y, HitTestResult& hitRes)
{
    if (mHitTestData.size() == 0)
    {
        hitRes._hitNum = HIT_NUM_NONE;
        return;
    }
    Point2D handlePt(x * mWidth, y * mHeight);
    for (int i = 0; i < mHitTestData.size(); i++)
    {
        if (HitPoint_i(handlePt, mHitTestData[i]))
        {
            hitRes._hitNum = HIT_NUM_DOSEGRID_FULLDIRECTION;
            return;
        }
    }
    for (int i = 0; i < mHitTestData.size() - 1; i++)
    {
        if (HitLine_i(handlePt, mHitTestData[i], mHitTestData[i + 1]))
        {
            switch (i)
            {
            case 0:
                hitRes._hitNum = HIT_NUM_DOSEGRID_TOP;
                return;
            case 1:
                hitRes._hitNum = HIT_NUM_DOSEGRID_LEFT;
                return;
            case 2:
                hitRes._hitNum = HIT_NUM_DOSEGRID_BOTTOM;
                return;
            }
        }
    }
    if (HitLine_i(handlePt, mHitTestData[mHitTestData.size() - 1], mHitTestData[0]))
    {
        hitRes._hitNum = HIT_NUM_DOSEGRID_RIGHT;
        return;
    }
    hitRes._hitNum = HIT_NUM_NONE;
}

bool DoseGridBoundaryRenderStrategy::HitPoint_i(const Viewer3D::Point2D& point, Viewer3D::Point2D& targetPt)
{
    double lengthPts = sqrt((point.x - targetPt.x) * (point.x - targetPt.x) +
        (point.y - targetPt.y) * (point.y - targetPt.y));
    if (lengthPts - HIT_TOLERANCE < -1e-6)
    {
        return true;
    }
    else
    {
    return false;
}
}

bool DoseGridBoundaryRenderStrategy::HitLine_i(const Viewer3D::Point2D& point, Viewer3D::Point2D& startPt, Viewer3D::Point2D& endPt)
{
    Vector2D vectX(endPt - startPt);
    Vector2D vectY(vectX.y, -vectX.x);
    vectY.Normalize();
    vectY *= HIT_TOLERANCE;
    Point2D rect[4];
    rect[0].x = startPt.x - vectY.x;
    rect[0].y = startPt.y - vectY.y;
    rect[1].x = startPt.x - vectY.x + vectX.x;
    rect[1].y = startPt.y - vectY.y + vectX.y;
    rect[2].x = startPt.x + vectY.x + vectX.x;
    rect[2].y = startPt.y + vectY.y + vectX.y;
    rect[3].x = startPt.x + vectY.x;
    rect[3].y = startPt.y + vectY.x;
    double angle = 0.0;
    Vector2D vectTmp1 = point - rect[0];
    Vector2D vectTmp2;
    for (int i = 1; i <= 4; ++i){
        vectTmp2 = point - rect[i % 4];
        angle += vectTmp2.AngleBetween(vectTmp1);
        vectTmp1 = vectTmp2;
    }
    return fabs(angle - 2 * PI) <= 1e-6;
}

TPS_END_NAMESPACE
