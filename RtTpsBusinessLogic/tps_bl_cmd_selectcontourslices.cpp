//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  kai.zuo kai.zuo@united-imaging.com
///
///  \file    tps_cca_selectcontourslices_cmd.cpp
///  \brief   SelectContourSlices
///
///  \version 1.0
///  \date    06. 12, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_selectcontourslices.h"
//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
//TPS
#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_image3d.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"

TPS_BEGIN_NAMESPACE

//Mcsf::Point2f* SelectContourSlicesCmd::mFirstPoint = new Mcsf::Point2f();

SelectContourSlicesCmd::SelectContourSlicesCmd(CertifiedSelectContourSlices certifieContourSlice) 
    : mCertifiedSelectContourSlices(certifieContourSlice),mFirstPoint(new Mcsf::Point2f())
{
}

SelectContourSlicesCmd::~SelectContourSlicesCmd(void)
{
    DEL_PTR(mFirstPoint);
}

int SelectContourSlicesCmd::Execute() {
    mContourDeleted = false;
    if (mRenderProxy == nullptr){
        TPS_LOG_DEV_ERROR<<"The contouring render proxy is empty.";
        return TPS_ER_FAILURE;
    }

    Mcsf::MedViewer3D::Point2D currentPoint;
    int actionType;
    std::string vOIUID;
    std::string imageUID;
    currentPoint.x = mCertifiedSelectContourSlices.mX;
    currentPoint.y = mCertifiedSelectContourSlices.mY;
    vOIUID = mCertifiedSelectContourSlices.mVOIUid;
    WINDOW_TYPE windowType = (WINDOW_TYPE)mCertifiedSelectContourSlices.mWindowType;
    std::string windowUid = mCertifiedSelectContourSlices.mWindowUid;
    Mcsf::Point2f lastPoint;
    Mcsf::Point3f firstPointInPat;
    Mcsf::Point3f lastPointInPat;
    if (mCertifiedSelectContourSlices.mIsEditingStarted){
        switch(mCertifiedSelectContourSlices.mActionType){
        case MOVE:
            mCertifiedSelectContourSlices.mRealActionType = UPDATE_CANDIDATE_POINT;
            break;
        case DRAG:
        case DOWN:
            mCertifiedSelectContourSlices.mRealActionType = ADD_NEW_POINT;
            break;
        case DOUBLE_CLICK_:
            mCertifiedSelectContourSlices.mRealActionType = CLOSE_CONTOUR;
            break;
        default:
            mCertifiedSelectContourSlices.mRealActionType = SMART_CONTOUR_INVALID;
        }
    }
    else if (mCertifiedSelectContourSlices.mActionType == DOWN){
        mCertifiedSelectContourSlices.mRealActionType = CREATE_NEW_CONTOUR;
    }
    else {
        mCertifiedSelectContourSlices.mRealActionType = SMART_CONTOUR_INVALID;
    }

    actionType = mCertifiedSelectContourSlices.mRealActionType;

    if(actionType == CREATE_NEW_CONTOUR) {
        mFirstPoint->SetX(currentPoint.x);
        mFirstPoint->SetY(currentPoint.y);
    }
    else if (actionType == CLOSE_CONTOUR) {
        std::string imageUID = "";
        mRenderProxy->GetDataSource(windowType, IMAGE, imageUID);
        if(imageUID.empty()) {
            TPS_LOG_DEV_ERROR<<"failed to get image uid";
            return TPS_ER_FAILURE;
        }
        TpsImage3DEntity* image = nullptr;
        if(!mDataRepository->GetImageDataManager()->GetImage3DData(imageUID,&image)) {
            TPS_LOG_DEV_ERROR<<"failed to get image 3D";
            return TPS_ER_FAILURE;
        }
        RtImage3DHeader *header = nullptr;
        image->GetImage3DHeader(header);
        if(header == nullptr)
        {
            return TPS_ER_FAILURE;
        }
        int zCount = header->m_iSliceCount;
        lastPoint.SetX(currentPoint.x);
        lastPoint.SetY(currentPoint.y);
        mRenderProxy->TransformPoint(windowType, windowUid, *mFirstPoint, firstPointInPat);
        mRenderProxy->TransformPoint(windowType, windowUid, lastPoint, lastPointInPat);
        Mcsf::Matrix4f matrixV2P;
        image->GetVolumeToPatientMatrix(matrixV2P);
        mCertifiedSelectContourSlices.mFirstSlice = matrixV2P.Inverse().Transform(firstPointInPat).GetZ();
        mCertifiedSelectContourSlices.mSecondSlice = matrixV2P.Inverse().Transform(lastPointInPat).GetZ();
        if (mCertifiedSelectContourSlices.mFirstSlice < 0) {
            mCertifiedSelectContourSlices.mFirstSlice = 0;
        }
        if (mCertifiedSelectContourSlices.mSecondSlice >= zCount) {
            mCertifiedSelectContourSlices.mSecondSlice = zCount - 1;
        }
        mRenderProxy->OperateContourSlices(windowType, windowUid, vOIUID,mCertifiedSelectContourSlices.mFirstSlice,
            mCertifiedSelectContourSlices.mSecondSlice, DELETEMode, REPLACE);
        mContourDeleted = true;
    }
    mRenderProxy->SelectContourSlices(windowType, windowUid, currentPoint, actionType, vOIUID, 
        mCertifiedSelectContourSlices.mFirstSlice, 
        mCertifiedSelectContourSlices.mSecondSlice);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

CertifiedSelectContourSlices SelectContourSlicesCmd::GetCertifiedSelectContourSlices() {
    return mCertifiedSelectContourSlices;
}

int SelectContourSlicesCmd::PostExecute(){
    if (!mContourDeleted) return TPS_ER_SUCCESS;

    //delete all related commands in redo/undo list of CommandManager
    auto mng = mDataRepository->GetCommandManager();
    TpsSafeLock<TpsCommandManager> lock(mng);
    mng->RemoveCommands([&](TpsRedoUndoCommand* cmd)->bool{
        auto voiCmd = dynamic_cast<TpsVOIRedoUndoCmdBase*>(cmd);
        return voiCmd != nullptr && voiCmd->GetVoiUid() == 
            mCertifiedSelectContourSlices.mVOIUid;
    });
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
