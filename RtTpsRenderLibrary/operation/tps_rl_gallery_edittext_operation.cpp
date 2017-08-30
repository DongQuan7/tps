//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_edittext_operation.cpp
///  \brief   Gallery Edit Text declaration
///
///  \version 1.01
///  \date    Jul. 01, 2015
///  
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
//#include "RtTpsRenderLibrary/tps_rl_gallery_measurehittest_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_edittext_operation.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_textgraphicobject.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"

// ZHENGHE

#include "McsfViewer3DModel/mcsf_viewer3d_model.h" 

TPS_BEGIN_NAMESPACE   // begin namespace tps

    TpsGalleryEditTextOperation::TpsGalleryEditTextOperation(const std::string textKey, const LAYOUT_UNIT &unit, 
    const float startX, const float startY, const float stopX, const float stopY,
    const bool isDelete)
    :mTextKey(textKey), mUnit(unit), mStartX(startX), mStartY(startY),
    mStopX(stopX), mStopY(stopY), mIsDelete(isDelete){
}

TpsGalleryEditTextOperation::~TpsGalleryEditTextOperation(){

}

bool TpsGalleryEditTextOperation::ModifyGraphicObject() const {

    try{
        auto go = mModelWarehouse->GetGraphicObject(mTextKey);
        auto textGO = std::dynamic_pointer_cast<GalleryTextGraphicObject>(go);
        if(textGO == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to GalleryTextGraphicObject\n";
            return false;
        }
        if(mIsDelete)
        {
            textGO->DeleteHitText();
        }
        textGO->UpdateTextGO();
        textGO->SetDirty(true);

        //go = mModelWarehouse->GetModelObject(mMeasureValKey);
        //auto measureDistanceValGO = std::dynamic_pointer_cast<GalleryMeasureValGraphicObject>(go);
        //if(measureDistanceValGO == nullptr){
        //    TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to GalleryMeasureValGraphicObject\n";
        //    return false;
        //}
        //measureGO->SetMeasureValGO(measureDistanceValGO);

        //measureGO->UpdateMeasureValGO();
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"TpsGalleryEditTextOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
