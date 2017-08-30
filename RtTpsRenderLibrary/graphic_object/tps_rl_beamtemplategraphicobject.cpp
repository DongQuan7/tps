
//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_rl_beamtemplategraphicobject.cpp
///  \brief  
///  \
///  class  TpsBeamTemplateGraphicObject
///  \version 1.0
///  \date  2014/11/21
///  \
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_simplebeamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamtemplategraphicobject.h"




TPS_BEGIN_NAMESPACE

TpsBeamTemplateGraphicObject::TpsBeamTemplateGraphicObject(){
    memset(&mtagMLC, 0, sizeof(mtagMLC));
    for (int i = 0; i < tagMLC::MAX_LEAF_NUM; i++)
    {
        mtagMLC.m_pLeaf[i].independent = 2;
    }
    //mtagMLC.m_nSelectionMode = MLC_IDLE;
    mtagMLC.m_nSelectedLeaf = -1;
}
TpsBeamTemplateGraphicObject::~TpsBeamTemplateGraphicObject(){

}
bool TpsBeamTemplateGraphicObject::Initialize() {
    return true;
}

bool TpsBeamTemplateGraphicObject::Finalize() {
    mSimpleBeamGoCollection.clear();
    return true;
}



void TpsBeamTemplateGraphicObject::AddBeamGo(std::shared_ptr<TpsSimpleBeamGraphicObject> simpleBeamGo){
    mSimpleBeamGoCollection.push_back(simpleBeamGo);
}

void TpsBeamTemplateGraphicObject::RemoveAll(){
    mSimpleBeamGoCollection.clear();
}

void TpsBeamTemplateGraphicObject::GetBeamGoCollection(std::vector<std::shared_ptr<TpsSimpleBeamGraphicObject>>& sbgo) const{
    sbgo = mSimpleBeamGoCollection;
}




void TpsBeamTemplateGraphicObject::InitJawPosition(){
    mJawPosition[0] = -50.0f;
    mJawPosition[1] = 50.0f;
    mJawPosition[2] = -50.0f;
    mJawPosition[3] = 50.0f;
}

void TpsBeamTemplateGraphicObject::InitLeafPosition(){
    int i = 0;
    for(; i < 42; i++){
        mtagMLC.m_pLeaf[i].pos_1 = -50;
        mtagMLC.m_pLeaf[i].pos_2 = 50;
        mtagMLC.m_pLeaf[i].min_w = -200 + i * 10;
        mtagMLC.m_pLeaf[i].max_w = mtagMLC.m_pLeaf[i].min_w + 10;
    }
    mtagMLC.m_Leaf_num = i - 1;
}

bool TpsBeamTemplateGraphicObject::GetDefaultSegmentOutLine(std::vector<Viewer3D::Point3D> *ptList){
    if(nullptr == ptList)
        return false;

    float minX = mJawPosition[0];
    float maxX = mJawPosition[1];
    float minY = mJawPosition[2];
    float maxY = mJawPosition[3];

    ptList->clear();
    Viewer3D::Point3D point;
    for (int i = 0; i < mtagMLC.m_Leaf_num; i++)
    {
        if (mtagMLC.m_pLeaf[i].max_w < minY)   // Below the jaw bottom
            continue;
        if (mtagMLC.m_pLeaf[i].min_w > maxY)   // Above the jaw top
            break;

        point.x = mtagMLC.m_pLeaf[i].pos_1;
        if (point.x < minX) point.x = minX;
        if (point.x > maxX) point.x = maxX;

        point.y = mtagMLC.m_pLeaf[i].min_w;
        if (point.y < minY) point.y = minY;
        if (point.y > maxY) point.y = maxY;
        point.z = -1000;
        ptList->push_back(point);

        point.x = mtagMLC.m_pLeaf[i].pos_1;
        if (point.x < minX) point.x = minX;
        if (point.x > maxX) point.x = maxX;
        point.y = mtagMLC.m_pLeaf[i].max_w;
        if (point.y < minY) point.y = minY;
        if (point.y > maxY) point.y = maxY;
        point.z = -1000;
        ptList->push_back(point);
    }

    for (int i = mtagMLC.m_Leaf_num - 1; i >= 0; i--)
    {
        if (mtagMLC.m_pLeaf[i].min_w > maxY)   // Above the jaw top
            continue;
        if (mtagMLC.m_pLeaf[i].max_w < minY)   // Below the jaw bottom
            break;

        point.x = mtagMLC.m_pLeaf[i].pos_2;
        if (point.x < minX) point.x = minX;
        if (point.x > maxX) point.x = maxX;
        point.y = mtagMLC.m_pLeaf[i].max_w;
        if (point.y < minY) point.y = minY;
        if (point.y > maxY) point.y = maxY;
        point.z = -1000;
        ptList->push_back(point);

        point.x = mtagMLC.m_pLeaf[i].pos_2;
        if (point.x < minX) point.x = minX;
        if (point.x > maxX) point.x = maxX;
        point.y = mtagMLC.m_pLeaf[i].min_w;
        if (point.y < minY) point.y = minY;
        if (point.y > maxY) point.y = maxY;
        point.z = -1000;
        ptList->push_back(point);
    }
    return true;
}

void TpsBeamTemplateGraphicObject::SetDirty(bool bFlag){

    TpsGraphicObjectBase::SetDirty(bFlag);
    if (!bFlag){
        for (auto it = mSimpleBeamGoCollection.begin(); it != mSimpleBeamGoCollection.end(); ++it){
            (*it)->SetDirty(bFlag);
        }
    }
}

TPS_END_NAMESPACE
