////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_update_graphic_object_operation.cpp
/// 
///  \brief class UpdateGraphicObjectOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/07/18
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_update_graphic_object_operation.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

UpdateGraphicObjectOperation::UpdateGraphicObjectOperation(
const std::string& mainUID, GO_TYPE goType, int dataSourceType/* = -1*/) : 
mMainUID(mainUID), mGOType(goType), mDataSrcType(dataSourceType)
{

}

UpdateGraphicObjectOperation::~UpdateGraphicObjectOperation()
{

}

bool UpdateGraphicObjectOperation::ModifyGraphicObject() const
{
    std::string sGoType = GOTypeHelper::ToString(mGOType);
    std::string key = mMainUID + "|" + sGoType;
    auto go = mModelWarehouse->GetModelObject(key);
    if (go == nullptr){
        TPS_LOG_DEV_ERROR<<"The graphic object does not exist.";
        return false;
    }

    auto converter = mConverterFactory->GetConverter(mGOType);
    if (converter == nullptr){
        TPS_LOG_DEV_ERROR<<"The GO collection converter does not exist.";
        return false;
    }

    GOConverterParameter para;
    if (mDataSrcType == -1)
    {
        para.obj_uid = mMainUID;
    }
    else
    {
        para.data_source_map[(DATA_SOURCE_ID)mDataSrcType] = mMainUID;
    }
    if (!converter->Update(para, go)){
        TPS_LOG_DEV_ERROR<<"Failed to convert cache graphic object.";
        return false;
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps