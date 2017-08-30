////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_measurevaluegraphicobject.h
/// 
///  \brief class GalleryMeasureValGraphicObject definition 
/// 
///  \version 1.0
/// 
///  \date    2015/03/26
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_GALLERYMEASUREVALGRAPHICOBJECT_H_
#define TPS_RL_GALLERYMEASUREVALGRAPHICOBJECT_H_

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
//#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "tps_rl_defs.h"

TPS_BEGIN_NAMESPACE

struct MeasureValTextBlock {
    MeasureValTextBlock(){};

    MeasureValTextBlock(std::string textKey, unsigned char* strBuff, int strwidth, int strheight) {
        mTextKey = textKey;
        Margin.mLeft = 0;
        Margin.mTop = 0;

        mTextWidth = strwidth;
        mTextHeight = strheight;

        mTextBuffer = new unsigned char[strwidth * strheight * 4];
        memcpy(mTextBuffer, strBuff, strwidth * strheight * sizeof(unsigned char) * 4);
    }

    MeasureValTextBlock(std::string textKey, float left, float top, unsigned char* strBuff, int strwidth, int strheight) {
        mTextKey = textKey;
        Margin.mLeft = left;
        Margin.mTop = top;

        mTextWidth = strwidth;
        mTextHeight = strheight;

        mTextBuffer = new unsigned char[strwidth * strheight * 4];
        memcpy(mTextBuffer, strBuff, strwidth * strheight * sizeof(unsigned char) * 4);
    }

    MeasureValTextBlock(const MeasureValTextBlock& textBlock) {
        mTextKey = textBlock.mTextKey;
        Margin.mLeft = textBlock.Margin.mLeft;
        Margin.mTop = textBlock.Margin.mTop;
        mTextWidth = textBlock.mTextWidth;
        mTextHeight = textBlock.mTextHeight;

        mTextBuffer = new unsigned char[textBlock.mTextWidth * textBlock.mTextHeight * 4];
        memcpy(mTextBuffer, textBlock.mTextBuffer, textBlock.mTextWidth * textBlock.mTextHeight * sizeof(unsigned char) * 4);
    }

    ~MeasureValTextBlock(){
        if (mTextBuffer != nullptr) {
            delete []mTextBuffer;
            mTextBuffer = nullptr;
        }
    }

    struct {
        float mLeft; //in image CS (-1,-1)--->(1,1)
        float mTop;  //in image CS (-1,-1)--->(1,1)
    } Margin;

    std::string mTextKey;
    unsigned char *mTextBuffer;
    int mTextWidth;
    int mTextHeight;
};

class TPS_RL_DECLSPEC GalleryMeasureValGraphicObject : public TpsGraphicObjectBase {
public:
    GalleryMeasureValGraphicObject(void);
    ~GalleryMeasureValGraphicObject(void);

    virtual bool Initialize();
    virtual bool Finalize();

    void AddTextBlock(std::string key, MeasureValTextBlock *newTextBlock);
    void RemoveTextBlock(std::string key);
    void GetTextBlock(std::string key, MeasureValTextBlock **textBlock);
    void GetAllTextBlock(std::vector<MeasureValTextBlock*> &textBlocks);
    void UpdateTextBlock(std::string key, unsigned char* newTextBuffer, int newTextWidth, int newTextHeight);

private:
    std::map<std::string, MeasureValTextBlock*>   mTextOverlayQueryData;
    std::vector<MeasureValTextBlock*>             mTextOverlayData;
    boost::mutex                        mMutexTextObj;

    std::string                         mModalityType;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(GalleryMeasureValGraphicObject);
};

TPS_END_NAMESPACE

#endif
