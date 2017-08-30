//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_createviewmodule_cmdhandler.cpp
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_ga_createviewmodule_cmdhandler.h"

//TPS
#include "tps_logger.h"
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"
#include "RtTpsProtoLib/rt_tps_layoutinfos.pb.h"

TPS_BEGIN_NAMESPACE// begin namespace tps

CreateViewModuleGalleryCmdHandler::CreateViewModuleGalleryCmdHandler(
    IGalleryController *pController) 
    : IGalleryCmdHandler(pController) {
}

CreateViewModuleGalleryCmdHandler::~CreateViewModuleGalleryCmdHandler(){

}

int CreateViewModuleGalleryCmdHandler::HandleCommand(
    const Mcsf::CommandContext *pContext, std::string * pReplyObject){

        TPS_LOG_DEV_INFO<<"CreateViewModuleGalleryCmdHandler::HandleCommand";
#ifdef _DEBUG
        printf("\n[TPS][BE][Gallery]CreateViewModuleGalleryCmdHandler::HandleCommand");
#endif
        if (nullptr == pContext || nullptr == pReplyObject)
        {
            TPS_LOG_DEV_ERROR<<"CreateViewModuleGalleryCmdHandler::HandleCommand"
                << "nullptr == pContext || nullptr == pReplyObject";
            return TPS_FAILURE;
        }

        //parse the context
        proto::RT_TPS_LAYOUT layoutInfo;
        layoutInfo.ParseFromString(pContext->sSerializeObject);

        //receive the create view module from FE 
        WINDOW_TYPE windowType;
        for (int i= 0; i<layoutInfo.units_size(); ++i){
            proto::RT_TPS_LAYOUT_UNIT protoUnit = layoutInfo.units(i);

            LAYOUT_UNIT unit;
            unit.cellID = protoUnit.cellid();
            unit.viewerControlID = protoUnit.viewercontrolid();

            DISPLAY_SIZE disSize;
            disSize.height = protoUnit.height();
            disSize.width = protoUnit.width();
            windowType = protoUnit.window_type();

            if (mIGalleryController->HasViewModule(windowType)){
                mIGalleryController->ResizeViewModule(windowType, disSize);
            }
            else {
                mIGalleryController->CreateViewModule(windowType, disSize, unit);
            }
        }

        //SyncSendCommand to replay FE , else FE will waiting for the signal
        *pReplyObject = mIGalleryController->GetReplyString(true);;
        return TPS_SUCCESS;

}

TPS_END_NAMESPACE  // end namespace tps
