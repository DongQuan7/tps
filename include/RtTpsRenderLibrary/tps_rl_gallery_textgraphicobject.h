////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_textgraphicobject.h
/// 
///  \brief class GalleryTextGraphicObject definition 
/// 
///  \version 1.01
/// 
///  \date    2015/06/12
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_GALLERYTEXTGRAPHICOBJECT_H_
#define TPS_RL_GALLERYTEXTGRAPHICOBJECT_H_

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
//#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "tps_rl_defs.h"

// Mcsf
#include "McsfMedViewer3DGraphicObject/graphic_object_plane.h"
#include "McsfMedViewer3DArithmetic/point2d.h"
#include "McsfMedViewer3DArithmetic/point3d.h"

TPS_BEGIN_NAMESPACE

struct GalleryTextBlock {
    GalleryTextBlock(){};

    GalleryTextBlock(std::string textKey, unsigned char* strBuff, int strwidth, int strheight) {
        mTextKey = textKey;
        Margin.mLeft = 0;
        Margin.mTop = 0;

        mTextWidth = strwidth;
        mTextHeight = strheight;

        mTextBuffer = new unsigned char[strwidth * strheight * 4];
        memcpy(mTextBuffer, strBuff, strwidth * strheight * sizeof(unsigned char) * 4);
    }

    GalleryTextBlock(std::string textKey, float left, float top, unsigned char* strBuff, int strwidth, int strheight) {
        mTextKey = textKey;
        Margin.mLeft = left;
        Margin.mTop = top;

        mTextWidth = strwidth;
        mTextHeight = strheight;

        mTextBuffer = new unsigned char[strwidth * strheight * 4];
        memcpy(mTextBuffer, strBuff, strwidth * strheight * sizeof(unsigned char) * 4);
    }

    GalleryTextBlock(const GalleryTextBlock& textBlock) {
        mTextKey = textBlock.mTextKey;
        Margin.mLeft = textBlock.Margin.mLeft;
        Margin.mTop = textBlock.Margin.mTop;
        mTextWidth = textBlock.mTextWidth;
        mTextHeight = textBlock.mTextHeight;

        mTextBuffer = new unsigned char[textBlock.mTextWidth * textBlock.mTextHeight * 4];
        memcpy(mTextBuffer, textBlock.mTextBuffer, textBlock.mTextWidth * textBlock.mTextHeight * sizeof(unsigned char) * 4);
    }

    ~GalleryTextBlock(){
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

class TPS_RL_DECLSPEC GalleryTextGraphicObject : public TpsGraphicObjectBase {
public:
    GalleryTextGraphicObject(void);
    ~GalleryTextGraphicObject(void);

    virtual bool Initialize();
    virtual bool Finalize();

    //void SetTextList(std::vector<std::string> textList);
    void AddTextList(std::string text);
    void GetTextList(std::vector<std::string>& textList);

    //void SetTextStartCursorPos(std::Mcsf::MedViewer3D::Point2D textStartCursorPos, int iWidth, int iHeight);
    void AddTextStartCursorPosList(Mcsf::MedViewer3D::Point2D textStartCursorPos, int iWidth, int iHeight);
    void GetTextStartCursorPosList(std::vector<Mcsf::MedViewer3D::Point2D>& textStartCursorPosList, int iWidth, int iHeight);

    void AddText4VertexList(std::list<Mcsf::MedViewer3D::Point2D> text4Vertex, int iWidth, int iHeight);
    //void GetText4VertexList(std::vector<std::list<Mcsf::MedViewer3D::Point2D>>& text4VertexList, int iWidth, int iHeight);

    void AddText4Vertex3DList(std::list<Mcsf::MedViewer3D::Point3D> text4Vertex3D);
    void GetText4Vertex3DList(std::vector<std::list<Mcsf::MedViewer3D::Point3D>>& text4Vertex3DList);

    void AddTextIsOnPlaneList(bool isOnPlane);
    void UpdateTextIsOnPlaneList(int i, bool isOnPlane);
    //void GetTextIsOnPlaneList(std::vector<bool>& isOnPlaneList);

    void SetTextPan(double translationX, double translationY);
    void SetTextScale(double oldfactor, double factor);

    void TextHitTest(float x, float y, int iWidth, int iHeight);
    void GetTextHitIndex(int *hitIndex);

    void DeleteHitText();

    void AddTextBlock(std::string key, GalleryTextBlock *newTextBlock);
    void RemoveTextBlock(std::string key);
    void GetTextBlock(std::string key, GalleryTextBlock **textBlock);
    void GetAllTextBlock(std::vector<GalleryTextBlock*> &textBlocks);
    void UpdateTextBlock(std::string key, unsigned char* newTextBuffer, int newTextWidth, int newTextHeight);

    void UpdateTextGO();

private:
    Mcsf::MedViewer3D::Point2D ScreenCoordinateToImageCoordinate(Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight);
    Mcsf::MedViewer3D::Point2D ImageCoordinateToScreenCoordinate(Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight);
    bool IntersectInRegion(const std::vector<Mcsf::MedViewer3D::Point2D> &list, Mcsf::MedViewer3D::Point2D ptCheck);

private:
    std::map<std::string, GalleryTextBlock*>            mTextOverlayQueryData;
    std::vector<GalleryTextBlock*>                      mTextOverlayData;
    boost::mutex                                        mMutexTextObj;

    std::vector<std::string>                            mTextList;
    std::vector<Mcsf::MedViewer3D::Point2D>             mTextStartCursorPosList; //ImageCS
    std::vector<std::list<Mcsf::MedViewer3D::Point2D>>  mText4VertexList; //ImageCS
    std::vector<std::list<Mcsf::MedViewer3D::Point3D>>  mText4Vertex3DList;
    std::vector<bool>                                   mTextIsOnPlaneList;
    int                                                 mTextHitIndex;

    //std::string                         mModalityType;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(GalleryTextGraphicObject);
};

TPS_END_NAMESPACE

#endif
