////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_measurevaluegraphicobject.cpp
/// 
///  \brief class GalleryMeasureValGraphicObject implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/03/26
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measurevaluegraphicobject.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

TPS_BEGIN_NAMESPACE

GalleryMeasureValGraphicObject::GalleryMeasureValGraphicObject() : mModalityType("CT") {
        mTextOverlayData.clear();
        mTextOverlayQueryData.clear();
}

GalleryMeasureValGraphicObject::~GalleryMeasureValGraphicObject() {
    auto it = mTextOverlayData.begin();
    while (it != mTextOverlayData.end()) {
        delete *it;
        *it = nullptr;
        ++it;
    }

    mTextOverlayData.clear();

    mTextOverlayQueryData.clear();
}

bool GalleryMeasureValGraphicObject::Initialize(){

    mTextOverlayData.clear();
    mTextOverlayQueryData.clear();

    return true;
}

bool GalleryMeasureValGraphicObject::Finalize() {
    for_each(mTextOverlayData.begin(), mTextOverlayData.end(), 
        [](MeasureValTextBlock* it){delete it;});

    mTextOverlayData.clear();

    mTextOverlayQueryData.clear();
    return true;
}

void GalleryMeasureValGraphicObject::AddTextBlock(std::string key, MeasureValTextBlock *newTextBlock) {
    //\ for concurrency
    //boost::lock_guard<boost::mutex> lockGuard(mMutexTextObj);
    if(newTextBlock == nullptr){
        TPS_LOG_DEV_ERROR<<"newTextBlock is nullptr";
        return;
    }

    MeasureValTextBlock *cloneTextBlock = new MeasureValTextBlock(*newTextBlock);

    auto it = mTextOverlayQueryData.find(key);
    if (it != mTextOverlayQueryData.end()){
        TPS_LOG_DEV_INFO << "TextBlock: "<<key << " already exist!";
        return;
    }
    mTextOverlayQueryData[key] = cloneTextBlock;
    mTextOverlayData.push_back(cloneTextBlock);
}

void GalleryMeasureValGraphicObject::UpdateTextBlock(std::string key, 
    unsigned char* newTextBlockBuffer, int newTextWidth, int newTextHeight){
        // for concurrency
        //boost::lock_guard<boost::mutex> lockGuard(mMutexTextObj);
        if(newTextBlockBuffer == nullptr){
            TPS_LOG_DEV_ERROR<<"TextBlockBuffer is nullptr";
            return ;
        }
        auto it = mTextOverlayQueryData.find(key);
        if(it == mTextOverlayQueryData.end()){
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

void GalleryMeasureValGraphicObject::RemoveTextBlock(std::string key) {
    auto it = mTextOverlayQueryData.find(key);
    if (it != mTextOverlayQueryData.end()) {
        for (auto itVec = mTextOverlayData.begin(); itVec != mTextOverlayData.end(); ++itVec) {
            if (*itVec == it->second) {
                mTextOverlayData.erase(itVec);
                break;
            }
        }

        delete it->second;
        it->second = nullptr;

        mTextOverlayQueryData.erase(it);
    }
}

void GalleryMeasureValGraphicObject::GetTextBlock(std::string key, MeasureValTextBlock **textBlock){
    auto it = mTextOverlayQueryData.find(key);
    if (it != mTextOverlayQueryData.end()) {
        *textBlock = it->second;
    }
}

void GalleryMeasureValGraphicObject::GetAllTextBlock(std::vector<MeasureValTextBlock*> &textBlocks) {
    //textBlocks = mTextOverlayData;
    for(int i = 0; i < mTextOverlayData.size(); i++)
    {
        textBlocks.push_back(new MeasureValTextBlock(*mTextOverlayData[i]));
    }
}

TPS_END_NAMESPACE
