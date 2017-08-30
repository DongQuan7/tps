////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong Zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_measureangle_operation.cpp 
/// 
///  \brief class TpsEasyPlanMeasureAngleOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/11
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h "
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measuregraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measureangle_operation.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"

TPS_BEGIN_NAMESPACE
TpsEasyPlanMeasureAngleOperation::TpsEasyPlanMeasureAngleOperation(const std::string &planUID, const std::string &drrImageUID,
	const list<Mcsf::MedViewer3D::Point2D>& anglePoints, 
    const float cursorX, const float cursorY,
    const bool actionStarted, const bool firstSideDone, const bool secondSideDone) :  
    mPlanUID(planUID), mDrrImageUID(drrImageUID), 
    mAnglePoints(anglePoints), mCursorX(cursorX), mCursorY(cursorY),
    mActionStarted(actionStarted), mFirstSideDone(firstSideDone), mSecondSideDone(secondSideDone){

}

TpsEasyPlanMeasureAngleOperation::~TpsEasyPlanMeasureAngleOperation()
{

}

bool TpsEasyPlanMeasureAngleOperation::ModifyGraphicObject() const
{
    try{
        using namespace Mcsf::MedViewer3D;
        auto goDrrMeasureAngle = mModelWarehouse->GetModelObject(mDrrImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_EASYPLAN_MEASURE));

        auto go = mModelWarehouse->GetModelObject(mPlanUID + "|" + 
            GOTypeHelper::ToString(GO_TYPE_RT_IMAGE_COLLECTION));
        if(nullptr == go){
            TPS_LOG_DEV_ERROR<<"failed to get easyplan drr collection go";
            return false;
        }
        std::shared_ptr<EasyPlanDrrGraphicObjectCollection> drrCollectionGO = std::dynamic_pointer_cast<EasyPlanDrrGraphicObjectCollection> (go);

        auto drr = drrCollectionGO->GetDrr(mDrrImageUID);

        auto drrmeasureangle = std::dynamic_pointer_cast<EasyPlanMeasureGraphicObject>(goDrrMeasureAngle);
        if(drrmeasureangle == nullptr || drr == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to EasyPlanMeasureGraphicObject\n";
            return false;
        }
        drrmeasureangle->SetMeasureAnglePoints(mAnglePoints);
        drrmeasureangle->SetMeasureAngleCursorPoint(mCursorX, mCursorY);
        drrmeasureangle->SetMeasureAngleActionStatus(mActionStarted, mFirstSideDone, mSecondSideDone);
        drrmeasureangle->SetDirty(true);
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsEasyPlanMeasureDistanceOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
    return true;
}

TPS_END_NAMESPACE