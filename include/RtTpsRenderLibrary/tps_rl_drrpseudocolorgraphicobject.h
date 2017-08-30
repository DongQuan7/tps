//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rl_drrpseudocolorgraphicobject.h 
///  \brief   pseudo color go for tda drr go base
///
///  \version 1.0
///  \date  2014/07/04  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RL_DRRPSEUDOCOLORGRAPHICOBJECT_H_
#define TPS_RL_DRRPSEUDOCOLORGRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC DrrPseudoColorGOBase : public TpsGraphicObjectBase {
public:
    /////////////////////////////////////////////////////////////////
    ///  \brief Constructor
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    DrrPseudoColorGOBase();
    /////////////////////////////////////////////////////////////////
    ///  \brief Destructor
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    virtual ~DrrPseudoColorGOBase();

    /////////////////////////////////////////////////////////////////
    ///  \brief Set pseudo color table for fix image
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    virtual void SetFixPseudoColorTable(unsigned char* pData, unsigned int uiSize);
    /////////////////////////////////////////////////////////////////
    ///  \brief set pseudo color table for float image
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    virtual void SetFloatPseudoColorTable(unsigned char* pData, unsigned int uiSize);

protected:
    std::shared_ptr<unsigned char> mFixPseudoColorTable;
    std::shared_ptr<unsigned char> mFloatPseudoColorTable;
};

TPS_END_NAMESPACE
#endif