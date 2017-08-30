//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_mproperationcmdhandler.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GA_MPROPERATIONCMDHANDLER_H_
#define TPS_GA_MPROPERATIONCMDHANDLER_H_

#include "tps_defs.h"

// local
#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"

TPS_BEGIN_NAMESPACE // begin namespace tps

class IGalleryController;
/// \class TpsGaMPRPageTurnCmdHandler
/// 
/// \brief mpr page turn operation
class TpsGaMPRPageTurnCmdHandler : public IGalleryCmdHandler 
{
public:
    //the constructor and destructor
    TpsGaMPRPageTurnCmdHandler(IGalleryController *commonController);
    virtual ~TpsGaMPRPageTurnCmdHandler();

    virtual int HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGaMPRPageTurnCmdHandler);
};

/// \class TpsGaMPRPanCmdHandler
/// 
/// \brief mpr pan operation
class TpsGaMPRPanCmdHandler : public IGalleryCmdHandler 
{
public:
    //the constructor and destructor
    TpsGaMPRPanCmdHandler(IGalleryController *commonController);
    virtual ~TpsGaMPRPanCmdHandler();

    virtual int HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGaMPRPanCmdHandler);
};

/// \class TpsGaMPRWindowingCmdHandler
/// 
/// \brief mpr zoom operation
class TpsGaMPRZoomCmdHandler : public IGalleryCmdHandler 
{
public:
    //the constructor and destructor
    TpsGaMPRZoomCmdHandler(IGalleryController *commonController);
    virtual ~TpsGaMPRZoomCmdHandler();

    virtual int HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGaMPRZoomCmdHandler);
};

/// \class TpsGaMPRWindowingCmdHandler
/// 
/// \brief mpr windowing operation
class TpsGaMPRWindowingCmdHandler : public IGalleryCmdHandler 
{
public:
    //the constructor and destructor
    TpsGaMPRWindowingCmdHandler(IGalleryController *commonController);
    virtual ~TpsGaMPRWindowingCmdHandler();

    virtual int HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGaMPRWindowingCmdHandler);
};

/// \class TpsGaMPRRotateCmdHandler
/// 
/// \brief mpr rotate operation
class TpsGaMPRRotateCmdHandler : public IGalleryCmdHandler 
{
public:
    //the constructor and destructor
    TpsGaMPRRotateCmdHandler(IGalleryController *commonController);
    virtual ~TpsGaMPRRotateCmdHandler();

    virtual int HandleCommand(const Mcsf::CommandContext *pContext, std::string *pReplyObject);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGaMPRRotateCmdHandler);
};

TPS_END_NAMESPACE

#endif  // TPS_GA_MPROPERATIONCMDHANDLER_H_
