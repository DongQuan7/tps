////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  RONG Chengcheng chengcheng.rong@united-imaging.com
/// 
///  \file tps_rl_updatelightsettingoperation.cpp
/// 
///  \brief class ActiveChessEdit implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/03
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_updatelightsettingoperation.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_rendercontrollerbase.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsFramework/tps_fw_go_converter_parameter.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsUpdateLightSettingOperation::TpsUpdateLightSettingOperation(
    const std::string& imageUID, int mprSection, float mprTransparency3D, float rx, float ry, float rz, 
    float scale, float specf, float ambf, float difff, float shininess, 
    float opacity, float lightx, float lighty, float lightz, int smoothness, 
    bool useCameraLight, int displayMode,  bool displayVertexCube,
    int startStripIndex, int endStripIndex)
    : mImageUID(imageUID), mMprSection(mprSection), mMprTransparency3D(mprTransparency3D), mrx(rx), mry(ry), mrz(rz), mscale(scale), mspecf(specf), mambf(ambf),
    mdifff(difff), mshininess(shininess), mopacity(opacity), mlightx(lightx),
    mlighty(lighty), mlightz(lightz), msmoothness(smoothness), museCameraLight(useCameraLight),
    mDisplayMode(displayMode), mdisplayVertexCube(displayVertexCube),
    mStartStripIndex(startStripIndex), mEndStripIndex(endStripIndex)
{

}

TpsUpdateLightSettingOperation::~TpsUpdateLightSettingOperation(){

}

bool TpsUpdateLightSettingOperation::ModifyGraphicObject() const
{
    // 如何拿到 shaded surface GO？
    // 把这些参数都填进去
    std::string goTypeStr = GOTypeHelper::ToString(GO_TYPE_SHADEDSURFCE);
    std::string key = mImageUID + "|" + goTypeStr;
    IGraphicObjectPtr goShadedSurface = mModelWarehouse->GetModelObject(key);
    if (goShadedSurface==nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to execute operation due to go is nullptr.";
        return false;
    }

    auto shadedVoi = std::dynamic_pointer_cast<ShadedVoiGraphicObject>(goShadedSurface);
    shadedVoi->SetRotateX(mrx);
    shadedVoi->SetRotateY(mry);
    shadedVoi->SetRotateZ(mrz);
    shadedVoi->SetScale(mscale);
    shadedVoi->SetShininess(mshininess);
    shadedVoi->SetSpecularFactor(mspecf);
    shadedVoi->SetAmbFactor(mambf);
    shadedVoi->SetDiffFactor(mdifff);
    shadedVoi->SetLightX(mlightx);
    shadedVoi->SetLightY(mlighty);
    shadedVoi->SetLightZ(mlightz);
    shadedVoi->SetDisplayMode(mDisplayMode);
    shadedVoi->SetUseCameraLight(museCameraLight);
    shadedVoi->SetStartStripIndex(mStartStripIndex);
    shadedVoi->SetEndStripIndex(mEndStripIndex);
    shadedVoi->SetDisplayVertexCube(mdisplayVertexCube);
	TpsDisplayStateManager* displayStateManager = mDataRepository->GetDisplayStateManager();
    shadedVoi->SetMprSection(displayStateManager->GetMprSection());
	shadedVoi->SetMprTransparency3D(displayStateManager->GetMPRTransparency3D());

    int oldSmoothness = shadedVoi->GetSmoothness();
    // 如果曲线降采样率发生变化，就调用converter重新生成新的prism条带进行绘制
    shadedVoi->SetSmoothness(msmoothness);
    if (oldSmoothness!=msmoothness) {
        auto converter = mConverterFactory->GetConverter(GO_TYPE_SHADEDSURFCE);
        if (converter == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed get shadedsurface graphic object converter.";
            return false;
        }
        GOConverterParameter para;
        para.data_source_map[IMAGE] = mImageUID;
        if(!converter->Update(para, shadedVoi)){
            TPS_LOG_DEV_ERROR<<"The cache collection converter does not exist.";
            return false;
        }
    }
    shadedVoi->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE  // end namespace tps