//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  xiaoqing.shangguan xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_poi_manager.cpp
/// 
///  \brief

///  \version 1.0
///  \date    Jan. 17, 2014
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_POI_MANAGER_H_
#define TPS_POI_MANAGER_H_

#include "tps_da_defs.h"
#include "McsfMedViewer3DArithmetic/point2d.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

namespace Mcsf {
    namespace MedViewer3D {
        class Point2D;
    }
}
TPS_BEGIN_NAMESPACE;

class RtPoi;

class TPS_DA_DECLSPEC TpsPOIManager : public TpsDataManagerBase
{
public:
    TpsPOIManager(void);
    ~TpsPOIManager(void);


    struct POIPosInfo{
        std::string      Uid;
        ::Mcsf::MedViewer3D::Point2D          Pos;
        int              Counter;

        unsigned char*  TxtBuffer;
        int              TxtWidth;
        int              TxtHeight;
    };

    /// \brief Add a POI to the manager
    /// \param[in] poi
    /// \return bool 
    bool AddPOI(RtPoi *poi);
    bool AddMaxPoint(RtPoi *poi);

    /// \brief Remove a POI from the manager
    /// \param[in] poiUID
    /// \return bool 
    bool DeletePOI(const std::string &poiUID);
    bool DeleteMaxPoint(const std::string &poiUID);

    void UpdatePois(std::vector<RtPoi*>& poiList);

    /// \brief Get a POI from the manager
    /// \param[in] poiUID
    /// \param[out] pPOI
    /// \return bool  
    RtPoi* GetPOI(const std::string &poiUID);
    RtPoi* GetMaxPoint(const std::string &poiUID);

    RtPoi* GetPoiViaName(const std::string &strPoiName, const std::string &seriesUid);

    /////////////////////////////////////////////////////////////////
    ///  \brief get poi list by planUID
    ///     query from memory NOT database
    ///  \param[in]     const std::string &sPlanUID
    ///  \return        std::vector<std::string> may be empty
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    std::vector<std::string> GetPoiUidListBySeriesUid(const std::string &sSeriesUID) const;
    std::vector<RtPoi*> GetAllPoiInSeries(const std::string& sSeriesUid) const;

    void ClearPatientData();

    bool GetPoiPosText(const std::string& poiUid, POIPosInfo& poiPos) const;

    bool AddPoiPos(const std::string& uid, ::Mcsf::MedViewer3D::Point2D pos, const std::string& name, const float color[4]);

    void ClearPoiPosMap();

private:
    void Dispose(bool isPatientDataOnly = false);

private:
    typedef std::map<std::string, RtPoi*>           POIMap;
    POIMap                                          mPoiMap;
    std::map<std::string, POIPosInfo>               mAllPOIPosInfo;
    POIMap                                          mMaxPointMap;
};

TPS_END_NAMESPACE

#endif
