//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_mouseposinfo_cmdhandler.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 11, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GA_MOUSEPOSINFO_COMMANDHANDLER_H_
#define TPS_GA_MOUSEPOSINFO_COMMANDHANDLER_H_

#include "tps_defs.h"
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"

TPS_BEGIN_NAMESPACE
class IGalleryController;

/// \class InitializeCommandHandler
/// 
/// \brief *****
class TpsGaMousePosInfoCmdHandler : public IGalleryCmdHandler 
{
public:
    //the constructor and destructor
    TpsGaMousePosInfoCmdHandler(IGalleryController *commonController);
    virtual ~TpsGaMousePosInfoCmdHandler();

    /// \brief HandleCommand to add a beam from FE
    /// \param[in] pContext
    /// \param[out] pReplyObject
    /// \return int 
    virtual int HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGaMousePosInfoCmdHandler);
};

TPS_END_NAMESPACE

#endif  // TPS_COMA_LOADMOUSEPOSINFO_COMMANDHANDLER_H_
