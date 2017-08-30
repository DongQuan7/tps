//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
/// All rights reserved. 
/// 
/// \author  ZHOU Qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
//
/// \file    tps_rl_update_imagetext_operation.cpp
///
/// \brief   UpdateImageTextOperation
/// 
/// \version 1.0
/// 
/// \date    June 26, 2015
/////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_update_imagetext_operation.h"

//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"      // ClsGraphicObjectMPR
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"

//TPS
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"

TPS_BEGIN_NAMESPACE

using namespace Mcsf::MedViewer3D;

UpdateImageTextOperation::UpdateImageTextOperation()
{

}

UpdateImageTextOperation::~UpdateImageTextOperation()
{
}

void UpdateImageTextOperation::SetGraphicObjectKeys(const std::string& sMprKey, 
    const std::string &sImageTextKey, const std::string& sPlanName) 
{
    mMprKey = sMprKey;
    mImageTextKey = sImageTextKey;
    mPlanName = sPlanName;
}

bool UpdateImageTextOperation::ModifyGraphicObject() const 
{
    auto go = mModelWarehouse->GetGraphicObject(mMprKey);
    auto spMprGO = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(go);
    if (spMprGO == nullptr) {
        TPS_LOG_DEV_ERROR<<"[ERROR][UpdateCornerInfoOperation][ModifyGraphicObject]: "
            <<"Failed to get MPR GO.";
        return false;
    }

    go = mModelWarehouse->GetGraphicObject(mImageTextKey);
    auto spImageTextGo = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(go);
    if (spImageTextGo == nullptr) {
        TPS_LOG_DEV_ERROR<<"[ERROR][UpdateCornerInfoOperation][ModifyGraphicObject]: "
            <<"Failed to get corner info GO.";
        return false;
    }
    ConvertPlanNameToImageText(mPlanName,spImageTextGo);

    spMprGO->SetDirty(true);

    return true;
}

bool UpdateImageTextOperation::ConvertPlanNameToImageText(const std::string sPlanName, 
    std::shared_ptr<TpsTextOverlayGraphicObject> spImageTextGo) const
{
    // create new buffer
    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
    std::wstringstream wssStream;
    Pixel32* pStringBuffer = nullptr;
    int iStrWidth, iStrHeight;

    std::wstring wplanName;
    wplanName.assign(sPlanName.begin(), sPlanName.end());
    wssStream<<wplanName;
    if (!pBufGen->GenerateRasterBuffer(wssStream.str(), &pStringBuffer, iStrWidth, iStrHeight)) {
        TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
        return false;
    }

    spImageTextGo->UpdateTextBlock("PlanName", (unsigned char*)pStringBuffer, iStrWidth, iStrHeight);

    if(pStringBuffer!=nullptr){
        delete[] pStringBuffer;
        pStringBuffer = nullptr;
    }

    return true;
}

TPS_END_NAMESPACE
