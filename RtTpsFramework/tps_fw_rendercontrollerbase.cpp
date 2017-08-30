////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_rendercontrollerbase.cpp
/// 
///  \brief class TpsRenderControllerBase implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////
#include"StdAfx.h"
#include "RtTpsFramework/tps_fw_rendercontrollerbase.h"

#include "McsfMedViewer3D/McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DArithmetic/point2d.h"
#include "McsfMedViewer3DArithmetic/point3d.h"

#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsRenderControllerBase::TpsRenderControllerBase(WINDOW_TYPE type)
    : mWindowType(type), mView(nullptr), mDisplayCache(nullptr), mVisibility(true){

}

TpsRenderControllerBase::~TpsRenderControllerBase()
{
    //等3D修复问题，打开注释，释放view
    //if(mView != nullptr)
    //{
    //    mView->Finalize();
    //}
}

bool TpsRenderControllerBase::Initialize(DISPLAY_SIZE displaySize, 
    ITpsDataRepository* dataRepos, ModelWarehouse* modelWarehouse,
    GOConverterFactoryBase* goConvFactory)
{
    if (displaySize.width < 0 || displaySize.height < 0)
    {
        TPS_LOG_DEV_ERROR<<"The display size is invalid.";
        return false;
    }

    mDataRepository = dataRepos;
    mGOConverterFactory = goConvFactory;
    mModelWarehouse = modelWarehouse;
    //create view
    mView.reset(new Mcsf::MedViewer3D::ClsView(displaySize.width, displaySize.height, 
        mModelWarehouse->GetModelEntity()));
    if (!mView->Initialize()){
        TPS_LOG_DEV_ERROR<<"Failed to initialize ClsView.";
        return false;
    }
    mModelWarehouse->AttachObserver(mView);

    //display cache
    mDisplayCache.reset(new TpsDisplayCache());
    mDisplayCache->SetRenderUnit(mView);
    mDisplayCache->SetCacheID(mWindowType);
    mDisplayCache->SetModelWarehouse(mModelWarehouse);
    mView->AttachObserver(mDisplayCache);
    if (!SetupScene_i()){
        TPS_LOG_DEV_ERROR<<"Failed to setup scene.";
        return false;
    }

    return true;
}

WINDOW_TYPE TpsRenderControllerBase::GetWindowType() const{
    return mWindowType;
}

bool TpsRenderControllerBase::SetDataSource(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid){
    if (uid.empty()){
        TPS_LOG_DEV_ERROR<<"The data source to be set is empty.";
        return false;
    }

    auto it = mDataSourceMap.find(id);

    bool isFound = (it != mDataSourceMap.end());
    if (isFound && it->second == uid){
        TPS_LOG_DEV_INFO<<"[INFO][TpsRenderControllerBase][SetDataSource]: "
            <<"The data source to be set is same to the current.";
        return true;
    }

    mDataSourceMap[id] = uid;
    return ResetDataSource_i(id); // will call GO-converters to generate GOs accordingly
}

bool TpsRenderControllerBase::GetDataSource(DATA_SOURCE_ID id, DATA_SOURCE_UID& uid)
{
    auto it = mDataSourceMap.find(id);
    if (it == mDataSourceMap.end())
    {
        TPS_LOG_DEV_INFO<<"[INFO][TpsRenderControllerBase][GetDataSource]: "
            <<"The data source does not exist.";
        return false;
    }
    uid = it->second;
    return true;
}

void TpsRenderControllerBase::RemoveDataSource(DATA_SOURCE_ID id){

    auto it = mDataSourceMap.find(id);
    if (it == mDataSourceMap.end()){
        TPS_LOG_TRACE_INFO<<"[INFO][TpsRenderControllerBase][RemoveDataSource]: "
            <<"The data source to be removed does not exist.";
        return;
    }

    mDataSourceMap.erase(it);
    RemoveDataSource_i(id);
}

void TpsRenderControllerBase::ClearGraphicObjects(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    ClearGraphicObjects_i(id, uid);

    auto it = mDataSourceMap.find(id);
    if (it != mDataSourceMap.end() && it->second == uid)
    {
        RemoveDataSource(id);
    }
}

bool TpsRenderControllerBase::TransformPoint(const Mcsf::MedViewer3D::Point2D& /*ptIn*/, 
    Mcsf::MedViewer3D::Point3D& /*ptOut*/)
{
    return false;
}

bool TpsRenderControllerBase::TransformPoint(const Mcsf::MedViewer3D::Point3D& /*ptIn*/,
    Mcsf::MedViewer3D::Point2D& /*ptOut*/)
{
    return false;
}

bool TpsRenderControllerBase::TransformPoints(
    const std::vector<Mcsf::MedViewer3D::Point2D>& /*ptListIn*/, 
    std::vector<Mcsf::MedViewer3D::Point3D>& /*ptListOut*/,
    bool /*isSquaredPoints*//* = true*/)
{
    return false;
}

bool TpsRenderControllerBase::MapMilimeter2Pixel(double /*lengthInmm*/,
    double* /*radiuXinNP*/, double* /*radiusYinNP*/)
{
    return false;
}

void TpsRenderControllerBase::Resize(DISPLAY_SIZE displaySize){
    if (mView != nullptr) {
        mView->SetDisplaySize(displaySize.width, displaySize.height, Mcsf::MedViewer3D::ClsView::ALL);
        TPS_LOG_DEV_INFO << "New rendering request commited";
        mView->Render(true);
    }
}

TpsDisplayCache* TpsRenderControllerBase::GetDisplayCache() const {
    return mDisplayCache.get();
}

bool TpsRenderControllerBase::CheckGraphicObject_i(const std::string& baseUID,
                                                   GO_TYPE goType, 
                                                   IGraphicObjectPtr& go,
                                                   bool forceToUpdate/* = false*/, 
                                                   GOConverterParameter* para/* = nullptr*/)
{
    GOConverterParameter realPara;
    if (para)
    {
        realPara = *para;
    }
    else
    {
        realPara.data_source_map = mDataSourceMap;
    }

    std::string goTypeStr = GOTypeHelper::ToString(goType);
    std::string key = baseUID + "|" + goTypeStr;
    go = mModelWarehouse->GetModelObject(key);
    if(go == nullptr || forceToUpdate)
    {
        TPS_LOG_DEV_INFO<<"[INFO][TpsMPRRenderController][CheckGraphicObject_i]: "
            <<"Graphic object with key: "<<key<<" does not exist, create new.";
        //get converter via graphic object type
        auto converter = this->GetConverter_i(goType);
        if (converter == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRRenderController][CheckGraphicObject_i]: "
                <<"Failed to get graphic object converter which type is: "<<goType;
            return false;
        }
        //convert/update data
        if (go == nullptr) 
        {
            go = converter->Convert(realPara);
            if (go == nullptr)
            {
#ifdef _DEBUG
                printf("\n***ERROR***convert to graphic object [%x] failed!!!", goType);
#endif // _DEBUG
                TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRRenderController][CheckGraphicObject_i]: "
                    <<"Failed to convert RT data to graphic object which key is: "<<baseUID;
                return false;
            }
            mModelWarehouse->AddModelObject(key, go);
        }
        else if(!converter->Update(realPara, go))
        {
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRRenderController][CheckGraphicObject_i]: "
                <<"Failed to update RT data to graphic object which key is: "<<baseUID;
            return false;
        }
    }
    return true;
}

void TpsRenderControllerBase::ClearGraphicObjects_i(DATA_SOURCE_ID /*id*/, const DATA_SOURCE_UID& /*uid*/)
{

}

void TpsRenderControllerBase::RemoveGraphicObject_i(const std::string& baseUid, GO_TYPE goType, int componentID /*= -1*/)
{
    std::string key = baseUid + "|" + GOTypeHelper::ToString(goType);
    mModelWarehouse->RemoveModelObject(key, componentID);
}

bool TpsRenderControllerBase::GetDisplaySize(DISPLAY_SIZE& displaySize){

    int width, height;
    mView->GetDisplaySize(width, height);
    displaySize.width = width;
    displaySize.height = height;
    return true;
}

void TpsRenderControllerBase::SynchronizeRenderThread()
{
	if (!mView) return;
	int width, height;
	unsigned short* buffer;
	mView->Get16BitsROIImageResult(width, height, buffer);
	//mView->Render(true);
}

HitTestResult TpsRenderControllerBase::HitTest(double /*screenX*/, double /*screenY*/)
{
    return HitTestResult();
}

void TpsRenderControllerBase::RegisterRenderStrategy_i(int overlay, const TpsRSPtr& rs)
{
    mView->AddRenderStrategy(0, rs, (Mcsf::MedViewer3D::ClsView::eOverlay)overlay);

    auto it = mStrategyList.find(overlay);
    if (it == mStrategyList.end())
    {
        mStrategyList[overlay] = std::vector<TpsRSPtr>();
    }
    mStrategyList[overlay].push_back(rs);
}

void TpsRenderControllerBase::Active()
{
    for (auto it = mStrategyList.begin(); it != mStrategyList.end(); ++it)
    {
        for (int i = 0; i < it->second.size(); ++i)
        {
            it->second[i]->SetIsActive(true);
        }
    }
}

void TpsRenderControllerBase::Deactive()
{
    for (auto it = mStrategyList.begin(); it != mStrategyList.end(); ++it)
    {
        for (int i = 0; i < it->second.size(); ++i)
        {
            it->second[i]->SetIsActive(false);
        }
    }
}

bool TpsRenderControllerBase::GetCurrentPixelSpacingIn10MM(double& spacingX, double& spacingY)
{
    spacingX = 0.0;
    spacingY = 0.0;
    return false;
}

// void TpsRenderControllerBase::LazyRender()
// {
//     if (!mIsLazy) return;
// 
//     for (auto it = mStrategyList.begin(); it != mStrategyList.end(); ++it)
//     {
//         for (int i = 0; i < it->second.size(); ++i)
//         {
//             it->second[i]->EnableLazyPass();
//         }
//     }
// 
//     mView->Render(false);
// }
TPS_END_NAMESPACE  // end namespace tps
