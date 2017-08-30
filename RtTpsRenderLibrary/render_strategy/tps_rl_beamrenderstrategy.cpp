#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_beamrenderstrategy.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_wedgegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_antialiased_circle_painter.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"
#include "RtTpsAlgorithmProxy/tps_ap_sat.h"
#include <cmath>

#define DOUBLE_BIG_ENOUGH 100000.0
//use tolerance(0.01 pixel) to avoid FLOAT precision issue
#define PIXEL_TOLERANCE 0.01
#define ANGLE_TOLERANCE 20
#define ARC_RADIUS_RATIO 0.2
#define ARC_RADIUS_INCREMENT_RATIO 0.15
#define INTERVAL_DEGREE 2

using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE

float _normal_color[4] = { 0.f, 1.f, 0.f, 1.f };
float _active_color[4] = { 1.f, 1.f, 0.f, 1.f };
//float _active_color[4] = {0.38f, 0.66f, 0.83f, 1.f};
float _block_color[4] = { 0.67f, 0.42f, 0.f, 1.f };

TpsBeamRenderStrategy::TpsBeamRenderStrategy()
    : mNormalGroupCollection(nullptr), mMprGO(nullptr)
    , mVolumeGO(nullptr), mSectionType(AXIAL), mPointPainter(nullptr)
{

}

TpsBeamRenderStrategy::~TpsBeamRenderStrategy()
{

}

void TpsBeamRenderStrategy::Initialize()
{
    mPointPainter = new AntialiasedCirclePainter();
    mPointPainter->Initialize();

    glGenBuffers(2, mWedgeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mWedgeVBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mWedgeVBO[1]);
    glGenBuffers(1, &mWedgeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mWedgeEBO);

    unsigned int block_cube_index[24] = {
        0, 1, 5, 4, 2, 3, 7, 6,
        0, 3, 2, 1, 4, 5, 6, 7,
        1, 2, 6, 5, 0, 4, 7, 3,
    };

    glGenBuffers(1, &mBlockVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mBlockVBO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &mBlockEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBlockEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(block_cube_index), block_cube_index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int TpsBeamRenderStrategy::CacheChanges()
{
    if (mMprGO == nullptr || mVolumeGO == nullptr ||
        /*mBeamGoCollection == nullptr*/mNormalGroupCollection == nullptr){
        return -1;
    }

    if (!mVolumeGO->IsValid()){
        return -1;
    }

    if (mMprGO->IsDirty() || mVolumeGO->IsDirty() ||
        /*mBeamGoCollection->IsDirty()*/mNormalGroupCollection->IsDirty()){
        mDirty = true;
    }
    return 0;
}

void TpsBeamRenderStrategy::Resize(int iWidth, int iHeight)
{
    mWidth = iWidth;
    mHeight = iHeight;
}

void TpsBeamRenderStrategy::Finalize()
{
    if (mPointPainter != nullptr){
        delete mPointPainter;
        mPointPainter = nullptr;
    }

    glDeleteBuffers(2, mWedgeVBO);
    glDeleteBuffers(1, &mWedgeEBO);
    glDeleteBuffers(1, &mBlockVBO);
    glDeleteBuffers(1, &mBlockEBO);
}

void TpsBeamRenderStrategy::SetSectionType(FIRST_POSTFIX_COMPONENT section)
{
    mSectionType = section;
}

//////////////////////////////////////////////////////////////////////////
// include beam template render
void TpsBeamRenderStrategy::Render_i()
{
    mHitTestData.clear();
    //check again
    if (mMprGO == nullptr || mVolumeGO == nullptr || mNormalGroupCollection == nullptr){
        return;
    }

    CalculateWorld2ViewTM_i();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glViewport(0, 0, mWidth, mHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, mWidth, 0, mHeight, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

    Matrix4x4 matBeam2View, matBeam2Patient, matBeam2World;
    Matrix4x4 matPatient2World = mVolumeGO->GetWorld2Patient().Inverse();

    bool isActiveNG = false, isHittable = false, hasIntersectionWithMpr = false;
    std::map<std::string, std::shared_ptr<TpsBeamGraphicObject>> beamList;
    std::shared_ptr<TpsBeamGraphicObject> beamGO = nullptr;
    std::shared_ptr<TpsSegmentGraphicObject> segmentGO = nullptr;
    std::map<std::string, NormalGroupPtr> normalGroupMap =
        mNormalGroupCollection->GetNormalGroupMap();
    mIsoCenterTempData.clear();
    for (auto itNG = normalGroupMap.begin(); itNG != normalGroupMap.end(); ++itNG)
    {
        if (itNG->second == nullptr) continue;
        isActiveNG = itNG->first == mNormalGroupCollection->GetActiveNormalGroupUID();
        isHittable = !itNG->second->IsApproved();
        beamList = itNG->second->GetBeamMap();
        double sad = itNG->second->GetSAD();
        for (auto itBeam = beamList.begin(); itBeam != beamList.end(); ++itBeam)
        {
            beamGO = itBeam->second;
            if (beamGO == nullptr)
            {
                TPS_LOG_DEV_ERROR << "BeamGo is null.";
                continue;
            }
            if (!beamGO->GetVisible()) continue;

            segmentGO = beamGO->GetActiveSegmentGo();
            if (nullptr == segmentGO)
            {
                TPS_LOG_DEV_ERROR << "segment is null.";
                continue;
            }

            bool isActiveBeam = isActiveNG && beamGO == itNG->second->GetActiveBeamGo();
            if (isActiveBeam) {
                glColor4f(_active_color[0], _active_color[1], _active_color[2], _active_color[3]);
                glLineWidth(1.0f);
            }
            else {
                glColor4f(_normal_color[0], _normal_color[1], _normal_color[2], _normal_color[3]);
                glLineWidth(0.5f);
            }

            segmentGO->GetStartCollimatorToPatMatrix(&matBeam2Patient);
            matBeam2World = matBeam2Patient;
            matBeam2World.Prepend(matPatient2World);
            matBeam2View = matBeam2World;
            matBeam2View.Prepend(mWorld2View);

            hasIntersectionWithMpr = IfMprIntersectsWithBeamCombinationFields_i(
                beamGO, matBeam2World.Inverse(), sad);

            DrawBeamLines_i(beamGO, segmentGO, matPatient2World,
                matBeam2Patient, matBeam2View, isActiveBeam,
                isHittable, hasIntersectionWithMpr, sad);
            if (hasIntersectionWithMpr)
            {
                DrawAccessories_i(beamGO, segmentGO, sad);
            }
        }
    }

    RenderIsoCenters_i();

    glLineWidth(1.0f);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void TpsBeamRenderStrategy::DrawBeamLines_i(
    const BeamGoPtr& beam,
    const SegmentGoPtr& segment,
    const Viewer3D::Matrix4x4& patient2World,
    const Viewer3D::Matrix4x4& beam2Patient,
    const Viewer3D::Matrix4x4& beam2View,
    bool isActive,
    bool isHittable,
    bool isToDrawCentralLine, 
    double sad)
{
    bool isArc = beam->GetBeamType() == ARC;
    if (isArc && beam->IsArcEditModeEnabled()){
        DrawEditModeArc_i(beam, patient2World, sad);
        return;
    }

    auto beamLines = ComputeBeamLine_i(segment, beam2View);
    if (!beamLines.empty())
    {
        glBegin(GL_LINES);
        for (auto it = beamLines.begin(); it != beamLines.end(); ++it) {
            glVertex3d(it->x, it->y, it->z);
        }
        glEnd();
    }

    if (isArc) {
        DrawArc_i(beam, patient2World, isHittable, sad);
    }
    //draw current segment central line
    //the central line of arc beam can not be hit
    if (isToDrawCentralLine)
    {
        DrawCentralLine_i(beam->GetUID(), patient2World, beam2Patient,
            beam->GetIsoCenterUID(), isActive, isArc ? false : isHittable, sad);
    }
}

void TpsBeamRenderStrategy::DrawAccessories_i(const BeamGoPtr& beam, 
    const SegmentGoPtr& segment, double sad)
{
    DrawWedge_i(beam, segment);

    //     Matrix4x4 matPatient2World = mVolumeGO->GetWorld2Patient().Inverse();
    //     Matrix4x4 matBeam2World;
    //     segment->GetStartCollimatorToPatMatrix(&matBeam2World);
    //     matBeam2World.Prepend(matPatient2World);
    // 
    //     Point3D isoCenter = matBeam2World.Transform(_isoCenter);
    //     Point3D source = matBeam2World.Transform(_source);
    //     Vector3D vecS2A = isoCenter - source;
    //     Vector3D mprNormal;
    //     mMprGO->GetNormal(&mprNormal);
    // 
    //     double tolerance = ANGLE_TOLERANCE * PI / 180;
    //     //当前segment source to isocenter与mpr足够平行时才显示各个附件
    //     if (fabs(mprNormal.AngleBetween(vecS2A) - PI / 2) > tolerance)
    //     {
    //         return;
    //     }
    DrawBlock_i(beam, segment, sad);
}

void TpsBeamRenderStrategy::DrawWedge_i(const BeamGoPtr& beam, const SegmentGoPtr& segment)
{
    if (beam->GetPhysicalWedgeGO() == nullptr &&
        beam->GetVirtualWedgeGO() == nullptr) return;

    Matrix4x4 matPatient2World = mVolumeGO->GetWorld2Patient().Inverse();
    Matrix4x4 matBeam2World;
    segment->GetStartCollimatorToPatMatrix(&matBeam2World);
    matBeam2World.Prepend(matPatient2World);
    Vector3D normal;
    mMprGO->GetNormal(&normal);

    Camera camera;
    mMprGO->GetProjectionCamera(mWidth, mHeight, camera);
    auto matView = camera.GetViewMatrix();
    auto matPrj = camera.GetProjectionMatrix();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(matPrj._m);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    std::shared_ptr<TpsWedgeGraphicObject> wedges[2];
    wedges[0] = beam->GetPhysicalWedgeGO();
    wedges[1] = beam->GetVirtualWedgeGO();
    std::shared_ptr<TpsWedgeGraphicObject> wedge;

    unsigned int planeIndex[WEDGE_GO_VERTEX_INDEX_COUNT];
    unsigned int lineIndex[WEDGE_GO_LINE_COUNT * 2];
    unsigned int lineBufferCount;
    Point3D verteices[WEDGE_GO_VERTEX_COUNT];
    double color[WEDGE_GO_VERTEX_COUNT * 3];
    Matrix4x4 modelView, world2Wedge;
    Vector3D lookAtToEye;
    for (int i = 0; i < 2; ++i){
        wedge = wedges[i];
        if (wedge == nullptr) continue;

        wedge->GetVertexIndex(planeIndex);
        wedge->GetVertices(verteices);
        wedge->GetVertexColor(color);

        modelView = wedge->GetWedgeToBeam();
        modelView.Prepend(matBeam2World);
        world2Wedge = modelView.Inverse();
        modelView.Prepend(matView);
        lookAtToEye = world2Wedge.Transform(normal);
        wedge->CalcVisibleLineIndexBuffer(lookAtToEye, lineIndex, lineBufferCount);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(modelView._m);
        DrawSingleWedge_i(verteices, planeIndex, WEDGE_GO_VERTEX_INDEX_COUNT,
            lineIndex, lineBufferCount, color);
    }
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void TpsBeamRenderStrategy::DrawSingleWedge_i(
    Point3D* vertices, unsigned int* planeIndex, unsigned int planeBufferCount,
    unsigned int* lineIndex, unsigned int lineBufferCount, double* color)
{
    double vertexBuffer[WEDGE_GO_VERTEX_COUNT * 3];
    for (int i = 0; i < WEDGE_GO_VERTEX_COUNT; ++i){
        vertexBuffer[3 * i] = vertices[i].x;
        vertexBuffer[3 * i + 1] = vertices[i].y;
        vertexBuffer[3 * i + 2] = vertices[i].z;
    }
    double colorBuffer[WEDGE_GO_VERTEX_COUNT * 4];
    for (int i = 0; i < WEDGE_GO_VERTEX_COUNT; ++i){
        colorBuffer[4 * i] = color[3 * i];
        colorBuffer[4 * i + 1] = color[3 * i + 1];
        colorBuffer[4 * i + 2] = color[3 * i + 2];
        colorBuffer[4 * i + 3] = 0.3;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mWedgeVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_DYNAMIC_DRAW);
    glVertexPointer(3, GL_DOUBLE, 0, NULL);
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, mWedgeVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorBuffer), colorBuffer, GL_DYNAMIC_DRAW);
    glColorPointer(4, GL_DOUBLE, 0, NULL);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mWedgeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeBufferCount * sizeof(unsigned int), 
        planeIndex, GL_STATIC_DRAW);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_QUADS, planeBufferCount, GL_UNSIGNED_INT, NULL);

    //an issue using polygon line method, so we decide whether to draw the line by ourselves
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDrawElements(GL_QUADS, planeBufferCount, GL_UNSIGNED_INT, planeIndex);

    glLineWidth(2.0f);
    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    for (int i = 0; i < WEDGE_GO_VERTEX_COUNT; ++i){
        colorBuffer[4 * i + 3] = 1.0;
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorBuffer), colorBuffer, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineBufferCount * sizeof(unsigned int), 
        lineIndex, GL_DYNAMIC_DRAW);
    glDrawElements(GL_LINES, lineBufferCount, GL_UNSIGNED_INT, NULL);

    glLineWidth(1.0f);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void TpsBeamRenderStrategy::DrawBlock_i(const BeamGoPtr& beam, 
    const SegmentGoPtr& segment, double sad)
{
    if (beam->GetApertureBlock() == nullptr &&
        beam->GetShieldingBlock() == nullptr) return;

    Camera camera;
    mMprGO->GetProjectionCamera(mWidth, mHeight, camera);
    auto matView = camera.GetViewMatrix();
    auto matPrj = camera.GetProjectionMatrix();

    Matrix4x4 matBeam2Pat;
    segment->GetStartCollimatorToPatMatrix(&matBeam2Pat);
    auto matModelView = matView;
    matModelView.Append(mVolumeGO->GetWorld2Patient().Inverse());
    matModelView.Append(matBeam2Pat);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(matPrj._m);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(matModelView._m);

    //block vertex array buffer objects
    double ratioToSAD = 0.81;
    //     float jawPos[4];
    //     segment->GetJawPosition(jawPos);
    double width = 100 * ratioToSAD * 1.1;
    double height = 100 * ratioToSAD * 1.1;
    double thickness = TPS_MAX(width, height) * ratioToSAD * 0.2;
    double block_vertex_buffer[24] =
    {
        -width / 2, -height / 2, -sad * ratioToSAD - thickness,
        width / 2, -height / 2, -sad * ratioToSAD - thickness,
        width / 2, height / 2, -sad * ratioToSAD - thickness,
        -width / 2, height / 2, -sad * ratioToSAD - thickness,
        -width / 2, -height / 2, -sad * ratioToSAD,
        width / 2, -height / 2, -sad * ratioToSAD,
        width / 2, height / 2, -sad * ratioToSAD,
        -width / 2, height / 2, -sad * ratioToSAD,
    };

    glBindBuffer(GL_ARRAY_BUFFER, mBlockVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(block_vertex_buffer), block_vertex_buffer, GL_STATIC_DRAW);
    glVertexPointer(3, GL_DOUBLE, 0, NULL);
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBlockEBO);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glColor4f(_block_color[0], _block_color[1], _block_color[2], 0.2f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, NULL);
    glColor4f(_block_color[0], _block_color[1], _block_color[2], 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, NULL);

    glDisable(GL_CULL_FACE);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopAttrib();
    //Matrix4x4 matPatient2World = mVolumeGO->GetWorld2Patient().Inverse();
    //Matrix4x4 matBeam2World;
    //segment->GetStartCollimatorToPatMatrix(&matBeam2World);
    //matBeam2World.Prepend(matPatient2World);
    //Matrix4x4 matBeam2View = matBeam2World;
    //matBeam2View.Prepend(mWorld2View);

    //double ratioToSAD = 0.8;
    //float jawPos[4];
    //segment->GetJawPosition(jawPos);
    //double width = TPS_MAX(fabs(jawPos[1] - jawPos[0]), fabs(jawPos[3] - jawPos[2])) * 1.2;
    ////length in view(pixels)
    //width = matBeam2View.Transform(Vector3D(width, 0, 0)).Magnitude() * ratioToSAD;
    //double height = width * 0.2;

    ////iso center point projected to mpr plane
    //auto isoCenterWorld = matBeam2World.Transform(_isoCenter);
    //auto isoCenterProjected = mMprGO->ProjectToPlane(isoCenterWorld);
    //auto isoCenterV = mWorld2View.Transform(isoCenterProjected);
    ////source point projected to mpr plane
    //auto sourceProjected = matBeam2World.Transform(_source);
    //sourceProjected = mMprGO->ProjectToPlane(sourceProjected);
    //auto sourceV = mWorld2View.Transform(sourceProjected);

    //Vector3D ortY = isoCenterV - sourceV;
    //double sadPrjd = ortY.Magnitude();
    //ortY.Normalize();
    //Vector3D ortX(-ortY.y, ortY.x, 0.0);
    //ortX.Normalize();
    //Point3D topCenter = sourceV + sadPrjd * ratioToSAD * ortY;
    //Point3D vertices[4] = 
    //{
    //    topCenter + width / 2 * ortX,
    //    topCenter + width / 2 * ortX + height * ortY,
    //    topCenter + -width / 2 * ortX + height * ortY,
    //    topCenter + -width / 2 * ortX
    //};
    //
    //glColor4f(_block_color[0], _block_color[1], _block_color[2], 1.0f);
    //glBegin(GL_LINE_LOOP);
    //for (int i = 0; i < 4; ++i){
    //    glVertex3d(vertices[i].x, vertices[i].y, 0.0);
    //}
    //glEnd();

    //glColor4f(_block_color[0], _block_color[1], _block_color[2], 0.2f);
    //glBegin(GL_QUADS);
    //for (int i = 0; i < 4; ++i){
    //    glVertex3d(vertices[i].x, vertices[i].y, 0.0);
    //}
    //glEnd();
}

void TpsBeamRenderStrategy::RenderIsoCenters_i()
{
    if (mPointPainter == nullptr || mIsoCenterTempData.empty()) return;
    int count = mIsoCenterTempData.size();

    float* centers = new float[count * 2];
    float* radius = new float[count];
    float* colors = new float[count * 3];
    bool* isFilled = new bool[count];
    int i = 0;
    for (auto it = mIsoCenterTempData.begin(); it != mIsoCenterTempData.end(); ++it, ++i){
        centers[2 * i] = it->second.x;
        centers[2 * i + 1] = it->second.y;
        radius[i] = 2.f;
        if (it->second.isActive){
            colors[3 * i] = _active_color[0];
            colors[3 * i + 1] = _active_color[1];
            colors[3 * i + 2] = _active_color[2];
        }
        else{
            colors[3 * i] = _normal_color[0];
            colors[3 * i + 1] = _normal_color[1];
            colors[3 * i + 2] = _normal_color[2];
        }
        isFilled[i] = true; //it->second.isFilled;
    }
    mPointPainter->SetParas(centers, radius, colors, isFilled, count, false);
    mPointPainter->Render();

//     glDisable(GL_DEPTH_TEST);
//     glEnable(GL_BLEND);
//     glEnable(GL_POINT_SMOOTH);
//     glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
// 
//     glViewport(0, 0, mWidth, mHeight);
//     glBegin(GL_POINTS);
//     for (int i = 0; i < count; ++i)
//     {
//         glPointSize(radius[i]);
//         glColor3f(colors[3 * i], colors[3 * i + 1], colors[3 * i + 2]);
//         glVertex2f(centers[2 * i], centers[2 * i + 1]);
//     }
//     glEnd();
//     glDisable(GL_BLEND);
//     glDisable(GL_POINT_SMOOTH);
// 

    delete[] centers;
    delete[] radius;
    delete[] colors;
    delete[] isFilled;
}

void TpsBeamRenderStrategy::SetMprGraphicObject(std::shared_ptr<MPRGraphicObject> mprGo) {
    if (mMprGO != mprGo) {
        mMprGO = mprGo;
    }
}

void TpsBeamRenderStrategy::SetVolumeGraphicObject(std::shared_ptr<GraphicObjVolume> volumeGo) {
    if (mVolumeGO != volumeGo){
        mVolumeGO = volumeGo;
        mDirty = true;
    }
}

std::vector<Point3D> TpsBeamRenderStrategy::ComputeBeamLine_i(
    const std::shared_ptr<TpsSegmentGraphicObject>& segment,
    const Viewer3D::Matrix4x4& matBeam2View)
{
    std::vector<Point3D> beamLine;
    std::vector<Point3D> outlinePoints;
    if (!segment->GetOutLine(&outlinePoints)){
        TPS_LOG_DEV_ERROR << "Failed to get segment outline.";
        return beamLine;
    }
    if (outlinePoints.size() < 2){
        TPS_LOG_DEV_WARNING << "The size of segment outline is less than 2.";
        return beamLine;
    }
    //make the outline points a loop
    outlinePoints.push_back(outlinePoints.front());

    Point3D source(0, 0, 0);
    Point3D sourceView = matBeam2View.Transform(source);
    Vector3D vecFirst = matBeam2View.Transform(outlinePoints[0] - source), vecSecond;
    vecFirst.Normalize();
    Point3D start, end;
    for (int i = 1; i < outlinePoints.size(); ++i){
        vecSecond = matBeam2View.Transform(outlinePoints[i] - source);
        vecSecond.Normalize();
        if (this->ComputeIntersectionLineInView_i(
            sourceView, vecFirst, vecSecond, start, end)){
            beamLine.push_back(start);
            beamLine.push_back(end);
        }
        vecFirst = vecSecond;
    }
    return beamLine;
}

bool TpsBeamRenderStrategy::ComputeIntersectionLineInView_i(
    const Point3D& ptSource, const Vector3D& vecFirst,
    const Vector3D& vecSecond, Point3D& start, Point3D& end)
{
    bool noInterWithFirst =
        (ptSource.z > DOUBLE_EPSILON && vecFirst.z >= -DOUBLE_EPSILON) ||
        (ptSource.z < -DOUBLE_EPSILON && vecFirst.z <= DOUBLE_EPSILON);
    bool noInterWithSecond =
        (ptSource.z > DOUBLE_EPSILON && vecSecond.z >= -DOUBLE_EPSILON) ||
        (ptSource.z < -DOUBLE_EPSILON && vecSecond.z <= DOUBLE_EPSILON);
    if (noInterWithFirst & noInterWithSecond) return false;

    //get intersection points
    std::vector<Point3D> points;
    double lamda;
    if (!noInterWithFirst){
        lamda = (0.0 - ptSource.z) / vecFirst.z;
        points.push_back(ptSource + lamda * vecFirst);
    }
    if (!noInterWithSecond){
        lamda = (0.0 - ptSource.z) / vecSecond.z;
        points.push_back(ptSource + lamda * vecSecond);
    }

    double maxInterval;
    Vector3D vecLine;
    Point3D ptStart = points[0];
    if (points.size() > 1){
        vecLine = points[1] - points[0];
        maxInterval = vecLine.Magnitude();
        vecLine.Normalize();
    }
    else{
        vecLine = noInterWithFirst ? vecFirst : vecSecond;
        vecLine.z = 0.0;
        vecLine.Normalize();
        maxInterval = DOUBLE_BIG_ENOUGH;//std::numeric_limits<double>::max();
    }
    bool clippedToWindow = false;
    if (!clippedToWindow)
    {
        start = ptStart; end = ptStart + maxInterval * vecLine;
    }
    else
    {
        //clip to the window range, only get the lines int current window
        std::vector<double> intervals;
        intervals.push_back(0.0);
        intervals.push_back(maxInterval);
        if (fabs(vecLine.x) > DOUBLE_EPSILON){
            intervals.push_back(-ptStart.x / vecLine.x);
            intervals.push_back((mWidth - 1 - ptStart.x) / vecLine.x);
        }
        if (fabs(vecLine.y) > DOUBLE_EPSILON){
            intervals.push_back(-ptStart.y / vecLine.y);
            intervals.push_back((mHeight - 1 - ptStart.y) / vecLine.y);
        }
        Point3D point;
        points.clear();
        for (int i = 0; i < intervals.size(); ++i){
            if (intervals[i] < -DOUBLE_EPSILON ||
                intervals[i] - maxInterval > DOUBLE_EPSILON) continue;
            point = ptStart + intervals[i] * vecLine;
            if (IsInWindowRange_i(point)){
                points.push_back(point);
            }
        }
        if (points.size() < 2 || points[0] == points[1]) return false;
        start = points[0]; end = points[1];
    }
    return true;
}

bool TpsBeamRenderStrategy::IsInWindowRange_i(const Viewer3D::Point3D& point)
{
    return point.x >= -PIXEL_TOLERANCE && point.x <= mWidth - 1.0 + PIXEL_TOLERANCE &&
        point.y >= -PIXEL_TOLERANCE && point.y <= mHeight - 1.0 + PIXEL_TOLERANCE;
}

void TpsBeamRenderStrategy::CalculateWorld2ViewTM_i()
{
    //view coordinate system: 
    //Origin->left bottom point on screen
    //X->left to right on screen
    //Y->bottom to up on screen
    //Z->Point towards the screen
    Point3D aVertices[4];
    mMprGO->GetVertices(aVertices);
    Vector3D vecY = aVertices[3] - aVertices[0];
    vecY.Normalize();
    Vector3D vecZ;
    mMprGO->GetNormal(&vecZ); vecZ *= -1;
    Vector3D vecX = vecZ.CrossProduct(vecY);
    vecY = vecX.CrossProduct(vecZ);

    double scale, ratio;
    double planeWidth = (aVertices[1] - aVertices[0]).Magnitude();
    double planeHeight = (aVertices[3] - aVertices[0]).Magnitude();
    Point3D ptOrigin = aVertices[0];
    double ratioPlane = planeWidth / planeHeight;
    double ratioView = (double)mWidth / mHeight;
    if (ratioPlane <= ratioView){
        scale = mHeight / planeHeight;
        ratio = ratioView / ratioPlane;
        ptOrigin += (aVertices[0] - aVertices[1]) * (ratio - 1) * 0.5;
    }
    else{
        scale = mWidth / planeWidth;
        ratio = ratioPlane / ratioView;
        ptOrigin += (aVertices[0] - aVertices[3]) * (ratio - 1) * 0.5;
    }

    Vector3D temp(ptOrigin.x, ptOrigin.y, ptOrigin.z);
    mWorld2View = Matrix4x4(vecX.x, vecY.x, vecZ.x, 0.0,
        vecX.y, vecY.y, vecZ.y, 0.0,
        vecX.z, vecY.z, vecZ.z, 0.0,
        -1 * temp.DotProduct(vecX),
        -1 * temp.DotProduct(vecY),
        -1 * temp.DotProduct(vecZ), 1.0);

    mWorld2View.Prepend(MakeScale(Vector3D(scale, scale, scale)));
}

void TpsBeamRenderStrategy::DrawArc_i(
    const std::shared_ptr<TpsBeamGraphicObject>& beamGo,
    const Matrix4x4& matPatientToWorld, bool hittable, double sad)
{
    std::string beamUid = beamGo->GetUID();
    auto segmentList = beamGo->GetSegmentGoList();
    if (segmentList.empty()) return;

    Vector3D mprNormal;
    mMprGO->GetNormal(&mprNormal);
    auto iecFixToWorld = beamGo->GetIECFixToPatientMatrix();
    iecFixToWorld.Prepend(matPatientToWorld);
    auto rotationAxis = iecFixToWorld.Transform(Vector3D(0, 1, 0));
    double tolerance = ANGLE_TOLERANCE * PI / 180;
    if (fabs(mprNormal.AngleBetween(rotationAxis) - PI / 2) <= tolerance)
    {
        return;
    }

    auto startSegment = segmentList.front();
    auto endSegment = segmentList.back();
    //save central line for hit test
    if (hittable)
    {
        Matrix4x4 startBeamToWorld;
        startSegment->GetStartCollimatorToPatMatrix(&startBeamToWorld);
        startBeamToWorld.Prepend(matPatientToWorld);
        Matrix4x4 endBeamToWorld;
        endSegment->GetEndCollimatorToPatMatrix(&endBeamToWorld);
        endBeamToWorld.Prepend(matPatientToWorld);
        Point3D isoCenter = startBeamToWorld.Transform(Point3D(0, 0, -sad));
        Point3D startSource = startBeamToWorld.Transform(Point3D(0, 0, 0));
        Point3D endSource = endBeamToWorld.Transform(Point3D(0, 0, 0));
        Vector3D startS2A = isoCenter - startSource;
        Vector3D endS2A = isoCenter - endSource;

        isoCenter = mMprGO->ProjectToPlane(isoCenter);
        isoCenter = mWorld2View.Transform(isoCenter);
        startSource = mMprGO->ProjectToPlane(startSource);
        startSource = mWorld2View.Transform(startSource);
        endSource = mMprGO->ProjectToPlane(endSource);
        endSource = mWorld2View.Transform(endSource);
        startS2A = isoCenter - startSource;
        endS2A = isoCenter - endSource;

        BeamHitData hitData;
        hitData.type = ARC;
        hitData.uid = beamUid;
        hitData.startLine.start.x = startSource.x;
        hitData.startLine.start.y = startSource.y;
        hitData.startLine.end.x = isoCenter.x;
        hitData.startLine.end.y = isoCenter.y;
        hitData.endLine.start.x = endSource.x;
        hitData.endLine.start.y = endSource.y;
        hitData.endLine.end.x = isoCenter.x;
        hitData.endLine.end.y = isoCenter.y;
        mHitTestData.push_back(hitData);
    }

    //draw arc curve
    //gantry rotation direction
    bool isClockwise = startSegment->GetGantryIsClockWise();
    //gantry angles for every segments
    std::vector<double> startGantryAngles;
    for (int i = 0; i < segmentList.size(); ++i){
        startGantryAngles.push_back(segmentList[i]->GetStartGantryAngle());
    }
    std::vector<double> arcLengths;
    for (int i = 0; i < segmentList.size(); ++i){
        arcLengths.push_back(segmentList[i]->GetGantryArcLength());
    }
    //start gantry to world
    double collimatorRad = startSegment->GetCollimatorAngle() * PI / 180.0;
    Quaternion quat(-collimatorRad, Vector3D(0.0, 0.0, 1.0));
    Matrix4x4 gantry2Collimator = MakeTranslate(Vector3D(0.0, 0.0, -sad));
    gantry2Collimator.Prepend(quat.ToMatrix());
    Matrix4x4 startGantryToWorld;
    startSegment->GetStartCollimatorToPatMatrix(&startGantryToWorld);
    startGantryToWorld.Prepend(matPatientToWorld);
    startGantryToWorld.Append(gantry2Collimator);
    //end gantry to world
    collimatorRad = endSegment->GetCollimatorAngle() * PI / 180.0;
    quat = Quaternion(-collimatorRad, Vector3D(0.0, 0.0, 1.0));
    Matrix4x4 endGantryToWorld;
    endSegment->GetEndCollimatorToPatMatrix(&endGantryToWorld);
    endGantryToWorld.Prepend(matPatientToWorld);
    endGantryToWorld.Append(gantry2Collimator);
    DrawArc_i(startGantryAngles, arcLengths, isClockwise, startGantryToWorld, endGantryToWorld, sad);
    DrawArrow_i(beamGo, matPatientToWorld, sad);
}

void TpsBeamRenderStrategy::DrawEditModeArc_i(
    const std::shared_ptr<TpsBeamGraphicObject>& beamGo,
    const Viewer3D::Matrix4x4& matPatientToWorld, double sad)
{
    Vector3D mprNormal;
    mMprGO->GetNormal(&mprNormal);
    auto iecFixToWorld = beamGo->GetIECFixToPatientMatrix();
    iecFixToWorld.Prepend(matPatientToWorld);
    auto rotationAxis = iecFixToWorld.Transform(Vector3D(0, 1, 0));
    double tolerance = ANGLE_TOLERANCE * PI / 180;
    if (fabs(mprNormal.AngleBetween(rotationAxis) - PI / 2) <= tolerance)
    {
        return;
    }

    //draw edit mode arc
    ArcBeamEditCache& arcCache = beamGo->GetArcEditCache();
    Matrix4x4 startGantry2World = arcCache._matStartGantry2Patient;
    startGantry2World.Prepend(matPatientToWorld);
    Matrix4x4 endGantry2World = arcCache._matEndGantry2Patient;
    endGantry2World.Prepend(matPatientToWorld);

    std::vector<double> startAngles, arcLengths;
    double start = arcCache._startGantry;
    double arcLength = arcCache._arcLength;
    if (arcCache._isSimpleArc)
    {
        startAngles.push_back(start);
        arcLengths.push_back(arcLength);
    }
    else
    {
        double len = 0.0;
        int sign = arcCache._isClockwise ? 1 : -1;
        for (; len - arcLength < -1e-6; len += INTERVAL_DEGREE)
        {
            startAngles.push_back(TpsUtility::ToClockPosition(start + sign * len));
            arcLengths.push_back(TPS_MIN(INTERVAL_DEGREE, arcLength - len));
        }
    }
    DrawArc_i(startAngles, arcLengths, arcCache._isClockwise, startGantry2World, endGantry2World, sad);
}

void TpsBeamRenderStrategy::DrawArc_i(
    const std::vector<double>& startGantryAngles,
    const std::vector<double>& arcLengths,
    bool isClockwise,
    const Viewer3D::Matrix4x4& matStartGantryToWorld,
    const Viewer3D::Matrix4x4& matEndGantryToWorld, 
    double sad)
{
    if (startGantryAngles.size() != arcLengths.size() ||
        startGantryAngles.empty()) return;

    //take the start segment according to gantry direction flag, 
    //always draw the arc in clockwise direction, 
    //because it is easy to use cos & sin.
    int sign = isClockwise ? 1 : -1;
    Matrix4x4 matGantryToWorld = isClockwise ? matStartGantryToWorld : matEndGantryToWorld;
    //     double startAngle = isClockwise ? startGantryAngles.front() : 
    //         TpsUtility::ToClockPosition(startGantryAngles.back() + sign * arcLengths.back());

    Camera camera;
    mMprGO->GetProjectionCamera(mWidth, mHeight, camera);
    Matrix4x4 matProjection = camera.GetProjectionMatrix();
    //get model view matrix, which model c.s.is is gantry
    Matrix4x4 matModelView = camera.GetViewMatrix();
    matModelView.Append(matGantryToWorld);

    double radiusBase = ARC_RADIUS_RATIO * sad;
    double radiusIncrement = ARC_RADIUS_INCREMENT_RATIO * radiusBase;
    //the rotation radian, should always be positive
    double totalArcLength = 0.0;
    double start, end;
    std::vector<double> milestones;
    milestones.push_back(0.0);
    for (int i = 0; i < arcLengths.size(); ++i)
    {
        totalArcLength += arcLengths[i];
        milestones.push_back(totalArcLength);
        if (i == arcLengths.size() - 1) break;
        start = startGantryAngles[i] + sign * arcLengths[i];
        end = startGantryAngles[i + 1];
        totalArcLength += TpsUtility::ToClockPosition(sign * (end - start));
        milestones.push_back(totalArcLength);
    }
    double rotationRad = totalArcLength * PI / 180;
    int msCounter = 0;

    int maxPtCount = ceil(totalArcLength / 2) + 1 + milestones.size() + floor(totalArcLength / 360);
    maxPtCount *= 2;
    double* solidArc = new double[maxPtCount * 3];
    double* dotArc = new double[maxPtCount * 3];
    double* shortLines = new double[milestones.size() * 3 * 2];
    memset(solidArc, 0, maxPtCount * 3 * sizeof(double));
    memset(dotArc, 0, maxPtCount * 3 * sizeof(double));
    memset(shortLines, 0, milestones.size() * 3 * 2 * sizeof(double));
    int solidCounter = 0, dotCounter = 0, slCounter = 0;

    double* targetArc = solidArc; int* targetCounter = &solidCounter;
    double* idleArc = dotArc; int* idleCounter = &dotCounter;
    double arcLength = 0.0, rad, _sin, _cos;
    double radius = radiusBase;
    Vector3D vector(0.0, 0.0, 0.0);
    int circleCount = 1;
    while (arcLength <= totalArcLength)
    {
        rad = arcLength / 180 * PI;
        _sin = sin(rad); _cos = cos(rad);
        targetArc[3 * (*targetCounter)] = _sin * radius;
        targetArc[3 * (*targetCounter)++ + 2] = _cos * radius;

        //if new circle begins, expand the radius
        if (fabs(arcLength - circleCount * 360) <= 1e-6)
        {
            circleCount++;
            radius += radiusIncrement;
        }

        bool hitTheEdge = false;
        bool comingToZeroMilestone = false;
        //if hit a milestone, add short line, swap target with idle if not the loop beginning or end
        if (fabs(arcLength - milestones[msCounter]) <= 1e-6)
        {
            vector.x = _sin; vector.z = _cos;
            shortLines[slCounter * 3] = radius * vector.x;
            shortLines[slCounter++ * 3 + 2] = radius * vector.z;
            shortLines[slCounter * 3] = 0.95 * radius * vector.x;
            shortLines[slCounter++ * 3 + 2] = 0.95 * radius * vector.z;
            //if not the loop beginning or end
            hitTheEdge = msCounter == 0 || msCounter == milestones.size() - 1;
            comingToZeroMilestone = msCounter != milestones.size() - 1 &&
                fabs(milestones[msCounter] - milestones[msCounter + 1]) <= 1e-6;
            //if at the edge or next milestone's length is 0, need not swap
            if (!hitTheEdge && !comingToZeroMilestone)
            {
                swap(targetArc, idleArc);
                swap(targetCounter, idleCounter);
            }
            if (comingToZeroMilestone) msCounter += 2;
            else msCounter++;
        }

        //只有hit首尾的milestone时，不需要添加第二个点
        if (!hitTheEdge)
        {
            targetArc[3 * (*targetCounter)] = _sin * radius;
            targetArc[3 * (*targetCounter)++ + 2] = _cos * radius;
        }

        //calculate the step
        arcLength = TPS_MIN(arcLength + 2, circleCount * 360);
        if (msCounter < milestones.size())
        {
            arcLength = TPS_MIN(arcLength, milestones[msCounter]);
        }
    }

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(matModelView._m);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(matProjection._m);

    //start & end central line
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xCCCC);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, sad);
    glEnd();
    glLineStipple(1, 0x3F3F);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(sin(rotationRad) * sad, 0.0, cos(rotationRad) * sad);
    glEnd();

    glLineStipple(1, 0xCCCC);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_VERTEX_ARRAY);
    //dot arc
    glVertexPointer(3, GL_DOUBLE, 0, dotArc);
    glDrawArrays(GL_LINES, 0, dotCounter);
    glDisable(GL_LINE_STIPPLE);
    //solid arc
    glVertexPointer(3, GL_DOUBLE, 0, solidArc);
    glDrawArrays(GL_LINES, 0, solidCounter);
    //short lines
    glVertexPointer(3, GL_DOUBLE, 0, shortLines);
    glDrawArrays(GL_LINES, 0, slCounter);
    glDisableClientState(GL_VERTEX_ARRAY);

    TPS_DEL_ARRAY(dotArc);
    TPS_DEL_ARRAY(solidArc);
    TPS_DEL_ARRAY(shortLines);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void TpsBeamRenderStrategy::DrawCentralLine_i(const std::string& beamUid,
    const Matrix4x4& matPatientToWorld, const Matrix4x4& matBeamToPatient,
    const std::string& isoCenterUid, bool isActive, bool hittable, double sad)
{
    Matrix4x4 beamToWorld = matBeamToPatient;
    beamToWorld.Prepend(matPatientToWorld);

    //iso center point projected to mpr plane
    auto isoCenterWorld = beamToWorld.Transform(Point3D(0, 0, -sad));
    auto isoCenterProjected = mMprGO->ProjectToPlane(isoCenterWorld);
    double dist = (isoCenterWorld - isoCenterProjected).Magnitude();
    bool isOnPlane = fabs(dist) < 1e-5;
    auto isoCenterV = mWorld2View.Transform(isoCenterProjected);
    //source point projected to mpr plane
    auto sourceProjected = beamToWorld.Transform(Point3D(0, 0, 0));
    sourceProjected = mMprGO->ProjectToPlane(sourceProjected);
    auto sourceV = mWorld2View.Transform(sourceProjected);

    glLineStipple(2, 0xFC30);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glVertex3d(sourceV.x, sourceV.y, 0.0);
    glVertex3d(isoCenterV.x, isoCenterV.y, 0.0);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    //dump the iso center data
    auto it = mIsoCenterTempData.find(isoCenterUid);
    if (it == mIsoCenterTempData.end()){
        IsoCenterTempData tmpData;
        tmpData.isFilled = isOnPlane;
        tmpData.x = isoCenterV.x;
        tmpData.y = isoCenterV.y;
        tmpData.isActive = isActive;
        mIsoCenterTempData[isoCenterUid] = tmpData;
    }
    else
    {
        it->second.isActive |= isActive;
    }

    if (hittable){
        //save central line for hit test
        BeamHitData hitData;
        hitData.type = IMRT;
        hitData.uid = beamUid;
        hitData.startLine.start.x = sourceV.x;
        hitData.startLine.start.y = sourceV.y;
        hitData.startLine.end.x = isoCenterV.x;
        hitData.startLine.end.y = isoCenterV.y;
        mHitTestData.push_back(hitData);
    }
}

void TpsBeamRenderStrategy::DrawArrow_i(
    const std::shared_ptr<TpsBeamGraphicObject>& beam,
    const Viewer3D::Matrix4x4& matPatientToWorld, double sad)
{
    auto segment = beam->GetActiveSegmentGo();
    int index = beam->GetActiveSegmentIndex();
    auto segments = beam->GetSegmentGoList();
    double activeSegmentArcPos = 0.0, start, end;
    int sign = segment->GetGantryIsClockWise() ? 1 : -1;
    for (int i = 0; i < index; ++i)
    {
        activeSegmentArcPos += segments[i]->GetGantryArcLength();
        start = segments[i]->GetStartGantryAngle() + sign * segments[i]->GetGantryArcLength();
        end = segments[i + 1]->GetStartGantryAngle();
        activeSegmentArcPos += TpsUtility::ToClockPosition(sign * (end - start));
    }
    int circle_num = floor(activeSegmentArcPos / 360);
    //gantry to world
    double collimatorRad = segment->GetCollimatorAngle() * PI / 180.0;
    Quaternion quat(-collimatorRad, Vector3D(0.0, 0.0, 1.0));
    Matrix4x4 gantry2Collimator = MakeTranslate(Vector3D(0.0, 0.0, -sad));
    gantry2Collimator.Prepend(quat.ToMatrix());
    Matrix4x4 gantryToWorld;
    segment->GetStartCollimatorToPatMatrix(&gantryToWorld);
    gantryToWorld.Prepend(matPatientToWorld);
    gantryToWorld.Append(gantry2Collimator);

    //size in mm
    double arrowHeadLen = 15.0;
    double arrowHeadWidth = 10.0;
    double arrowTailLen = 15.0;
    double arrowTailWidth = 3.0;

    double radiusBase = ARC_RADIUS_RATIO * sad;
    double radiusIncrement = ARC_RADIUS_INCREMENT_RATIO * radiusBase;
    double radius = radiusBase + circle_num * radiusIncrement;
    Matrix4x4 matGantryToView = gantryToWorld;
    matGantryToView.Prepend(mWorld2View);
    Vector3D arrowDirt(segment->GetGantryIsClockWise() ? 1.0 : -1.0, 0.0, 0.0);
    Point3D center(0.0, 0.0, radius);
    Point3D arrowHead = center + arrowDirt * arrowHeadLen;
    Point3D arrowTail = center + -arrowDirt * arrowTailLen;

    Camera camera;
    mMprGO->GetProjectionCamera(mWidth, mHeight, camera);
    Matrix4x4 matProjection = camera.GetProjectionMatrix();
    //get model view matrix, which model c.s.is is gantry
    Matrix4x4 matModelView = camera.GetViewMatrix();
    matModelView.Append(gantryToWorld);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(matModelView._m);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(matProjection._m);

    glBegin(GL_TRIANGLES);
    glVertex3d(center.x, center.y, center.z - arrowHeadWidth * 0.5);
    glVertex3d(center.x, center.y, center.z + arrowHeadWidth * 0.5);
    glVertex3d(arrowHead.x, arrowHead.y, arrowHead.z);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3d(center.x, center.y, center.z - arrowTailWidth * 0.5);
    glVertex3d(center.x, center.y, center.z + arrowTailWidth * 0.5);
    glVertex3d(arrowTail.x, arrowTail.y, arrowTail.z + arrowTailWidth * 0.5);
    glVertex3d(arrowTail.x, arrowTail.y, arrowTail.z - arrowTailWidth * 0.5);
    glEnd();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void TpsBeamRenderStrategy::HitTest(double x, double y, HitTestResult& hitRes)
{
    Point2D handlePt(x * mWidth, y * mHeight);
    for (auto it = mHitTestData.begin(); it != mHitTestData.end(); ++it){
        if (HitLine_i(handlePt, it->startLine)){
            hitRes._targetUid = it->uid;
            hitRes._hitNum = it->type == IMRT ?
            HIT_NUM_IMRT_BEAM : HIT_NUM_ARC_BEAM_START;
            return;
        }
        //for arc beam end line
        if (it->type == ARC && HitLine_i(handlePt, it->endLine)){
            hitRes._targetUid = it->uid;
            hitRes._hitNum = HIT_NUM_ARC_BEAM_END;
            return;
        }
    }
    hitRes._hitNum = HIT_NUM_NONE;
}

bool TpsBeamRenderStrategy::HitLine_i(const Viewer3D::Point2D& point, const Line line)
{
    Vector2D vectX(line.end - line.start);
    vectX *= 0.95;
    Vector2D vectY(vectX.y, -vectX.x);
    vectY.Normalize();
    vectY *= HIT_TOLERANCE;
    Point2D rect[4];
    rect[0].x = line.start.x - vectY.x;
    rect[0].y = line.start.y - vectY.y;
    rect[1].x = line.start.x - vectY.x + vectX.x;
    rect[1].y = line.start.y - vectY.y + vectX.y;
    rect[2].x = line.start.x + vectY.x + vectX.x;
    rect[2].y = line.start.y + vectY.y + vectX.y;
    rect[3].x = line.start.x + vectY.x;
    rect[3].y = line.start.y + vectY.x;
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

void TpsBeamRenderStrategy::AdjustMpr_i(Point3D* vertices) const
{
    Vector3D orignalSegA(vertices[1] - vertices[0]);
    Vector3D orignalSegB(vertices[2] - vertices[3]);
    double segmentLen = orignalSegA.Magnitude();
    Vector3D orignalSegNormA = orignalSegA;
    orignalSegNormA.Normalize();
    Vector3D orignalSegNormB = orignalSegB;
    orignalSegNormB.Normalize();
    double lenMore;
    if (mWidth / mHeight >= 1)
    {
        lenMore = (segmentLen / mHeight) * ((mWidth - mHeight) / 2.0);
    }
    else
    {
        lenMore = (segmentLen / mWidth) * ((mHeight - mWidth) / 2.0);
    }
    vertices[0] -= lenMore * orignalSegNormA;
    vertices[1] += lenMore * orignalSegNormA;
    vertices[3] -= lenMore * orignalSegNormB;
    vertices[2] += lenMore * orignalSegNormB;
}

//////////////////////////////////////////////////////////////////////////
///目前这里还遗留一个问题：
///缩放后，adjustMPR会部分超越source点，这时MPR投影到ISO平面失真，待以后修复
//////////////////////////////////////////////////////////////////////////
bool TpsBeamRenderStrategy::IfMprIntersectsWithBeamCombinationFields_i(
    const BeamGoPtr& beam, const Viewer3D::Matrix4x4& world2Beam, double sad) const
{
    Point3D vertices[4];
    mMprGO->GetVertices(vertices);
    for (int i = 0; i < 4; ++i)
    {
        vertices[i] = world2Beam.Transform(vertices[i]);
    }
    AdjustMpr_i(vertices);
    //如果整个MPR面位于SourceToIsoCenter的反面，则不相交
    Point3D source(0, 0, 0);
    Vector3D vecS2A(0, 0, -sad);
    bool mprBehindTheSource = true;
    for (int i = 0; i < 4; ++i)
    {
        if (vecS2A.DotProduct(vertices[i] - source) > 0.0)
        {
            mprBehindTheSource = false;
            break;
        }
    }
    if (mprBehindTheSource) return false;
    //映射至ISO平面
    double alpha;
    Vector3D vec;
    std::vector<Point2D> verticesVec;
    for (int i = 0; i < 4; ++i)
    {
        vec = vertices[i] - source;
        alpha = fabs(vec.z) < 1e-6 ? DOUBLE_BIG_ENOUGH : -sad / vec.z;
        vertices[i] += vec * (alpha - 1.0);
        Point2D verticeXY(vertices[i].x, vertices[i].y);
        verticesVec.push_back(verticeXY);
    }

    //我们将CombinationFields切割成Beam坐标系下的矩形，以方便与MPR做相交判断
    auto rects = beam->GetCombinationFieldRects();
    //全闭合野时，中心线一直显示
    if (rects.size() == 0)
    {
        return true;
    }
    std::vector<Point2D> rectVec;
    for (int i = 0; i < rects.size(); ++i)
    {
        rectVec.push_back(Point2D(rects[i].boundings[0], rects[i].boundings[2]));
        rectVec.push_back(Point2D(rects[i].boundings[0], rects[i].boundings[3]));
        rectVec.push_back(Point2D(rects[i].boundings[1], rects[i].boundings[3]));
        rectVec.push_back(Point2D(rects[i].boundings[1], rects[i].boundings[2]));
        if (TpsSat::Sat(verticesVec, rectVec))
        {
            return true;
        }
        rectVec.clear();
    }
    return false;
}
TPS_END_NAMESPACE
