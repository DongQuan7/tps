//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_igallerycmdhandler.h
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_GA_IGALLERYCMDHANDLER_H_
#define TPS_GA_IGALLERYCMDHANDLER_H_

#include "RtTpsGalleryControllerApp/tps_gca_igallery_controller.h"

TPS_BEGIN_NAMESPACE

class IGalleryCmdHandler : public Mcsf::ICommandHandler
{
public:
    IGalleryCmdHandler(IGalleryController *pGalleryController) 
        : mIGalleryController(pGalleryController) {};
    virtual ~IGalleryCmdHandler(){};

    virtual int HandleCommand(const Mcsf::CommandContext* pContext, 
        std::string* pReplyObject) = 0;

protected:
    IGalleryController *mIGalleryController;
};

TPS_END_NAMESPACE

#endif //TPS_CA_IGALLERYCMDHANDLER_H_
