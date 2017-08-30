////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  SHI chunhui chunhui.shi@united-imaging.com
/// 
///  \file tps_rl_cornertextinforenderstrategy.cpp
/// 
///  \brief class CornerTextInfoRenderStrategy 
/// 
///  \version 1.0
/// 
///  \date    2014/06/19
////////////////////////////////////////////////////////////////
#include "StdAfx.h"

// ZhengHe
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"

#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_cornertextinforenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"


TPS_BEGIN_NAMESPACE

CornerTextInfoRenderStrategy::CornerTextInfoRenderStrategy() : mIsInitialed(false), 
    mMprGraphicObject(nullptr), mVolumeGraphicObject(nullptr),
    mCornerTextInfoGraphicObject(nullptr) {

}

CornerTextInfoRenderStrategy::~CornerTextInfoRenderStrategy(){

}

void CornerTextInfoRenderStrategy::SetMprGraphicObject(std::shared_ptr<MPRGraphicObject> mprGo) {
    mMprGraphicObject = mprGo;
}

void CornerTextInfoRenderStrategy::SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume) {
    mVolumeGraphicObject = volume;
}

void CornerTextInfoRenderStrategy::SetCornerTextInfoGraphicObject(std::shared_ptr<TpsTextOverlayGraphicObject> cornerTextInfo){
    mCornerTextInfoGraphicObject = cornerTextInfo;
}

bool CornerTextInfoRenderStrategy::Render(unsigned char*& buffer, 
    int bufWidth, int bufHeight) {
        if (buffer == nullptr || bufWidth <= 0 || bufHeight <= 0
            || mCornerTextInfoGraphicObject == nullptr) {
                TPS_LOG_DEV_ERROR<<"buffer == nullptr || bufWidth <= 0 || bufHeight <= 0";
                return false;
        }

        if (!mCornerTextInfoGraphicObject->GetVisible()) {
            // Hidden CornerText
            return true;
        }

        unsigned char* pStrBuffer = nullptr;
        int strBufHeight = 0;
        int strBufWidth = 0;
        std::vector<TextBlock*> allTextInfo;

        mCornerTextInfoGraphicObject->GetAllTextBlock(allTextInfo);

        for (std::vector<TextBlock*>::iterator itText = allTextInfo.begin();
             itText != allTextInfo.end(); ++itText) {

            pStrBuffer = (*itText)->mTextBuffer;
            
            if(nullptr == pStrBuffer){
                continue;
            }

            strBufHeight = (*itText)->mTextHeight;
            strBufWidth = (*itText)->mTextWidth;
            if (strBufHeight > bufHeight || strBufWidth > bufWidth){
                continue;
            }

            int left = (*itText)->Margin.mLeft;
            if ((*itText)->mHorizontalAlignment == ALIGH_RIGHT) {
                left = bufWidth - ((*itText)->Margin.mRight + strBufWidth);
            }

            int top = (*itText)->Margin.mTop;
            if((*itText)->mVerticalAlignment == ALIGN_BOTTOM) {
                top = bufHeight - ((*itText)->Margin.mBottom + strBufHeight);
            }

            //if(top + strBufHeight > bufHeight){
            //    strBufHeight = bufHeight - top;
            //}
            if(top < 0) top = 0;
            //if(left + strBufWidth > bufWidth){
            //    strBufWidth = bufWidth - left;
            //}
            if(left < 0) left = 0;

            //if(top + strBufHeight > bufHeight || top < 0
            //    || left + strBufWidth > bufWidth || left < 0){
            //        TPS_LOG_DEV_ERROR<<"Out of range!";
            //        return false;
            //}

            top = bufHeight - top - strBufHeight;
            if(top < 0) top = 0;
            unsigned char* pDstBuffer = buffer + (top*bufWidth+left)*4;
            unsigned char* pSrcBuffer = pStrBuffer;

            int adjustedHeight = (top + strBufHeight) > bufHeight ? bufHeight - top :strBufHeight;
            int adjustedWidth = (left + strBufWidth) > bufWidth ? bufWidth - left :strBufWidth;

            for (int m=0; m<adjustedHeight; ++m) {
                for (int n=0; n<adjustedWidth; ++n) {
                    if (pSrcBuffer[(m*strBufWidth+n)*4+3]!=0) {
                        pDstBuffer[(m*bufWidth+n)*4] = pSrcBuffer[(m*strBufWidth+n)*4];
                        pDstBuffer[(m*bufWidth+n)*4+1] = pSrcBuffer[(m*strBufWidth+n)*4+1];
                        pDstBuffer[(m*bufWidth+n)*4+2] = pSrcBuffer[(m*strBufWidth+n)*4+2];
                        pDstBuffer[(m*bufWidth+n)*4+3] = 255;
                    }
                }
            }
        }

        return true;
}
TPS_END_NAMESPACE
