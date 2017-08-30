////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_textgraphicobject.cpp
/// 
///  \brief class GalleryTextGraphicObject implementation 
/// 
///  \version 1.01
/// 
///  \date    2015/06/12
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include <Windows.h>

#include "RtTpsRenderLibrary/tps_rl_gallery_textgraphicobject.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

TPS_BEGIN_NAMESPACE

GalleryTextGraphicObject::GalleryTextGraphicObject()/* : mModalityType("CT") */
{
    //mTextOverlayData = new std::vector<GalleryTextBlock*>();
    //mTextStartCursorPosList = new std::vector<std::Mcsf::MedViewer3D::Point2D>();
    //mText4VertexList = new std::vector<std::list<Mcsf::MedViewer3D::Point2D>>();
    //mText4Vertex3DList = new std::vector<std::list<Mcsf::MedViewer3D::Point3D>>();
    //mTextIsOnPlaneList = new std::vector<bool>();

    mTextOverlayData.clear();
    mTextOverlayQueryData.clear();
}

GalleryTextGraphicObject::~GalleryTextGraphicObject() 
{
    auto it = mTextOverlayData.begin();
    while (it != mTextOverlayData.end()) 
    {
        delete *it;
        *it = nullptr;
        ++it;
    }

    mTextOverlayData.clear();
    mTextOverlayQueryData.clear();
}

bool GalleryTextGraphicObject::Initialize()
{
    mTextOverlayData.clear();
    mTextOverlayQueryData.clear();

    return true;
}

bool GalleryTextGraphicObject::Finalize() 
{
    for_each(mTextOverlayData.begin(), mTextOverlayData.end(), 
        [](GalleryTextBlock* it){delete it;});

    mTextOverlayData.clear();
    mTextOverlayQueryData.clear();
    return true;
}

void GalleryTextGraphicObject::AddTextList(std::string text)
{
    mTextList.push_back(text);
}

void GalleryTextGraphicObject::GetTextList(std::vector<std::string>& textList)
{
    textList = mTextList;
}

void GalleryTextGraphicObject::AddTextStartCursorPosList(Mcsf::MedViewer3D::Point2D textStartCursorPos, int iWidth, int iHeight)
{
    Mcsf::MedViewer3D::Point2D textStartCursorPosNew = 
        ScreenCoordinateToImageCoordinate(textStartCursorPos, iWidth, iHeight);
    mTextStartCursorPosList.push_back(textStartCursorPosNew);
}

void GalleryTextGraphicObject::GetTextStartCursorPosList(std::vector<Mcsf::MedViewer3D::Point2D>& textStartCursorPosList, int iWidth, int iHeight)
{
    for(int i = 0; i < mTextStartCursorPosList.size(); i++)
    {
        Mcsf::MedViewer3D::Point2D textStartCursorPosListNew = 
            ImageCoordinateToScreenCoordinate(mTextStartCursorPosList[i], iWidth, iHeight);
        textStartCursorPosList.push_back(textStartCursorPosListNew);
    }
}

void GalleryTextGraphicObject::AddText4VertexList(std::list<Mcsf::MedViewer3D::Point2D> text4Vertex, int iWidth, int iHeight)
{
    std::list<Mcsf::MedViewer3D::Point2D> text4VertexInImageCS;
    for(std::list<Mcsf::MedViewer3D::Point2D>::const_iterator iter = text4Vertex.begin(); iter != text4Vertex.end(); iter++)
    {
        Mcsf::MedViewer3D::Point2D textVertexInImageCS = 
            ScreenCoordinateToImageCoordinate(*iter, iWidth, iHeight);
        text4VertexInImageCS.push_back(textVertexInImageCS);
    }
    mText4VertexList.push_back(text4VertexInImageCS);
}
//
//void GalleryTextGraphicObject::GetText4VertexList(std::vector<std::list<Mcsf::MedViewer3D::Point2D>>& text4VertexList, int iWidth, int iHeight)
//{
//
//}
//
void GalleryTextGraphicObject::AddText4Vertex3DList(std::list<Mcsf::MedViewer3D::Point3D> text4Vertex3D)
{ 
    mText4Vertex3DList.push_back(text4Vertex3D);
}

void GalleryTextGraphicObject::GetText4Vertex3DList(std::vector<std::list<Mcsf::MedViewer3D::Point3D>>& text4Vertex3DList)
{
    text4Vertex3DList = mText4Vertex3DList;
}

void GalleryTextGraphicObject::AddTextIsOnPlaneList(bool isOnPlane)
{
    mTextIsOnPlaneList.push_back(isOnPlane);
}

void GalleryTextGraphicObject::UpdateTextIsOnPlaneList(int i, bool isOnPlane)
{
    if (i >= mTextIsOnPlaneList.size())
        return;

    mTextIsOnPlaneList[i] = isOnPlane;
}

//void GalleryTextGraphicObject::GetTextIsOnPlaneList(std::vector<bool>& isOnPlaneList)
//{
//
//}

void GalleryTextGraphicObject::SetTextPan(double translationX, double translationY)
{
    for(int i = 0; i < mTextStartCursorPosList.size(); i++)
    {
        mTextStartCursorPosList[i].x += translationX;
        mTextStartCursorPosList[i].y += translationY;
    }
    for(int i = 0; i < mText4VertexList.size(); i++)
    {
        for(std::list<Mcsf::MedViewer3D::Point2D>::iterator iter = mText4VertexList[i].begin(); iter != mText4VertexList[i].end(); iter++)
        {
            (iter->x) += translationX;
            (iter->y) += translationY;
        }
    }
}

void GalleryTextGraphicObject::SetTextScale(double oldfactor, double factor)
{
    for(int i = 0; i < mTextStartCursorPosList.size(); i++)
    {
        mTextStartCursorPosList[i].x = mTextStartCursorPosList[i].x * factor / oldfactor;
        mTextStartCursorPosList[i].y = mTextStartCursorPosList[i].y * factor / oldfactor;
    }
    for(int i = 0; i < mText4VertexList.size(); i++)
    {
        for(std::list<Mcsf::MedViewer3D::Point2D>::iterator iter = mText4VertexList[i].begin(); iter != mText4VertexList[i].end(); iter++)
        {
            (iter->x) = (iter->x) * factor / oldfactor;
            (iter->y) = (iter->y) * factor / oldfactor;
        }
    }
}

void GalleryTextGraphicObject::TextHitTest(float x, float y, int iWidth, int iHeight)
{
    Mcsf::MedViewer3D::Point2D hitPoint;
    hitPoint.x = x;
    hitPoint.y = y;
    Mcsf::MedViewer3D::Point2D hitPointInImageCS = ScreenCoordinateToImageCoordinate(hitPoint, iWidth, iHeight);

    int hitIndex = -1;
    for(int i = 0; i < mText4VertexList.size(); i++)
    {
        if(mTextIsOnPlaneList[i])
        {
            std::vector<Mcsf::MedViewer3D::Point2D> text4Vertex;
            for(std::list<Mcsf::MedViewer3D::Point2D>::iterator iter = mText4VertexList[i].begin(); iter != mText4VertexList[i].end(); ++iter)
            {
                 text4Vertex.push_back(*iter);
            }
            bool isHit = IntersectInRegion(text4Vertex, hitPointInImageCS);
            if(isHit)
            {
                hitIndex = i;
                break;
            }
        }
    }
    mTextHitIndex = hitIndex;
}

void GalleryTextGraphicObject::GetTextHitIndex(int *hitIndex)
{
    *hitIndex = mTextHitIndex;
}

void GalleryTextGraphicObject::DeleteHitText()
{
    std::vector<std::string>::iterator iterS = mTextList.begin() + mTextHitIndex;
    mTextList.erase(iterS);
    std::vector<Mcsf::MedViewer3D::Point2D>::iterator iterStartCur = mTextStartCursorPosList.begin() + mTextHitIndex;
    mTextStartCursorPosList.erase(iterStartCur);
    std::vector<std::list<Mcsf::MedViewer3D::Point2D>>::iterator iter2D = mText4VertexList.begin() + mTextHitIndex;
    mText4VertexList.erase(iter2D);
    std::vector<std::list<Mcsf::MedViewer3D::Point3D>>::iterator iter3D = mText4Vertex3DList.begin() + mTextHitIndex;
    mText4Vertex3DList.erase(iter3D);
    std::vector<bool>::iterator iterB = mTextIsOnPlaneList.begin() + mTextHitIndex;
    mTextIsOnPlaneList.erase(iterB);
    mTextHitIndex = -1;
}

void GalleryTextGraphicObject::AddTextBlock(std::string key, GalleryTextBlock *newTextBlock) 
{
    //\ for concurrency
    //boost::lock_guard<boost::mutex> lockGuard(mMutexTextObj);
    if(newTextBlock == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"newTextBlock is nullptr";
        return;
    }

    GalleryTextBlock *cloneTextBlock = new GalleryTextBlock(*newTextBlock);

    auto it = mTextOverlayQueryData.find(key);
    if (it != mTextOverlayQueryData.end())
    {
        TPS_LOG_DEV_INFO << "TextBlock: "<<key << " already exist!";
        return;
    }
    mTextOverlayQueryData[key] = cloneTextBlock;
    mTextOverlayData.push_back(cloneTextBlock);
}

void GalleryTextGraphicObject::UpdateTextBlock(std::string key, 
    unsigned char* newTextBlockBuffer, int newTextWidth, int newTextHeight)
{
        // for concurrency
        //boost::lock_guard<boost::mutex> lockGuard(mMutexTextObj);
        if(newTextBlockBuffer == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"TextBlockBuffer is nullptr";
            return ;
        }
        auto it = mTextOverlayQueryData.find(key);
        if(it == mTextOverlayQueryData.end())
        {
            TPS_LOG_DEV_ERROR<<"the requested key does not exist in current data manager!";
            return;
        }

        //if(it->second->mTextBuffer != nullptr){
        //    delete[] it->second->mTextBuffer ;
        //    it->second->mTextBuffer = nullptr;
        //}

        delete[] it->second->mTextBuffer ;
        int buf_size = newTextWidth * newTextHeight * sizeof(unsigned char) * 4;
        it->second->mTextBuffer = new unsigned char[buf_size];
        //it->second->mTextBuffer = static_cast<unsigned char*>operator new(buf_size);
        std::memcpy(it->second->mTextBuffer, newTextBlockBuffer, buf_size);
        it->second->mTextWidth = newTextWidth;
        it->second->mTextHeight = newTextHeight;


}

void GalleryTextGraphicObject::RemoveTextBlock(std::string key) 
{
    auto it = mTextOverlayQueryData.find(key);
    if (it != mTextOverlayQueryData.end()) 
    {
        for (auto itVec = mTextOverlayData.begin(); itVec != mTextOverlayData.end(); ++itVec) 
        {
            if (*itVec == it->second)
           {
                mTextOverlayData.erase(itVec);
                break;
            }
        }

        delete it->second;
        it->second = nullptr;

        mTextOverlayQueryData.erase(it);
    }
}

void GalleryTextGraphicObject::GetTextBlock(std::string key, GalleryTextBlock **textBlock)
{
    auto it = mTextOverlayQueryData.find(key);
    if (it != mTextOverlayQueryData.end()) 
    {
        *textBlock = it->second;
    }
}

void GalleryTextGraphicObject::GetAllTextBlock(std::vector<GalleryTextBlock*> &textBlocks) 
{
    //textBlocks = mTextOverlayData;
    for(int i = 0; i < mTextOverlayData.size(); i++)
    {
        textBlocks.push_back(new GalleryTextBlock(*mTextOverlayData[i]));
    }
}

void GalleryTextGraphicObject::UpdateTextGO()
{
    if (mTextList.size() != mTextIsOnPlaneList.size() ||
        mTextList.size() != mText4VertexList.size()||
        mTextList.size() != mText4Vertex3DList.size()||
        mTextList.size() != mTextStartCursorPosList.size())
        return;

    //initialize all textblock
    if  (!Initialize())
        return;

    std::vector<std::pair<std::string, std::wstring>> textLine;

    for(int i = 0; i < mTextList.size(); ++i)
    {
        std::stringstream ss;
        ss<<i;
        std::string key = "Text" + ss.str();

        if (!mTextIsOnPlaneList[i])
        {
            std::wstringstream wssStream;
            wssStream<<" "; //The first parameter of GenerateRasterBuffer can't be empty, so be assigned to space.
            textLine.push_back(std::make_pair(key, wssStream.str()));
        }
        else
        {
            std::wstring wcharTextValueString;
            LPCSTR pszSrc = mTextList[i].c_str();
            int nLen = mTextList[i].size();
            int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);

            if(nSize <= 0)
            {
                std::wstringstream wssStream;
                wssStream<<" "; //The first parameter of GenerateRasterBuffer can't be empty, so be assigned to space.
                textLine.push_back(std::make_pair(key, wssStream.str()));
            }
            else
            {
                WCHAR *pwszDst = new WCHAR[nSize+1];
                MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc, nLen, pwszDst, nSize);
                pwszDst[nSize] = 0;
                if( pwszDst[0] == 0xFEFF) // skip Oxfeff
                    for(int i = 0; i < nSize; i ++) 
                        pwszDst[i] = pwszDst[i+1]; 
                wcharTextValueString = pwszDst;
                delete pwszDst;
                textLine.push_back(std::make_pair(key, wcharTextValueString));
            }
        }

        FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
        Pixel32* pStrBuffer = nullptr;
        int strHeight = 0;
        int strWidth = 0;

        float left = mTextStartCursorPosList[i].x;
        float top = 1 - mTextStartCursorPosList[i].y;
        Pixel32 fontColor = (i == mTextHitIndex) ? GREEN_VALUE : WHITE_VALUE;

        if(!pBufGen->GenerateRasterBuffer(textLine[i].second, &pStrBuffer, strWidth, strHeight, 12, fontColor))
        {
            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<key;
            return;
        }

        GalleryTextBlock textBlock(key, left, top, (unsigned char*)pStrBuffer, strWidth, strHeight);
        RemoveTextBlock(key);
        AddTextBlock(key, &textBlock);

        ////////////////////////////////just for test the vertex pos///////////////////////////////////////////////
        //std::list<Mcsf::MedViewer3D::Point2D>::const_iterator iter = mText4VertexList[i].begin();

        //key = "Point1 Of Text" + ss.str();
        //std::wstringstream wssStream;
        //wssStream<<"^";
        //left = (*iter).x;
        //top = 1 - (*iter).y;
        //if(!pBufGen->GenerateRasterBuffer(wssStream.str(), &pStrBuffer, strWidth, strHeight))
        //{
        //    TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<key;
        //    return;
        //}
        //GalleryTextBlock textBlock1(key, left, top, (unsigned char*)pStrBuffer, strWidth, strHeight);
        //RemoveTextBlock(key);
        //AddTextBlock(key, &textBlock1);
        //iter++;

        //key = "Point2 Of Text" + ss.str();
        //wssStream<<"^";
        //left = (*iter).x;
        //top = 1 - (*iter).y;
        //if(!pBufGen->GenerateRasterBuffer(wssStream.str(), &pStrBuffer, strWidth, strHeight))
        //{
        //    TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<key;
        //    return;
        //}
        //GalleryTextBlock textBlock2(key, left, top, (unsigned char*)pStrBuffer, strWidth, strHeight);
        //RemoveTextBlock(key);
        //AddTextBlock(key, &textBlock2);
        //iter++;

        //key = "Point3 Of Text" + ss.str();
        //wssStream<<"^";
        //left = (*iter).x;
        //top = 1 - (*iter).y;
        //if(!pBufGen->GenerateRasterBuffer(wssStream.str(), &pStrBuffer, strWidth, strHeight))
        //{
        //    TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<key;
        //    return;
        //}
        //GalleryTextBlock textBlock3(key, left, top, (unsigned char*)pStrBuffer, strWidth, strHeight);
        //RemoveTextBlock(key);
        //AddTextBlock(key, &textBlock3);
        //iter++;

        //key = "Point4 Of Text" + ss.str();
        //wssStream<<"^";
        //left = (*iter).x;
        //top = 1 - (*iter).y;
        //if(!pBufGen->GenerateRasterBuffer(wssStream.str(), &pStrBuffer, strWidth, strHeight))
        //{
        //    TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<key;
        //    return;
        //}
        //GalleryTextBlock textBlock4(key, left, top, (unsigned char*)pStrBuffer, strWidth, strHeight);
        //RemoveTextBlock(key);
        //AddTextBlock(key, &textBlock4);

        ////////////////////////////////just for test the vertex pos///////////////////////////////////////////////
        

        if(pStrBuffer != nullptr)
        {
            delete[] pStrBuffer;
            pStrBuffer = nullptr;
        }
    }

}

//common private method
Mcsf::MedViewer3D::Point2D GalleryTextGraphicObject::ScreenCoordinateToImageCoordinate
    (Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight)
{
    int iMinL = std::min(iWidth, iHeight);

    Mcsf::MedViewer3D::Point2D anglePointOut(0, 0);
    anglePointOut.x = (anglePoint.x - 0.5) * iWidth / iMinL * 2;
    anglePointOut.y = (anglePoint.y - 0.5) * iHeight / iMinL * 2;

    return anglePointOut;
}

Mcsf::MedViewer3D::Point2D GalleryTextGraphicObject::ImageCoordinateToScreenCoordinate
    (Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight)
{
    int iMinL = std::min(iWidth, iHeight);

    Mcsf::MedViewer3D::Point2D anglePointOut(0, 0);
    anglePointOut.x = (anglePoint.x / 2) * iMinL / iWidth + 0.5;
    anglePointOut.y = (anglePoint.y / 2) * iMinL / iHeight + 0.5;

    return anglePointOut;
}

bool GalleryTextGraphicObject::IntersectInRegion(const std::vector<Mcsf::MedViewer3D::Point2D> &list, Mcsf::MedViewer3D::Point2D ptCheck)
{
    // 以测试点为坐标原点对多边形的点转换坐标 
    std::vector<Mcsf::MedViewer3D::Vector2D> listTemp;

    for (int i = 0; i < list.size(); i++)
    {
        listTemp.push_back(list[i] - ptCheck);
    }

    // 验证多边形的正确性
    if (listTemp.size() < 3) 
    { 
        return false; 
    }

    // 取得第一个不在射线上的点
    //int iStart = getFirstIndex(list); 
    int iStart = 0;
    if (iStart == -1) 
    { 
        return false; 
    }

    int iCrossCount = 0; 
    double fCrossY = 0; 

    int listCount = (int)listTemp.size();
    int iEnd = listCount + iStart; 
    Mcsf::MedViewer3D::Vector2D ptFirst, ptEnd;

    for (int i = iStart; i < iEnd; i++) 
    { 
        ptFirst = listTemp[i % listCount]; 
        ptEnd = listTemp[(i + 1) % listCount];

        if ((ptFirst.x > 0 && ptEnd.x < 0) || (ptFirst.x < 0 && ptEnd.x>0)) 
        {// 端点在射线两边 
            fCrossY = (ptFirst.x * ptEnd.y - ptFirst.y * ptEnd.x) * (ptFirst.x - ptEnd.x);

            if (std::abs(fCrossY) < 0.0001) 
            {// 射线与测试边在一条直线上 
                return true; 
            }

            if (fCrossY > 0) 
            { 
                iCrossCount++; 
            } 
        } 
        else if (std::abs(ptEnd.x) < 0.0001 && ptEnd.y > 0) 
        {// 结束点在射线上，取得下一个有效测试点 
            int j; 
            int index = (i+2) % listCount; 
            for (j = i+2; j < iEnd+1; j++) 
            { 
                index = j%listCount; 
                if(std::abs(listTemp[index].x) > 0.0001) 
                { 
                    break; 
                } 
            }

            if ((ptFirst.x > 0 && listTemp[index].x < 0) || (ptFirst.x < 0 && listTemp[index].x > 0)) 
            { 
                iCrossCount++; 
            } 
            i = j-1; 
        } 
        else if(std::abs(ptEnd.x) < 0.0001 && std::abs(ptEnd.x) < 0.0001) 
        {// 结束点与原点重合 
            return true; 
        }
    }

    if (iCrossCount % 2 == 1) 
    { 
        return true; 
    } 
    else 
    { 
        return false; 
    }
}


TPS_END_NAMESPACE
