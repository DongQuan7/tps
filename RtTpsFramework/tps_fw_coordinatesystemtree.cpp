////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_coordinatesystemtree.cpp
/// 
///  \brief class TpsCoordinateSystemTree implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_coordinatesystemtree.h"
#include "RtTpsFramework/tps_fw_coordinatesystemnode.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsCoordinateSystemTree::TpsCoordinateSystemTree()
    try : mRootNode(new TpsCoordinateSystemNode()) {

}
catch(const std::bad_alloc &){
    //LOG  bad allocate occurs
}

TpsCoordinateSystemTree::~TpsCoordinateSystemTree(){

}

bool TpsCoordinateSystemTree::GetTransformMatrix(
    const std::string& sourceNodeID, 
    const std::string& targetNodeID, 
    Mcsf::Matrix4f& matrix) const{

    Mcsf::Matrix4f matSource2Root, matTarget2Root;
    if (!GetToRoot_i(sourceNodeID, matSource2Root) || 
        !GetToRoot_i(targetNodeID, matTarget2Root)){
        //LOG not exist
        TPS_LOG_DEV_ERROR<<"Failed to get transformation matrix to root.";
        return false;
    }

    if (!matTarget2Root.HasInverse()){
        //LOG wrong matrix result
        TPS_LOG_DEV_ERROR<<"The target 2 root has no inverse.";
        return false;
    }
    matrix = matTarget2Root.Inverse();
    matrix.Append(matSource2Root);
    return true;
}

CSNodePtr TpsCoordinateSystemTree::GetNode(const std::string& nodeID) const{

    return mRootNode->GetChild(nodeID);
}

bool TpsCoordinateSystemTree::RemoveNode(const std::string& nodeID){
    return mRootNode->RemoveChild(nodeID);
}

bool TpsCoordinateSystemTree::GetToRoot_i(
    const std::string& nodeID, Mcsf::Matrix4f& matrix) const{

    CSNodePtr node = mRootNode;
    Mcsf::Matrix4f tempMatrix = mRootNode->GetMatrix();
    for (unsigned int i=0; i<nodeID.length(); ++i)
    {
        node = node->GetImmediateChild(nodeID[i]-1);
        if (node == nullptr)
        {
            //LOG not exist
            return false;
        }
        tempMatrix.Append(node->GetMatrix());
    }

    matrix = tempMatrix;
    return true;
}

TPS_END_NAMESPACE  // end namespace tps