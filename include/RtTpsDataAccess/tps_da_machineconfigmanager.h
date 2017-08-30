//////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    ms_configuration_manager.h
///  \brief   entry off configuration operator
///
///  \version 1.0
///  \date    Dec. 3, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef MS_MACHINEINFOMANAGER11_H
#define MS_MACHINEINFOMANAGER11_H

#include "tps_da_defs.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

TPS_BEGIN_NAMESPACE

class IRtDatabaseWrapper;
class RtMachine;
class RtCommissionedunit;
class RtTray;
class RtMcphocommissionedunit;
class RtMeasuredprofile;
class RtWedgefactor;
class RtApplicator;
class RtKernelweight;
class RtMeasureddata;
class RtCommissionedunit_tray;
class RtGoldenstt;
class RtAccessory;
class RtContour;
class RtAccessorychunk;

class TPS_DA_DECLSPEC TpsMachineSettingManager : public TpsDataManagerBase{

public:

    TpsMachineSettingManager(void);
    virtual ~TpsMachineSettingManager(void);
    bool LoadMachineSetting();

    bool ClearMachineSettingData();
    bool ReadFile_i(std::string sFilePath, std::string& fileContent);
    bool WriteFile_i(std::string sFilePath, std::string& fileContent);
    void MeasureProfileDataFormat(std::vector<RtMeasuredprofile*>& measuredProfileList);
    void KernelWeightDataFormat(std::vector<RtKernelweight*>& vkernelWeightList);
    bool ParseStringToVector(std::string& kernelweightfilepath, std::string& kernelidlistfilepath,std::vector<float>& vecFValue, std::vector<int>& vecIValue);
  

    template<class Table>
    Table* GetTable()
    {
        auto it = mtableMap.find(typeid(Table).name());
        if (it == mtableMap.end())
        {
            Table* table = new Table();
            mtableMap[typeid(Table).name()] = table;
            return table;
        }
        return (Table*)(*it).second;
    }
private:

    IRtDatabaseWrapper          *mDatabaseWrapper;
    std::map<std::string, void*> mtableMap;

};

TPS_END_NAMESPACE

#endif //MS_CONFIGURATION_H
