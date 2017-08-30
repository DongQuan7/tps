//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_crosshairmovement_cmdhandler.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 11, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_GA_CROSSHAIR_MOVEMENT_COMMANDHANDLER_H_
#define TPS_GA_CROSSHAIR_MOVEMENT_COMMANDHANDLER_H_

#include "tps_defs.h"

#include "RtTpsGalleryAdaptor/tps_ga_igallerycmdhandler.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class IGalleryController;

class TpsGaCrosshairMovementCmdHandler : public IGalleryCmdHandler {

public:
    //the constructor and destructor
    TpsGaCrosshairMovementCmdHandler(IGalleryController *galleryController);
    virtual ~TpsGaCrosshairMovementCmdHandler();

    virtual int HandleCommand(const Mcsf::CommandContext* pContext, std::string* pReplyObject);
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGaCrosshairMovementCmdHandler);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_CA_CROSSHAIR_COMMANDHANDLER_H_