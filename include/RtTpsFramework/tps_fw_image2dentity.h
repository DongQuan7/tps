//////////////////////////////////////////////////////////////////////////
/// \defgroup  TDA of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  yilong.cheng mailto:yilong.cheng@united-imaging.com
///
///  \file    tps_da_Image2DEntity.h
///  \brief   Image2DEntity interface
///
///  \version 1.0
///  \date    2014-10-29
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_DA_IMAGE2DENTITY_H_
#define TPS_DA_IMAGE2DENTITY_H_

#include "tps_fw_defs.h"

#include <map>

namespace Mcsf{
    class Matrix3f;
}

TPS_BEGIN_NAMESPACE
class RtRtimage;

class TPS_FW_DECLSPEC TpsImage2DEntity{
public:
    //the constructor and destructor
    TpsImage2DEntity();
    ~TpsImage2DEntity();

    //////////////////////////////////////////////////////////////////////////
    // \brief get TMS image 2d of the entity
    // \return RTFWK::RTFWKDLDOImage2D* TMS image 2d pointer
    void SetRTImage(RtRtimage* pRtImage);

    //////////////////////////////////////////////////////////////////////////
    // \brief get TMS image 2d of the entity
    // \return RtRtimage* TMS image 2d pointer
    RtRtimage* get() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief get TMS image data buffer
    // \param[out] buffer  the buffer pointer output
    // \return bool true if succeeded, false otherwise
    bool GetImage2DBuffer(char*& buffer);

    //////////////////////////////////////////////////////////////////////////
    // \brief get TMS image 2d UID
    // \param[out] uid  the uid output
    // \return bool true if succeeded, false otherwise
    bool GetUID(std::string& uid) const;

    //////////////////////////////////////////////////////////////////////////
    // \brief read image 2d from specified file
    // \param[in] sFilePath  the file path
    // \return bool true if succeeded, false otherwise
    bool ReadImage2DFromFile(const std::string& sFilePath);
    //////////////////////////////////////////////////////////////////////////
    // \brief set registration matrix through specified image 2d UID & matrix
    // \param[in] sDrrFloatUID, the float image data UID
    // \param[in] matrix  the registration matrix, float to fixed image model
    // \return bool true if succeeded, false otherwise
    bool SetRegistrationMatrix(const std::string &sDrrFloatUID,
        Mcsf::Matrix3f &matrix);
    //////////////////////////////////////////////////////////////////////////
    // \brief get registration matrix through specified image 2d UID & matrix
    // \param[in] sDrrFloatUID, the float image data UID
    // \param[in] matrix  the registration matrix, float to fixed image model.
    // \return bool true if succeeded, false otherwise
    bool GetRegistrationMatrix(const std::string &sDrrFloatUID,
        Mcsf::Matrix3f &matrix);
    //////////////////////////////////////////////////////////////////////////
    // \brief write image 2d to specified file
    // \param[in] sFilePath  the file path
    // \return bool true if succeeded, false otherwise
    bool WriteImage2DToFile(const std::string& sFilePath) const;
    unsigned long GetBufferLen();
    bool GetImageSize(int &width,int &height);
    bool GetImageWindowingInfo(int &winCenter, int &winWidth);

private:
    RtRtimage*        mImage2D;
    std::map<std::string, Mcsf::Matrix3f>    mRegistrationMatrixMap;
    unsigned long mBufferLen;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsImage2DEntity);
};
TPS_END_NAMESPACE  // end namespace tps

#endif