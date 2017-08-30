//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_ga_measureangle_cmdhandler.h
///  \brief   
///
///  \version 1.0
///  \date    Mar. 05, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GA_MEASUREANGLECMDHANDLER_H_
#define TPS_GA_MEASUREANGLECMDHANDLER_H_

#include "tps_defs.h"

//local
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"

TPS_BEGIN_NAMESPACE

class IGalleryController;

class TpsGaMeasureAngleCmdHandler : public IGalleryCmdHandler 
{
    public:
        /////////////////////////////////////////////////////////////////
        ///  \brief Constructor
        ///
        ///  \param[in]    
        ///  \param[out]   
        ///  \return       
        ///  \exceptions:   none  
        /////////////////////////////////////////////////////////////////
        TpsGaMeasureAngleCmdHandler(IGalleryController *controller);
        /////////////////////////////////////////////////////////////////
        ///  \brief Destructor
        ///
        ///  \param[in]    
        ///  \param[out]   
        ///  \return       
        ///  \exceptions:   none  
        /////////////////////////////////////////////////////////////////
        ~TpsGaMeasureAngleCmdHandler();
        /////////////////////////////////////////////////////////////////
        ///  \brief Handle command
        ///
        ///  \param[in]    
        ///  \param[out]   
        ///  \return       
        ///  \exceptions:   none  
        /////////////////////////////////////////////////////////////////
        int HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject);

    private:
        TPS_DISALLOW_COPY_AND_ASSIGN(TpsGaMeasureAngleCmdHandler);
};

TPS_END_NAMESPACE

#endif