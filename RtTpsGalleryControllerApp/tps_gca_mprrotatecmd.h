//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_gca_mprrotatecmd.h
///  \brief   
///
///  \version 1.0
///  \date    Feb. 05, 2015
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_MPRROTATECMD_H_
#define TPS_GCA_MPRROTATECMD_H_

#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class TpsGcaMPRRotateCmd : public TpsCommand 
{
public:
    /////////////////////////////////////////////////////////////////
    ///  \brief Constructor
    ///
    ///  \param[in]    CertifiedPoi
    ///  \param[out]   
    ///  \return       
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    TpsGcaMPRRotateCmd(const tps::LAYOUT_UNIT &unit, /*const std::string &drrImageUID,*/
        const float startX, const float startY, const float stopX, const float stopY);
    /////////////////////////////////////////////////////////////////
    ///  \brief Destructor
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return       
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    ~TpsGcaMPRRotateCmd();
    ///////////////////////////////////////////////////////////////////
    /////  \brief Set RenderProxy
    /////
    /////  \param[in]    
    /////  \param[out]   
    /////  \return       
    /////  \exceptions:   none  
    ///////////////////////////////////////////////////////////////////
    //void SetRenderProxy(TpsEasyPlanRenderProxy *renderProxy);
protected:
    virtual bool PreExecute();
    virtual bool Execute();
    virtual bool PostExecute();
    TpsGcaMPRRotateCmd * Clone();

private:
    //TpsEasyPlanRenderProxy    *mEasyPlanRenderProxy;
    LAYOUT_UNIT                mLayoutUnit;
    //std::string                mDrrImageUID; 
    float                    mStartX;
    float                    mStartY;
    float                   mStopX;
    float                   mStopY;
//private:
//    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGcaMPRRotateCmd);
};

TPS_END_NAMESPACE

#endif