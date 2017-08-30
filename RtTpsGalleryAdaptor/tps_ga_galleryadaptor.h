//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_galleryadaptor.h
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GALLERY_ADAPTOR_H
#define TPS_GALLERY_ADAPTOR_H

#include "RtTpsFramework/tps_fw_commproxy.h"

#include "RtTpsFramework/tps_fw_adaptorbase.h"
#include "RtTpsGalleryControllerApp/tps_gca_igallery_controller.h"

TPS_BEGIN_NAMESPACE

class TpsGalleryAdaptor : public TpsAdaptorBase {
public:
    TpsGalleryAdaptor(int id);
    virtual ~TpsGalleryAdaptor(void);    

    virtual int Start(const std::vector<std::string> &args);
    virtual int End();
    virtual int RegisterCommand();
private:
    IGalleryController             *mIGalleryController;
};

DECLARE_APP_ADAPTOR(TpsGalleryAdaptor);

TPS_END_NAMESPACE

#endif //TPS_GALLERY_ADAPTOR_H
