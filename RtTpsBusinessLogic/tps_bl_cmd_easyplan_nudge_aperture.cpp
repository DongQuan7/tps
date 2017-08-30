////////////////////////////////////////////////////////////////////////////
/////  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
/////  All rights reserved.
/////
/////  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
/////
/////  \file   tps_epca_nudge_aperture_cmd.cpp 
/////  \brief   nudge mlc and block
/////
/////  \version 1.0
/////  \date  2015/04/09  
/////  \
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
//#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_nudge_aperture.h "
//
//#include "alg/RtAlgDataLayer/rt_alg_dl_object_beam.h"
//#include "alg/RtAlgDataLayer/rt_alg_dl_object_block.h"
//
//#include "RtTpsDataAccess/tps_da_beammanager.h"
//#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
//#include "RtTpsRenderController/tps_rc_renderproxy.h"
////#include "RtTpsFramework/tps_rc_renderproxybase.h"
////#include "RtTpsRenderController/tps_rc_easyplanrenderproxy.h"
//#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
//#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
//#include "RtTpsDataAccess/tps_da_idatarepository.h"
////#include "RtTpsCommonControllerApp/tps_comca_commoncontrollerapp.h"
////#include "RtTpsCommonControllerApp/tps_comca_idatasaver.h"
//#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
//#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
//#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
//#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"
//#include "tps_logger.h"
//
//#define SAMPLE_INTERCEPT 0.25
//#define DIFFUSE_PERCENTAGE 1.0
//using namespace Mcsf::MedViewer3D;
//TPS_BEGIN_NAMESPACE
//
//    Point2D TpsNudgeApertureEasyPlanCmd::mPrePoint(0.0, 0.0);
//NUDGE_STATE TpsNudgeApertureEasyPlanCmd::mEditState = _IDLE;
//
//TpsNudgeApertureEasyPlanCmd::TpsNudgeApertureEasyPlanCmd(const CertifiedMlcEditInfo &certMlc )
//    : mCertMlc(certMlc), mHandlePoint(certMlc.mX,certMlc.mY){
//}
//
//TpsNudgeApertureEasyPlanCmd::~TpsNudgeApertureEasyPlanCmd() {
//
//}
//
//int TpsNudgeApertureEasyPlanCmd::PreExecute() {
//    return TPS_ER_SUCCESS;
//}
//
//int TpsNudgeApertureEasyPlanCmd::Execute() {
//
//    TpsBeamManager* beamMgr = mDataRepository->GetBeamManager();
//    auto beam = beamMgr->GetBeam(mCertMlc.mBeamUid);
//    if(beam == nullptr) {
//        return TPS_ER_FAILURE;
//    }
//    auto aperturetype = beam->get_aperturetype();
//
//    if (mRenderProxy == nullptr) {
//        return TPS_ER_FAILURE;
//    }
//
//    mRenderProxy->EasyPlanShowNudgeCircle(mCertMlc.mX, mCertMlc.mY, mCertMlc.mRadius, mCertMlc.mBeamUid);
//
//    switch(aperturetype) {
//    case RTFWK::Aperture_Block:
//        NudgeBlock_i(beam);
//        break;
//    case RTFWK::Mlc:
//        mRenderProxy->EasyPlanMlcNudge(mCertMlc);
//        break;
//    default:
//        break;
//    }
//    return TPS_ER_SUCCESS;
//}
//
//int TpsNudgeApertureEasyPlanCmd::PostExecute() {
//    return TPS_ER_SUCCESS;
//}
//
//TpsNudgeApertureEasyPlanCmd * TpsNudgeApertureEasyPlanCmd::Clone() {
//    return nullptr;
//}
//
//void TpsNudgeApertureEasyPlanCmd::NudgeBlock_i(RtBeam *beam) {
//    mHandlePoint.x -= mCertMlc.mWidth/2;
//    mHandlePoint.y -= mCertMlc.mHeight/2;
//
//    auto segments = beam->get_beamsegments();
//    //get active segment. By default, there is only one segment in easy beam
//    if (segments.size() == 0)
//    {
//        TPS_LOG_DEV_INFO<<"No segment in current beam! Beam uid is: " << beam->get_uid();
//        return;
//    }
//    auto shape = segments[0]->get_startmlcshape();
//
//    if (shape == nullptr)
//    {
//        TPS_LOG_DEV_INFO<<"MLC's shape is null! Segment uid is:" << segments[0]->get_uid();
//        return;
//    }
//
//    double jawPos;
//    double board[4];
//
//    // set board as the size of jaw 
//    jawPos = shape->get_xlowerjawpos();
//    board[0] = jawPos;
//
//    jawPos = shape->get_xupperjawpos();
//    board[1] = jawPos;
//
//    jawPos = shape->get_ylowerjawpos();
//    board[2] = jawPos;
//
//    jawPos = shape->get_yupperjawpos();
//    board[3] = jawPos;
//
//    int width = board[1] - board[0];
//    int height = board[3] - board[2];
//    unsigned char *blockMask = nullptr;
//    TpsDisplayStateManager* pDisplayStateManager = mDataRepository->GetDisplayStateManager();
//    blockMask = pDisplayStateManager->GetBlockMaskCache(width, height);
//
//    bool bRes = false;
//    switch (mCertMlc.mRealActionType){
//    case NUDGE_BEGIN:
//        bRes &= PreNudgeBlock_i(beam, blockMask, width, height);
//        break;
//    case NUDGING:
//        bRes &= ExecuteNudgeBlock_i(beam, blockMask,width, height);
//        break;
//    case NUDGE_OVER:
//        pDisplayStateManager->ClearBlockCache();
//        if (false/*!TpsCommonControllerApp::GetInstance()->GetDataSaver()->SaveBeamToTMS(beam->GetUID())*/) {
//            TPS_LOG_DEV_ERROR<<"Failed to save beam to tms!";
//            return;
//        }
//        break;
//    }
//
//}
//
//bool TpsNudgeApertureEasyPlanCmd::PreNudgeBlock_i( RtBeam* beam, 
//    unsigned char *&blockMask,
//    int width,
//    int height) {
//
//        if (blockMask == nullptr || width <= 0 || height <= 0) {
//            TPS_LOG_DEV_ERROR<<"The mask cache is invalid.";
//            return false;
//        }
//
//        mPrePoint = mHandlePoint;
//        RtBlock *block = beam->get_aperture_block();
//        if (block == nullptr) {
//            TPS_LOG_DEV_ERROR<<"Failed to get block from beam.";
//            return false;
//        }
//        std::vector<db_Point2f> blockContour = block->get_points();
//
//        std::vector<Mcsf::Point2f> newContour;
//        newContour.reserve(blockContour.size());
//        Mcsf::Point2f newPoint;
//        for (auto it = blockContour.begin(); it != blockContour.end(); ++it) {
//            newPoint.SetX((*it).x + width / 2);
//            newPoint.SetY((*it).y + height / 2);
//            newContour.push_back(newPoint);
//        }
//
//        std::vector<std::vector<Mcsf::Point2f>> vvContour;
//        vvContour.push_back(newContour);
//
//        if (TPS_ER_SUCCESS != SubVolumeHelperAPI::GetMaskFromContours(vvContour, width, height, blockMask)) {
//            TPS_LOG_DEV_ERROR<<"Failed to get mask from contour.";
//            return false;
//        }
//
//        //#ifdef _DEBUG
//        //    unsigned char* bmpBuffer = new unsigned char[width * height * 4];
//        //    for (int j=0; j < width * height; ++j){
//        //        bmpBuffer[4 * j] = blockMask[j] * 255;
//        //        bmpBuffer[4 * j + 1] = blockMask[j] * 255;
//        //        bmpBuffer[4 * j + 2] = blockMask[j] * 255;
//        //        bmpBuffer[4 * j + 3] = 255;
//        //    }
//        //    char fileName[256];
//        //    sprintf_s(fileName, "d:\\temp\\block_data_before.bmp");
//        //    SubVolumeHelperAPI::SaveToTrueColorBitmap(fileName, bmpBuffer, height, width);
//        //    delete []bmpBuffer;
//        //#endif
//
//        return ExecuteNudgeBlock_i(beam,blockMask,width, height);
//}
//
//bool TpsNudgeApertureEasyPlanCmd::ExecuteNudgeBlock_i( RtBeam* beam, 
//    unsigned char * blockMask,
//    unsigned int width,
//    unsigned int height ) {
//        if (blockMask == nullptr || width <= 0 || height <= 0) {
//            TPS_LOG_DEV_ERROR<<"The mask cache is invalid.";
//            return false;
//        }
//
//        if (mCertMlc.mRadius > 0) {
//            //sample from pre-point to current point
//            SampleNudgePoints_i();
//        }
//        else {
//            TPS_LOG_DEV_ERROR<<"The radius of nudge is less or equal than 0.";
//            return false;
//        }
//
//        //check edit state if edit not started
//        bool isFirstEdit = mEditState == _IDLE;
//        if (isFirstEdit) CheckEditState_i(blockMask, width, height);
//        if (mEditState == _IDLE) return true;
//        //change mask
//        ChangeMask_i(blockMask, width, height);
//        //¸üÐÂpreµã
//        mPrePoint = mHandlePoint;
//        return UpdateBlockContour_i(beam, blockMask, width, height);
//}
//
//void TpsNudgeApertureEasyPlanCmd::SampleNudgePoints_i() {
//    mNudgePointList.clear();
//
//    Vector2D vec = mHandlePoint - mPrePoint;
//
//    double longitude = sqrt(vec.x * vec.x + vec.y * vec.y);
//
//    //double tangent = vec.y / vec.x;
//    vec.Normalize();
//
//    double samplesCount = floor(longitude / SAMPLE_INTERCEPT / mCertMlc.mRadius);
//
//    //mNudgePointList.reserve(samplesCount + 1);
//    Point2D ptTemp;
//    for (int i = 0; i < (int)samplesCount; ++i) {
//        ptTemp = mPrePoint + i * vec * SAMPLE_INTERCEPT * mCertMlc.mRadius;
//        mNudgePointList.push_back(ptTemp);
//    }
//
//    mNudgePointList.push_back(mHandlePoint);
//}
//
//void TpsNudgeApertureEasyPlanCmd::CheckEditState_i( unsigned char * blockMask,
//    unsigned int width,
//    unsigned int height ) {
//
//        if (blockMask == nullptr || width <= 0 || height <= 0) {
//            TPS_LOG_DEV_ERROR<<"The mask cache is invalid.";
//            return;
//        }
//        if (mNudgePointList.empty()) return;
//
//        Mcsf::MedViewer3D::Point2D currtPt = mNudgePointList.back();
//        currtPt.x += width / 2;
//        currtPt.y += height / 2;
//        int rect[4] = {
//            TPS_CLAMP(int(currtPt.x - mCertMlc.mRadius - 1), 0, width - 1), //left
//            TPS_CLAMP(int(currtPt.x + mCertMlc.mRadius + 1), 0, width - 1), //right
//            TPS_CLAMP(int(currtPt.y - mCertMlc.mRadius - 1), 0, height - 1),//bottom
//            TPS_CLAMP(int(currtPt.y + mCertMlc.mRadius + 1), 0, height - 1) //top
//        };
//
//        int mask = 0, unMask = 0;
//        bool isOutofImage = true;
//        for (int i = rect[0]; i < rect[1]; ++i){
//            for (int j = rect[2]; j < rect[3]; ++j){
//                isOutofImage = false;
//                if (abs(i - currtPt.x) > mCertMlc.mRadius || 
//                    abs(j - currtPt.y) > mCertMlc.mRadius){
//                        continue;
//                }
//                if (blockMask[j * width + i] == 0){
//                    unMask++;
//                }
//                else{
//                    mask++;
//                }
//            }
//        }
//        if (mask != 0){
//            mEditState = (mask < unMask) ? DEMASK : NUDGE_MASK;
//        }
//        else{
//            //if no mask in circle range, diffuse the circle & check if need to create new contour
//            int diffusedRect[4] = {
//                TPS_CLAMP(int(rect[0] - mCertMlc.mRadius * DIFFUSE_PERCENTAGE - 1), 0, width - 1), //left
//                TPS_CLAMP(int(rect[1] + mCertMlc.mRadius * DIFFUSE_PERCENTAGE + 1), 0, width - 1), //right
//                TPS_CLAMP(int(rect[2] - mCertMlc.mRadius * DIFFUSE_PERCENTAGE - 1), 0, height - 1),//bottom
//                TPS_CLAMP(int(rect[3] + mCertMlc.mRadius * DIFFUSE_PERCENTAGE + 1), 0, height - 1) //top
//            };
//            double radius[2] = { mCertMlc.mRadius * (1 + DIFFUSE_PERCENTAGE), 
//                mCertMlc.mRadius * (1 + DIFFUSE_PERCENTAGE) };
//            for (int i = diffusedRect[0]; i < diffusedRect[1]; ++i){
//                for (int j = diffusedRect[2]; j < diffusedRect[3]; ++j){
//                    if ((i - currtPt.x) / radius[0] * 
//                        (i - currtPt.x) / radius[0] + 
//                        (j - currtPt.y) / radius[1] * 
//                        (j - currtPt.y) / radius[1] > 1){
//                            continue;
//                    }
//                    mask |= blockMask[j * width + i];
//                }
//            }
//            mEditState = (mask == 0) ? NUDGE_MASK : DEMASK;
//        }
//}
//
//void TpsNudgeApertureEasyPlanCmd::ChangeMask_i( unsigned char * blockMask,
//    unsigned int width,
//    unsigned int height ) {
//        Point2D handlePoint;
//        int rect[4];
//
//        if (blockMask == nullptr || width <= 0 || height <= 0) {
//            TPS_LOG_DEV_ERROR<<"The mask cache is invalid.";
//            return;
//        }
//        double deltaX, deltaY;
//
//        for (int index=0; index < mNudgePointList.size(); ++index){
//            handlePoint.x = mNudgePointList[index].x + width / 2;
//            handlePoint.y = mNudgePointList[index].y + height / 2;
//
//            rect[0] = TPS_CLAMP(int(handlePoint.x - mCertMlc.mRadius - 1), 0, width - 1); //left
//            rect[1] = TPS_CLAMP(int(handlePoint.x + mCertMlc.mRadius + 1), 0, width - 1); //right
//            rect[2] = TPS_CLAMP(int(handlePoint.y - mCertMlc.mRadius - 1), 0, height - 1);//bottom
//            rect[3] = TPS_CLAMP(int(handlePoint.y + mCertMlc.mRadius + 1), 0, height - 1);//top
//
//            for (int j = rect[2]; j < rect[3]; ++j) {
//                for (int i = rect[0]; i < rect[1]; ++i) {
//                    deltaX = i - handlePoint.x;
//                    deltaY = j - handlePoint.y;
//                    if ((deltaX / mCertMlc.mRadius * 
//                        deltaX / mCertMlc.mRadius + 
//                        deltaY / mCertMlc.mRadius * 
//                        deltaY / mCertMlc.mRadius) > 1){
//                            continue;
//                    }
//
//                    blockMask[j * width + i] = (mEditState == NUDGE_MASK) ? 1 : 0;
//                }
//            }
//        }
//
//        //#ifdef _DEBUG
//        //    unsigned char* bmpBuffer = new unsigned char[width * height * 4];
//        //    for (int j=0; j < width * height; ++j){
//        //        bmpBuffer[4 * j] = blockMask[j] * 255;
//        //        bmpBuffer[4 * j + 1] = blockMask[j] * 255;
//        //        bmpBuffer[4 * j + 2] = blockMask[j] * 255;
//        //        bmpBuffer[4 * j + 3] = 255;
//        //    }
//        //    char fileName[256];
//        //    sprintf_s(fileName, "d:\\temp\\block_data_after.bmp");
//        //    SubVolumeHelperAPI::SaveToTrueColorBitmap(fileName, bmpBuffer, height, width);
//        //    delete []bmpBuffer;
//        //#endif
//}
//
//bool TpsNudgeApertureEasyPlanCmd::UpdateBlockContour_i(RtBeam* beam,
//    unsigned char * blockMask,
//    unsigned int width,
//    unsigned int height ) {
//
//        if (blockMask == nullptr || width <= 0 || height <= 0) {
//            TPS_LOG_DEV_ERROR<<"The mask cache is invalid.";
//            return false;
//        }
//
//        std::vector<std::vector<Mcsf::Point2f>> newBlockContour;
//        if (TPS_ER_SUCCESS != SubVolumeHelperAPI::GetContoursFromMask(blockMask, width,
//            height, newBlockContour)) {
//                TPS_LOG_DEV_ERROR<<"The new mask cannot generate contour.";
//                return false;
//        }
//
//        if (newBlockContour.empty()) {
//            TPS_LOG_DEV_ERROR<<"The new mask cannot generate contour.";
//            return false;
//        }
//
//        std::vector<db_Point2f> blockData;
//        auto block = beam->get_aperture_block();
//
//        db_Point2f tmsPoint;
//        for (auto it = newBlockContour[0].begin(); it != newBlockContour[0].end(); ++it) {
//            tmsPoint.x = it->GetX() - width / 2;
//            tmsPoint.y = it->GetY() - height / 2;
//            blockData.push_back(tmsPoint);
//        }
//        // deep copy here
//        block->set_points(blockData);
//
//        mRenderProxy->EasyPlanUpdateBlock(mCertMlc);
//
//        //DisplayStateManager::GetInstance()->SetBlockMaskCache(width, height, blockMask);
//        return true;
//}
//
//TPS_END_NAMESPACE