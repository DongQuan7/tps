////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_coordinatesystemtree.h
/// 
///  \brief class TpsCoordinateSystemTree declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_COORDINATESYSTEMTREE_H_
#define TPS_FW_COORDINATESYSTEMTREE_H_

#include "tps_fw_defs.h"
#include <memory>

namespace Mcsf{
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsCoordinateSystemNode;
typedef std::shared_ptr<TpsCoordinateSystemNode> CSNodePtr;

/// \class TpsCoordinateSystemTree
/// 
/// \brief *****
class TPS_FW_DECLSPEC TpsCoordinateSystemTree{
public:
    //the constructor and destructor
    TpsCoordinateSystemTree();
    ~TpsCoordinateSystemTree();

    bool GetTransformMatrix(const std::string& sourceNodeID,
                            const std::string& targetNodeID, 
                            Mcsf::Matrix4f& matrix) const;

    CSNodePtr Root() const { return mRootNode; }
    CSNodePtr GetNode(const std::string& nodeID) const;
    bool RemoveNode(const std::string& nodeID);

private:
    bool GetToRoot_i(const std::string& nodeID, Mcsf::Matrix4f& matrix) const;

private:
    CSNodePtr           mRootNode;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsCoordinateSystemTree);
};

TPS_END_NAMESPACE  // end namespace tps

#endif