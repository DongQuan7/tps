////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_phantom_create.h
/// 
///  \brief class PhantomCreateCmd 
/// 
///  \version 1.0
/// 
///  \date    2015/10/26
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_PHANTOM_CREATE_H_
#define TPS_BL_CMD_PHANTOM_CREATE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "boost/date_time/posix_time/posix_time.hpp"

namespace Mcsf
{
    class IDataHeaderElementMap;
}

TPS_BEGIN_NAMESPACE;
                                                                              
class RtSeries;
class RtImage;
class RtStudy;
typedef boost::gregorian::date DATE_BOOST;

struct PhantomSeries
{
    string patientUID;
    string phantomName;
    string description;
    int xDim;
    int yDim;
    int zDim;
    float xSpacing;
    float ySpacing;
    float sliceSpacing;
    float phantomWidth;
    float phantomHeight;
    float phantomLength;
    float ctvalue;
};

class TPS_BL_DECLSPEC PhantomCreateCmd : public TpsCommand 
{
public:
    PhantomCreateCmd(PhantomSeries &phantomSeries);

    ~PhantomCreateCmd();

    std::string PhantomCreateCmd::GetPhantomSeriesUID();

protected:

    virtual int PreExecute();

    virtual int Execute();

    virtual int PostExecute();

private:
    bool SaveDcmFile(const std::string& sImageFilePath, Mcsf::IDataHeaderElementMap* const pDataHeaderImage);

    float Density2CT(float fDensity, const float* Ct2densityArray);

    int CreateOneImage(const RtSeries& series, RtImage* pImage);

    bool ConvertDcmFile2Cache(const std::string& sImageFilePath, char*& pBuffer, unsigned long* ulSize);

    int SaveImage(RtImage* pImage, Mcsf::IDataHeaderElementMap* pDataHeader, int index, float xStartPos, float yStartPos);
    bool CreateMultiFolder(const std::string& sFolderName) const;
    bool DeleteMultiFolder(const std::string& sFolderName) const;

	std::string ConvertDateTime(const DATE_BOOST& date);

private:
    std::string                 m_strNewPhantomUid;

    unsigned char*              m_pImageDataBuffer;
    int                         m_iImageDataBufferWidth;
    int                         m_iImageDataBufferHeight;

    PhantomSeries               m_PhantomSeries;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_PHANTOM_CREATE_H_