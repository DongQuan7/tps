//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  kai.zuo (mailto:kai.zuo@united-imaging.com)
///
///  \file   tps_bl_cmd_addqaplan.h 
///  \brief   add qa plan
///
///  \version 1.0
///  \date  2016/05/24
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_COPYPHANTOMTOQA_H_
#define TPS_BL_CMD_COPYPHANTOMTOQA_H_

#include <map>

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/filesystem.hpp"//create directory
#include "McsfDataHeader/mcsf_data_header_defs.h"

namespace Mcsf
{
    class IDataHeaderElementMap;
}

TPS_BEGIN_NAMESPACE;

class RtSeries;
typedef boost::gregorian::date DATE_BOOST;

class TPS_BL_DECLSPEC TpsCopyPhantomToQACmd : public TpsCommand {
public:
    TpsCopyPhantomToQACmd(const std::string& seriesUid);
    virtual ~TpsCopyPhantomToQACmd();
    void GetQASeries(const RtSeries*& pSeries);
    void Dispose();
protected:
    virtual int Execute();

private:
    bool CopyVoisBySeries_i(const std::string& phantomSeriesUid, const std::string& seriesUid, std::map<std::string, std::string>& imageUidMap);
    bool CopyPoisBySeries_i(const std::string& phantomSeriesUid, const std::string& seriesUid);
    bool CopyImagesBySeries_i(const std::string& phantomSeriesUid, const RtSeries& qaSeries, std::map<std::string, std::string>& imageUidMap);
    bool CreateMultiFolder_i(const std::string& sFolderName) const;
    bool ConvertDcmFile2Cache_i(const std::string& sImageFilePath, char*& pBuffer, unsigned long* ulSize);
    bool DeleteMultiFolder_i(const std::string& sFolderName) const;
    std::string ConvertDateTime_i(const DATE_BOOST& date);
    bool ResetDicomData_i(Mcsf::IDataHeaderElementMap* dataHeaderPtr, const Mcsf::Tag tag, const std::string& sBufferString);

private:
    std::string         mPhantomSeriesUid;
    RtSeries*           mQASeries;
};

TPS_END_NAMESPACE

#endif