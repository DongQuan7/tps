//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    
///  \brief   
///
///  \version 1.0
///  \date    Apr. 17, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GA_VRROTATE_CMDHANDLER_H_
#define TPS_GA_VRROTATE_CMDHANDLER_H_

#include "tps_defs.h"

// local
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"

TPS_BEGIN_NAMESPACE // begin namespace tps

class IGalleryController; 
/// \class TpsGaVRRotateCmdHandler
/// 
/// \brief mpr rotate operation
class TpsGaVRRotateCmdHandler : public IGalleryCmdHandler 
{
public:
    //the constructor and destructor
    TpsGaVRRotateCmdHandler(IGalleryController *commonController);
    virtual ~TpsGaVRRotateCmdHandler();

    virtual int HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGaVRRotateCmdHandler);
};

TPS_END_NAMESPACE

#endif  // TPS_GA_MPROPERATIONCMDHANDLER_H_
