////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rc_rendermanager.h
/// 
///  \brief class TpsRenderManager declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/11/29
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_RENDERPROXYBASE_H_
#define TPS_FW_RENDERPROXYBASE_H_

#include "RtTpsFramework/tps_fw_defs.h"

#include <memory>
#include <map>
#include <algorithm>
// #include "boost/thread.hpp"
// #include "boost/thread/mutex.hpp"

#include "McsfMedViewer3D/McsfMedViewer3DCommon/mcsf_viewer3d_observer_interface.h"
#include "McsfMedViewer3D/McsfMedViewer3DCommon/mcsf_viewer3d_subject.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

namespace Mcsf{
    class Point2f;
    class Point3f;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsDataSourceGroup;
class TpsRenderControllerBase;
typedef std::map<DATA_SOURCE_GROUP_ID, TpsDataSourceGroup*> DataSourceGroupMap;
//class TpsLayoutManager;
//class TpsDisplayDelegate;
//typedef std::shared_ptr<TpsDisplayDelegate> TpsDisplayDelegatePtr;
//typedef std::map<WINDOW_TYPE, TpsRenderControllerBase*> RenderControllerMap;
class ITpsDataRepository;
class ModelWarehouse;
class OperationBase;
class OperationProcessThread;
class GOConverterFactoryBase;
struct HitTestResult;
/// \class TpsRenderProxyBase
/// 
/// \brief this class is the render module of APP controller
///        it is responsible for the initialization of LayoutManager & DisplayDelegate
///        it is also in charge of the management of render controllers & LayoutManager
///        overall, if wanna render something, get the TpsRenderManager instance
class TPS_FW_DECLSPEC TpsRenderProxyBase{
public:
    //the constructor and destructor
    TpsRenderProxyBase(/*bool needCheckIn = true*/);
    virtual ~TpsRenderProxyBase();

    //////////////////////////////////////////////////////////////////////////
    // \brief  initialize the render proxy, setup data source group
    // \return bool  true when succeeded, false otherwise
    virtual bool Initialize(ITpsDataRepository* dataRepository);

    //////////////////////////////////////////////////////////////////////////
    // \brief  finalize render proxy, clear data source group, close operation 
    //         thread, clear model warehouse, & delete all objects
    virtual void Finalize();

    //////////////////////////////////////////////////////////////////////////
    // \brief get the layout manager instance pointer
    // \return TpsLayoutManager*, NULL when failed
    //TpsLayoutManager* GetLayoutManager() const;

    //////////////////////////////////////////////////////////////////////////
    /// \brief get data source group via specified id
    /// \return nullptr if not exist
    TpsDataSourceGroup* GetDataSourceGroup(DATA_SOURCE_GROUP_ID id);

    //////////////////////////////////////////////////////////////////////////
    /// \brief request data source group via id, if exists, get the existing one, 
    ///        otherwise, create new one
    /// \return the requested data source group instance
    TpsDataSourceGroup* RequestDataSourceGroup(DATA_SOURCE_GROUP_ID id);

    //////////////////////////////////////////////////////////////////////////
    /// \brief remove all data sources of all data source group
    void RemoveAllDataSources();

    //////////////////////////////////////////////////////////////////////////
    // \brief query a render controller via specified window type, 
    // \param[in] window_type the unique id for windows, 
    //            it also indicates the rendering content
    // \return TpsRenderControllerBase*  a render controller base pointer, 
    //         return NULL if not exist or any other error occurs.

    // 这里之所以使用 WINDOW_TYPE和window uid两个条件进行查询，是因为
    // rendercontroller按两级分组，所以为了提高效率，先使用WINDOW_TYPE查到第一个组
    // 然后使用Uid进行二级查询。备忘。
    TpsRenderControllerBase* GetRenderController(WINDOW_TYPE window, const std::string& wndUid);

    //////////////////////////////////////////////////////////////////////////
    // \brief remove a render controller via specified rendering content
    // \param[in] window_type the unique id for windows, 
    //            it also indicates the rendering content
    void RemoveRenderController(WINDOW_TYPE window, const std::string& wndUid);

    //////////////////////////////////////////////////////////////////////////
    // \brief create new view module including create render controller & update 
    //        layout information. furthermore, if data source is already set, 
    //        set current data source in the new render controller.
    // \param[in] window_type the unique id for windows, 
    //            it also indicates the rendering content
    // \param[in] displaySize  the width & height of window
    // \param[in] unit  the layout information
    // \return bool  true when succeeded, false otherwise
    bool CreateViewModule(const std::string& wndUid, WINDOW_TYPE window, DISPLAY_SIZE displaySize,
        Mcsf::MedViewer3D::IObserverPtr cacheObserver = nullptr);

    // \brief  set data source 
    // \param[in] sourceID  the identity of data source
    // \param[in] uid  the UID of data source in database
    // \param[in] groupID  the ID of data group source group
    virtual void SetDataSource(DATA_SOURCE_GROUP_ID groupID, 
        DATA_SOURCE_ID sourceID, const DATA_SOURCE_UID& uid);
    
    //////////////////////////////////////////////////////////////////////////
    // \brief  remove data source 
    // \param[in] sourceID  the identity of data source
    // \param[in] groupID  the ID of data group source group
    virtual void RemoveDataSource(DATA_SOURCE_GROUP_ID groupID, DATA_SOURCE_ID sourceID);

    //////////////////////////////////////////////////////////////////////////
    // \brief  remove data source of groups if 
    //         DATA_SOURCE_ID[sourceID]'s data equals to specified DATA_SOURCE_UID[uid].
    // \param[in] sourceID  the identity of data source
    // \param[in] uid  specified data source uid
    virtual void RemoveSpecifiedDataSource(DATA_SOURCE_ID sourceID, const DATA_SOURCE_UID& uid);
    
    //////////////////////////////////////////////////////////////////////////
    // \brief  get data source 
    // \param[in] groupID  the ID of data group source group
    // \param[in] sourceID  the identity of data source
    // \param[out] uid  the UID of data source in database
    // \return bool  true if succeeded, false otherwise
    virtual bool GetDataSource(DATA_SOURCE_GROUP_ID groupID, 
        DATA_SOURCE_ID sourceID, DATA_SOURCE_UID& uid);
    
    //////////////////////////////////////////////////////////////////////////
    // \brief  get data source via specified window
    // \param[in] window_type the unique id for windows, 
    //            it also indicates the rendering content
    // \param[in] sourceID  the identity of data source
    // \param[out] uid  the UID of data source in database
    // \return bool  true if succeeded, false otherwise
    virtual bool GetDataSource(WINDOW_TYPE window,
        DATA_SOURCE_ID sourceID, DATA_SOURCE_UID& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief fire to render all updated render controller
    void Render(bool bForce=false);

    //////////////////////////////////////////////////////////////////////////
    // \brief Synchronize command waiting for the operation finished!
    virtual void Synchronize();

    ////////////////////////////////////////////////////////////////////////// 
    // \brief  transform point form 2d screen coordinate to \ 
    //         specified c.s. related to the render controller 
    // \param[in] window_type the unique id for windows, 
    //            it also indicates the rendering content
    // \param[in] ptIn  the view 2d normalized point 
    // \param[out] ptOut  result point of render controller's coordinate interested
    // \return bool  true when succeeded, false otherwise 
    bool TransformPoint(WINDOW_TYPE window, const std::string& wndUid, const Mcsf::Point2f& ptIn, Mcsf::Point3f& ptOut);
    bool TransformPoint(WINDOW_TYPE window, const std::string& wndUid, const Mcsf::Point3f& ptIn, Mcsf::Point2f&ptOut);
    bool TransformPoints(
        WINDOW_TYPE window, const std::string& wndUid, 
        const std::vector<Mcsf::MedViewer3D::Point2D>& ptListIn, 
        std::vector<Mcsf::MedViewer3D::Point3D>& ptListOut,
        bool isSquaredPoints = true);
    bool MapMilimeter2Pixel(int wndType, const std::string& wndUid, double lengthInmm, double* pixelinXNP, double* pixelinYNP);

    //////////////////////////////////////////////////////////////////////////
    // \brief  get the display size of view
    // \param[in]  window  the unique id for windows, 
    //            it also indicates the rendering content
    // \param[out]  displaySize  the result display size
    // \return bool true when succeeded, false otherwise
    bool GetDisplaySize(WINDOW_TYPE window, const std::string& wndUid, DISPLAY_SIZE& displaySize);

    //////////////////////////////////////////////////////////////////////////
    // \brief resize view module. furthermore, if data source is already set, 
    //        set current data source in the new render controller.
    // \param[in] window  the unique id for windows, 
    //            it also indicates the rendering content
    // \param[in] displaySize  the width & height of window
    // \param[in] unit  the layout information
    // \return bool  true when succeeded, false otherwise
    bool ResizeViewModule(WINDOW_TYPE window, const std::string& wndUid, DISPLAY_SIZE displaySize);

    //////////////////////////////////////////////////////////////////////////
    // \brief check the data source group id of specified window
    // \param[in] window  the window type
    // \param[out] id  the data source group id
    // \return bool  true if succeeded, false otherwise
    //         for default implementation, id will be 0
    virtual bool CheckDataSourceGroup(WINDOW_TYPE window, DATA_SOURCE_GROUP_ID& id) = 0;

    //////////////////////////////////////////////////////////////////////////
    // \brief test if specified window exists
    bool HasWindow(WINDOW_TYPE window, const std::string& wndUid);

    //////////////////////////////////////////////////////////////////////////
    // \brief push an operation object into operation thread job list
    bool ProcessOperation(const std::shared_ptr<OperationBase>& op);

    ModelWarehouse* GetModelWarehouse() const {return mModelWarehouse;}
    
    DataSourceGroupMap* GetAllDataSourceGroup() {return &mDataSourceGroupMap;}

    template<typename T>
    std::vector<TpsDataSourceGroup*> SelectGroups(T selector) {
        std::vector<TpsDataSourceGroup*> vecGroups;
        for_each(mDataSourceGroupMap.begin(), mDataSourceGroupMap.end(), 
            [&](std::pair<DATA_SOURCE_GROUP_ID, TpsDataSourceGroup*> pr){
            if (selector(pr.first, pr.second)) vecGroups.push_back(pr.second);
        });
        return vecGroups;
    }

    HitTestResult HitTest(WINDOW_TYPE window, const std::string& wndUid, double x, double y);

    void ClearAllGrpahicObjects();

    //if window is deactived, no rendering command will be executed.
    void ActiveWindow(const std::string& wndUid, WINDOW_TYPE window);
    void DeactiveWindow(const std::string& wndUid, WINDOW_TYPE window);
private:
    //////////////////////////////////////////////////////////////////////////
    // \brief create render controller instance according to specified 
    //        window type & data source(optional)
    // \param[in] type  the unique id for windows, 
    //            it also indicates the rendering content
    // \return TpsRenderControllerBase instance pointer
    virtual TpsRenderControllerBase* CreateRenderController_i(WINDOW_TYPE window) = 0;

    //////////////////////////////////////////////////////////////////////////
    // \brief create graphic object converter factory
    virtual GOConverterFactoryBase* CreateGOConverterFactory_i() = 0;

	//virtual bool GenerateCommonGraphicObjects_i() = 0;
    //void LazyRenderingCycle_i();
protected:
    //RenderControllerMap         mRenderControllerMap;
    //TpsLayoutManager*           mLayoutManager;
    //because of 3D engine, we need use shared_ptr here.
    //TpsDisplayDelegatePtr       mDisplayDelegate;

    DataSourceGroupMap          mDataSourceGroupMap;

    ITpsDataRepository*         mDataRepository;
    ModelWarehouse*             mModelWarehouse;
    OperationProcessThread*     mOperationProcessor;
    GOConverterFactoryBase*     mConverterFactory;

    bool                        mInitialized;
    //bool                        mNeedCheckIn;
    //RENDERING_COMPONENT_ID      mID;
    std::map<WINDOW_TYPE, int>  mCreatedWindowTypes;

    //bool                            mIsLazyRenderingEnabled;
    ////窗口被设置为LAZY_RENDERING模式后，如果有绘制命令请求，不会立即绘制，
    ////而是由lazy_rendering线程接管按照固定时间间隔触发绘制
    //std::shared_ptr<boost::thread>  mLazyRenderingThread;
    //boost::mutex                    mMtxLazyRendering;
    //unsigned int                    mLazyWindowCount;
    //bool                            mIsRenderingDelayed;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsRenderProxyBase);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
