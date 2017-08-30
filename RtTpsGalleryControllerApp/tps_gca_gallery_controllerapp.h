//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_gallery_controllerapp.h
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_GCA_GALLERY_CONTROLLERAPP_H_
#define TPS_GCA_GALLERY_CONTROLLERAPP_H_

#include "RtTpsGalleryControllerApp/tps_gca_igallery_controller.h"

TPS_BEGIN_NAMESPACE

class TpsImgRenderController;
class TpsGalleryRenderProxy;
class TpsImage3DEntity;

class GalleryControllerApp : public IGalleryController{

public:
    GalleryControllerApp(int id);
    virtual ~GalleryControllerApp(void);

    virtual int Start(const std::vector<std::string> &args);
    virtual int End();

    /// \brief CreateViewModule
    ///  
    /// \param[in] window
    /// \param[in] displaySize
    /// \param[in] unit
    /// \return int TPS_SUCCESS or TPS_FAILURE
    virtual int CreateViewModule(WINDOW_TYPE window, 
        DISPLAY_SIZE displaySize, LAYOUT_UNIT unit);

    virtual bool HasViewModule(WINDOW_TYPE window);

    /// \brief ResizeViewModule
    ///  
    /// \param[in] window
    /// \param[in] displaySize
    /// \return int TPS_SUCCESS or TPS_FAILURE
    virtual int ResizeViewModule(WINDOW_TYPE window, 
        DISPLAY_SIZE displaySize);

    /// \brief Load a series and show it
    /// \param[in] sImageDataUID
    /// \return int 
    virtual int LoadImageData(const std::string& sImageDataUID);

    virtual int LoadImage4DData(const std::vector<std::string>& series4dUidList);

    /// \brief GetMPROperator
    ///  
    /// \return IMPROperator*    pointer to IMPROperator interface
    IMPROperator* GetMPROperator();

    virtual int UpdateCrosshair(LAYOUT_UNIT unit, int iMouseAction, 
        double xStart, double yStart, double xEnd, double yEnd, 
        int nHitTestMode);

    virtual int UpdateCrosshairMode(int nCrosshairMode, int nVisible);

    virtual int GetCrosshairMode(int& iCrosshairMode);    

    /// \brief GetDataLoader
    ///  
    /// \return IDataLoader*    pointer to IDataLoader interface
    virtual ITpsGcaDataLoader* GetDataLoader();

    /// \brief GetDataAccessor
    ///  
    /// \return IDataAccessor*    pointer to IDataAccessor interface
    virtual ITpsGcaDataAccessor* GetDataAccessor();

    virtual void ClearImageDatas();

    virtual void ClearImageData(const std::string& sSeriesUID);

    virtual bool RotateShadedSurface(const Mcsf::Point2f& prePt, const Mcsf::Point2f &curPt);

    virtual bool GetCurrentMPRSlice(WINDOW_TYPE strWidowType, int& currentSlice, int& totalSlice);

    virtual bool DeleteSliceInLocal(const std::string& sSeriesUID, int& currentSlice);    

    virtual std::string GetCurrentSeriesUID();

    virtual std::string GetReplyString(bool bIsSucceeded);

private:
    /// \brief render window
    void Render_i();
private:
    //Render controller
    TpsImgRenderController      *mImgRenderController;

    std::string                 mCurrentImageDataUid;

    //Render proxy
    TpsGalleryRenderProxy    *mRenderProxy;     

    IMPROperator        *mMPROperator;

    ITpsGcaDataAccessor       *mDataAccessor;
    ITpsGcaDataLoader         *mDataLoader;
};

DECLARE_APP_CONTROLLER(GalleryControllerApp);

TPS_END_NAMESPACE

#endif
