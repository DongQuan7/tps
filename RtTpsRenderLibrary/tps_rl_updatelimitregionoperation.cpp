//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rl_updatelimitregionoperation.cpp 
///  \brief   limit region
///
///  \version 1.0
///  \date  2014/05/13  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_updatelimitregionoperation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundarygraphicobject.h"

TPS_BEGIN_NAMESPACE

TpsUpdateRegionOperation::TpsUpdateRegionOperation(const std::string &imageUid,
    Mcsf::Point3f startPoint, Mcsf::Point3f endPoint ) : 
        mImageUid(imageUid), mStartPoint(startPoint), mEndPoint(endPoint),
        mCanShowRegion(true), mIsFirstShow(false){

}

TpsUpdateRegionOperation::TpsUpdateRegionOperation(const std::string &imageUid, bool canShowRegion,
     Mcsf::Point3f startPoint, Mcsf::Point3f endPoint)
    :mImageUid(imageUid), mStartPoint(startPoint), mEndPoint(endPoint),
    mCanShowRegion(canShowRegion), mIsFirstShow(true){

}

TpsUpdateRegionOperation::~TpsUpdateRegionOperation() {

}

bool TpsUpdateRegionOperation::ModifyGraphicObject() const {
    std::string limitRegionKey = mImageUid + "|" + 
        GOTypeHelper::ToString(GO_TYPE_LIMIT_REGION);
    auto go = mModelWarehouse->GetModelObject(limitRegionKey);
    auto limitRegionGO = std::dynamic_pointer_cast<TpsDoseGridBoundaryGraphicObject>(go);
    if (limitRegionGO == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get limit region go with key: "
            <<limitRegionKey;
        return false;
    }
    if (mCanShowRegion) {
        Mcsf::MedViewer3D::Point3D leftBottomPoint;
        Mcsf::MedViewer3D::Point3D rightTopPoint;
        leftBottomPoint.x = mStartPoint.GetX();
        leftBottomPoint.y = mStartPoint.GetY();
        leftBottomPoint.z = mStartPoint.GetZ();
        rightTopPoint.x = mEndPoint.GetX();
        rightTopPoint.y = mEndPoint.GetY();
        rightTopPoint.z = mEndPoint.GetZ();

        limitRegionGO->SetLeftBottomPoint(leftBottomPoint);
        limitRegionGO->SetRightTopPoint(rightTopPoint);
    }
    limitRegionGO->SetDirty(true);
    limitRegionGO->SetVisible(mCanShowRegion);
    return true;
}

TPS_END_NAMESPACE
