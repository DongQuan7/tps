//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_da_normgroupmanager.h
///  \brief   entry off normGroup operator
///
///  \version 1.0
///  \date    Oct. 9, 2013
///
///  \modified by maoliang.gong(maoliang.gong@united-imaging.com)
///  \brief  refactor normGroup manager
///  \date    Jan. 13, 2014
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_normGroup_MANAGER_H
#define TPS_normGroup_MANAGER_H

#include "tps_da_defs.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

TPS_BEGIN_NAMESPACE
class RtNormgroup;
class RtDrrsetting;

class TPS_DA_DECLSPEC TpsNormGroupManager : public TpsDataManagerBase
{
public:
    TpsNormGroupManager(void);
    virtual ~TpsNormGroupManager(void);

    // \brief  get normGroupning beam group by uid in database
    // \param[in] uid  the normGroupning beam group uid in database
    // \param[out] normGroup  the output normGroupning beam group pointer
    // \return bool  true when succeeded, false otherwise
    bool GetNormGroup(const std::string& normGroupUid, 
                RtNormgroup** pNormGroup) const;

    // \brief  add normGroupning beam group by uid & beam group pair
    // \param[in] normGroup  the normGroupning beam group pointer
    // \return bool  true when succeeded, false otherwise
    bool AddNormGroup(RtNormgroup* normGroup);

    //////////////////////////////////////////////////////////////////////////
    // \brief  remove normGroupning beam group by uid in database
    // \param[in] uid  the normGroupning beam group uid in database
    // \return bool  true when succeeded, false otherwise
    void RemoveNormGroup(const std::string& uid);

    bool GetDrrSetting(const std::string& normGroupUid, 
        RtDrrsetting** pDrrsetting) const;

    bool AddDrrSetting(RtDrrsetting* drrsetting);

    void RemoveDrrSetting(const std::string& uid);

    // \brief  get dose list   by normGroup uid
    // \param[in] uid  the normGroupning beam group uid in database
    // \param[out] doseListUID  the output dose listuid
    // \return bool  true when succeeded, false otherwise
    bool GetDoseListUIDByNormGroupUID( std::string normGroupUID, std::vector<std::string>& doseListUID);

    bool AddDoseLineToNormGroup(std::string normGroupUID, std::vector<std::string> doseListUID);

    bool DeleteDoseLineToNormGroup(std::string normGroupUID,std::string doseLineUID);

    bool DeleteDoseLineToNormGroup(std::string normGroupUID);

    bool AddDoseLineToNormGroup(std::string normGroupUID,std::string doseLineUID);

    /// \brief  get Plan UID by Norm Group
    /// \param[in] uid  the normGroupning beam group uid in database
    /// \return Plan UID NOTE: may be empty
    std::string GetPlanUIDByNormGroupUID(const std::string& sNormGroupUID) const;

    void ClearPatientData();

    /// \brief  query normgroup uid list from memory NOT DB
    /// \param[in] const std::string& sPlanUID
    /// \return std::vector<std::string>
    std::vector<std::string> GetNormGroupUIDListByPlan(const std::string& sPlanUID) const;

    std::vector<RtNormgroup*> GetNormalGroupList(const std::string& sPlanUID) const;

    std::vector<std::string> FilterLoadedDoseGridUid(const std::vector<std::string>& doseGridUidList);

    bool AddNgDoseCalculateMode(const std::string& ngUid, RtDbDef::DoseCalculateMode doseCalculateMode);

    bool GetDoseCalculateModeViaNgUid(const std::string& ngUid, RtDbDef::DoseCalculateMode& doseCalculateMode);

    bool UpdateDoseCalculateMode(const std::string& ngUid, RtDbDef::DoseCalculateMode doseCalculateMode);

    void RemoveNormGroupDoseCalculateMode(const std::string& uid);


private:
    void Dispose();

private:

    // 用于存储normalgroup对应的剂量计算模式(lock mu/weight) 暂时不存数据库 wzd-2017-08-15
    std::map<std::string, RtDbDef::DoseCalculateMode> mNormalgroupDoseCalModeMap;
    std::map<std::string, RtNormgroup*> mNormGroupList;
    std::map<std::string, RtDrrsetting*> mDrrsettingList;
    typedef std::map<std::string, RtNormgroup*>::iterator mTpsNormGroupMapIt;
};

TPS_END_NAMESPACE
#endif //TPS_normGroup_MANAGER_H
