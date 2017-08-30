////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Xi.Chen xi.chen@united-imaging.com
/// 
///  \file tps_rl_textoverlaygraphicobject.h
/// 
///  \brief class TpsTextOverlayGraphicObject definition 
/// 
///  \version 1.0
/// 
///  \date    2014/06/23
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_TEXTOVERLAYGRAPHICOBJECT_H_
#define TPS_RL_TEXTOVERLAYGRAPHICOBJECT_H_

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
//#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "tps_rl_defs.h"

TPS_BEGIN_NAMESPACE

enum HorizontalAlignment {
        ALIGN_LEFT = 0,
        ALIGH_RIGHT
};

enum VerticalAlignment{
    ALIGN_TOP = 0,
    ALIGN_BOTTOM
};

struct TextBlock {
    TextBlock(){};

    TextBlock(std::string textKey, unsigned char* strBuff, int strwidth, int strheight) {
        mTextKey = textKey;
        Margin.mLeft = 0;
        Margin.mTop = 0;
        Margin.mRight = 0;
        Margin.mBottom = 0;
        mHorizontalAlignment = ALIGN_LEFT;
        mVerticalAlignment = ALIGN_TOP;
        mTextWidth = strwidth;
        mTextHeight = strheight;

        mTextBuffer = new unsigned char[strwidth * strheight * 4];
        memcpy(mTextBuffer, strBuff, strwidth * strheight * sizeof(unsigned char) * 4);
    }

    TextBlock(std::string textKey, int left, int top, int right, int bottom, unsigned char* strBuff, int strwidth, int strheight) {
        mTextKey = textKey;
        Margin.mLeft = left;
        Margin.mTop = top;
        Margin.mRight = right;
        Margin.mBottom = bottom;
        mHorizontalAlignment = ALIGN_LEFT;
        mVerticalAlignment = ALIGN_TOP;
        mTextWidth = strwidth;
        mTextHeight = strheight;

        mTextBuffer = new unsigned char[strwidth * strheight * 4];
        memcpy(mTextBuffer, strBuff, strwidth * strheight * sizeof(unsigned char) * 4);
    }

    TextBlock(std::string textKey, int left, int top, int right, int bottom, unsigned char* strBuff, int strwidth, int strheight,
        HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment) {
            mTextKey = textKey;
            Margin.mLeft = left;
            Margin.mTop = top;
            Margin.mRight = right;
            Margin.mBottom = bottom;
            mTextWidth = strwidth;
            mTextHeight = strheight;
            mHorizontalAlignment = horizontalAlignment;
            mVerticalAlignment = verticalAlignment;

            mTextBuffer = new unsigned char[strwidth * strheight * 4];
            memcpy(mTextBuffer, strBuff, strwidth * strheight * sizeof(unsigned char) * 4);
    }

    TextBlock(const TextBlock& textBlock) {
        mTextKey = textBlock.mTextKey;
        Margin.mLeft = textBlock.Margin.mLeft;
        Margin.mTop = textBlock.Margin.mTop;;
        Margin.mRight = textBlock.Margin.mRight;;
        Margin.mBottom = textBlock.Margin.mBottom;;
        mTextWidth = textBlock.mTextWidth;
        mTextHeight = textBlock.mTextHeight;
        mHorizontalAlignment = textBlock.mHorizontalAlignment;
        mVerticalAlignment = textBlock.mVerticalAlignment;

        mTextBuffer = new unsigned char[textBlock.mTextWidth * textBlock.mTextHeight * 4];
        memcpy(mTextBuffer, textBlock.mTextBuffer, textBlock.mTextWidth * textBlock.mTextHeight * sizeof(unsigned char) * 4);
    }

    ~TextBlock(){
        if (mTextBuffer != nullptr) {
            delete []mTextBuffer;
            mTextBuffer = nullptr;
        }
    }

    HorizontalAlignment mHorizontalAlignment;
    VerticalAlignment mVerticalAlignment;

    struct {
        int mLeft;
        int mTop;
        int mRight;
        int mBottom;
    } Margin;

    std::string mTextKey;
    unsigned char *mTextBuffer;
    int mTextWidth;
    int mTextHeight;
};

class TPS_RL_DECLSPEC TpsTextOverlayGraphicObject : public TpsGraphicObjectBase {
public:
    TpsTextOverlayGraphicObject(void);
    ~TpsTextOverlayGraphicObject(void);

    virtual bool Initialize();
    virtual bool Finalize();

    void AddTextBlock(std::string key, TextBlock *newTextBlock);
    void RemoveTextBlock(std::string key);
    void GetTextBlock(std::string key, TextBlock **textBlock);
    void GetAllTextBlock(std::vector<TextBlock*> &textBlocks);
    void UpdateTextBlock(std::string key, unsigned char* newTextBuffer, int newTextWidth, int newTextHeight);

    inline void SetVisible(bool bVisible) { mVisible = bVisible;}
    inline bool GetVisible() const { return mVisible;}

private:
    boost::mutex                        mMutexTextObj;
    std::map<std::string, TextBlock*> mTextOverlayQueryData;
    std::vector<TextBlock*>            mTextOverlayData;
    std::string                         mModalityType;
    bool                                mVisible;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsTextOverlayGraphicObject);
};

TPS_END_NAMESPACE

#endif
