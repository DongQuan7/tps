////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rc_voi_preview_rendercontroller.h
/// 
///  \brief class TpsVoiPreviewRenderController declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/24
////////////////////////////////////////////////////////////////

#ifndef TPS_RC_VOI_PREVIEW_RENDERCONTROLLER_H_
#define TPS_RC_VOI_PREVIEW_RENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_rendercontroller.h"

TPS_BEGIN_NAMESPACE
class MPRRenderStrategy;
class MprRoiRenderStrategy;

class TPS_RC_DECLSPEC TpsVoiPreviewRenderController : public TpsRenderController 
{
public:
    TpsVoiPreviewRenderController(WINDOW_TYPE type);
    virtual ~TpsVoiPreviewRenderController();

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

    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    void RemoveDataSource_i(DATA_SOURCE_ID id);
private:
    //MPR strategy
    std::shared_ptr<MPRRenderStrategy>          m_spMPRStrategy;

    //VOI strategy
    std::shared_ptr<MprRoiRenderStrategy>       m_spVOIStrategy;
    FIRST_POSTFIX_COMPONENT                     m_nSectionType;// = EMPTY_COMPONENT;
    SECOND_POSTFIX_COMPONENT                    m_nSectionExtendType;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsVoiPreviewRenderController);
};

TPS_END_NAMESPACE

#endif
