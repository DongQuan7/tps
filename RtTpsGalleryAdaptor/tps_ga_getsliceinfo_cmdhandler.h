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
///  \date    Apr. 18, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GA_GETSLICEINFO_CMDHANDLER_H_
#define TPS_GA_GETSLICEINFO_CMDHANDLER_H_

#include "tps_defs.h"

// local
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"

TPS_BEGIN_NAMESPACE // begin namespace tps

class IGalleryController; 
/// \class TpsGaGetSliceInfoCmdHandler
/// 
/// \brief mpr rotate operation
class TpsGaGetSliceInfoCmdHandler : public IGalleryCmdHandler 
{
public:
    //the constructor and destructor
    TpsGaGetSliceInfoCmdHandler(IGalleryController *commonController);
    virtual ~TpsGaGetSliceInfoCmdHandler();

    virtual int HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGaGetSliceInfoCmdHandler);
};

TPS_END_NAMESPACE

#endif  // TPS_GA_MPROPERATIONCMDHANDLER_H_
