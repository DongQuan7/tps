////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_poigraphicobjectcollection.h
/// 
///  \brief class TpsPoiGraphicObjectCollection declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/2/12
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_POIGRAPHICOBJECTCOLLECTION_H_
#define TPS_RL_POIGRAPHICOBJECTCOLLECTION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

TPS_BEGIN_NAMESPACE
class TpsPoiGraphicObject;

class TPS_RL_DECLSPEC TpsPoiGraphicObjectCollection : public TpsGraphicObjectBase {
public:
    TpsPoiGraphicObjectCollection();
    ~TpsPoiGraphicObjectCollection();

    void AddPoi(const std::string &uid, std::shared_ptr<TpsPoiGraphicObject>);

    bool DeletePoi(const std::string &);

    void RemoveAll();

    std::shared_ptr<TpsPoiGraphicObject> SelectPoi(const std::string &);

    std::map<std::string, std::shared_ptr<TpsPoiGraphicObject>> GetPoiMap() const;

private:

    std::map<std::string, std::shared_ptr<TpsPoiGraphicObject>>  mPoiMap;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsPoiGraphicObjectCollection);
};

TPS_END_NAMESPACE

#endif