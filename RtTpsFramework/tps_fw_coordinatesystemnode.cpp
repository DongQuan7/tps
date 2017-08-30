////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_coordinatesystemnode.cpp
/// 
///  \brief class TpsCoordinateSystemNode implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_coordinatesystemnode.h"

#include "tps_logger.h"

//#ifdef _DEBUG
//#include <iostream>
//#endif

#define MAX_CHILDREN_COUNT 255

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsCoordinateSystemNode::TpsCoordinateSystemNode()
    : mParent(nullptr), mNodeID("")
{
    mTransformMatrix.SetIdentity();
}

TpsCoordinateSystemNode::~TpsCoordinateSystemNode()
{

}

bool TpsCoordinateSystemNode::AddChild(
    const TpsCoordinateSystemNode::Ptr& node)
{
    if (mChildren.size() == MAX_CHILDREN_COUNT){
        TPS_LOG_DEV_ERROR<<"The node is full.";
        return false;
    }

    if (node == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The node to be added is null.";
        return false;
    }

    size_t newIndex = mChildren.size() + 1;
    std::string newID = mNodeID + char(newIndex);
    node->SetID_i(newID);
    node->SetParent_i(this->shared_from_this());
    mChildren.push_back(node);
    return true;
}

TpsCoordinateSystemNode::Ptr TpsCoordinateSystemNode::GetChild(
    const std::string& nodeID)
{
    size_t childLen = nodeID.length();
    size_t thisLen = mNodeID.length();
    std::string fatherID = nodeID.substr(0, thisLen);
    if (childLen <= thisLen || 
        fatherID.compare(mNodeID) != 0) {
        //LoG not its child
        TPS_LOG_DEV_ERROR<<"The node has no child named: "<<nodeID;
        return nullptr;
    }

    Ptr child = this->shared_from_this();
    std::string childIndexStr = nodeID.substr(thisLen, childLen - thisLen);
    for (size_t i=0; i<childIndexStr.length(); ++i) {
        child = child->GetImmediateChild(childIndexStr[i]-1);
        if (child == nullptr) {
            //LOG not exist
            TPS_LOG_DEV_ERROR<<"The child named: "<<nodeID<<" does not exist.";
            return nullptr;
        }
    }
    return child;
}

TpsCoordinateSystemNode::Ptr TpsCoordinateSystemNode::GetImmediateChild(
    unsigned int index)
{
    //LOG  out of range
    if (index >= mChildren.size())
    {
        TPS_LOG_DEV_ERROR<<"The index is out of range.";
        return nullptr;
    }

//#ifdef _DEBUG
//        printf("Father: ");
//        for (int i=0; i<mNodeID.length(); ++i)
//        {
//            printf("%d", mNodeID[i]);
//        }
//        printf(" --> Child: ");
//        std::string childID = mChildren[index]->GetID();
//        for (int i=0; i<childID.length(); ++i)
//        {
//            printf("%d", childID[i]);
//        }
//        printf("\n");
//#endif
    
    return mChildren[index];
}

TpsCoordinateSystemNode::Ptr TpsCoordinateSystemNode::GetParent()
{
    return mParent;
}

bool TpsCoordinateSystemNode::RemoveChild(const std::string& nodeID)
{
    size_t childLen = nodeID.length();
    size_t thisLen = mNodeID.length();
    std::string fatherID = nodeID.substr(0, thisLen);
    if (childLen <= thisLen || 
        fatherID.compare(mNodeID) != 0) {
        TPS_LOG_DEV_ERROR<<"The target node is not a child of this node.";
        return false;
    }

    Ptr father = this->shared_from_this();
    std::string childIndexStr = nodeID.substr(thisLen, childLen - thisLen);
    size_t last = childIndexStr.length()-1;
    for (size_t i=0; i<last; ++i) {
        father = father->GetImmediateChild(childIndexStr[i]-1);
        if (father == nullptr) {
            //LOG not exist
            TPS_LOG_DEV_ERROR<<"The node to be removed is null.";
            return nullptr;
        }
    }

    return father->RemoveImmediateChild(childIndexStr[last]-1);
}

bool TpsCoordinateSystemNode::RemoveImmediateChild(unsigned int index){

    //LOG  out of range
    if (index >= mChildren.size()) return false;

    auto it = mChildren.begin() + index;
    if (*it != nullptr)
    {
        (*it)->SetParent_i(nullptr);
        (*it)->SetID_i("");
    }
    mChildren.erase(it);
    for ( ; index < mChildren.size(); ++index){
        mChildren[index]->SetID_i(mNodeID + (char)(index+1));
        mChildren[index]->UpdateChildID_i();
    }
    return true;
}

void TpsCoordinateSystemNode::SetID_i(const std::string& ID){
    mNodeID = ID;
}

std::string TpsCoordinateSystemNode::GetID(){
    return mNodeID;
}

Mcsf::Matrix4f TpsCoordinateSystemNode::GetMatrix(){
    return mTransformMatrix;
}

void TpsCoordinateSystemNode::SetMatrix(const Mcsf::Matrix4f& matrix){
    mTransformMatrix = matrix;
}

void TpsCoordinateSystemNode::Rotate(AXIS a, float radians){

    switch (a) {
    case _X:
        mTransformMatrix.Append(mTransformMatrix.RotationX(radians));
        break;
    case _Y:
        mTransformMatrix.Append(mTransformMatrix.RotationY(radians));
        break;
    case _Z:
        mTransformMatrix.Append(mTransformMatrix.RotationZ(radians));
        break;
    default:
        //log
        break;
    }
}

void TpsCoordinateSystemNode::Rotate(const Mcsf::Vector3f& vec, float radians){

    mTransformMatrix.Append(mTransformMatrix.Rotation(radians, vec));
}

void TpsCoordinateSystemNode::Translate(const Mcsf::Vector3f& vec){

    mTransformMatrix.Append(mTransformMatrix.Translation(vec));
}

void TpsCoordinateSystemNode::Scale(const Mcsf::Vector3f& vec){

    mTransformMatrix.Append(mTransformMatrix.Scale(vec));
}


void TpsCoordinateSystemNode::UpdateChildID_i(){

    for (int i=0; i<mChildren.size(); ++i){
        mChildren[i]->SetID_i(mNodeID + char(i));
        mChildren[i]->UpdateChildID_i();
    }
}

void TpsCoordinateSystemNode::SetParent_i(const TpsCoordinateSystemNode::Ptr& node)
{
    mParent = node;
}

TPS_END_NAMESPACE  // end namespace tps