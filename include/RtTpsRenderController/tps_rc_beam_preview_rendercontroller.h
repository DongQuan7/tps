////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  CHUNHUI SHI  chunhui.shi@united-imaging.com
/// 
///  \file tps_rc_beam_preview_rendercontroller.h
/// 
///  \brief class TpsBeamPreviewRenderController declaration
/// 
///  \version 1.0
/// 
///  \date    2014/11/10
////////////////////////////////////////////////////////////////

#ifndef TPS_RC_BEAM_PREVIEW_RENDERCONTROLLER_H_
#define TPS_RC_BEAM_PREVIEW_RENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_rendercontroller.h"

TPS_BEGIN_NAMESPACE
class MPRRenderStrategy;
class TpsBeamRenderStrategy;

class TPS_RC_DECLSPEC TpsBeamPreviewRenderController : public TpsRenderController 
{
public:
    TpsBeamPreviewRenderController(WINDOW_TYPE type);
    virtual ~TpsBeamPreviewRenderController();

    FIRST_POSTFIX_COMPONENT  GetSectionType() ;
    SECOND_POSTFIX_COMPONENT GetSectionExtendType();

protected:
    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image)
    // \return  bool  true if succeeded
    //                if no data source has been set or other error occurs
    virtual bool SetupScene_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset data source, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \param[in] id  the identity of data source
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetDataSource_i(DATA_SOURCE_ID id);

    /// clear graphic objects from ModelWarehouse
    virtual void ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);
private:
    //////////////////////////////////////////////////////////////////////////
    // \brief reset image data, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetImage_i();

    virtual bool ResetPlan_i();
private:
    
    std::shared_ptr<MPRRenderStrategy>              m_spMPRStrategy;
    std::shared_ptr<TpsBeamRenderStrategy>          mBeamStrategy;

    FIRST_POSTFIX_COMPONENT                     m_nSectionType;// = EMPTY_COMPONENT;
    SECOND_POSTFIX_COMPONENT                    m_nSectionExtendType;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsBeamPreviewRenderController);
};

TPS_END_NAMESPACE

#endif
