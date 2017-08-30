//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_beam_manager.h
///  \brief   entry of beam operator
///
///  \version 1.0
///  \date    Oct. 9, 2013
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BEAM_MANAGER_H
#define TPS_BEAM_MANAGER_H

#include "tps_da_defs.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

namespace boost
{
    class mutex;
}

TPS_BEGIN_NAMESPACE

class TpsFeBeam;
class TpsIECCoordinateSystem;
class RtBeam;
class RtContour;
class RtMlcshape;
class RtAccessory;
class FluenceMapEntity;

//class RT_TPS_Beam;

/// \class TpsBeamManager
/// 
/// \brief TpsBeamManager is:
///        1. the map collection of all beams, key is beam uid, value is beam object defined in TMS data layer
///        2. provide beam operation that related to business logic, TMS data layer provide getter/setter
class TPS_DA_DECLSPEC TpsBeamManager : public TpsDataManagerBase
{
public:
    TpsBeamManager(void);
    ~TpsBeamManager(void);

    //////////////////////////////////////////////////////////////////////////
    // \brief create a beam and add it to NormGroup
    //        deep copy for the beam
    // \param[in] const RtBeam&
    // \return bool
    bool AddBeam2Map(RtBeam* beam);

    //////////////////////////////////////////////////////////////////////////
    // \brief delete a beam
    //        
    // \param[in] beam uid
    // \return TPS_ER_SUCCESS when success, return TPS_ER_FAILURE when failed
    int DeleteBeam(const std::string& beamUID);

    int UpdateBeam(RtBeam* const beam);

    int UpdateBeams(std::vector<RtBeam*>& beamList);

    //////////////////////////////////////////////////////////////////////////
    // \brief get a beam
    //        
    // \param[in] beam uid
    // \return TMS beam 
    RtBeam* GetBeam(const std::string& beamUID);

    /// \brief  get beam uid list by normGroup uid
    /// \param[in] uid  the normGroupning beam group uid in database
    /// \return std::vector<std::string> the output beam uid list  NOTE! may be emtpy
    std::vector<std::string> GetBeamUIDListByNormgroupUID(const std::string& sNormGroupUid);

    std::vector<RtBeam*> GetBeamsByNormGroup(const std::string& sNormgroupUid);

    std::vector<RtBeam*> GetBeamsByPlan(const std::string& sPlanUid);

    //////////////////////////////////////////////////////////////////////////
    // \brief rotate an IMRT beam, all segments will be rotated
    //        
    // \param[in] beam uid
    // \param[in] rotate degree
    // \return TPS_ER_SUCCESS when success, return TPS_ER_FAILURE when failed
    int RotateNonArcBeam(const std::string& beamUID, float degree);

    //////////////////////////////////////////////////////////////////////////
    // \brief get beam's iec coordinate system
    //        
    // \param[in] beam uid
    // \param[out] beam's iec coordinate system
    // \return TPS_ER_SUCCESS when success, return TPS_ER_FAILURE when failed
    TpsIECCoordinateSystem* GetBeamCS(const std::string& beamUID);

    int AddBeamCS(const std::string& beamUID, TpsIECCoordinateSystem* const beamCS);

    //////////////////////////////////////////////////////////////////////////
    // \brief update segment's outline
    //        
    // \param[in] beam uid
    // \param[in] segment uid
    // \param[in] beam outline
    // \return TPS_ER_SUCCESS when success, return TPS_ER_FAILURE when failed
    int UpdateSegmentOutline(const std::string& beamUID, const std::string& segmentUID, RtContour& beamOutline);

    //////////////////////////////////////////////////////////////////////////
    // \brief update segment's MLCShape
    //        
    // \param[in] beam uid
    // \param[in] segment uid
    // \param[in] mlc shape
    // \return TPS_ER_SUCCESS when success, return TPS_ER_FAILURE when failed
    int UpdateSegmentMLC(const std::string& beamUID, const std::string& segmentUID, RtMlcshape& MLCShape);

    void ClearPatientData();

    void AddFluenceMap(const std::string& beamUid, FluenceMapEntity* fluenceMap);
    FluenceMapEntity* GetFluenceMap(const std::string& beamUid) const;
    void RemoveFluenceMap(const std::string& beamUid);

    std::string GetNormalGroupUIDViaBeamUID(const std::string& beamUid) const;

    std::vector<std::string> FilterLoadedDoseGridUid(const std::vector<std::string>& doseGridUidList);

private:
    void Dispose(); 

private:
    //BeamUid, TmsBeam
    std::map<std::string, RtBeam*> mBeams;
    //BeamUid, RTFWKDLDOAccessory
    //std::map<std::string, std::vector<RtAccessory*>> mBeamAccossory;

    std::map<std::string, TpsIECCoordinateSystem*>  mBeamCS;
    std::map<std::string, FluenceMapEntity*>        mBeamFluenceMapCollection;

    //iterator
    typedef std::map<std::string, RtBeam*>::iterator mTpsBeamMapIt;
    boost::mutex *mMutex;
};

TPS_END_NAMESPACE

#endif
