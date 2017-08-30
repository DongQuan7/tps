//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_igallery_controller.h
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_GCA_IGALLERY_CONTROLLER_H_
#define TPS_GCA_IGALLERY_CONTROLLER_H_

#include "tps_gca_defs.h"

//ZHENGHE
#include "McsfContainee/mcsf_containee_base.h"

//TPS
#include "RtTpsRenderController/tps_rc_renderdefines.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_controllerbase.h"
#include "RtTpsProtoLib/rt_tps_certifiedpoi.h"

#include "RtTpsGalleryControllerApp/tps_gca_idataloader.h"
#include "RtTpsGalleryControllerApp/tps_gca_idataaccessor.h"

namespace Mcsf {
    class Point2f;
}

TPS_BEGIN_NAMESPACE

//class ITpsGcaDataLoader;
//class ITpsGcaDataAccessor;
class IMPROperator;

const std::string INAME_GALLERY_CONTROLLER = "TpsGalleryControllerInterface";

class TpsControllerBase;

class IGalleryController : public TpsControllerBase{
public:
    IGalleryController(int id):TpsControllerBase(id) {};
    virtual ~IGalleryController(void){};

    /// \brief CreateViewModule
    ///  
    /// \param[in] window
    /// \param[in] displaySize
    /// \param[in] unit
    /// \return int TPS_SUCCESS or TPS_FAILURE
    virtual int CreateViewModule(WINDOW_TYPE window, 
        DISPLAY_SIZE displaySize, LAYOUT_UNIT unit) = 0;

    virtual bool HasViewModule(WINDOW_TYPE window) = 0;

    /// \brief ResizeViewModule
    ///  
    /// \param[in] window
    /// \param[in] displaySize
    /// \return int TPS_SUCCESS or TPS_FAILURE
    virtual int ResizeViewModule(WINDOW_TYPE window, 
        DISPLAY_SIZE displaySize) = 0;

    /// \brief Load a series and show it
    /// \param[in] sImageDataUID
    /// \return int 
    virtual int LoadImageData(const std::string& sImageDataUID) = 0;

    virtual int LoadImage4DData(const std::vector<std::string>& series4dUidList) = 0;

    /// \brief GetMPROperator
    ///  
    /// \return IMPROperator*    pointer to IMPROperator interface
    virtual IMPROperator* GetMPROperator() = 0;

    virtual int UpdateCrosshair(LAYOUT_UNIT unit, int iMouseAction, 
        double xStart, double yStart, double xEnd, double yEnd, 
        int nHitTestMode) = 0;

    virtual int UpdateCrosshairMode(int nCrosshairMode, int nVisible) = 0;

    virtual int GetCrosshairMode(int& iCrosshairMode) = 0;

    /// \brief GetDataLoader
    ///  
    /// \return IDataLoader*    pointer to IDataLoader interface
    virtual ITpsGcaDataLoader* GetDataLoader() = 0;

    /// \brief GetDataAccessor
    ///  
    /// \return IDataAccessor*    pointer to IDataAccessor interface
    virtual ITpsGcaDataAccessor* GetDataAccessor() = 0;

    virtual void ClearImageDatas() = 0;

    virtual void ClearImageData(const std::string& sSeriesUID) = 0;

    virtual bool RotateShadedSurface(const Mcsf::Point2f& prePt, const Mcsf::Point2f &curPt) =0;

    virtual bool GetCurrentMPRSlice(WINDOW_TYPE strWidowType, int& currentSlice, int& totalSlice) = 0;

    virtual bool DeleteSliceInLocal(const std::string& sSeriesUID, int& currentSlice) = 0;     

    virtual std::string GetCurrentSeriesUID() = 0;

    // Temporary, Use Proto?
    virtual std::string GetReplyString(bool bIsSucceeded) = 0;
};


TPS_END_NAMESPACE

#endif //TPS_GCA_IGALLERY_CONTROLLER_H_