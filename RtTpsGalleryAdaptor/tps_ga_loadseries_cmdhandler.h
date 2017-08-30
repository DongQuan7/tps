//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_loadseries_cmdhandler.h
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GA_LOADSERIES_CMDHANDLER_H_
#define TPS_GA_LOADSERIES_CMDHANDLER_H_

#include "tps_defs.h"
#include "mcsfnetbase/mcsf_netbase_interface.h"

#include "RtTpsFramework/tps_fw_pluginbase.h"
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"

TPS_BEGIN_NAMESPACE // begin namespace tps
class IGalleryController;

/// \class LoadSeriesCommandHandler
/// 
/// \brief To load a series
class LoadSeriesCmdHandler : public IGalleryCmdHandler 
{
public:
    //the constructor and destructor
    LoadSeriesCmdHandler(IGalleryController *galleryController);
    virtual ~LoadSeriesCmdHandler();

    virtual int HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(LoadSeriesCmdHandler);
};

TPS_END_NAMESPACE

#endif  //TPS_CA_LOADSERIESCOMMANDHANDLER_H_
