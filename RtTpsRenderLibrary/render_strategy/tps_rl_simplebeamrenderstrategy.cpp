//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_rl_simplebeamrenderstrategy.cpp
///  \brief  
///  \
///  class  TpsSimpleBeamRenderStrategy
///  \version 1.0
///  \date  2014/11/25
///  \
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_simplebeamrenderstrategy.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"
//#include "McsfMedViewer3DArithmetic/arithmetic.h"

#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_simplebeamgraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_simplesegmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamtemplategraphicobject.h"
#include "RtTpsFramework/tps_fw_common_enums.h"


TPS_BEGIN_NAMESPACE
TpsSimpleBeamRenderStrategy::TpsSimpleBeamRenderStrategy() {

}

TpsSimpleBeamRenderStrategy::~TpsSimpleBeamRenderStrategy() {

}

void TpsSimpleBeamRenderStrategy::Initialize() {
}

int TpsSimpleBeamRenderStrategy::CacheChanges() {
    if (mMprGO == nullptr || mVolumeGO == nullptr || mBeamTemplateGO == nullptr){
        return -1;
    }

    if (!mVolumeGO->IsValid()){
        return -1;
    }

    if (mMprGO->IsDirty() || mVolumeGO->IsDirty() || mBeamTemplateGO->IsDirty()){
        mDirty = true;
    }
    return 0;
}

void TpsSimpleBeamRenderStrategy::Resize(int iWidth, int iHeight) {
    mWidth = iWidth;
    mHeight = iHeight;
}

void TpsSimpleBeamRenderStrategy::Render_i() 
{
    return;//useless 

    Scopped_SetValue<bool, false> setFalse(&mDirty);
    if (mMprGO == nullptr || mVolumeGO == nullptr || mBeamTemplateGO == nullptr){
        return;
    }

    CalculateWorld2ViewTM_i();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
    glOrtho(0, mWidth, 0, mHeight,-1.0f, 10.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::shared_ptr<TpsSimpleBeamGraphicObject>> sbgoCollection;
    mBeamTemplateGO->GetBeamGoCollection(sbgoCollection);

    for(auto it = sbgoCollection.begin(); it != sbgoCollection.end(); ++it){
        Viewer3D::Matrix4x4 matPatient2World = mVolumeGO->GetWorld2Patient().Inverse();
        Viewer3D::Matrix4x4 matBeam2Pat, matBeam2View;
        Viewer3D::Point3D ptSource, ptEnd, ptIOS;
        TpsSimpleBeamGraphicObject::BEAM_TYPE beam_type = (*it)->GetBeamType();

        (*it)->GetBeamToPatMatrix(&matBeam2Pat);
        matBeam2View = mWorld2View;
        matBeam2View.Append(matPatient2World);
        matBeam2View.Append(matBeam2Pat);
        ptIOS = matBeam2View.Transform(Viewer3D::Point3D(0,0,-1000));

        BeamLine beamLine = ComputeIntersectPairList_i(matBeam2View);
        glColor3f(0.0f, 0.9f, 0.0f);

        for(auto it_bl = beamLine.begin(); it_bl != beamLine.end(); ++ it_bl) {
            glBegin(GL_LINES);
            glVertex3d(it_bl->first.x,it_bl->first.y,it_bl->first.z);
            glVertex3d(it_bl->second.x,it_bl->second.y, it_bl->second.z);
            glEnd();
        }

        glLineWidth(1.0f);
        glLineStipple(1,0x0F0F);
        ptSource = matBeam2View.Transform(Viewer3D::Point3D(0, 0, 0));
        ptEnd = matBeam2View.Transform(Viewer3D::Point3D(0, 0, -3000));

        glEnable(GL_LINE_STIPPLE);
        glBegin(GL_LINES);
        glVertex3d(ptSource.x, ptSource.y, ptSource.z);
        glVertex3d(ptEnd.x, ptEnd.y, ptEnd.z);
        glEnd();
        glDisable(GL_LINE_STIPPLE);

        // ==========================================================================================
        // draw arc beam segments
        // ==========================================================================================
        if(beam_type == TpsSimpleBeamGraphicObject::ARC){
            auto segmentList = (*it)->GetSegmentGoList();
            int size = segmentList.size();
            if(size <= 0) return;
            bool isClockWise  = (*it)->GetGantryIsClockWise();

            //render first segment
            auto firstSegment = segmentList[0];
            firstSegment->GetStartCollimatorToPatMatrix(&matBeam2Pat);
            matBeam2View = mWorld2View;
            matBeam2View.Append(matPatient2World);
            matBeam2View.Append(matBeam2Pat);
            ptSource = matBeam2View.Transform(Viewer3D::Point3D(0,0,0));
            Viewer3D::Point3D ptArcBegin = matBeam2View.Transform(Viewer3D::Point3D(0,0,-3000));

            glColor4f(1.0f,1.0f,0.0f,1.0f);
            glEnable(GL_LINE_STIPPLE);
            glBegin(GL_LINES);
            glVertex3d(ptSource.x, ptSource.y, ptSource.z);
            glVertex3d(ptArcBegin.x, ptArcBegin.y, ptArcBegin.z);
            glEnd();
            glDisable(GL_LINE_STIPPLE);
            float gantryAngleFirst = firstSegment->GetStartGantryAngle();

            //last segment
            auto lastSegment = segmentList[size - 1];
            lastSegment->GetStartCollimatorToPatMatrix(&matBeam2Pat);

            matBeam2View = mWorld2View;
            matBeam2View.Append(matPatient2World);
            matBeam2View.Append(matBeam2Pat);

            ptSource = matBeam2View.Transform(Viewer3D::Point3D(0,0,0));
            Viewer3D::Point3D ptArcEnd = matBeam2View.Transform(Viewer3D::Point3D(0,0,-3000));

            //draw the EndAngle of the end segment
            Viewer3D::Point3D ptSourceForRatate(
                (ptSource - ptIOS).x,
                (ptSource - ptIOS).y,
                (ptSource - ptIOS).z);
            Viewer3D::Point3D ptArcEndForRatate(
                (ptArcEnd - ptIOS).x,
                (ptArcEnd - ptIOS).y,
                (ptArcEnd - ptIOS).z);

            const double twoDegree = 0.03489; // 2 * PI/180
            Mcsf::MedViewer3D::Matrix4x4 rotationMatrix;
            rotationMatrix.SetIdintity();
            
            if (isClockWise){
                rotationMatrix.m[0][0] = cos(twoDegree);
                rotationMatrix.m[0][1] = -sin(twoDegree);
                rotationMatrix.m[1][0] = sin(twoDegree);
                rotationMatrix.m[1][1] = cos(twoDegree);
            }
            else{
                rotationMatrix.m[0][0] = cos(PI - twoDegree);
                rotationMatrix.m[0][1] = -sin(PI - twoDegree);
                rotationMatrix.m[1][0] = sin(PI - twoDegree);
                rotationMatrix.m[1][1] = cos(PI - twoDegree);
            }
            ptSourceForRatate = rotationMatrix.Transform(ptSourceForRatate) + ptIOS;
            ptArcEndForRatate = rotationMatrix.Transform(ptArcEndForRatate) + ptIOS;

            //float gantryAngleLast = lastSegment->GetGantryAngle();
            glEnable(GL_LINE_STIPPLE);
            glBegin(GL_LINES);
                glVertex3d(ptSourceForRatate.x,ptSourceForRatate.y,ptSourceForRatate.z);
                glVertex3d(ptArcEndForRatate.x,ptArcEndForRatate.y,ptArcEndForRatate.z);
            glEnd();
            glDisable(GL_LINE_STIPPLE);
            ptIOS = matBeam2View.Transform(Viewer3D::Point3D(0,0,-1000));

            //draw arc
            int minWindowSize = mWidth > mHeight ? mHeight : mWidth;
            float radius = 0.4* minWindowSize;

            ptArcBegin.z = 0.0;
            ptArcEnd.z = 0.0;
            ptIOS.z = 0.0;

            Viewer3D::Vector3D vecFirst = ptArcBegin - ptIOS;
            Viewer3D::Vector3D vecLast = ptArcEnd - ptIOS;

            float angleStart = (float)vecFirst.AngleBetween(Viewer3D::Vector3D(1,0,0));
            if(vecFirst.y < 0) {
                angleStart = 2 * PI  - angleStart;
            }

            bool ori = true;
            glBegin(GL_LINE_STRIP);
            for(int i = 0; i < size + 1  ; ++i) {
                float gantryAngleTemp = twoDegree * i;
                if(ori){
                    float angleTemp = angleStart - gantryAngleTemp;
                    if (!isClockWise){
                        angleTemp = angleStart + gantryAngleTemp;
                    }
                    float x = ptIOS.x + radius * cos(angleTemp);
                    float y = ptIOS.y + radius * sin(angleTemp);
                    glVertex3d(x,y,0);
                }
            }
            glEnd();

            //short lines
            glBegin(GL_LINES);
            for(int i = 0; i < size ; ++ i) {
                auto segmentTemp = segmentList[i];
                float gantryAngleTemp = (segmentTemp->GetStartGantryAngle() - gantryAngleFirst) /** scale*/ / 180 * PI;
                float angleTemp = angleStart - gantryAngleTemp;
                float x = ptIOS.x + radius * cos(angleTemp);
                float y = ptIOS.y + radius * sin(angleTemp);
                glVertex3d(x,y,0);
                glVertex3d(0.9 *x + 0.1*ptIOS.x, 0.9 * y + 0.1 * ptIOS.y ,0);
            }
            glEnd();
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////

    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

std::vector<std::pair<Viewer3D::Point3D,Viewer3D::Point3D>> TpsSimpleBeamRenderStrategy::ComputeIntersectPairList_i(const Viewer3D::Matrix4x4 &matB2V){
    using namespace Mcsf::MedViewer3D;
    std::pair<Point3D, Point3D> ptPairTmp;
    std::vector<std::pair<Point3D,Point3D>> ptPairTmpList;

    Point3D beamSource(0.0, 0.0, 0.0);
    Point3D ptBegin = matB2V.Transform(beamSource);

    std::vector<Point3D> outlinePtList;
    mBeamTemplateGO->GetDefaultSegmentOutLine(&outlinePtList);
    if (outlinePtList.empty()){
        TPS_LOG_DEV_WARNING<<"The outline point list is empty.";
        return std::vector<std::pair<Viewer3D::Point3D,Viewer3D::Point3D>>();
    }

    for(auto it = outlinePtList.begin(); (it+1)!=outlinePtList.end(); ++it) {
        //ptPairTmp.first = *itr;
        //ptPairTmp.second = *(itr + 1);
        ptPairTmpList.push_back(std::make_pair(*it, *(it + 1)));
    }
    ptPairTmpList.push_back(std::make_pair(outlinePtList.back(),outlinePtList.front()));

    ////////////////////////////////////////////////////////////////////////////////////////
    // compute intersect points list, each is pair<pt1, pt2>
    std::vector<std::pair<Point3D, Point3D>> intersectPairList;
    Viewer3D::Point3D ptEnd1, ptEnd2, interSect1, interSect2, interSect3;
    for(auto it = ptPairTmpList.begin(); it != ptPairTmpList.end(); ++it) {
        ptEnd1 = matB2V.Transform(it->first * 3.0);
        ptEnd2 = matB2V.Transform(it->second * 3.0);
        int hit1 = ComputeIntersect_i(ptBegin, ptEnd1, &interSect1);
        int hit2 = ComputeIntersect_i(ptEnd1, ptEnd2, &interSect2);
        int hit3 = ComputeIntersect_i(ptBegin, ptEnd2, &interSect3);
        int hit = hit1 + hit2 + hit3;
        if(hit1 == -1) {
            intersectPairList.push_back(std::make_pair(ptBegin, ptEnd1));
        }
        if(hit2 == -1) {
            intersectPairList.push_back(std::make_pair(ptEnd1, ptEnd2));
        }
        if(hit3 == -1) {
            intersectPairList.push_back(std::make_pair(ptBegin, ptEnd2));
        }
        if(hit == 0 || hit == 1) {
            continue;
        }
        if(hit == 2) {
            if(hit1 == 0) {
                intersectPairList.push_back(std::make_pair(interSect2, interSect3));
            }
            if(hit2 == 0) {
                intersectPairList.push_back(std::make_pair(interSect1, interSect3));
            }
            if(hit3 == 0) {
                intersectPairList.push_back(std::make_pair(interSect1, interSect2));
            }
        }
        if(hit == 3) {
            intersectPairList.push_back(std::make_pair(ptBegin, interSect2));
        }
    }
    return intersectPairList;
}


//void TpsSimpleBeamRenderStrategy::SetBeamCollGraphicObject(std::shared_ptr<TpsBeamGraphicObjectCollection> beamCollGo) {
//    mBeamGoCollection = beamCollGo;
//}

void TpsSimpleBeamRenderStrategy::SetMprGO(std::shared_ptr<MPRGraphicObject> mprGo) {
    if (mMprGO != mprGo){
        mMprGO = mprGo;
        mDirty = true;
    }
}

void TpsSimpleBeamRenderStrategy::SetVolumeGO(std::shared_ptr<Viewer3D::GraphicObjVolume> volumeGo) {
    if (mVolumeGO != volumeGo){
        mVolumeGO = volumeGo;
        mDirty = true;
    }
}

void TpsSimpleBeamRenderStrategy::SetBeamTemplateGO(std::shared_ptr<TpsBeamTemplateGraphicObject> btGo) {
    if (mBeamTemplateGO != btGo){
        mBeamTemplateGO = btGo;
        mDirty = true;
    }
}

int TpsSimpleBeamRenderStrategy::ComputeIntersect_i(const Viewer3D::Point3D &pt1,
    const Viewer3D::Point3D &pt2, Viewer3D::Point3D *ptRes){
        Viewer3D::Vector3D vec = pt2 - pt1;
        if(std::abs(vec.z) <= 10e-6) {
            if(std::abs(pt1.z) <= 10e-6) 
                return -1;
            else 
                return 0;
        }
        double alpha = (0 - pt1.z) / vec.z;
        if(alpha >= -10e-6 && alpha <= 1.0) {
            ptRes->x = pt1.x + alpha * vec.x;
            ptRes->y = pt1.y + alpha * vec.y;
            ptRes->z = pt1.z + alpha * vec.z;
            return 1;
        }
        else
            return 0;
}

void TpsSimpleBeamRenderStrategy::CalculateWorld2ViewTM_i(){

    Viewer3D::Point3D aVertices[4];
    mMprGO->GetVertices(aVertices);
    aVertices[0].z = - aVertices[0].z;
    Viewer3D::Vector3D vecY = aVertices[3] - aVertices[0];
    vecY.Normalize();
    Viewer3D::Vector3D vecZ; 
    mMprGO->GetNormal(&vecZ); vecZ *= -1;
    Viewer3D::Vector3D vecX = vecZ.CrossProduct(vecY);
    vecY = vecX.CrossProduct(vecZ);

    double scale, ratio;
    double planeWidth = (aVertices[1] - aVertices[0]).Magnitude();
    double planeHeight = (aVertices[3] - aVertices[0]).Magnitude();
    Viewer3D::Point3D ptOrigin = aVertices[0];
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

    Viewer3D::Vector3D temp(ptOrigin.x, ptOrigin.y, ptOrigin.z);
    mWorld2View = Viewer3D::Matrix4x4(vecX.x, vecY.x, vecZ.x, 0.0, 
        vecX.y, vecY.y, vecZ.y, 0.0, 
        vecX.z, vecY.z, vecZ.z, 0.0, 
        -1 * temp.DotProduct(vecX), 
        -1 * temp.DotProduct(vecY), 
        -1 * temp.DotProduct(vecZ), 1.0);

    mWorld2View.Prepend(MakeScale(Viewer3D::Vector3D(scale, scale, scale)));
}

TPS_END_NAMESPACE
