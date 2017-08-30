////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_dummygraphicobject.h
/// 
///  \brief class DummyGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/07/25
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DUMMYGRAPHICOBJECT_H_
#define TPS_RL_DUMMYGRAPHICOBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class DummyGraphicObject
/// 
/// \brief *****
class TPS_RL_DECLSPEC DummyGraphicObject : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    DummyGraphicObject() {}
    virtual ~DummyGraphicObject() {}
    
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(DummyGraphicObject);
};

TPS_END_NAMESPACE  // end namespace tps

#endif