//////////////////////////////////////////////////////////////////////////
/// \defgroup TDA of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  yilong.cheng mailto:yilong.cheng@united-imaging.com
///
///  \file    tda_rl_pixelbufferbmpalphawindowingconverter.h
///  \brief   pixel buffer bmp and alpha and windowing converter
///
///  \version 1.0
///  \date   2014/10/29
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RL_PIXELBUFFERBMPALPHAWINDOWINGCONVERTER_
#define TPS_RL_PIXELBUFFERBMPALPHAWINDOWINGCONVERTER_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC PixelBufferBmpAlphaAndWindowingConverter {

public:
    PixelBufferBmpAlphaAndWindowingConverter();
    ~PixelBufferBmpAlphaAndWindowingConverter();
    /////////////////////////////////////////////////////////////////
    ///  \brief ImageRGBAndWindowingConverter
    ///
    ///  \param[in]  srcDcmBuffer and  srcWindowingBuffer has the same length and datatype;  
    ///             imageWindowingType:(true:need adjust the WW and WL)
    ///  \param[out]  unsigned char* dstBmpImage
    ///  \return   dstBmpImage
    ///  \exceptions:    transform from DICOM image to BMP image , alpha and windowing operation of the image
    /////////////////////////////////////////////////////////////////
    static bool DrrPixelBufferBmpAlphaAndWindowingConverter(
         bool isFixedPseudoColor,bool isPseudoColorReady,
        char* srcDcmBuffer, char *srcWindowingBuffer, unsigned char* fixPseudoColor,
        unsigned char* floatPseudoColor,int srcDcmBufferWidth, int srcDcmBufferHeight,float winWidth,
        float winCenter,float alpha,DATATYPE dataType,unsigned char* &dstBmpImage);
    /////////////////////////////////////////////////////////////////
    ///  \brief ImageRGBAndWindowingConverter
    ///
    ///  \param[in] char* srcDcmBuffer, char *srcWindowingBuffer, 
    ///             int srcDcmBufferWidth, int srcDcmBufferHeight,float winWidth, float winCenter, unsigned char* dstBmpImage,  DATATYPE dataType 
    ///  \param[out]  unsigned char* dstBmpImage
    ///  \return   dstBmpImage
    ///  \exceptions: srcDcmBuffer and  srcWindowingBuffer has the same length and datatype;  transform from DICOM image to BMP image , windowing operation of the image
    /////////////////////////////////////////////////////////////////
    static bool QaPixelBufferBmpAndWindowingConverter( 
        char* srcDcmBuffer, char *srcWindowingBuffer, 
        int srcDcmBufferWidth, int srcDcmBufferHeight,float winWidth,
        float winCenter, unsigned char* &dstBmpImage, DATATYPE dataType,double rescaleIntercept = 0);
    /////////////////////////////////////////////////////////////////
    ///  \brief PixelBufferWindowingConverter
    ///
    ///  \param[in] 
    ///              
    ///  \param[out]  unsigned char*&dstWindowingBuffer
    ///  \return   dstWindowingBuffer
    ///  \exceptions: 
    /////////////////////////////////////////////////////////////////
    static bool PixelBufferWindowingConverter( 
        char* srcDcmBuffer,int srcDcmBufferWidth, int srcDcmBufferHeight,float winWidth,
        float winCenter,DATATYPE dataType,unsigned char*&dstWindowingBuffer,double rescaleIntercept = 0);
    /////////////////////////////////////////////////////////////////
    ///  \brief WindowingBufferToRGBAConverter
    ///
    ///  \param[in] 
    ///              
    ///  \param[out]  unsigned char*&dstRGBABuffer
    ///  \return   dstRGBABuffer
    ///  \exceptions: 
    /////////////////////////////////////////////////////////////////
    static bool WindowingBufferToRGBAConverter( 
        unsigned char* srcWindowingBuffer,int srcWindowingBufferWidth, int srcWindowingBufferHeight,
        unsigned char*&dstRGBABuffer,float alpha = 1.0f);

};
TPS_END_NAMESPACE
#endif
