//////////////////////////////////////////////////////////////////////////
/// \defgroup TDA of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  yilong.cheng mailto:yilong.cheng@united-imaging.com
///
///  \file    tda_rl_pixelbufferbmpalphawindowingconverter.cpp
///  \brief   pixel buffer bmp and alpha and windowing converter
///
///  \version 1.0
///  \date   2014/10/29
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_pixelbufferbmpalphawindowingconverter.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps



PixelBufferBmpAlphaAndWindowingConverter::PixelBufferBmpAlphaAndWindowingConverter() {

}

PixelBufferBmpAlphaAndWindowingConverter::~PixelBufferBmpAlphaAndWindowingConverter() {

}

bool PixelBufferBmpAlphaAndWindowingConverter::DrrPixelBufferBmpAlphaAndWindowingConverter(
    bool isFixedPseudoColor,bool isPseudoColorReady,char* srcDcmBuffer, 
    char *srcWindowingBuffer, unsigned char* fixPseudoColor,unsigned char* floatPseudoColor,
    int srcDcmBufferWidth, int srcDcmBufferHeight,float winWidth,
    float winCenter,float alpha,DATATYPE dataType,unsigned char* &dstBmpImage) {
    if (dataType == DATA_TYPE_CHAR) {
        char *buffer_temp1=(char*)srcDcmBuffer;
        unsigned char *window = (unsigned char *)srcWindowingBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)((buffer_temp1[i]-winCenter)/winWidth +0.5f);
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                }  else {
                    window[i] = (unsigned char )(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)(buffer_temp1[i]-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }
        for(int i = 0; i <srcDcmBufferHeight ; ++i) {
            for(int j = 0; j<srcDcmBufferWidth; ++j) {
                unsigned char bufferwindow=*(window++ );
                if (isPseudoColorReady) {
                    if (isFixedPseudoColor) {
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = fixPseudoColor[bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = fixPseudoColor[bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = fixPseudoColor[bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }else{
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = floatPseudoColor[bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = floatPseudoColor[bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = floatPseudoColor[bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }
                } else {

                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                }
            }
        }
    } else if (dataType == DATA_TYPE_UNSIGNED_CHAR) {
        unsigned char *buffer_temp1=(unsigned char*)srcDcmBuffer;
        unsigned char *window = (unsigned char*)srcWindowingBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)((buffer_temp1[i]-winCenter)/winWidth +0.5f);
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                }  else {
                    window[i] = (unsigned char )(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)(buffer_temp1[i]-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }
        for(int i = 0; i <srcDcmBufferHeight ; ++i) {
            for(int j = 0; j<srcDcmBufferWidth; ++j) {
                unsigned char bufferwindow=*(window++ );
                if (isPseudoColorReady) {
                    if (isFixedPseudoColor) {
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = fixPseudoColor[bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = fixPseudoColor[bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = fixPseudoColor[bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }else{
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = floatPseudoColor[bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = floatPseudoColor[bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = floatPseudoColor[bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }
                } else {

                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                }
            }
        }
    } else if (dataType == DATA_TYPE_SHORT) {
        short *buffer_temp1=(short*)srcDcmBuffer;
        short *window = (short*)srcWindowingBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)((buffer_temp1[i]-winCenter)/winWidth +0.5f);
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                }  else {
                    window[i] = (short )(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)(buffer_temp1[i]-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }
        for(int i = 0; i <srcDcmBufferHeight ; ++i) {
            for(int j = 0; j<srcDcmBufferWidth; ++j) {
                short bufferwindow=*(window++ );
                if (isPseudoColorReady) {
                    if (isFixedPseudoColor) {
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = fixPseudoColor[bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = fixPseudoColor[bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = fixPseudoColor[bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }else{
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = floatPseudoColor[bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = floatPseudoColor[bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = floatPseudoColor[bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }
                } else {

                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                }
            }
        }
    } else if (dataType == DATA_TYPE_UNSIGNED_SHORT) {
        unsigned short *buffer_temp1=(unsigned short*)srcDcmBuffer;
        unsigned short *window = (unsigned short*)srcWindowingBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)((buffer_temp1[i]-winCenter)/winWidth +0.5f);
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                }  else {
                    window[i] = (unsigned short )(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)(buffer_temp1[i]-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }
        for(int i = 0; i <srcDcmBufferHeight ; ++i) {
            for(int j = 0; j<srcDcmBufferWidth; ++j) {
                unsigned short bufferwindow=*(window++ );
                if (isPseudoColorReady) {
                    if (isFixedPseudoColor) {
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = fixPseudoColor[bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = fixPseudoColor[bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = fixPseudoColor[bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }else{
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = floatPseudoColor[bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = floatPseudoColor[bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = floatPseudoColor[bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }
                } else {

                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                }
            }
        }
    } else if (dataType == DATA_TYPE_FLOAT) {
        float *buffer_temp1=(float*)srcDcmBuffer;
        float *window = (float*)srcWindowingBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)((buffer_temp1[i]-winCenter)/winWidth +0.5f);
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                }  else {
                    window[i] = (float )(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)(buffer_temp1[i]-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }
        for(int i = 0; i <srcDcmBufferHeight ; ++i) {
            for(int j = 0; j<srcDcmBufferWidth; ++j) {
                float bufferwindow=*(window++ );
                if (isPseudoColorReady) {
                    if (isFixedPseudoColor) {
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = fixPseudoColor[(int)bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = fixPseudoColor[(int)bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = fixPseudoColor[(int)bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }else{
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = floatPseudoColor[(int)bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = floatPseudoColor[(int)bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = floatPseudoColor[(int)bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }
                } else {

                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                }
            }
        }
    } else if (dataType == DATA_TYPE_DOUBLE) {
        double *buffer_temp1=(double*)srcDcmBuffer;
        double *window = (double*)srcWindowingBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)((buffer_temp1[i]-winCenter)/winWidth +0.5f);
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                }  else {
                    window[i] = (double)(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = (float)(buffer_temp1[i]-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    window[i] = 255;
                } else if(temp <= 0) {
                    window[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }
        for(int i = 0; i <srcDcmBufferHeight ; ++i) {
            for(int j = 0; j<srcDcmBufferWidth; ++j) {
                double bufferwindow=*(window++ );
                if (isPseudoColorReady) {
                    if (isFixedPseudoColor) {
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = fixPseudoColor[(int)bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = fixPseudoColor[(int)bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = fixPseudoColor[(int)bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }else{
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4] = floatPseudoColor[(int)bufferwindow * 3 + 0];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 1] = floatPseudoColor[(int)bufferwindow * 3 + 1];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = floatPseudoColor[(int)bufferwindow * 3 + 2];
                        dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                    }
                } else {

                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = (unsigned char)(alpha*255);
                }
            }
        }
    }
    return true;
}

bool PixelBufferBmpAlphaAndWindowingConverter::QaPixelBufferBmpAndWindowingConverter( 
    char* srcDcmBuffer, char *srcWindowingBuffer, int srcDcmBufferWidth, 
    int srcDcmBufferHeight,float winWidth, float winCenter,
    unsigned char* &dstBmpImage, DATATYPE dataType,double rescaleIntercept /*= 0*/ ) {
        if (dataType == DATA_TYPE_CHAR) {
            char *buffer_temp1=(char*)srcDcmBuffer;
            unsigned char *window = (unsigned char*)srcWindowingBuffer;
            if(winWidth > 1){
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)((buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f);
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    }  else {
                        window[i] = (unsigned char )(temp*255);
                    }
                } 
            }else if(1==winWidth) {
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)(buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f;
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    } 
                } 
            } else {
                TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
                return false;            
            }
            for(int i = 0; i <srcDcmBufferHeight ; ++i) {
                for(int j = 0; j<srcDcmBufferWidth; ++j) {
                    unsigned char bufferwindow=*(window++ );
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = 255;
                }
            }
        } else if (dataType == DATA_TYPE_UNSIGNED_CHAR) {
            unsigned char *buffer_temp1=(unsigned char*)srcDcmBuffer;
            unsigned char *window = (unsigned char*)srcWindowingBuffer;
            if(winWidth > 1){
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)((buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f);
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    }  else {
                        window[i] = (unsigned char )(temp*255);
                    }
                } 
            }else if(1==winWidth) {
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)(buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f;
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    } 
                } 
            } else {
                TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
                return false;            
            }
            for(int i = 0; i <srcDcmBufferHeight ; ++i) {
                for(int j = 0; j<srcDcmBufferWidth; ++j) {
                    unsigned char bufferwindow=*(window++ );
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = 255;
                }
            }
        } else if (dataType ==DATA_TYPE_SHORT) {
            short *buffer_temp1=(short*)srcDcmBuffer;
            short *window = (short*)srcWindowingBuffer;
            if(winWidth > 1){
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)((buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f);
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    }  else {
                        window[i] = (short)(temp*255);
                    }
                } 
            }else if(1==winWidth) {
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)(buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f;
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    } 
                } 
            } else {
                TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
                return false;            
            }
            for(int i = 0; i <srcDcmBufferHeight ; ++i) {
                for(int j = 0; j<srcDcmBufferWidth; ++j) {
                    short bufferwindow=*(window++ );
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = 255;
                }
            }
        } else if (dataType == DATA_TYPE_UNSIGNED_SHORT) {
            unsigned short *buffer_temp1=(unsigned short*)srcDcmBuffer;
            unsigned short *window = (unsigned short*)srcWindowingBuffer;
            if(winWidth > 1){
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)((buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f);
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    }  else {
                        window[i] = (unsigned short)(temp*255);
                    }
                } 
            }else if(1==winWidth) {
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)(buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f;
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    } 
                } 
            } else {
                TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
                return false;            
            }
            for(int i = 0; i <srcDcmBufferHeight ; ++i) {
                for(int j = 0; j<srcDcmBufferWidth; ++j) {
                    unsigned short bufferwindow=*(window++ );
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = 255;
                }
            }

        } else if (dataType == DATA_TYPE_FLOAT) {
            float *buffer_temp1=(float*)srcDcmBuffer;
            float *window = (float*)srcWindowingBuffer;
            if(winWidth > 1){
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)((buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f);
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    }  else {
                        window[i] = (float)(temp*255);
                    }
                } 
            }else if(1==winWidth) {
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)(buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f;
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    } 
                } 
            } else {
                TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
                return false;            
            }
            for(int i = 0; i <srcDcmBufferHeight ; ++i) {
                for(int j = 0; j<srcDcmBufferWidth; ++j) {
                    float bufferwindow=*(window++ );
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = 255;
                }
            }
        } else if (dataType == DATA_TYPE_DOUBLE) {
            double *buffer_temp1=(double*)srcDcmBuffer;
            double *window = (double*)srcWindowingBuffer;
            if(winWidth > 1){
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)((buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f);
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    }  else {
                        window[i] = (double)(temp*255);
                    }
                } 
            }else if(1==winWidth) {
                for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                    float temp = (float)(buffer_temp1[i]+rescaleIntercept-winCenter)/winWidth +0.5f;
                    if(temp >= 1) {
                        window[i] = 255;
                    } else if(temp <= 0) {
                        window[i] = 0;
                    } 
                } 
            } else {
                TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
                return false;            
            }
            for(int i = 0; i <srcDcmBufferHeight ; ++i) {
                for(int j = 0; j<srcDcmBufferWidth; ++j) {
                    double bufferwindow=*(window++ );
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 +1] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 2] = (unsigned char)bufferwindow;
                    dstBmpImage[(srcDcmBufferWidth *i+j)*4 + 3] = 255;
                }
            }
        }
        //transform the orientation of the window show
            unsigned char *bufferTemp = new unsigned char[srcDcmBufferHeight * srcDcmBufferWidth *4];
        memcpy(bufferTemp,dstBmpImage,srcDcmBufferHeight*srcDcmBufferWidth*sizeof(unsigned char) * 4);
        int iInverse = 0;
        for(int i = 0; i <srcDcmBufferHeight ; ++i) {
            for(int j =0 ; j < srcDcmBufferWidth ; ++ j) {
                iInverse = srcDcmBufferHeight - i - 1;
                dstBmpImage[(i*srcDcmBufferWidth + j)*4] = bufferTemp[(iInverse*srcDcmBufferWidth + j)*4];
                dstBmpImage[(i*srcDcmBufferWidth + j)*4+1] = bufferTemp[(iInverse*srcDcmBufferWidth + j)*4+1];
                dstBmpImage[(i*srcDcmBufferWidth + j)*4+2] = bufferTemp[(iInverse*srcDcmBufferWidth + j)*4+2];
                dstBmpImage[(i*srcDcmBufferWidth + j)*4+3] = bufferTemp[(iInverse*srcDcmBufferWidth + j)*4+3];

            }
        }
        delete [] bufferTemp;
        bufferTemp = nullptr;
        return true;
}

bool PixelBufferBmpAlphaAndWindowingConverter::PixelBufferWindowingConverter( char* srcDcmBuffer,
    int srcDcmBufferWidth, int srcDcmBufferHeight,float winWidth, 
    float winCenter,DATATYPE dataType, unsigned char*&dstWindowingBuffer,
    double rescaleIntercept /*= 0*/ ) {
    float fIntercept = float(rescaleIntercept);
    if (dataType == DATA_TYPE_CHAR) {
        char *buffer_temp1=(char*)srcDcmBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1.0f) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0.0f) {
                    dstWindowingBuffer[i] = 0;
                }  else {
                    dstWindowingBuffer[i] = (unsigned char )(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0) {
                    dstWindowingBuffer[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }

    } else if (dataType == DATA_TYPE_UNSIGNED_CHAR) {
        unsigned char *buffer_temp1=(unsigned char*)srcDcmBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0) {
                    dstWindowingBuffer[i] = 0;
                }  else {
                    dstWindowingBuffer[i] = (unsigned char )(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0) {
                    dstWindowingBuffer[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }
    } else if (dataType ==DATA_TYPE_SHORT) {
        short *buffer_temp1=(short*)srcDcmBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0) {
                    dstWindowingBuffer[i] = 0;
                }  else {
                    dstWindowingBuffer[i] = (unsigned char )(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0) {
                    dstWindowingBuffer[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }
    } else if (dataType == DATA_TYPE_UNSIGNED_SHORT) {
        unsigned short *buffer_temp1=(unsigned short*)srcDcmBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0) {
                    dstWindowingBuffer[i] = 0;
                }  else {
                    dstWindowingBuffer[i] = (unsigned char )(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0) {
                    dstWindowingBuffer[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }

    } else if (dataType == DATA_TYPE_FLOAT) {
        float *buffer_temp1=(float*)srcDcmBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0) {
                    dstWindowingBuffer[i] = 0;
                }  else {
                    dstWindowingBuffer[i] = (unsigned char )(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0) {
                    dstWindowingBuffer[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }
    } else if (dataType == DATA_TYPE_DOUBLE) {
        double *buffer_temp1=(double*)srcDcmBuffer;
        if(winWidth > 1){
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0) {
                    dstWindowingBuffer[i] = 0;
                }  else {
                    dstWindowingBuffer[i] = (unsigned char )(temp*255);
                }
            } 
        }else if(1==winWidth) {
            for(int i = 0; i<srcDcmBufferWidth*srcDcmBufferHeight; ++i) {
                float temp = ((float)buffer_temp1[i]+fIntercept-winCenter)/winWidth +0.5f;
                if(temp >= 1) {
                    dstWindowingBuffer[i] = 255;
                } else if(temp <= 0) {
                    dstWindowingBuffer[i] = 0;
                } 
            } 
        } else {
            TPS_LOG_DEV_ERROR<<"the windowing width is wrong";
            return false;            
        }
    }
    return true;
}

bool PixelBufferBmpAlphaAndWindowingConverter::WindowingBufferToRGBAConverter(
    unsigned char* srcWindowingBuffer,int srcWindowingBufferWidth, 
    int srcWindowingBufferHeight, unsigned char*&dstRGBABuffer,float alpha /*= 1.0f*/  ) {
        for(int i = 0; i <srcWindowingBufferHeight ; ++i) {
            for(int j = 0; j<srcWindowingBufferWidth; ++j) {
                unsigned char bufferwindow=*(srcWindowingBuffer++);
                dstRGBABuffer[(srcWindowingBufferWidth *i+j)*4] = bufferwindow;
                dstRGBABuffer[(srcWindowingBufferWidth *i+j)*4 +1] = bufferwindow;
                dstRGBABuffer[(srcWindowingBufferWidth *i+j)*4 + 2] = bufferwindow;
                dstRGBABuffer[(srcWindowingBufferWidth *i+j)*4 + 3] =(unsigned char ) alpha*255;
            }
        }
        //    transform the orientation of the window show
        //unsigned char *bufferTemp = new unsigned char[srcWindowingBufferHeight * srcWindowingBufferWidth *4];
        //memcpy(bufferTemp,dstRGBABuffer,srcWindowingBufferHeight*srcWindowingBufferWidth*sizeof(unsigned char) * 4);
        //int iInverse = 0;
        //for(int i = 0; i <srcWindowingBufferHeight ; ++i) {
        //    for(int j =0 ; j < srcWindowingBufferWidth ; ++ j) {
        //        iInverse = srcWindowingBufferHeight - i - 1;
        //        dstRGBABuffer[(i*srcWindowingBufferWidth + j)*4] = bufferTemp[(iInverse*srcWindowingBufferWidth + j)*4];
        //        dstRGBABuffer[(i*srcWindowingBufferWidth + j)*4+1] = bufferTemp[(iInverse*srcWindowingBufferWidth + j)*4+1];
        //        dstRGBABuffer[(i*srcWindowingBufferWidth + j)*4+2] = bufferTemp[(iInverse*srcWindowingBufferWidth + j)*4+2];
        //        dstRGBABuffer[(i*srcWindowingBufferWidth + j)*4+3] = 255;

        //    }
        //}
        //delete [] bufferTemp;
        //bufferTemp = nullptr;
        return true;
}

TPS_END_NAMESPACE  // end namespace tps