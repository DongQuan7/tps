////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_statecontrolforbevgraphicobejct.h
/// 
///  \brief class StateControlForBEVGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/07/07
////////////////////////////////////////////////////////////////


#ifndef TPS_RL_STATECONTROLFORBEVGRAPHICOBJECT_H_
#define TPS_RL_STATECONTROLFORBEVGRAPHICOBJECT_H_

#include "tps_rl_defs.h"
//

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"


TPS_BEGIN_NAMESPACE



class TPS_RL_DECLSPEC StateControlGraphicObject : public TpsGraphicObjectBase {
public:
    StateControlGraphicObject();
    virtual ~StateControlGraphicObject();

    inline void SetType(BEV_DISPLAY_TYPE type) 
	{
		mType = type;
	}
    inline BEV_DISPLAY_TYPE GetType() const { return mType;}

    inline void SetDrrVisibility(bool bVisibility) { mDrrVisibility = bVisibility;}
    inline bool GetDrrVisibility() const { return mDrrVisibility;}

    inline void SetMlcTransparency(float fMlcTransparency){
        mMlcTransparency = fMlcTransparency;
    }

    inline float GetMlcTransparency() const { return mMlcTransparency;}

    inline void SetDownSampleRate(float downSampleRatio) {
        mDownSampleRate = downSampleRatio; }
    inline float GetDownSampleRate() const { return mDownSampleRate; }

    //only accepted BEV & SURFACE
//     inline void SetWindowType(WINDOW_TYPE_TPS type) { mWindowType = type;}
//     inline WINDOW_TYPE_TPS GetWindowType() const { return mWindowType;}

private:
    BEV_DISPLAY_TYPE        mType;
    bool                    mDrrVisibility;
    //WINDOW_TYPE_TPS         mWindowType;
    float                   mMlcTransparency;
    float                   mDownSampleRate;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(StateControlGraphicObject);

};
TPS_END_NAMESPACE
#endif
