////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_da_machinemanager.h
/// 
///  \brief class TpsMachineManager declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/26
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_MACHINEMANAGER_H_
#define TPS_DA_MACHINEMANAGER_H_

#include "tps_da_defs.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

struct CertifiedUnit;
class RtMachine;
class RtCommissionedunit;
class RtApplicator;
class RtAccessory;
class RtAccessorychunk;
class RtKerneldata;
class RtKerneldatalist;
class RtKernelweight;
class RtTray;
class RtContour;
class RtGoldenstt;
class RtMcphocommissionedunit;
class RtMcphomonoenergyweight;
class RtApproverecord;

/// \class TpsMachineManager
/// 
/// \brief *****
class TPS_DA_DECLSPEC TpsMachineManager : public TpsDataManagerBase
{
public:
    //the constructor and destructor
    TpsMachineManager();
    ~TpsMachineManager();

    //////////////////////////////////////////////////////////////////////////
    // \brief get unit via UID
    // \param[in] uid  the UID of unit in database
    // \param[out] machine  the unit pointer wanted
    // \return bool true if succeeded, false if not exist
    bool GetCommissionedUnit(const std::string unitUID, RtCommissionedunit*& commissionedUnit);
    void GetAllCommissionedUnit(std::vector<RtCommissionedunit*>& vCommissionedUnits);
    void GetSpecifiedCommissonedUnit(
        const std::string& machineUid,
        int radiationType,
        int techniqueType,
        std::vector<RtCommissionedunit*>& vCommissionedUnits);
    void AddCommissonedUnit(const std::string unitUID, RtCommissionedunit* commissionedUnit);

    //////////////////////////////////////////////////////////////////////////
    // \brief get machine via UID
    // \param[in] uid  the UID of machine in database
    // \param[out] machine  the machine pointer wanted
    // \return bool true if succeeded, false if not exist
    bool GetMachine(const std::string& uid, RtMachine*& machine);
    void GetAllMachine(std::vector<RtMachine*>& vMachines);
    void AddMachine(const std::string& machineUID, RtMachine* &machine);

    //////////////////////////////////////////////////////////////////////////
    // \brief get accessory by uid
    // \param[in] accessoryUID  the UID to be queried
    // \param[out] accessory  result accessory pointer
    // \return bool true if succeeded, false if not exist
    bool GetAccessory(const std::string& accessoryUID, RtAccessory*& accessory);
    // \brief get all accessory UID
    // \param[out] accessoryList  <the UID of commission , the accessory list of commission unit>
    void GetAllAccessory(std::vector<RtAccessory*>& accessoryList);
    void AddAccessory(RtAccessory* accessory);

    bool GetApplicator(const std::string& sApplicatorUID, RtApplicator*& pApplicator) const;
    void AddApplicator(const std::string& sApplicatorUID, RtApplicator* pApplicator);


    void AddAccessoryChunk(RtAccessorychunk* accessoryChunk);
    void GetAllAccessoryChunk(std::vector<RtAccessorychunk*>& accessoryChunkList);

    void AddGoldenstt(RtGoldenstt* goldenstt);
    void GetAllGoldenstt(std::vector<RtGoldenstt*>& goldensttList);

    void AddKerneldata(RtKerneldata* kerneldata);
    void GetAllKerneldata(std::vector<RtKerneldata*>& vKerneldata);

    void AddKerneldatalist(RtKerneldatalist* kerneldatalist);
    void GetAllKerneldatalist(std::vector<RtKerneldatalist*>& vKerneldatalist);

    void AddKernelweight(RtKernelweight* kernelweight);
    void GetAllKernelweight(std::vector<RtKernelweight*>& vKernelweight);

    void AddTray(RtTray* tray);
    void GetAllTray(std::vector<RtTray*>& vTray);

    void AddMcphocommissionedunit(RtMcphocommissionedunit* mcphocommissionedunit);
    void GetAllMcphocommissionedunit(std::vector<RtMcphocommissionedunit*>& vMcphocommissionedunit);

    void AddMcphomonoenergyweight(
        const std::string& sMcphocommissionedunitUid, 
        std::vector<RtMcphomonoenergyweight*>& vMcphomonoenergyweight);

    void GetMcphomonoenergyweightListByMcphocommissionedunituid(
        const std::string& sMcphocommissionedunitUid,
        std::vector<RtMcphomonoenergyweight*>& vMcphomonoenergyweight);


    void AddAccessoryChunkProfile(RtContour* profile);
    void GetAllAccessoryChunkProfile(std::vector<RtContour*>& vProfile);

    int GetMachineUnitAccessory(const std::string& machineUID,
        RtMachine*& machine,
        std::vector<RtCommissionedunit*>& vCommissionedunitList,
        std::vector<RtAccessorychunk*>& vRtAccessorychunkList,
        std::vector<RtGoldenstt*>& vRtGoldensttList,
        std::vector<RtAccessory*>& vRtAccessoryList);

    void AddApproveRecord(RtApproverecord* approverecord);
    void GetAllApproveRecord(std::vector<RtApproverecord*>& approveRecordList);


    void SetLeafGapOffsetInSmallField(const float& leafGapOffsetInSmallField);
    float GetLeafGapOffsetInSmallField() const;

    void SetLeafGapOffsetInLargeField(const float& leafGapOffsetInLargeField);
    float GetLeafGapOffsetInLargeField() const;

private:
    void Dispose();

private:
    std::map<std::string, RtMachine*>                                 mMachineMap;
    std::map<std::string, RtCommissionedunit*>                        mCommissonedUnitMap;
    std::map<std::string, RtAccessory*>                               mAccessoryMap;
    std::map<std::string, RtAccessorychunk*>                          mAccessoryChunkMap;
    std::map<std::string, RtApplicator*>                              mApplicatorMap;
    std::map<std::string, RtKerneldata*>                              mKerneldataMap;
    std::map<std::string, RtKerneldatalist*>                          mKerneldatalistMap;
    std::map<std::string, RtKernelweight*>                            mKernelweightMap;
    std::map<std::string, RtTray*>                                    mTrayMap;
    std::map<std::string, RtContour*>                                 mAccessoryChunkProfileMap;
    std::map<std::string, RtMcphocommissionedunit*>                   mMcphocommissionedunitMap;
    std::map<std::string, std::vector<RtMcphomonoenergyweight*>>      mMcphomonoenergyweightMap;
    std::map<std::string, RtGoldenstt*>                               mGoldenSttMap;
    std::map<std::string, RtApproverecord*>                           mApproveRecordMap;

    // wzd-2017-04-25 该参数应该挂在machine下，数据库未更新，临时方案
    float                           mLeafGapOffsetInSmallField;
    float                           mLeafGapOffsetInLargeField;
};

TPS_END_NAMESPACE  // end namespace tps

#endif
