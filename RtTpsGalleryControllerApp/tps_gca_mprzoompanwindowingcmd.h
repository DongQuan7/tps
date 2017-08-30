//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_mprzoompanwindowingcmd.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_MPRZOOMPANWINDOWINGCMD_H_
#define TPS_GCA_MPRZOOMPANWINDOWINGCMD_H_

#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsRenderController/tps_rc_renderdefines.h"

TPS_BEGIN_NAMESPACE

class TpsGcaMPRZoomCmd : public TpsCommand
{
public:
    enum ZOOMPARAMODE {
        BY_FACTOR,
        BY_VECTOR
    };

    TpsGcaMPRZoomCmd(LAYOUT_UNIT unit, float scaleFactor);
    TpsGcaMPRZoomCmd(LAYOUT_UNIT unit, float startX, float startY, 
        float stopX, float stopY);
    virtual ~TpsGcaMPRZoomCmd(void);

protected:
    virtual bool PreExecute();
    virtual bool Execute();
    virtual bool PostExecute();

    TpsGcaMPRZoomCmd * Clone();


private:
    LAYOUT_UNIT                 mLayoutUnit; 
    float                       mScaleFactor;
    float                       mStartX;
    float                       mStartY;
    float                       mStopX;
    float                       mStopY;
    ZOOMPARAMODE                mZoomInputMode;
};

class TpsGcaMPRPanCmd : public TpsCommand
{
public:
    TpsGcaMPRPanCmd(LAYOUT_UNIT unit, float startx, float starty, float stopx, 
        float stopy);
    virtual ~TpsGcaMPRPanCmd(void);

protected:
    virtual bool PreExecute();
    virtual bool Execute();
    virtual bool PostExecute();

    TpsGcaMPRPanCmd * Clone();


private:
    LAYOUT_UNIT                 mLayoutUnit;
    float                       mStartX;
    float                       mStartY;
    float                       mStopX;
    float                       mStopY;
};

class TpsGcaMPRWindowingCmd : public TpsCommand
{
public:
    TpsGcaMPRWindowingCmd(LAYOUT_UNIT unit, float deltaWidth, float deltaCenter, bool updateAll);
    virtual ~TpsGcaMPRWindowingCmd(void);

protected:
    virtual bool PreExecute();
    virtual bool Execute();
    virtual bool PostExecute();

    TpsGcaMPRWindowingCmd * Clone();


private:
    LAYOUT_UNIT                 mLayoutUnit;
    float                       mDeltaWidth;
    float                       mDeltaCenter;
    std::string                 mImageUid;
    bool                        mUpdateAll;
};

TPS_END_NAMESPACE

#endif //TPS_COMMON_MPRZOOMPANCMD_H_
