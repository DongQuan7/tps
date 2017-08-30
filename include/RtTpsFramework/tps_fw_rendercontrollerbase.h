////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_rendercontrollerbase.h
/// 
///  \brief class TpsRenderControllerBase declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/11/29
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_RENDERCONTROLLERBASE_H_
#define TPS_FW_RENDERCONTROLLERBASE_H_

#include "RtTpsFramework/tps_fw_defs.h"

#include <memory>
#include <map>
#include <vector>

namespace Mcsf{
    namespace MedViewer3D{
        class ClsView;
        class RenderStrategyGPU;
        class IGraphicObject;
        class Point3D;
        class Point2D;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsDisplayCache;
class TpsGOConverterBase;
typedef std::shared_ptr<TpsDisplayCache>                        TpsDisplayCachePtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::ClsView>             ClsViewPtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject>      IGraphicObjectPtr;
class ModelWarehouse;
class ITpsDataRepository;
class GOConverterFactoryBase;
struct HitTestResult;
class TpsRenderStrategyBase;
typedef std::shared_ptr<TpsRenderStrategyBase> TpsRSPtr;
struct GOConverterParameter;

/// \class TpsRenderControllerBase
/// 
/// \brief Render controller is used to setup a rendering scene using 3D 
///        render engine according to the data from TPS data layer, 
///        and take charge of it.
///        This class is an abstract base class
class TPS_FW_DECLSPEC TpsRenderControllerBase
{
public:
    typedef std::map<DATA_SOURCE_ID, DATA_SOURCE_UID> DataSourceMap;
    //the constructor and destructor
    TpsRenderControllerBase(WINDOW_TYPE type);
    virtual ~TpsRenderControllerBase();

    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image),
    //         check in render controller in RenderConponentRecorder if needed
    // \return  bool  true if succeeded
    //                if no data source has been set or other error occurs
    // \note    when group id equals to -1, it will neglect check in.
    virtual bool Initialize(DISPLAY_SIZE displaySize, ITpsDataRepository* dataRepos, 
        ModelWarehouse* modelWarehouse,
        GOConverterFactoryBase* goConvFactory);
    
    void SetModelWarehouse(ModelWarehouse* modelWarehouse) {mModelWarehouse=modelWarehouse;}

    //////////////////////////////////////////////////////////////////////////
    // \brief  set data source
    // \param[in] id  the identity of data source
    // \param[in] uid  the UID of data source in database
    // \return bool  true when succeeded, false otherwise
    virtual bool SetDataSource(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief  get data source
    // \param[in] id  the identity of data source
    // \param[in] uid  the UID of data source in database
    // \return bool  true when succeeded, false otherwise
    virtual bool GetDataSource(DATA_SOURCE_ID id, DATA_SOURCE_UID& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief  set data source
    // \param[in] id  the identity of data source
    virtual void RemoveDataSource(DATA_SOURCE_ID id);

    /// clear graphic objects from ModelWarehouse
    /// if the data source is the current loaded one, remove data source.
    virtual void ClearGraphicObjects(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief  transform point form 2d screen coordinate to \
    //         specified c.s. related to the render controller
    // \param[in] ptIn  the view 2d normalized point
    // \param[out] ptOut  result point of render controller's coordinate
    // \return bool  true when succeeded, false otherwise
    virtual bool TransformPoint(const Mcsf::MedViewer3D::Point2D& ptIn, 
        Mcsf::MedViewer3D::Point3D& ptOut);

    virtual bool TransformPoint(const Mcsf::MedViewer3D::Point3D& ptIn,
        Mcsf::MedViewer3D::Point2D& ptOut);

    virtual bool TransformPoints(
        const std::vector<Mcsf::MedViewer3D::Point2D>& ptListIn, 
        std::vector<Mcsf::MedViewer3D::Point3D>& ptListOut,
        bool isSquaredPoints = true);

    virtual bool MapMilimeter2Pixel(double lengthInmm,
        double* pixelinXNP, double* pixelinYNP);

    //////////////////////////////////////////////////////////////////////////
    // \brief get the window type of render controller
    // \return  WINDOW_TYPE, such as BEV, MPR_AXIAL, MPR_AXIAL_SOURCE_GROUP_FUSION_FIXED, .etc
    //          in our case, it corresponds to a view control of front end, 
    WINDOW_TYPE GetWindowType() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief get the data source group that id render controller belongs to
    DATA_SOURCE_GROUP_ID GetDataSourceGroupID() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief  Resize the render unit of render controller
    // \param[in] display size, width & height
    virtual void Resize(DISPLAY_SIZE displaySize);

    //////////////////////////////////////////////////////////////////////////
    // \brief  Get DisplayCache pointer
    // \return  TpsDisplayCache, pointer to internal TpsDisplayCache object
    TpsDisplayCache* GetDisplayCache() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief  get the display size of view
    // \param[out]  displaySize  the result display size
    // \return bool true when succeeded, false otherwise
    virtual bool GetDisplaySize(DISPLAY_SIZE& displaySize);

    //////////////////////////////////////////////////////////////////////////
    // \brief  synchronize the render thread using ClsView::Render(true) interface
    //         if view object has not been created yet, return.
    void SynchronizeRenderThread();

    //////////////////////////////////////////////////////////////////////////
    // \brief  hit test the graphics rendered on current window
    // \param[in] screenX  normalized screen x
    // \param[in] screenY  normalized screen y
    // \param[out] needRefresh if need to refresh window
    // \return HitTestResult  hit test result including: 
    //                        hit num, hit target uid, is dirty flag
    virtual HitTestResult HitTest(double screenX, double screenY);

    //if the window is deactived, no rendering command will be executed.
    void Active();
    void Deactive();
    //void LazyRender();

    virtual bool GetCurrentPixelSpacingIn10MM(double& spacingX, double& spacingY);
protected:
    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image)
    // \return  bool  true if succeeded
    //                if no data source has been set or other error occurs
    virtual bool SetupScene_i() = 0;

    //////////////////////////////////////////////////////////////////////////
    // \brief reset data source, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \param[in] id  the identity of data source
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetDataSource_i(DATA_SOURCE_ID /*id*/) { return true; }

    //////////////////////////////////////////////////////////////////////////
    // \brief remove data source via specified data source id
    virtual void RemoveDataSource_i(DATA_SOURCE_ID /*id*/) { return; }

    //////////////////////////////////////////////////////////////////////////
    // \brief check graphic object in model, if not exist, 
    //        get its converter and create a new one
    // \param[in] baseUID  the base data UID in database, image or plan
    // \param[in] goType  the graphic object type defined in tps_fw_renderdefines.h file
    // \param[out] go  the result graphic object pointer 
    // \param[in] forceToUpdate  flag indicates if need force to update graphic object even already exists.
    // \return bool  true when succeeded, false otherwise
    bool CheckGraphicObject_i(const std::string& baseUID, GO_TYPE goType, IGraphicObjectPtr& go,
         bool needForceToUpdate = false, GOConverterParameter* para = nullptr);

    void RemoveGraphicObject_i(const std::string& baseUid, GO_TYPE goType, int componentID = -1);

    /// clear graphic objects from ModelWarehouse
    virtual void ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);

    void RegisterRenderStrategy_i(int overlay, const TpsRSPtr& rs);
protected:
    //data source map
    DataSourceMap       mDataSourceMap;
    //WINDOW TYPE, it corresponds to a view control of front end, 
    //such as BEV, MPR_AXIAL, MPR_AXIAL_SOURCE_GROUP_FUSION_FIXED, .etc
    WINDOW_TYPE         mWindowType;
    //the activity of render controller, when hide, no rendering will be done.
    bool                mVisibility;

    //View component
    ClsViewPtr                  mView;
    TpsDisplayCachePtr          mDisplayCache;
    ModelWarehouse*             mModelWarehouse;
    ITpsDataRepository*         mDataRepository;
    GOConverterFactoryBase*     mGOConverterFactory;

    //overlay-strategy list map
    std::map<int, std::vector<TpsRSPtr>> mStrategyList;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsRenderControllerBase);

    //////////////////////////////////////////////////////////////////////////
    // \brief  get a unique converter instance via graphic object type
    // \param[in] goType the graphic object type defined in "tps_fw_renderdefines.h"
    // \return TpsGOConverterBase*  the converter pointer to be checked
    //         NULL will be returned if any error happens
    virtual TpsGOConverterBase* GetConverter_i(GO_TYPE goType) = 0;
};

TPS_END_NAMESPACE  // end namespace tps

#endif
