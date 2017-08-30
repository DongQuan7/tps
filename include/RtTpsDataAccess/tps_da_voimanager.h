//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_voi_manager.h
///  \brief   entry of voi operator
///
///  \version 1.0
///  \date    Oct. 9, 2013
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_VOI_MANAGER_H_
#define TPS_VOI_MANAGER_H_

#include "tps_da_defs.h"

#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

namespace Mcsf{
    class Point3f;
}

TPS_BEGIN_NAMESPACE

class RtVoi;
class ROIEntity;
class TPSMask;
struct PlaneCache;
class RtVoisetting;

/// \class TpsVOIManager
/// 
/// \brief *****
class TPS_DA_DECLSPEC TpsVOIManager : public TpsDataManagerBase
{
public:
    TpsVOIManager(void);
    virtual ~TpsVOIManager(void);

    void ClearPatientData();

    void SetVOIDoseInfo(const std::string &voiUID, float minDose, float maxDose, float meanDose);

    bool SaveContourMaskToClipboard(const PlaneCache& contourMask);
    bool GetContourMaskFromClipboard(PlaneCache** contourMask);
    bool Contains(const std::string& voiUid);

    void SetVoiSettings(const std::vector<RtVoisetting*>& voisettings);
    std::vector<RtVoisetting*> GetVoiSettings() const;
    bool GetVoiSettingFromVoiName(const std::string& voiname, RtVoisetting* voisetting);

    ROIEntity* CreateROIEntity(RtVoi* voi);
    bool AddROI(ROIEntity* pRoiEntity);
    ROIEntity* GetROI(const std::string& uid);
    std::vector<ROIEntity*> GetROIBySeries(const std::string seriesUid) const;
    std::vector<std::string> GetROIUidListBySeries(const std::string seriesUid) const;
    std::vector<ROIEntity*> GetROIListViaStructureUids(const std::vector<std::string>& structureSetUidList) const;
    void DeleteROI(const std::string& sRoiUID);
    ROIEntity* GetExternal(const std::string seriesUID) const;

private:
    void Dispose(bool isPatientDataOnly = false);

private:
    typedef std::map<std::string, ROIEntity*>    TpsROIMap;
    TpsROIMap                                    mRoiMap;
    std::vector<RtVoisetting*>                   mVoiSettings;
    PlaneCache* mVOIPlaneCacheInClipboard;
};

TPS_END_NAMESPACE

#endif //TPS_VOI_MANAGER_H_
