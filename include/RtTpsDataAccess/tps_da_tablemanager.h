//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  Zuo Kai kai.zuo@united-imaging.com
///
///  \file    tps_da_tablemanager.h
///  \brief
///
///  \version 1.0
///  \date    01.29,2016
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_TABLE_MANAGER_H_
#define TPS_TABLE_MANAGER_H_

#include "tps_da_defs.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

TPS_BEGIN_NAMESPACE
class RtTable;
class SeriesTable;

class TPS_DA_DECLSPEC TpsTableManager : public TpsDataManagerBase
{
public:
    TpsTableManager(void);
    ~TpsTableManager(void);

    void AddRtTable(RtTable* table);
    RtTable* GetRtTable(const std::string& uid) const;
    void RemoveRtTable(const std::string& uid);
    std::vector<RtTable*> GetAllRtTables() const;

    void AddSeriesTable(const std::string& seriesUid, SeriesTable* seriesTable);
    SeriesTable* GetSeriesTable(const std::string& seriesUid) const;

    void ClearPatientData();
private:
    std::map<std::string, RtTable*> mOrgTableMap;
    //series uid, series table
    std::map<std::string, SeriesTable*> mSeriesTableMap;
};
TPS_END_NAMESPACE

#endif