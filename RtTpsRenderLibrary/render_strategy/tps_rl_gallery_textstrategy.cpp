////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_textstrategy.cpp
/// 
///  \brief class GalleryTextStrategy 
/// 
///  \version 1.01
/// 
///  \date    2015/06/15
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
#include "RtTpsRenderLibrary/tps_rl_gallery_textstrategy.h"
//#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_textgraphicobject.h"


TPS_BEGIN_NAMESPACE

GalleryTextStrategy::GalleryTextStrategy() : mIsInitialed(false), mIsDirty(false),
    mTextGraphicObject(nullptr) {

}

GalleryTextStrategy::~GalleryTextStrategy(){

}


void GalleryTextStrategy::SetTextGraphicObject(std::shared_ptr<GalleryTextGraphicObject> textGO){
    mTextGraphicObject = textGO;
}

Mcsf::MedViewer3D::Point2D GalleryTextStrategy::ImageCoordinateToScreenCoordinate
    (Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight)
{
    int iMinL = std::min(iWidth, iHeight);

    Mcsf::MedViewer3D::Point2D anglePointOut(0, 0);
    anglePointOut.x = (anglePoint.x / 2) * iMinL / iWidth + 0.5;
    anglePointOut.y = (anglePoint.y / 2) * iMinL / iHeight + 0.5;

    return anglePointOut;

}

bool GalleryTextStrategy::Render(unsigned char*& buffer, 
    int bufWidth, int bufHeight) {
        if (buffer == nullptr || bufWidth <= 0 || bufHeight <= 0
            || mTextGraphicObject == nullptr) {
                TPS_LOG_DEV_ERROR<<"buffer == nullptr || bufWidth <= 0 || bufHeight <= 0";
                return false;
        }

        unsigned char* pStrBuffer = nullptr;
        int strBufHeight = 0;
        int strBufWidth = 0;
        std::vector<GalleryTextBlock*> allTextInfo;

        mTextGraphicObject->GetAllTextBlock(allTextInfo);

        for (std::vector<GalleryTextBlock*>::iterator itText = allTextInfo.begin();
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

                float leftInImageCoordinate = (*itText)->Margin.mLeft;
                //if ((*itText)->mHorizontalAlignment == ALIGH_RIGHT) {
                //    left = bufWidth - ((*itText)->Margin.mRight + strBufWidth);
                //}

                float bottomInImageCoordinate = 1 - ((*itText)->Margin.mTop);
                //if((*itText)->mVerticalAlignment == ALIGN_BOTTOM) {
                //    top = bufHeight - ((*itText)->Margin.mBottom + strBufHeight);
                //}

                //if(top + strBufHeight > bufHeight){
                //    strBufHeight = bufHeight - top;
                //}

                Mcsf::MedViewer3D::Point2D pos(leftInImageCoordinate, bottomInImageCoordinate);
                Mcsf::MedViewer3D::Point2D posInScreenCoordinate = 
                    ImageCoordinateToScreenCoordinate(pos, bufWidth, bufHeight);

                int left = posInScreenCoordinate.x * bufWidth;
                int top = (1 - posInScreenCoordinate.y) * bufHeight;

                if(top < 0) top = 0;
                if(top > bufHeight - strBufHeight) top = bufHeight - strBufHeight;
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
