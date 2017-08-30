////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  RONG Chengcheng  chengcheng.rong@united-imaging.com
/// 
///  \file tps_da_tpsmask.h
/// 
///  \brief class TPSMask (2D) declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/07
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_TPSMASK_H_
#define TPS_DA_TPSMASK_H_

#include "tps_fw_defs.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TPSMask
/// 
/// \brief *****
class TPS_FW_DECLSPEC TPSMask
{
public:
    //the constructor and destructor
    TPSMask();
    TPSMask(size_t xdim, size_t ydim);
    TPSMask(const TPSMask& mask);
    void operator = (const TPSMask& mask);
    ~TPSMask();

    // get size of mask
    void GetMaskSize(size_t& xdim, size_t& ydim) const;
    // get data pointer of mask
    unsigned char* GetMaskBuffer() const;
    // set mask data
    void SetMaskData(unsigned char* pData, size_t xdim, size_t ydim);
    // clear the memory

    void SetMaskID(unsigned char maskID);
    unsigned char GetMaskID() const;

    void Clear();

    //save mask to true color bitmap, copy codes from ZHENGHE 3D
    bool SaveToTrueColorBitmap(const std::string& sPath);

private:
    // mask buffter
    unsigned char*      mBuffer;
    // size of buffer
    size_t              mSize[2];
    //mask ID
    unsigned char       mMaskID;

private:
    //save mask to true color bitmap, copy codes from ZHENGHE 3D
    bool SaveToTrueColorBitmap_i(const std::string& sPath, unsigned char* pPixelBuffer, 
        const int& nImageHeight, const int& nImageWidth);

    // the default copy and assign method are not allowed.
    //TPS_DISALLOW_COPY_AND_ASSIGN(TPSMask);
};

TPS_END_NAMESPACE  // end namespace tps

#endif