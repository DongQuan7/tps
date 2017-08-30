////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_renderproxybase.cpp
/// 
///  \brief class TpsRenderManager implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////
#include"StdAfx.h"

#include "RtTpsFramework/tps_fw_renderproxybase.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_operationprocessthread.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsFramework/tps_fw_fire_operation.h"
#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsFramework/tps_fw_rendercontrollerbase.h"
#include "RtTpsFramework/tps_fw_datasourcegroup.h"
#include "RtTpsFramework/tps_fw_graphic_object_converter_factory_base.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_safelock.h"
//
#include "tps_logger.h"
#include "boost/thread/locks.hpp"

#define LAZY_RENDERING_INTERVAL 1000 //ms

using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE

TpsRenderProxyBase::TpsRenderProxyBase() : mInitialized(false), 
mDataRepository(nullptr), mModelWarehouse(nullptr), 
mOperationProcessor(nullptr), mConverterFactory(nullptr)
//mIsLazyRenderingEnabled(true), mLazyWindowCount(0), mIsRenderingDelayed(false)
{

}

TpsRenderProxyBase::~TpsRenderProxyBase(){

}

bool TpsRenderProxyBase::Initialize(ITpsDataRepository* dataRepository) {
    if (mInitialized){
        TPS_LOG_TRACE_INFO<<"[INFO][TpsRenderProxyBase][Initialize]: "
            <<"The render manager has already been initialized.";
        return true;
    }
    try{
        //data repository
        if (dataRepository == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsRenderProxyBase][Initialize]: "
                <<"The input data repository is empty.";
            return false;
        }
        mDataRepository = dataRepository;
        //model warehouse
        mModelWarehouse = new ModelWarehouse();
        //operation thread
        //mOperationProcessor = new OperationProcessThread();
        //if (!mOperationProcessor->Activate()){
        //    TPS_LOG_DEV_ERROR<<"[ERROR][TpsRenderProxyBase][Initialize]: "
        //        <<"Failed to activate operation thread.";
        //    return false;
        //}
        //graphic object converter factory
        mConverterFactory = this->CreateGOConverterFactory_i();
        if (mConverterFactory == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsRenderProxyBase][Initialize]: "
                <<"Failed to create graphic object converter factory.";
            return false;
        }
        mConverterFactory->SetDataRepository(mDataRepository);
        mConverterFactory->SetModelWarehouse(mModelWarehouse);

        // init free type library
        FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
        //std::string filename = "C:/WINDOWS/Fonts/simsun.ttc";
        if(!pBufGen->Init())
        {
            printf("Failed to initialize the FreeType2 Font generator\n");
        }

//         if (mIsLazyRenderingEnabled)
//         {
//             mLazyRenderingThread.reset(new boost::thread(boost::bind(
//                 &TpsRenderProxyBase::LazyRenderingCycle_i, this)));
//         }

        mInitialized = true;
        return true;
    }
    catch (const std::bad_alloc& badAllocEx){
        TPS_LOG_DEV_ERROR<<"[EXCEPTION][TpsRenderProxyBase][Initialize]: "
            <<"Bad allocation exception occurs: "<<badAllocEx.what();
        return false;
    }
    catch (const std::exception& ex){
        TPS_LOG_DEV_ERROR<<"[EXCEPTION][TpsRenderProxyBase][Initialize]: "
            <<"Exception occurs: "<<ex.what();
        return false;
    }
}

void TpsRenderProxyBase::Finalize(){
    if (!mInitialized) return;

    if(!mDataSourceGroupMap.empty()){
        for_each(mDataSourceGroupMap.begin(), mDataSourceGroupMap.end(),
            [&](std::pair<DATA_SOURCE_GROUP_ID, TpsDataSourceGroup*> pr){
                if (pr.second){
                    delete pr.second;
                    pr.second = nullptr;
                }
        });
        mDataSourceGroupMap.clear();
    }

    if (mModelWarehouse){
        delete mModelWarehouse;
        mModelWarehouse = nullptr;
    }

    //if (mOperationProcessor){
    //    mOperationProcessor->Deactivate();
    //    mOperationProcessor->Quit();
    //    mOperationProcessor->Join();
    //    delete mOperationProcessor;
    //    mOperationProcessor = nullptr;
    //}

    if (mConverterFactory){
        delete mConverterFactory;
        mConverterFactory = nullptr;
    }
    mCreatedWindowTypes.clear();

//     if (mIsLazyRenderingEnabled)
//     {
//         mLazyRenderingThread->interrupt();
//     }
    mInitialized = false;
}

// TpsLayoutManager* TpsRenderProxyBase::GetLayoutManager() const{
//     return mLayoutManager;
// }

TpsDataSourceGroup* TpsRenderProxyBase::GetDataSourceGroup(DATA_SOURCE_GROUP_ID id){
    auto it = mDataSourceGroupMap.find(id);
    if (it == mDataSourceGroupMap.end()){
        TPS_LOG_TRACE_INFO<<"The requested data source group does not exist.";
        return nullptr;
    }
    return it->second;
}

TpsDataSourceGroup* TpsRenderProxyBase::RequestDataSourceGroup(DATA_SOURCE_GROUP_ID id){
    TpsDataSourceGroup* group = nullptr;
    if (mDataSourceGroupMap.find(id) != mDataSourceGroupMap.end()){
        group = mDataSourceGroupMap[id];
    }
    else{
        TPS_LOG_TRACE_INFO<<"Requested DataSourceGroup does not exist, create new one, ID: "<<id;
        group = new TpsDataSourceGroup();
        mDataSourceGroupMap[id] = group;
    }
    return group;
}

void TpsRenderProxyBase::RemoveAllDataSources(){
    for (auto it = mDataSourceGroupMap.begin(); it != mDataSourceGroupMap.end(); ++it){
        it->second->RemoveAllDataSources();
    }
}

TpsRenderControllerBase* TpsRenderProxyBase::GetRenderController(WINDOW_TYPE window, 
    const std::string& wndUid)
{
    DATA_SOURCE_GROUP_ID id;
    if (!CheckDataSourceGroup(window, id)){
        TPS_LOG_DEV_ERROR << 
            "Window type does not belong to the render proxy, window type: " 
            << wndUid;
        return nullptr;
    }

    auto itDSG = mDataSourceGroupMap.find(id);
    if (itDSG == mDataSourceGroupMap.end()){
        TPS_LOG_DEV_ERROR<<"Data source group does not exist, id: "<<id;
        return nullptr;
    }

    if (itDSG->second == nullptr){
        TPS_LOG_DEV_ERROR<<"The data source group is empty, id: "<<id;
        return nullptr;
    }
    return itDSG->second->GetRenderController(wndUid);
}

void TpsRenderProxyBase::RemoveRenderController(WINDOW_TYPE window,const std::string& wndUid)
{
    DATA_SOURCE_GROUP_ID id;
    if (!CheckDataSourceGroup(window, id)){
        TPS_LOG_DEV_ERROR << 
            "Window type does not belong to the render proxy, window type: " 
            << wndUid;
        return;
    }

    auto itDSG = mDataSourceGroupMap.find(id);
    if (itDSG == mDataSourceGroupMap.end()){
        TPS_LOG_DEV_ERROR<<"Data source group does not exist, id: "<<id;
        return;
    }

    if (itDSG->second == nullptr){
        TPS_LOG_DEV_ERROR<<"The data source group is empty, id: "<<id;
        return;
    }

    itDSG->second->RemoveRenderController(wndUid);
}

bool TpsRenderProxyBase::CreateViewModule(const std::string& wndUid,
    WINDOW_TYPE window, DISPLAY_SIZE displaySize, IObserverPtr cacheObserver)
{
    try{
        TPS_LOG_DEV_INFO<<"[INFO][TpsRenderProxyBase][CreateViewModule]: "
            << "Create view module execute, window_type: [" << window << "]";

        DATA_SOURCE_GROUP_ID groupID;
        if (!this->CheckDataSourceGroup(window, groupID)){
            TPS_LOG_DEV_ERROR<<"Failed to get data source group id of window: "<<window;
            return false;
        }

        // if window linked renderController already exists
        if (this->HasWindow(window, wndUid)) {
            auto renderController = GetRenderController(window, wndUid);

            // 判断是否需要resize
            DISPLAY_SIZE oldSize;
            renderController->GetDisplaySize(oldSize);
            if (oldSize.height != displaySize.height || oldSize.width != displaySize.width) {
                TPS_LOG_DEV_ERROR << "Different size for existed view. Abort.";
                return false;
            }
            
            if (cacheObserver == nullptr) {
                TPS_LOG_DEV_WARNING << "cache observer is null. Failed to attach to existed view.";
                return true;
            }

            renderController->GetDisplayCache()->AttachObserver(cacheObserver);
            TPS_LOG_TRACE_INFO<<"Attach to existed view. uid["<<wndUid
                <<"],type["<<window<<"].";
            return true;
        }

        TPS_LOG_DEV_INFO<<"[INFO][TpsRenderProxyBase][CreateViewModule]: "
            <<"Create render controller of window: "<<window;
        auto renderController = this->CreateRenderController_i(window);
        if (renderController == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsRenderProxyBase][CreateViewModule]: "
                <<"Failed to create render controller of window: "<<window;
            return false;
        }
        if (mCreatedWindowTypes.find(window) == mCreatedWindowTypes.end())
        {
            mCreatedWindowTypes[window] = mCreatedWindowTypes.size();
        }

        // initialize the scene of this render controller
        if(!renderController->Initialize(displaySize, mDataRepository, 
            mModelWarehouse, mConverterFactory)) {
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsRenderProxyBase][CreateViewModule]: "
                <<"Failed to initialize window: "<<window;
            return false;
        }

        if (cacheObserver!=nullptr) {
            renderController->GetDisplayCache()->AttachObserver(cacheObserver);
        }

        TpsDataSourceGroup* group = this->RequestDataSourceGroup(groupID);
        group->AddRenderController(wndUid, renderController);

        //todo: it may cause redundancy rendering, we can only set a flag instead
        if (!group->IsDataSourceEmpty()){
            this->Render();
        }
        TPS_LOG_DEV_INFO<<"[INFO][TpsRenderProxyBase][CreateViewModule]: "
            <<"Create view module leave!";
        return true;
    }
    catch (const std::bad_alloc& badAllocEx){
        TPS_LOG_DEV_ERROR<<"[EXCEPTION][TpsRenderProxyBase][CreateViewModule]: "
            <<"Bad allocation exception occurs: "<<badAllocEx.what();
        return false;
    }
    catch (const std::exception& ex){
        TPS_LOG_DEV_ERROR<<"[EXCEPTION][TpsRenderProxyBase][CreateViewModule]: "
            <<"Exception occurs: "<<ex.what();
        return false;
    }
}

void TpsRenderProxyBase::SetDataSource(DATA_SOURCE_GROUP_ID groupID, 
    DATA_SOURCE_ID sourceID, const DATA_SOURCE_UID& uid)
{
    TPS_LOG_DEV_INFO<<"[INFO][TpsRenderProxyBase][SetDataSource]: "
        <<"Render proxy SetDataSource, sourceID: "<<sourceID<<", sourceUID: "<<uid;

    auto group = this->RequestDataSourceGroup(groupID);
    if (group == nullptr){
        TPS_LOG_DEV_ERROR<<"The data requested source group does not exist, id: "<<groupID;
        return;
    }

    group->SetDataSource(sourceID, uid);
    TPS_LOG_DEV_INFO<<"[INFO][TpsRenderProxyBase][SetDataSource]: "
        <<"Render proxy SetDataSource leave!";
}

void TpsRenderProxyBase::RemoveDataSource(DATA_SOURCE_GROUP_ID groupID, 
    DATA_SOURCE_ID sourceID)
{
    TPS_LOG_DEV_INFO<<"[INFO][TpsRenderProxyBase][RemoveDataSource]: "
        <<"Render proxy RemoveDataSource, sourceID: "<<sourceID;

    auto group = this->GetDataSourceGroup(groupID);
    if (group == nullptr){
        TPS_LOG_DEV_ERROR<<"The data source group does not exist, id: "<<groupID;
        return;
    }

    group->RemoveDataSource(sourceID);
    TPS_LOG_DEV_INFO<<"[INFO][TpsRenderProxyBase][RemoveDataSource]: "
        <<"Render proxy RemoveDataSource leave!";
}

void TpsRenderProxyBase::RemoveSpecifiedDataSource(DATA_SOURCE_ID sourceID, 
    const DATA_SOURCE_UID& uid)
{
    DATA_SOURCE_UID currentUid;
    for (auto it = mDataSourceGroupMap.begin(); it != mDataSourceGroupMap.end(); ++it){
        if (it->second == nullptr || 
            !it->second->GetDataSource(sourceID, currentUid)) continue;
        if (currentUid == uid)
            it->second->RemoveDataSource(sourceID);
    }
}

bool TpsRenderProxyBase::GetDataSource(DATA_SOURCE_GROUP_ID groupID, 
    DATA_SOURCE_ID sourceID, DATA_SOURCE_UID& uid)
{
    auto group = this->GetDataSourceGroup(groupID);
    if (group == nullptr){
        TPS_LOG_DEV_INFO<<"The data source group does not exist, id: "<<groupID;
        return false;
    }

    return group->GetDataSource(sourceID, uid);
}

bool TpsRenderProxyBase::GetDataSource(WINDOW_TYPE window, 
    DATA_SOURCE_ID sourceID, DATA_SOURCE_UID& uid)
{
    DATA_SOURCE_GROUP_ID dataSourceGroup;
    if (!this->CheckDataSourceGroup(window, dataSourceGroup)){
        TPS_LOG_DEV_ERROR<<"Failed to get data source group.";
        return false;
    }

    return this->GetDataSource(dataSourceGroup, sourceID, uid);
}

void TpsRenderProxyBase::Render(bool bForce)
{
//     if (mLazyWindowCount)
//     {
//         boost::unique_lock<boost::mutex> lock(mMtxLazyRendering);
//         mIsRenderingDelayed = true;
//     }

    std::shared_ptr<TpsFireOperation> fire(new TpsFireOperation(bForce));
    this->ProcessOperation(fire);
}

void TpsRenderProxyBase::Synchronize(){
    //if (mOperationProcessor){
    //    mOperationProcessor->Synchronize();
    //}
}

bool TpsRenderProxyBase::TransformPoint(WINDOW_TYPE window, const std::string& wndUid,
    const Mcsf::Point2f& ptIn, Mcsf::Point3f& ptOut)
{
    auto renderController = this->GetRenderController(window, wndUid);
    if (renderController == nullptr){ 
        TPS_LOG_DEV_ERROR << "Render controller of window: " << wndUid
            <<"does not exists.";
        return false;
    }
    Mcsf::MedViewer3D::Point2D ptScreen = TpsArithmeticConverter::ConvertToPoint2D(ptIn);
    Mcsf::MedViewer3D::Point3D ptOut3D;
    if(!renderController->TransformPoint(ptScreen, ptOut3D)) {
        TPS_LOG_DEV_ERROR<<"transform point failed!";
        return false;
    }
    ptOut = TpsArithmeticConverter::ConvertToPoint3f(ptOut3D);
    return true;
}

bool TpsRenderProxyBase::TransformPoint(WINDOW_TYPE window, const std::string& wndUid,
    const Mcsf::Point3f& ptIn, Mcsf::Point2f&ptOut)
{
    auto renderController = this->GetRenderController(window, wndUid);
    if (renderController == nullptr){
        TPS_LOG_DEV_ERROR << "Render controller of window: " << wndUid
            << "does not exists.";
        return false;
    }
    Mcsf::MedViewer3D::Point3D ptIn3D = TpsArithmeticConverter::ConvertToPoint3D(ptIn);
    Mcsf::MedViewer3D::Point2D ptOut2D;
    if (!renderController->TransformPoint(ptIn3D, ptOut2D)) {
        TPS_LOG_DEV_ERROR << "transform point failed!";
        return false;
    }
    ptOut = TpsArithmeticConverter::ConvertToPoint2f(ptOut2D);
    return true;
}

bool TpsRenderProxyBase::TransformPoints(
    WINDOW_TYPE window, const std::string& wndUid, 
    const std::vector<Mcsf::MedViewer3D::Point2D>& ptListIn, 
    std::vector<Mcsf::MedViewer3D::Point3D>& ptListOut,
    bool isSquaredPoints/* = true*/)
{
    auto renderController = this->GetRenderController(window, wndUid);
    if (renderController == nullptr){
        TPS_LOG_DEV_ERROR << "Render controller of window: " << wndUid
            << "does not exists.";
        return false;
    }

    if (!renderController->TransformPoints(ptListIn, ptListOut, isSquaredPoints))
    {
        TPS_LOG_DEV_ERROR << "transform points failed!";
        return false;
    }
    return true;
}

bool TpsRenderProxyBase::MapMilimeter2Pixel(int wndType, const std::string& wndUid,
    double lengthInmm, double* pixelinXNP, double* pixelinYNP)
{
    auto renderController = this->GetRenderController((WINDOW_TYPE)wndType, wndUid);
    if (renderController == nullptr){
        TPS_LOG_DEV_ERROR << "Render controller of window: " << wndUid
            << "does not exists.";
        return false;
    }

    bool bRet = renderController->MapMilimeter2Pixel(lengthInmm, pixelinXNP, pixelinYNP);
    return bRet;
}

bool TpsRenderProxyBase::GetDisplaySize(WINDOW_TYPE window, const std::string& wndUid,
    DISPLAY_SIZE& displaySize) 
{
    auto renderController = this->GetRenderController(window, wndUid);
    if (renderController == nullptr){ 
        TPS_LOG_DEV_ERROR << "Render controller of window: " << wndUid
            <<"does not exists.";
        return false;
    }
    renderController->GetDisplaySize(displaySize);
    return true;
}

bool TpsRenderProxyBase::ResizeViewModule(WINDOW_TYPE window, 
    const std::string& wndUid, DISPLAY_SIZE displaySize) 
{
    auto renderController = this->GetRenderController(window, wndUid);
    if (renderController == nullptr){ 
        TPS_LOG_DEV_ERROR<<"[INFO][TpsRenderProxyBase][ResizeViewModule]: "
            << "Render controller of window: " << wndUid << " does not exists.";
        return false;
    }

    renderController->Resize(displaySize);
    return true;
}

bool TpsRenderProxyBase::HasWindow(WINDOW_TYPE window, const std::string& wndUid)
{
    return this->GetRenderController(window, wndUid) != nullptr;
}

bool TpsRenderProxyBase::ProcessOperation(const std::shared_ptr<OperationBase>& op)
{
    //if (mOperationProcessor != NULL) {
        op->Initialize(mModelWarehouse, mDataRepository, mConverterFactory);
        return op->Execute();
        //mOperationProcessor->PutCommand(op);
    //}
}

HitTestResult TpsRenderProxyBase::HitTest(WINDOW_TYPE window, 
    const std::string& wndUid, double x, double y)
{
    auto renderController = this->GetRenderController(window, wndUid);
    if (renderController == nullptr){ 
        TPS_LOG_DEV_ERROR<<"[INFO][TpsRenderProxyBase][HitTest]: "
            << "Render controller of window: " << wndUid << " does not exists.";
        return HitTestResult();
    }
    return renderController->HitTest(x, y);
}

void TpsRenderProxyBase::ClearAllGrpahicObjects()
{
    
    //converter 中残留GO temp引用，这里暂采取清空converter方式，清除引用计数
    if (mConverterFactory) mConverterFactory->ClearAll();

    if (mModelWarehouse == nullptr) return;

    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse); 
    mModelWarehouse->ClearAll();
}

void TpsRenderProxyBase::ActiveWindow(const std::string& wndUid, WINDOW_TYPE window)
{
//     if (!mIsLazyRenderingEnabled)
//     {
//         return;
//     }

    auto renderController = this->GetRenderController(window, wndUid);
    if (renderController)
    {
        renderController->Active();

//         boost::unique_lock<boost::mutex> lock(mMtxLazyRendering);
//         mLazyWindowCount++;
    }
}

void TpsRenderProxyBase::DeactiveWindow(const std::string& wndUid, WINDOW_TYPE window)
{
//     if (!mIsLazyRenderingEnabled)
//     {
//         return;
//     }

    auto renderController = this->GetRenderController(window, wndUid);
    if (renderController)
    {
        renderController->Deactive();
// 
//         boost::unique_lock<boost::mutex> lock(mMtxLazyRendering);
//         mLazyWindowCount--;
    }
}

// void TpsRenderProxyBase::LazyRenderingCycle_i()
// {
//     while(true)
//     {
//         ::Sleep(LAZY_RENDERING_INTERVAL);
// 
//         bool fire = false;
//         {
//             boost::unique_lock<boost::mutex> lock(mMtxLazyRendering);
// 
//             if (mLazyWindowCount == 0)
//             {
//                 continue;
//             }
// 
//             fire = mIsLazyRenderingEnabled;
//             if (fire)
//             {
//                 mIsLazyRenderingEnabled = false;
//             }
//         }
//         if (fire)
//         {
// 
//         }
//     }
// }
TPS_END_NAMESPACE
