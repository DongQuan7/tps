////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_manualremovetablegraphicobject.h
/// 
///  \brief class ManualRemoveTableGraphicObject 
/// 
///  \version 1.0
/// 
///  \date    2014/07/08
////////////////////////////////////////////////////////////////


#ifndef TPS_RL_MANUALREMOVETABLEGRAPHICOBJECT_H_
#define TPS_RL_MANUALREMOVETABLEGRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"


TPS_BEGIN_NAMESPACE
class TPS_RL_DECLSPEC ManualRemoveTableGraphicObject : public TpsGraphicObjectBase {
public:
    ManualRemoveTableGraphicObject();
    virtual ~ManualRemoveTableGraphicObject();

    void SetHeight(float);

    float GetHeight() const;

    void SetVisibilty(bool bVisible);

    bool GetVisibilty() const;

private:
    float  mLineHeight;
    bool   mIsVisible;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(ManualRemoveTableGraphicObject);

};
TPS_END_NAMESPACE
#endif