////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_coordinatesystemnode.h
/// 
///  \brief class CoordinateSystemNode declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_COORDINATESYSTEMNODE_H_
#define TPS_FW_COORDINATESYSTEMNODE_H_

#include "tps_fw_defs.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include <memory>
#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

enum AXIS
{
    _X,
    _Y,
    _Z
};

/// \class TpsCoordinateSystemNode
/// 
/// \brief *****
class TPS_FW_DECLSPEC TpsCoordinateSystemNode : 
    public std::enable_shared_from_this<TpsCoordinateSystemNode>{

public:
    typedef std::shared_ptr<TpsCoordinateSystemNode> Ptr;

    //the constructor and destructor
    TpsCoordinateSystemNode();
    ~TpsCoordinateSystemNode();

    bool AddChild(const Ptr& node);
    //return nullptr if not exist
    Ptr GetChild(const std::string& nodeID);
    Ptr GetImmediateChild(unsigned int index);
    Ptr GetParent();

    //this action will change the children's id, which index is bigger.
    //NOTE: because of its heavy cost, in this coordinate system tree,
    //removing node is not recommended.
    bool RemoveChild(const std::string& nodeID);
    bool RemoveImmediateChild(unsigned int index);

    std::string GetID();

    Mcsf::Matrix4f GetMatrix();
    void SetMatrix(const Mcsf::Matrix4f& matrix);
    void Rotate(AXIS a, float radians);
    void Rotate(const Mcsf::Vector3f& vec, float radians);
    void Translate(const Mcsf::Vector3f& vec);
    void Scale(const Mcsf::Vector3f& vec);
private:
    void SetID_i(const std::string& ID);
    void SetParent_i(const Ptr& node);
    //when node id changed, refresh all children's id
    void UpdateChildID_i();
private:
    std::string                         mNodeID;
    //transform matrix from this node to its father.
    Mcsf::Matrix4f                      mTransformMatrix;

    std::vector<Ptr>                    mChildren;
    Ptr                                 mParent;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsCoordinateSystemNode);
};

TPS_END_NAMESPACE  // end namespace tps

#endif