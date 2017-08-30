//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  chengcheng.rong chengcheng.rong@united-imaging.com
///
///  \file    
///  \brief   
///
///  \version 1.0
///  \date    Jul.13, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_IDATA_REPOSITORY_H
#define TPS_IDATA_REPOSITORY_H

#include "tps_da_defs.h"

TPS_BEGIN_NAMESPACE

class TpsPatientManager;
class TpsImageDataManager;
class TpsNormGroupManager;
class TpsDoseManager;
class TpsBeamManager;
class TpsVOIManager;
class TpsPOIManager;
class TpsMachineManager;
class TpsConfigurationManager;
class TpsDisplayStateManager;
class TpsHotColdSpotsManager;
class TpsDVHManager;
class TpsOptGoalManager;
class TpsFusionSecondaryDataManager;
class TpsGalleryImageManager;
class TpsGalleryPatientManager;
class TpsMachineSettingManager;
class TpsCommandManager;
class TpsTableManager;
class TpsTableSettingManager;
class TpsTcpNTcpManager;

class TPS_DA_DECLSPEC ITpsDataRepository
{
public:
    ITpsDataRepository(){}
    virtual ~ITpsDataRepository(){}

    virtual TpsPatientManager*          GetPatientManager() const = 0;
    virtual TpsImageDataManager*        GetImageDataManager() const = 0;
    virtual TpsNormGroupManager*        GetNormgroupManager() const = 0;
    virtual TpsDoseManager*             GetDoseManager() const = 0;
    virtual TpsBeamManager*             GetBeamManager() const = 0;
    virtual TpsVOIManager*              GetVoiManager() const = 0;
    virtual TpsPOIManager*              GetPoiManager() const = 0;
    virtual TpsMachineManager*          GetMachineManager() const = 0;
    virtual TpsDisplayStateManager*     GetDisplayStateManager() const = 0;
    virtual TpsConfigurationManager*    GetConfigurationManager() const = 0;
    virtual TpsHotColdSpotsManager*     GetHotColdSpotsManager() const = 0;
    virtual TpsDVHManager*              GetDVHManager() const = 0;
    virtual TpsOptGoalManager*          GetOptGoalManager() const = 0;
    virtual TpsFusionSecondaryDataManager* GetFusionSecondaryDataManager() const = 0;
    virtual TpsGalleryImageManager*     GetGalleryImageManager() const = 0;
    virtual TpsGalleryPatientManager*     GetGalleryPatientManager() const = 0;
    virtual TpsMachineSettingManager*       GetMachineSettingManager()const = 0;
    virtual TpsCommandManager*          GetCommandManager() const = 0;
    virtual TpsTableManager*            GetTableManager() const = 0;
    virtual TpsTcpNTcpManager*          GetTcpNTcpManager() const = 0;

    virtual bool Initialize() = 0;
    virtual void Dispose() = 0;
};

TPS_END_NAMESPACE

#endif //TPS_CONTROLLER_BASE_H
