////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_voirenderingcachegocollection.h
/// 
///  \brief class TpsVOIRenderingCacheGOCollection declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/03/13
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOIRENDERINGCACHEGOCOLLECTION_H_
#define TPS_RL_VOIRENDERINGCACHEGOCOLLECTION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsVOIRenderingCacheGO;
typedef std::shared_ptr<TpsVOIRenderingCacheGO> TpsVOIRenderingCacheGOPtr;;
typedef std::map<std::string, TpsVOIRenderingCacheGOPtr> TpsVOICacheGOMap;

/// \class TpsVOIRenderingCacheGOCollection
/// 
/// \brief a collection of VOI rendering cache, also a TpsGraphicObjectBase
class TPS_RL_DECLSPEC TpsVOIRenderingCacheGOCollection : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    TpsVOIRenderingCacheGOCollection();
    virtual ~TpsVOIRenderingCacheGOCollection();

    //////////////////////////////////////////////////////////////////////////
    // \brief get VOI cache map
    // \return TpsVOICacheGOMap  the VOI cache map
    TpsVOICacheGOMap GetVOICacheList() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief get VOI rendering cache via specified uid
    // \param[in] uid  the uid of VOI in database
    // \return TpsVOIRenderingCacheGOPtr  the VOI rendering pointer, NULL if not exist.
    TpsVOIRenderingCacheGOPtr GetCache(const std::string& uid) const;

    //////////////////////////////////////////////////////////////////////////
    // \brief add VOI rendering cache by uid - voi pair
    // \param[in] uid  the uid of VOI in database
    // \param[in] cache  the VOI cache pointer
    void AddCache(const std::string& uid, TpsVOIRenderingCacheGOPtr cache);

    //////////////////////////////////////////////////////////////////////////
    // \brief remove VOI rendering cache by uid - voi pair
    // \param[in] uid  the uid of VOI in database
    void RemoveCache(const std::string& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief remove all VOI rendering cache
    void RemoveAllCache();

    //////////////////////////////////////////////////////////////////////////
    // \brief set dirty flag
    virtual void SetDirty(bool bFlag);
    virtual bool IsDirty() const;

    //set section that the cache collection belongs to
    FIRST_POSTFIX_COMPONENT GetSection() const;
    void SetSection(FIRST_POSTFIX_COMPONENT section);

    SECOND_POSTFIX_COMPONENT GetLocation() const;
    void SetLocation(SECOND_POSTFIX_COMPONENT location);

    TpsVOIRenderingCacheGOCollection& operator=(const TpsVOIRenderingCacheGOCollection& src)
    {
        this->mVOICacheMap = src.mVOICacheMap;
        return *this;
    }

private:
    TpsVOICacheGOMap   mVOICacheMap;

    FIRST_POSTFIX_COMPONENT      mSectionType;
    SECOND_POSTFIX_COMPONENT     mLocation;
private:
    // the default copy and assign method are not allowed.
    //TPS_DISALLOW_COPY_AND_ASSIGN(TpsVOIRenderingCacheGOCollection);
};

TPS_END_NAMESPACE  // end namespace tps

#endif