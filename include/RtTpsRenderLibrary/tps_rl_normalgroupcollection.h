////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_normalgroupcollection.h
/// 
///  \brief class NormalGroupCollection declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/10
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_NORMALGROUPCOLLECTION_H_
#define TPS_RL_NORMALGROUPCOLLECTION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsBeamGraphicObjectCollection;
typedef std::shared_ptr<TpsBeamGraphicObjectCollection> NormalGroupPtr;
/// \class NormalGroupCollection
/// 
/// \brief *****
class TPS_RL_DECLSPEC NormalGroupCollection : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    NormalGroupCollection();
    virtual ~NormalGroupCollection();

    std::map<std::string, NormalGroupPtr> GetNormalGroupMap();

    NormalGroupPtr GetNormalGroup(const std::string& uid);


    bool AddNormalGroup(const std::string& uid, NormalGroupPtr normalGroup);

    bool RemoveNormalGroup(const std::string& uid);

    void ClearAll();

    bool SetActiveNormalGroup(const std::string& uid);

    NormalGroupPtr GetActiveNormalGroup();

    std::string GetActiveNormalGroupUID();

    virtual bool IsDirty();

    //////////////////////////////////////////////////////////////////////////
    // \brief set dirty flag
    virtual void SetDirty(bool bFlag);

    //bool IsPlanActive();

    //void SetPlanActivity(bool flag);

    void SetSeriesUID(const std::string& seriesUID);
    std::string GetSeriesUID() const;
private:
    std::map<std::string, NormalGroupPtr> mNormalGroupMap;
    std::string                           mActiveNormalGroupUID;
    //bool                                  mIsPlanActive;
    std::string                           mSeriesUID;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(NormalGroupCollection);
};

TPS_END_NAMESPACE  // end namespace tps

#endif