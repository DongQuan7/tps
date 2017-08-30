#ifndef TPS_FW_COMPOSABLE_GRAPHIC_OBJECT_H_
#define TPS_FW_COMPOSABLE_GRAPHIC_OBJECT_H_

#include "tps_fw_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

#include <map>

TPS_BEGIN_NAMESPACE  // begin namespace tps

class ComposableGraphicObject : public TpsGraphicObjectBase
{
public:
    typedef std::shared_ptr<TpsGraphicObjectBase> Ptr;
    typedef std::map<int, Ptr> ComponentsMap;

    ComposableGraphicObject() {}
    virtual ~ComposableGraphicObject() {}

    inline void ComposeOne(int id, const Ptr& go) 
    {
        DecomposeOne(id);
        mComponentsMap[id] = go;
    }

    inline void DecomposeOne(int id) 
    {
        auto it = mComponentsMap.find(id);
        if (it != mComponentsMap.end())
        {
            mComponentsMap.erase(it);
        }
    }
    
    inline void DecomposeAll()
    {
        mComponentsMap.clear();
    }

    inline int ComponentsCount() const 
    {
        return mComponentsMap.size();
    }

    inline ComponentsMap GetComponentsMap() const
    {
        return mComponentsMap;
    }
private:
    ComponentsMap mComponentsMap;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_FW_COMPOSABLE_GRAPHIC_OBJECT_H_