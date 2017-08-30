////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_voigraphicobjectcollection.h
/// 
///  \brief class TpsVOIGOCollection declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOIGRAPHICOBJECTCOLLECTION_H_
#define TPS_RL_VOIGRAPHICOBJECTCOLLECTION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsVOIGraphicObject;
typedef std::shared_ptr<TpsVOIGraphicObject> TpsVOIGraphicObjectPtr;
typedef std::map<std::string, TpsVOIGraphicObjectPtr> TpsVOIGOMap;

/// \class TpsVOIGOCollection
/// 
/// \brief a collection of VOI, also a TpsGraphicObjectBase
class TPS_RL_DECLSPEC TpsVOIGOCollection : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    TpsVOIGOCollection();
    virtual ~TpsVOIGOCollection();

    //////////////////////////////////////////////////////////////////////////
    // \brief get VOI map of VOI collection
    // \return TpsVOIGOMap  the VOI map
    TpsVOIGOMap GetVOIList() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief get VOI via specified uid
    // \param[in] uid  the uid of VOI in database
    // \return TpsVOIGraphicObjectPtr  the VOI pointer, NULL if not exist.
    TpsVOIGraphicObjectPtr GetVOI(const std::string& uid) const;

    //////////////////////////////////////////////////////////////////////////
    // \brief add VOI by uid - voi pair
    // \param[in] uid  the uid of VOI in database
    // \param[in] voi  the VOI pointer
    void AddVOI(const std::string& uid, TpsVOIGraphicObjectPtr voi);

    //////////////////////////////////////////////////////////////////////////
    // \brief remove VOI by uid - voi pair
    // \param[in] uid  the uid of VOI in database
    void RemoveVOI(const std::string& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief update block buffer of specified voi
    // \param[in] uid  the uid of VOI in database
    void UpdateVOI(const std::string& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief remove all VOI
    void RemoveAllVOI();

    //////////////////////////////////////////////////////////////////////////
    // \brief set dirty flag
    virtual void SetDirty(bool bFlag);

    TpsVOIGOCollection& operator=(const TpsVOIGOCollection& src);

private:
    TpsVOIGOMap   mVOIGOMap;
private:
    // the default copy and assign method are not allowed.
    //TPS_DISALLOW_COPY_AND_ASSIGN(TpsVOIGOCollection);
};

TPS_END_NAMESPACE  // end namespace tps

#endif