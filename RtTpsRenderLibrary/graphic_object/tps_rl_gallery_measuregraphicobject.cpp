////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan  yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_measuregraphicobject.cpp
/// 
///  \brief class MeasureGraphicObject definition 
/// 
///  \version 1.0
/// 
///  \date    2015/03/09
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobject.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"

TPS_BEGIN_NAMESPACE

GalleryMeasureGraphicObject::GalleryMeasureGraphicObject( void ) {

}

GalleryMeasureGraphicObject::~GalleryMeasureGraphicObject( void ) {

}

bool GalleryMeasureGraphicObject::Initialize() {
    return true;
}

bool GalleryMeasureGraphicObject::Finalize() {
    return true;
}

// Masure distance
//void GalleryMeasureGraphicObject::SetMeasureDistanceStartPoint( float startX, 
//    float startY ) {
//        mRulerStartX = startX;
//        mRulerStartY = startY;
//}
//
//void GalleryMeasureGraphicObject::GetMeasureDistanceStartPoint( float *startX, 
//    float *startY ) {
//        *startX = mRulerStartX;
//        *startY = mRulerStartY;
//}
//
//void GalleryMeasureGraphicObject::SetMeasureDistanceStopPoint( float stopX, float stopY ) {
//    mRulerStopX = stopX;
//    mRulerStopY = stopY;
//}
//
//void GalleryMeasureGraphicObject::GetMeasureDistanceStopPoint( float *stopX, float *stopY )
//{
//    *stopX = mRulerStopX;
//    *stopY = mRulerStopY;
//}

void GalleryMeasureGraphicObject::SetMeasureDistancePoints(float startX, float startY, 
    float stopX, float stopY, int iWidth, int iHeight)
{
    mRulerPoints.clear();

    Mcsf::MedViewer3D::Point2D distanceStartPoint;
    distanceStartPoint.x = startX;
    distanceStartPoint.y = startY;
    Mcsf::MedViewer3D::Point2D distanceStartPointNew = 
        ScreenCoordinateToImageCoordinate(distanceStartPoint, iWidth, iHeight);
    mRulerPoints.push_back(distanceStartPointNew);

    Mcsf::MedViewer3D::Point2D distanceStopPoint;
    distanceStopPoint.x = stopX;
    distanceStopPoint.y = stopY;
    Mcsf::MedViewer3D::Point2D distanceStopPointNew = 
        ScreenCoordinateToImageCoordinate(distanceStopPoint, iWidth, iHeight);
    mRulerPoints.push_back(distanceStopPointNew);
}

void GalleryMeasureGraphicObject::GetMeasureDistancePoints(std::list<Mcsf::MedViewer3D::Point2D>& distancePoints, int iWidth, int iHeight)
{
    for(std::list<Mcsf::MedViewer3D::Point2D>::const_iterator iter = mRulerPoints.begin(); iter != mRulerPoints.end(); iter++)
    {
        Mcsf::MedViewer3D::Point2D distancePointNew = 
            ImageCoordinateToScreenCoordinate(*iter, iWidth, iHeight);
        distancePoints.push_back(distancePointNew);
    }
}

void GalleryMeasureGraphicObject::GetMeasureDistancePointsList(std::vector<std::list<Mcsf::MedViewer3D::Point2D>>& distancePointsList, int iWidth, int iHeight)
{
    std::list<Mcsf::MedViewer3D::Point2D> distancePoints;
    for(int i = 0; i < mRulerPointsList.size(); i++)
    {
        distancePoints.clear();
        for(std::list<Mcsf::MedViewer3D::Point2D>::const_iterator iter = mRulerPointsList[i].begin(); iter != mRulerPointsList[i].end(); iter++)
        {

            Mcsf::MedViewer3D::Point2D distancePointNew = 
                ImageCoordinateToScreenCoordinate(*iter, iWidth, iHeight);
            distancePoints.push_back(distancePointNew);
        }
        distancePointsList.push_back(distancePoints);
    }

}

void GalleryMeasureGraphicObject::AddMeasureDistancePointsToList()
{
    mRulerPointsList.push_back(mRulerPoints);
    mRulerPoints.clear();
}

void GalleryMeasureGraphicObject::AddMeasureDistancePoints3DToList(std::list<Mcsf::MedViewer3D::Point3D> distancePoints3D)
{
    mRulerPoints3DList.push_back(distancePoints3D);
}

void GalleryMeasureGraphicObject::GetMeasureDistancePoints3DList(std::vector<std::list<Mcsf::MedViewer3D::Point3D>>& distancePoints3DList)
{
    distancePoints3DList = mRulerPoints3DList;
}

void GalleryMeasureGraphicObject::SetMeasureDistance(float distance)
{
    mRulerDistance = distance;
}

void GalleryMeasureGraphicObject::AddMeasureDistanceToList()
{
    mRulerDistanceList.push_back(mRulerDistance);
}

void GalleryMeasureGraphicObject::AddMeasureDistanceIsOnPlaneResultToList(bool isOnPlane)
{
    mRulerIsOnPlaneList.push_back(isOnPlane);
}

void GalleryMeasureGraphicObject::UpdateMeasureDistanceIsOnPlaneResultList(int i, bool isOnPlane)
{
    if (i >= mRulerIsOnPlaneList.size())
        return;

    mRulerIsOnPlaneList[i] = isOnPlane;
}

void GalleryMeasureGraphicObject::GetMeasureDistanceIsOnPlaneResultList(std::vector<bool>& isOnPlaneList)
{
    isOnPlaneList = mRulerIsOnPlaneList;
}

void GalleryMeasureGraphicObject::SetMeasureDistanceActionStatus( bool isActionStarted,
    bool isActionDone ) {
        mRulerIsActionStarted = isActionStarted;
        mRulerIsActionDone = isActionDone;
}

void GalleryMeasureGraphicObject::GetMeasureDistanceActionStatus( bool *isActionStarted,
    bool *isActionDone ) {
        *isActionStarted = mRulerIsActionStarted;
        *isActionDone = mRulerIsActionDone;
}

void GalleryMeasureGraphicObject::SetMeasureDistancePan(double translationX, double translationY)
{
    for(int i = 0; i < mRulerPointsList.size(); i++)
    {
        for(std::list<Mcsf::MedViewer3D::Point2D>::iterator iter = mRulerPointsList[i].begin(); iter != mRulerPointsList[i].end(); iter++)
        {
            (iter->x) += translationX;
            (iter->y) += translationY;
        }
    }
}

void GalleryMeasureGraphicObject::SetMeasureDistanceScale(double oldfactor, double factor)
{
    for(int i = 0; i < mRulerPointsList.size(); i++)
    {
        for(std::list<Mcsf::MedViewer3D::Point2D>::iterator iter = mRulerPointsList[i].begin(); iter != mRulerPointsList[i].end(); iter++)
        {
            (iter->x) = (iter->x) * factor / oldfactor;
            (iter->y) = (iter->y) * factor / oldfactor;
        }
    }
}

void GalleryMeasureGraphicObject::MeasureDistanceHitTest(float x, float y, int iWidth, int iHeight)
{
    Mcsf::MedViewer3D::Point2D hitPoint;
    hitPoint.x = x;
    hitPoint.y = y;
    Mcsf::MedViewer3D::Point2D hitPointInImageCS = ScreenCoordinateToImageCoordinate(hitPoint, iWidth, iHeight);

    int hitIndex = -1;
    for(int i = 0; i < mRulerPointsList.size(); i++)
    {
        double minDistanceSquare = 0.03;
        if(mRulerIsOnPlaneList[i])
        {
            double disPointtoLeneSquare = 0.0;
            bool isHit = IntersectLine(hitPointInImageCS, mRulerPointsList[i].front(), mRulerPointsList[i].back(), 0.03, &disPointtoLeneSquare);
            if(isHit)
            {
                hitIndex = (disPointtoLeneSquare < minDistanceSquare) ? i : hitIndex;
                minDistanceSquare = (disPointtoLeneSquare < minDistanceSquare) ? disPointtoLeneSquare : minDistanceSquare;
            }
        }
    }
    mRulerHitIndex = hitIndex;
}

void GalleryMeasureGraphicObject::GetMeasureDistanceHitIndex(int *hitIndex)
{
    *hitIndex = mRulerHitIndex;
}

void GalleryMeasureGraphicObject::DeleteHitDistance()
{
    //std::vector<std::list<Mcsf::MedViewer3D::Point2D>>   mRulerPointsList;
    //std::vector<std::list<Mcsf::MedViewer3D::Point3D>>   mRulerPoints3DList;
    //std::vector<float>                                   mRulerDistanceList;
    //std::vector<bool>                                    mRulerIsOnPlaneList;
    //int                                     mRulerHitIndex;
    std::vector<std::list<Mcsf::MedViewer3D::Point2D>>::iterator iter2D = mRulerPointsList.begin() + mRulerHitIndex;
    mRulerPointsList.erase(iter2D);
    std::vector<std::list<Mcsf::MedViewer3D::Point3D>>::iterator iter3D = mRulerPoints3DList.begin() + mRulerHitIndex;
    mRulerPoints3DList.erase(iter3D);
    std::vector<float>::iterator iterF = mRulerDistanceList.begin() + mRulerHitIndex;
    mRulerDistanceList.erase(iterF);
    std::vector<bool>::iterator iterB = mRulerIsOnPlaneList.begin() + mRulerHitIndex;
    mRulerIsOnPlaneList.erase(iterB);
    mRulerHitIndex = -1;
}


// Measure angle
void GalleryMeasureGraphicObject::SetMeasureAnglePoints( 
    std::list<Mcsf::MedViewer3D::Point2D> anglePoints, int iWidth, int iHeight) {
        mProtractorAnglePoints.clear();
        for(std::list<Mcsf::MedViewer3D::Point2D>::const_iterator iter = anglePoints.begin(); iter != anglePoints.end(); iter++)
        {
            Mcsf::MedViewer3D::Point2D anglePointNew = 
                ScreenCoordinateToImageCoordinate(*iter, iWidth, iHeight);
            mProtractorAnglePoints.push_back(anglePointNew);
        }

        //mProtractorAnglePoints = anglePoints;
}

void GalleryMeasureGraphicObject::GetMeasureAnglePoints(
    std::list<Mcsf::MedViewer3D::Point2D>& anglePoints, int iWidth, int iHeight){
        for(std::list<Mcsf::MedViewer3D::Point2D>::const_iterator iter = mProtractorAnglePoints.begin(); iter != mProtractorAnglePoints.end(); iter++)
        {
            Mcsf::MedViewer3D::Point2D anglePointNew = 
                ImageCoordinateToScreenCoordinate(*iter, iWidth, iHeight);
            anglePoints.push_back(anglePointNew);
        }

        //anglePoints = mProtractorAnglePoints;
}

void GalleryMeasureGraphicObject::GetMeasureAnglePointsList(
    std::vector<std::list<Mcsf::MedViewer3D::Point2D>>& anglePointsList, int iWidth, int iHeight)
{
    std::list<Mcsf::MedViewer3D::Point2D> anglePoints;
    for(int i = 0; i < mProtractorAnglePointsList.size(); i++)
    {
        anglePoints.clear();
        for(std::list<Mcsf::MedViewer3D::Point2D>::const_iterator iter = mProtractorAnglePointsList[i].begin(); iter != mProtractorAnglePointsList[i].end(); iter++)
        {

            Mcsf::MedViewer3D::Point2D anglePointNew = 
                ImageCoordinateToScreenCoordinate(*iter, iWidth, iHeight);
            anglePoints.push_back(anglePointNew);
        }
        anglePointsList.push_back(anglePoints);
    }

}

void GalleryMeasureGraphicObject::AddMeasureAnglePointsToList()
{    
    mProtractorAnglePointsList.push_back(mProtractorAnglePoints);
    mProtractorAnglePoints.clear();
}

void GalleryMeasureGraphicObject::AddMeasureAnglePoints3DToList(std::list<Mcsf::MedViewer3D::Point3D> anglePoints3D)
{
    mProtractorAnglePoints3DList.push_back(anglePoints3D);
}

void GalleryMeasureGraphicObject::GetMeasureAnglePoints3DList(std::vector<std::list<Mcsf::MedViewer3D::Point3D>>& anglePoints3DList)
{
    anglePoints3DList = mProtractorAnglePoints3DList;
}

void GalleryMeasureGraphicObject::SetMeasureAngleCursorPoint( float cursorX, 
    float cursorY, int iWidth, int iHeight) {
        Mcsf::MedViewer3D::Point2D cursor(cursorX, cursorY);
        Mcsf::MedViewer3D::Point2D cursorInImageCoordinate = 
            ScreenCoordinateToImageCoordinate(cursor, iWidth, iHeight);
        mProtractorCursorX = cursorInImageCoordinate.x;
        mProtractorCursorY = cursorInImageCoordinate.y;

        //mProtractorCursorX = cursorX;
        //mProtractorCursorY = cursorY;
}

void GalleryMeasureGraphicObject::GetMeasureAngleCursorPoint( float *cursorX, 
    float *cursorY, int iWidth, int iHeight) {
        Mcsf::MedViewer3D::Point2D cursor(mProtractorCursorX, mProtractorCursorY);
        Mcsf::MedViewer3D::Point2D cursorInScreenCoordinate = 
            ImageCoordinateToScreenCoordinate(cursor, iWidth, iHeight);

        *cursorX = cursorInScreenCoordinate.x;
        *cursorY = cursorInScreenCoordinate.y;

        //*cursorX = mProtractorCursorX;
        //*cursorY = mProtractorCursorY;
}

void GalleryMeasureGraphicObject::SetMeasureAngleActionStatus( bool isActionStarted, 
    bool isFirstSideDone, bool isSecondSideDone ) {
        mProtractorActionStarted = isActionStarted;
        mProtractorFirstSideDone = isFirstSideDone;
        mProtractorSecondSideDone = isSecondSideDone;
}

void GalleryMeasureGraphicObject::GetMeasureAngleActionStatus( bool *isActionStarted, 
    bool *isFirstSideDone, bool *isSecondSideDone ) {
        *isActionStarted = mProtractorActionStarted;
        *isFirstSideDone = mProtractorFirstSideDone;
        *isSecondSideDone = mProtractorSecondSideDone;
}

void GalleryMeasureGraphicObject::SetMeasureAnglePan(double translationX, double translationY)
{
    for(int i = 0; i < mProtractorAnglePointsList.size(); i++)
    {
        for(std::list<Mcsf::MedViewer3D::Point2D>::iterator iter = mProtractorAnglePointsList[i].begin(); iter != mProtractorAnglePointsList[i].end(); iter++)
        {
            (iter->x) += translationX;
            (iter->y) += translationY;
        }
    }
}

void GalleryMeasureGraphicObject::SetMeasureAngleScale(double oldfactor, double factor)
{
    for(int i = 0; i < mProtractorAnglePointsList.size(); i++)
    {
        for(std::list<Mcsf::MedViewer3D::Point2D>::iterator iter = mProtractorAnglePointsList[i].begin(); iter != mProtractorAnglePointsList[i].end(); iter++)
        {
            (iter->x) = (iter->x) * factor / oldfactor;
            (iter->y) = (iter->y) * factor / oldfactor;
        }
    }
}

void GalleryMeasureGraphicObject::SetMeasureAngle(float angle)
{
    mProtractorAngle = angle;
}

void GalleryMeasureGraphicObject::AddMeasureAngleToList()
{
    mProtractorAngleList.push_back(mProtractorAngle);
}

void GalleryMeasureGraphicObject::AddMeasureAngleIsOnPlaneResultToList(bool isOnPlane)
{
    mProtractorIsOnPlaneList.push_back(isOnPlane);
}

void GalleryMeasureGraphicObject::UpdateMeasureAngleIsOnPlaneResultList(int i, bool isOnPlane)
{
    if (i >= mProtractorIsOnPlaneList.size())
        return;

    mProtractorIsOnPlaneList[i] = isOnPlane;
}

void GalleryMeasureGraphicObject::GetMeasureAngleIsOnPlaneResultList(std::vector<bool>& isOnPlaneList)
{
    isOnPlaneList = mProtractorIsOnPlaneList;
}

//for measure value operation
void GalleryMeasureGraphicObject::SetMeasureValGO(std::shared_ptr<GalleryMeasureValGraphicObject> measureValGOPtr) 
{
    mMeasureValGOPtr = measureValGOPtr;
}

void GalleryMeasureGraphicObject::UpdateMeasureValGO()
{
    if (nullptr == mMeasureValGOPtr)
        return;

    if(mProtractorAnglePointsList.size() != mProtractorAngleList.size() ||
       mProtractorAnglePointsList.size() != mProtractorIsOnPlaneList.size())
        return;

    if(mRulerPointsList.size() != mRulerDistanceList.size() ||
       mRulerPointsList.size() != mRulerIsOnPlaneList.size())
        return;

    //initialize all textblock
    if(!mMeasureValGOPtr->Initialize())
        return;

    std::vector<std::vector<std::pair<std::string, std::wstring>>> textLineList;
    std::vector<std::pair<std::string, std::wstring>> textLine;
    int iTextLineListIndex = 0;

    //update completed angle value
    for(int i = 0; i < mProtractorAnglePointsList.size(); ++i)
    {
        std::stringstream ss;
        ss<<i;
        std::string key = "AngleVal" + ss.str();

        if (!mProtractorIsOnPlaneList[i])
        {
            std::wstringstream wssStream;
            wssStream<<" "; //The first parameter of GenerateRasterBuffer can't be empty, so be assigned to space.
            textLine.push_back(std::make_pair(key, wssStream.str()));
        }
        else
        {
            char txt_angle[64]={0};
            _snprintf_s(txt_angle, sizeof(txt_angle), "%.1f", mProtractorAngleList[i]);
            if(txt_angle[sizeof(txt_angle) - 1] != '\0')
            {
                txt_angle[sizeof(txt_angle)-1]='\0';
            }
            std::wstringstream wssStream;
            wssStream<<txt_angle;
            textLine.push_back(std::make_pair(key, wssStream.str() + L"°"));
            //textLine.push_back(std::make_pair("AngleUnit", L"+"));
        }

        textLineList.push_back(textLine);
        textLine.clear();
        std::vector<std::pair<std::string, std::wstring>>().swap(textLine);

        FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
        Pixel32* pStrBuffer = nullptr;
        int strHeight = 0;
        int strWidth = 0;

        float left = mProtractorAnglePointsList[i].back().x;
        float top = 1 - mProtractorAnglePointsList[i].back().y;
        for (int j = 0; j < textLineList[iTextLineListIndex].size(); ++j)
        {
            std::wstring nam = textLineList[iTextLineListIndex][j].second;
            if(!pBufGen->GenerateRasterBuffer(textLineList[iTextLineListIndex][j].second, &pStrBuffer, strWidth, strHeight))
            {
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<textLineList[iTextLineListIndex][j].first;
                return;
            }

            MeasureValTextBlock textBlock(textLineList[iTextLineListIndex][j].first, left, top, (unsigned char*)pStrBuffer, strWidth, strHeight);
            mMeasureValGOPtr->RemoveTextBlock(textLineList[iTextLineListIndex][j].first);
            mMeasureValGOPtr->AddTextBlock(textLineList[iTextLineListIndex][j].first, &textBlock);

            if(pStrBuffer != nullptr)
            {
                delete[] pStrBuffer;
                pStrBuffer = nullptr;
            }
            left += strWidth + 2;
        }

        iTextLineListIndex++;
    }

    //update uncompleted angle value
    if (mProtractorAnglePoints.size() == 2)
    {
        char txt_angle[64]={0};
        _snprintf_s(txt_angle, sizeof(txt_angle), "%.1f", mProtractorAngle);
        if(txt_angle[sizeof(txt_angle) - 1] != '\0')
        {
            txt_angle[sizeof(txt_angle)-1]='\0';
        }
        std::wstringstream wssStream;
        wssStream<<txt_angle;
        std::stringstream ss;
        //int i = mProtractorAngleList.size();
        ss<<mProtractorAngleList.size();
        std::string key = "AngleVal" + ss.str();
        textLine.push_back(std::make_pair(key, wssStream.str() + L"°"));
        //textLine.push_back(std::make_pair("AngleUnit", L"+"));

        textLineList.push_back(textLine);
        textLine.clear();
        std::vector<std::pair<std::string, std::wstring>>().swap(textLine);

        FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
        Pixel32* pStrBuffer = nullptr;
        int strHeight = 0;
        int strWidth = 0;

        float left = mProtractorCursorX;
        float top = 1 - mProtractorCursorY;
        for (int j = 0; j < textLineList[iTextLineListIndex].size(); ++j)
        {
            std::wstring nam = textLineList[iTextLineListIndex][j].second;
            if(!pBufGen->GenerateRasterBuffer(textLineList[iTextLineListIndex][j].second, &pStrBuffer, strWidth, strHeight))
            {
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<textLineList[iTextLineListIndex][j].first;
                return;
            }

            MeasureValTextBlock textBlock(textLineList[iTextLineListIndex][j].first, left, top, (unsigned char*)pStrBuffer, strWidth, strHeight);
            mMeasureValGOPtr->RemoveTextBlock(textLineList[iTextLineListIndex][j].first);
            mMeasureValGOPtr->AddTextBlock(textLineList[iTextLineListIndex][j].first, &textBlock);

            if(pStrBuffer != nullptr)
            {
                delete[] pStrBuffer;
                pStrBuffer = nullptr;
            }
            left += strWidth + 2;
        }

        iTextLineListIndex++;
    } 

    //update complete distance value
    //std::cout<<"************UpdateMeasureValGO***************"<<std::endl;
    //std::cout<<"Completed: "<<mRulerPointsList.size()<<std::endl;
    for(int i = 0; i < mRulerPointsList.size(); ++i)
    {
        std::stringstream ss;
        ss<<i;
        std::string key = "DistanceVal" + ss.str();

        if (!mRulerIsOnPlaneList[i])
        {
            std::wstringstream wssStream;
            wssStream<<" "; //The first parameter of GenerateRasterBuffer can't be empty, so be assigned to space.
            textLine.push_back(std::make_pair(key, wssStream.str()));
        }
        else
        {
            char txt_distance[64]={0};
            _snprintf_s(txt_distance, sizeof(txt_distance), "%.2f", mRulerDistanceList[i]);
            if(txt_distance[sizeof(txt_distance) - 1] != '\0')
            {
                txt_distance[sizeof(txt_distance)-1]='\0';
            }
            std::wstringstream wssStream;
            wssStream<<txt_distance;
            textLine.push_back(std::make_pair(key, wssStream.str() + L"cm"));
            //textLine.push_back(std::make_pair("DistanceUnit", L"cm"));
        }

        textLineList.push_back(textLine);
        textLine.clear();
        std::vector<std::pair<std::string, std::wstring>>().swap(textLine);

        FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
        Pixel32* pStrBuffer = nullptr;
        int strHeight = 0;
        int strWidth = 0;

        float left = mRulerPointsList[i].back().x;
        float top = 1 - mRulerPointsList[i].back().y;
        for (int j = 0; j < textLineList[iTextLineListIndex].size(); ++j)
        {
            std::wstring nam = textLineList[iTextLineListIndex][j].second;
            if(!pBufGen->GenerateRasterBuffer(textLineList[iTextLineListIndex][j].second, &pStrBuffer, strWidth, strHeight))
            {
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<textLineList[iTextLineListIndex][j].first;
                return;
            }

            MeasureValTextBlock textBlock(textLineList[iTextLineListIndex][j].first, left, top, (unsigned char*)pStrBuffer, strWidth, strHeight);
            mMeasureValGOPtr->RemoveTextBlock(textLineList[iTextLineListIndex][j].first);
            mMeasureValGOPtr->AddTextBlock(textLineList[iTextLineListIndex][j].first, &textBlock);

            if(pStrBuffer != nullptr)
            {
                delete[] pStrBuffer;
                pStrBuffer = nullptr;
            }
            left += strWidth + 2;
        }

        iTextLineListIndex++;
    }

    //update uncomplete distance value
    //std::cout<<"UnCompleted: "<<mRulerPoints.size()<<std::endl;
    if (mRulerPoints.size() != 0)
    {
        char txt_distance[64]={0};
        _snprintf_s(txt_distance, sizeof(txt_distance), "%.2f", mRulerDistance);
        if(txt_distance[sizeof(txt_distance) - 1] != '\0')
        {
            txt_distance[sizeof(txt_distance)-1]='\0';
        }
        std::wstringstream wssStream;
        wssStream<<txt_distance;
        std::stringstream ss;
        //int i = mProtractorAngleList.size();
        ss<<mRulerDistanceList.size();
        std::string key = "DistanceVal" + ss.str();
        textLine.push_back(std::make_pair(key, wssStream.str() + L"cm"));
        //textLine.push_back(std::make_pair("DistanceUnit", L"cm"));

        textLineList.push_back(textLine);
        textLine.clear();
        std::vector<std::pair<std::string, std::wstring>>().swap(textLine);

        FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
        Pixel32* pStrBuffer = nullptr;
        int strHeight = 0;
        int strWidth = 0;

        float left = mRulerPoints.back().x;
        float top = 1 - mRulerPoints.back().y;
        for (int j = 0; j < textLineList[iTextLineListIndex].size(); ++j)
        {
            std::wstring nam = textLineList[iTextLineListIndex][j].second;
            if(!pBufGen->GenerateRasterBuffer(textLineList[iTextLineListIndex][j].second, &pStrBuffer, strWidth, strHeight))
            {
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<textLineList[iTextLineListIndex][j].first;
                return;
            }

            MeasureValTextBlock textBlock(textLineList[iTextLineListIndex][j].first, left, top, (unsigned char*)pStrBuffer, strWidth, strHeight);
            mMeasureValGOPtr->RemoveTextBlock(textLineList[iTextLineListIndex][j].first);
            mMeasureValGOPtr->AddTextBlock(textLineList[iTextLineListIndex][j].first, &textBlock);

            if(pStrBuffer != nullptr)
            {
                delete[] pStrBuffer;
                pStrBuffer = nullptr;
            }
            left += strWidth + 2;
        }

        iTextLineListIndex++;
    }

}

//common private method
Mcsf::MedViewer3D::Point2D GalleryMeasureGraphicObject::ScreenCoordinateToImageCoordinate
    (Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight)
{
    int iMinL = std::min(iWidth, iHeight);

    Mcsf::MedViewer3D::Point2D anglePointOut(0, 0);
    anglePointOut.x = (anglePoint.x - 0.5) * iWidth / iMinL * 2;
    anglePointOut.y = (anglePoint.y - 0.5) * iHeight / iMinL * 2;

    return anglePointOut;
}

Mcsf::MedViewer3D::Point2D GalleryMeasureGraphicObject::ImageCoordinateToScreenCoordinate
    (Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight)
{
    int iMinL = std::min(iWidth, iHeight);

    Mcsf::MedViewer3D::Point2D anglePointOut(0, 0);
    anglePointOut.x = (anglePoint.x / 2) * iMinL / iWidth + 0.5;
    anglePointOut.y = (anglePoint.y / 2) * iMinL / iHeight + 0.5;

    return anglePointOut;
}

double GalleryMeasureGraphicObject::DistanceSquare(const Mcsf::MedViewer3D::Point2D srcPoint, const Mcsf::MedViewer3D::Point2D dstPoint)
{
    return (srcPoint.x -dstPoint.x)*(srcPoint.x -dstPoint.x)+ (srcPoint.y - dstPoint.y)*(srcPoint.y - dstPoint.y);
}

bool GalleryMeasureGraphicObject::IntersectPoint(const Mcsf::MedViewer3D::Point2D hitpoint, const Mcsf::MedViewer3D::Point2D dstPoint, double tolerance)
{
    return DistanceSquare(hitpoint, dstPoint) <= tolerance * tolerance;
}

bool GalleryMeasureGraphicObject::IntersectLine(const Mcsf::MedViewer3D::Point2D vPoint1, const Mcsf::MedViewer3D::Point2D vPoint2,
    const Mcsf::MedViewer3D::Point2D vPoint3, double tolerance, double* disPointtoLeneSquare)
{
    *disPointtoLeneSquare = DistanceSquare(vPoint2, vPoint3);

    if (disPointtoLeneSquare == 0)
    {
        return IntersectPoint(vPoint1, vPoint2, tolerance);
    }

    double t = ((vPoint1.x - vPoint2.x) * (vPoint3.x - vPoint2.x) + (vPoint1.y - vPoint2.y) * (vPoint3.y - vPoint2.y)) / *disPointtoLeneSquare;

    if (t < 0)
        *disPointtoLeneSquare =  DistanceSquare(vPoint1,vPoint2);
    else if (t > 1)
        *disPointtoLeneSquare =  DistanceSquare(vPoint1,vPoint3);
    else
        *disPointtoLeneSquare =  DistanceSquare(vPoint1, Mcsf::MedViewer3D::Point2D((vPoint2.x + t * (vPoint3.x - vPoint2.x)),(vPoint2.y + t * (vPoint3.y - vPoint2.y))));

    return *disPointtoLeneSquare <= tolerance * tolerance;
}
TPS_END_NAMESPACE