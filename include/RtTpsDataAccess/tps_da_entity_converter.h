////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_da_entity_converter.h
/// 
///  \brief class EntiyConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    Oct.13,2014
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_ENTITY_CONVERTER_H_
#define TPS_DA_ENTITY_CONVERTER_H_

#include "tps_da_defs.h"
#include "tps_singleton.h"

namespace RTFWK {
    class RTFWKDLDOBeam;
}

TPS_BEGIN_NAMESPACE

class ROIEntity;
struct CertifiedVOI;

/// \class EntiyConverter
/// 
/// \brief convert certified object to entities
class TPS_DA_DECLSPEC EntityConverter {
    DECLARE_SINGLETON(EntityConverter)
public:

    void ROIEntiyToCertifiedVOI(const ROIEntity& roiEntity, CertifiedVOI* voiCertified);
    void CertifiedVOIToROIEntiy(const CertifiedVOI& voiCertified, ROIEntity* pRoiEntity);
private:
    EntityConverter(void);
    virtual ~EntityConverter(void);
};


TPS_END_NAMESPACE

#endif //TPS_DA_IMAGEDATAMANAGER_H_
