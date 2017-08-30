//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
///
///  \file    tps_rl_updatevoioperation.h
///  \brief 
///
///  \version 1.0
///  \date    Mar. 08, 2013
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_updatevoidisplayoperation.h"

#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"


//for test
//#include "RtTpsRenderLibrary/tps_rl_surfacevolumerenderinggraphicobject.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsUpdateVoiDisplayOperation::TpsUpdateVoiDisplayOperation(
    const std::string& imageUid, const std::string& voiUid)
    : mImageUid(imageUid), mVoiUid(voiUid){
}

TpsUpdateVoiDisplayOperation::~TpsUpdateVoiDisplayOperation(){

}

bool TpsUpdateVoiDisplayOperation::ModifyGraphicObject() const
{
    if (mImageUid.empty())
    {
        TPS_LOG_DEV_ERROR<<"The plan UID is empty.";
        return false;
    }

    TpsVOIManager *voiMng = mDataRepository->GetVoiManager();
    ROIEntity *roiEntity = voiMng->GetROI(mVoiUid);
    if (nullptr == roiEntity)
    {
         TPS_LOG_DEV_ERROR<<"roiEntity is null";
        return false;
    }
    RtVoi* innerRoi = roiEntity->GetInnerRoi();

    auto go = mModelWarehouse->GetModelObject(
        mImageUid + "|" + GOTypeHelper::ToString(GO_TYPE_VOI_COLLECTION));
    auto voiCollection = dynamic_pointer_cast<TpsVOIGOCollection>(go);
    if (go != nullptr && voiCollection != nullptr)
    { // when change layout that only exit 3D window, the voiCollectionGo is null but I need to refresh mesh
        auto voiGO = voiCollection->GetVOI(mVoiUid);
        if (voiGO != nullptr)
        {
            voiGO->SetVisibility(innerRoi->get_isvisible());
            voiGO->SetColor(innerRoi->get_color());
            voiGO->SetIsShaded(innerRoi->get_isshaded());
            voiGO->SetAlpha3D(innerRoi->get_alpha3d());

            int sliceCount = voiGO->GetSliceCount();
            voiGO->SetHasInterpolate(true);
            bool *inters = nullptr;
            voiGO->GetInterpolateFlags(inters);
            auto interFlags = innerRoi->get_interpolate();
            if(interFlags.empty()) 
            {
                for(int i = 0; i < sliceCount; ++i) 
                {
                    interFlags.push_back(true);
                }
                innerRoi->set_interpolate(interFlags);
            }
            //×¢Òâstd::vector<bool>ÌØ»¯
            for (int i = 0; i < sliceCount; ++i)
            {
                inters[i] = interFlags[i];
            }

            voiGO->SetDirty(true);
            voiCollection->SetDirty(true);
        }
    }
    return true;
}


TPS_END_NAMESPACE  // end namespace tps
