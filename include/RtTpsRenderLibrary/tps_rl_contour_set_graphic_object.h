#ifndef TPS_RL_CONTOUR_SET_GRAPHIC_OBJECT_H_
#define TPS_RL_CONTOUR_SET_GRAPHIC_OBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_composable_graphic_object.h"
#include <map>

namespace Mcsf
{
    namespace MedViewer3D
    {
        class Vector3D;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class ContourSet;

/// \class ContourSetGraphicObject
/// 
/// \brief *****
class TPS_RL_DECLSPEC ContourSetGraphicObject : public TpsGraphicObjectBase
{
public:
    //the constructor and destructor
    ContourSetGraphicObject();
    virtual ~ContourSetGraphicObject();

    ContourSet* GetContourSet() const;
private:
    ContourSet* mContourSet;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ContourSetGraphicObject);
};

typedef std::shared_ptr<ContourSetGraphicObject> CSGOPtr;

class TPS_RL_DECLSPEC CSGOCollection : public TpsGraphicObjectBase
{
public:
    //the constructor and destructor
    CSGOCollection();
    virtual ~CSGOCollection();

    void Clear();

    void AddContourSet(const std::string& uid, const CSGOPtr& csGO);
    CSGOPtr GetContourSet(const std::string& uid) const;
    std::map<std::string, CSGOPtr> GetCSGOMap() const;

    Mcsf::MedViewer3D::Vector3D GetNormal() const;
    void SetNormal(const Mcsf::MedViewer3D::Vector3D& normal);
private:
    std::map<std::string, CSGOPtr> mCSGOMap;
    Mcsf::MedViewer3D::Vector3D    mNormal;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(CSGOCollection);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RL_CONTOUR_SET_GRAPHIC_OBJECT_H_
