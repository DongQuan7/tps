////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_datasourcegroup.h
/// 
///  \brief class TpsDataSourceGroup declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/07/17
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_DATASOURCEGROUP_H_
#define TPS_FW_DATASOURCEGROUP_H_

#include "tps_fw_defs.h"

#include <map>

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsRenderControllerBase;
/// \class TpsDataSourceGroup
/// 
/// \brief the data source group contains several render controller with same data source
class TPS_FW_DECLSPEC TpsDataSourceGroup{
    typedef std::map<std::string, TpsRenderControllerBase*> RenderControllerMap;
    typedef std::map<DATA_SOURCE_ID, DATA_SOURCE_UID> DataSourceMap;
public:
    //the constructor and destructor
    TpsDataSourceGroup();
    ~TpsDataSourceGroup();

    //check if data source group contains specified window
    bool HasWindow(const std::string& windowUid);

    //////////////////////////////////////////////////////////////////////////
    // \brief add render controller to group
    // \param[in] window_type the unique id for windows, 
    //            it also indicates the rendering content
    // \param[in] renderController the render controller pointer to be added
    void AddRenderController(const std::string& wndUid, TpsRenderControllerBase* renderController);

    //////////////////////////////////////////////////////////////////////////
    // \brief query a render controller via specified window type, 
    // \param[in] window  the window type, such as BEV, MPR_AXIAL, 
    //            MPR_AXIAL_SOURCE_GROUP_FUSION_FIXED, .etc. in our case, 
    //            it corresponds to a view control of front end.
    // \return TpsRenderControllerBase*  a render controller base pointer, 
    //         return NULL if not exist or any other error occurs.
    TpsRenderControllerBase* GetRenderController(const std::string& wndUid);

    //////////////////////////////////////////////////////////////////////////
    // \brief remove a render controller via specified rendering content
    // \param[in] window_type the unique id for windows, 
    //            it also indicates the rendering content
    void RemoveRenderController(const std::string& wndUid);

    //////////////////////////////////////////////////////////////////////////
    // \brief  set data source 
    // \param[in] id  the identity of data source
    // \param[in] uid  the UID of data source in database
    void SetDataSource(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief  remove data source via specified data source id
    void RemoveDataSource(DATA_SOURCE_ID id);

    //////////////////////////////////////////////////////////////////////////
    // \brief  get data source 
    // \param[in] id  the identity of data source
    // \param[out] uid  the UID of data source in database
    // \return bool  true if succeeded, false otherwise
    bool GetDataSource(DATA_SOURCE_ID id, DATA_SOURCE_UID& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief  remove all data sources
    void RemoveAllDataSources();

    // \brief  check if the data source is empty
    bool IsDataSourceEmpty() const;

    template<typename T>
    void ForEach(T t) {
        for (auto it = mRenderControllerMap.begin(); it != mRenderControllerMap.end(); ++it){
            t(it->first, it->second);
        }
    }
private:
    DataSourceMap           mDataSourceMap;
    RenderControllerMap     mRenderControllerMap;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDataSourceGroup);
};

TPS_END_NAMESPACE  // end namespace tps

#endif