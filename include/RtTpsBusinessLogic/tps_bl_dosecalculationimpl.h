//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_bl_dosecalculationimpl.h
///
/// \brief   DoseCalculationImpl
/// 
/// \version 1.0
/// 
/// \date    2014/01/25
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_DOSECALCULATIONIMPL_H_
#define TPS_BL_DOSECALCULATIONIMPL_H_

TPS_BEGIN_NAMESPACE



class ITpsCommonController;
class ITpsDataRepository;
class TpsDoseProxy;
class TpsRenderProxy;
class TpsSaveObjects;
class RtDvh;
class RtVoi;
class ROIEntity;
class RtDosegrid;
class RtPlan;
struct RescaleDosePara;




class DoseCalculationImpl
{
public:
    DoseCalculationImpl();
    ~DoseCalculationImpl();

    void SetDataRepository(ITpsDataRepository* repos) { mDataRepository = repos; }
    void SetDoseProxy(TpsDoseProxy* doseProxy) { mDoseProxy = doseProxy; }
    TpsDoseProxy* GetDoseProxy() { return mDoseProxy; }
    void SetRenderProxy(TpsRenderProxy* proxy) { mRenderProxy = proxy; }
    TpsRenderProxy* GetRenderProxy() { return mRenderProxy; }
    inline void SetSaveCmd(TpsSaveObjects* saveCmd) { mSaveObjects = saveCmd; }

    ///  \brief  DoseCalculationImpl Initialize.
    ///
    ///  \return  true if success ;
    ///           false if any error happens.
    virtual bool Initialize();

    ///  \brief  not implemented.
    ///
    ///  \return  true always.
    virtual bool Finalize();

    ///  \brief  calculate plan dvh and update voi dose.
    ///
    ///  \param[in]  planUid  uid of plan.
    ///
    ///  \param[out]  tpsDVHList  
    ///
    ///  \return  true if success ;
    ///           false if any error happens.
    virtual bool CalculateDvhAndUpdateVoiDose(const std::string& planUid);

    ///  \brief  calculate normalgroup dvh.
    ///
    ///  \param[in]  normalGroupUid  uid of normalgroup.
    ///
    ///  \param[in]  voiUidList  all Voi uid list.
    ///
    ///  \param[out]  tpsDVHList  
    ///
    ///  \return  true if success ;
    ///           false if any error happens.
    virtual bool CalculateNormGroupDvh(
        const std::string& normGroupUid,
        const std::vector<std::string>& voiUidList,
        std::vector<RtDvh*>& tpsDVHList);

    ///  \brief  calculate voi tcpntcp parameter base on one nomrmalgroup.
    ///
    ///  \param[in]  normalGroupUid  uid of normalgroup.
    ///
    ///  \param[in]  voiUidList  all Voi uid list.
    ///
    ///  \param[out]  vVois  set tcpntcp paremater of voi
    ///
    ///  \return  true if success ;
    ///           false if any error happens.
    virtual bool CalculateBioParams(
        const std::string& normalGroupUid,
        const std::vector<std::string>& voiUidList,
        std::vector<ROIEntity*>& vVois);

    ///  \brief  calculate dose of plan.
    ///
    ///  \param[in]  planUID  uid of plan.
    ///
    ///  \param[in]  isLockMU if true should lockmu when calculate dose.
    ///
    ///  \return  TPS_ER_SUCCESS if success ;
    ///            others if any error happens.
    virtual int CalculateDoseForPlan(const std::string& planUID);

    ///  \brief  calculate dose of normalgroup.
    ///
    ///  \param[in]  ngUID  uid of normalgroup.
    ///
    ///  \param[in]  isLockMU if true should lockmu when calculate dose.
    ///
    ///  \return  TPS_ER_SUCCESS if success ;
    ///            others if any error happens.
    virtual int CalculateDoseForNormgroup(const std::string& ngUID);

    ///  \brief  check normalgroup dose validation.
    ///
    ///  \param[in]  ngUID  uid of normalgroup.
    ///
    ///  \param[out] none
    ///
    ///  \return  true if normalgroup dose is valid;
    ///           false else normalgroup dose is invalid.
    bool CheckDoseValidForNormalgroup(const std::string& ngUID);

    ///  \brief  check plan dose validation.
    ///
    ///  \param[in]  planUID  uid of plan.
    ///
    ///  \param[out] none
    ///
    ///  \return  true if plan dose is valid;
    ///           false else plan dose is invalid.
    ///
    ///  \notes  if plan has no normalgroup return false.
    ///          any normalgroup dose of the plan is valid, return true.
    bool CheckDoseValidForPlan(const std::string& planUID);

    ///  \brief  recompose dose when plan dose changed.
    ///
    ///  \param[in]  planUID  uid of plan.
    ///
    ///  \param[out] none
    ///
    ///  \return  true if success;
    ///           false if any error happens.
    ///
    ///  \notes  compose dose for normalgroup then for plan.
    ///          update fluencemap of normalgroup.
    ///          update voi dose\poi dose\DVH of plan 
    virtual bool ComposeDoseForPlanWithoutRendering(const std::string& planUID);

    ///  \brief  recompose dose when normalgroup dose changed.
    ///
    ///  \param[in]  ngUid  uid of normalgroup.
    ///
    ///  \param[out] none
    ///
    ///  \return  true if success;
    ///           false if any error happens.
    ///
    ///  \notes  compose dose for normalgroup then for plan.
    ///          update fluencemap of normalgroup.
    ///          update voi dose\poi dose\DVH of plan 
    ///          update render.
    virtual bool ReComposeDoseForNormgroup(const std::string& ngUID);

        ///  \brief  recompose dose when normalgroup dose changed.
    ///
    ///  \param[in]  ngUid  uid of normalgroup.
    ///
    ///  \param[out] none
    ///
    ///  \return  true if success;
    ///           false if any error happens.
    ///
    ///  \notes  compose dose for normalgroup then for plan.
    ///          update fluencemap of normalgroup.
    ///          update voi dose\poi dose\DVH of plan 
    ///          update render.
    virtual int RecomposePlanDose(const std::string& planUID);

    ///  \brief clear dose of plan level.
    ///
    ///  \param[in]  planUID  uid of plan.
    ///
    ///  \param[in]  isSave  tag of whether to save data to database.
    ///
    ///  \param[in]  isLockMU  if isLockMU is true, then the beam MU should not be deleted;
    ///                        else beam MU will be set to zero.
    ///
    ///  \param[out] none
    ///
    ///  \return  TPS_ER_SUCCESS if success;
    ///           others if any error happens.
    ///
    ///  \notes  plan FluenceMap, beam parameters and plan dvh should be cleared when clear dose.
    virtual int ClearDoseForPlan(const std::string& planUID, bool isSave);

    ///  \brief clear dose of normalgroup level.
    ///
    ///  \param[in]  ngUid  uid of normalgroup.
    ///
    ///  \param[in]  isSave  tag of whether to save data to database.
    ///
    ///  \param[in]  isLockMU  if isLockMU is true, then the beam MU should not be deleted;
    ///                        else beam MU will be set to zero.
    ///
    ///  \param[out] none
    ///
    ///  \return  TPS_ER_SUCCESS if success;
    ///           others if any error happens.
    ///
    ///  \notes  Normalgroup FluenceMap, beam parameters and plan dvh should be cleared when clear dose.
    virtual int ClearDoseForNormgroup(const std::string& ngUID, bool isSave);

    ///  \brief save normvoi data to database when approve normalgroup.
    ///
    ///  \param[in]  ngUid  uid of normalgroup.
    ///
    ///  \param[out] none
    ///
    ///  \return  TPS_ER_SUCCESS if success;
    ///           others if any error happens.
    int UpadteNormalVoi(const std::string& ngUid);

    ///  \brief save plan data to database when approve plan.
    ///
    ///  \param[in]  planUid  uid of plan.
    ///
    ///  \param[out] none
    ///
    ///  \return  TPS_ER_SUCCESS if success;
    ///           others if any error happens.
    int UpdatePlanVoi(const std::string& planUid);

    ///  \brief delete normvoi data by normalgroupuid.
    ///
    ///  \param[in]  ngUid  uid of normalgroup.
    ///
    ///  \param[out] none
    ///
    ///  \return  TPS_ER_SUCCESS if success;
    ///           others if any error happens.
    int DeleteNormalVoi(const std::string& ngUid);

    ///  \brief delete planvoi data by planuid.
    ///
    ///  \param[in]  planuid uid of plan.
    ///
    ///  \param[out] none
    ///
    ///  \return  TPS_ER_SUCCESS if success;
    ///           others if any error happens.
    int DeletePlanVoi(const std::string& planUid);

    ///  \brief Rescale Dose when normalgroup goal target and dose are modified.
    ///
    ///  \param[in]  ngUid  uid of normalgroup.
    ///
    ///  \param[in]  ngGoalModifyPara  parameter of normalgroup perscroption modify.
    ///
    ///  \param[out] none
    ///
    ///  \return  TPS_ER_SUCCESS if success;
    ///           others if any error happens.
    int RescaleDose(const std::string& ngUid, const RescaleDosePara& doseRescalePara);



private:
    ITpsCommonController        *mCommonController;
    ITpsDataRepository          *mDataRepository;
    TpsDoseProxy                *mDoseProxy;
    TpsRenderProxy              *mRenderProxy;
    TpsSaveObjects              *mSaveObjects;

private:
    void ClearPoiDoseForNormgroup_i(const std::string& ngUid);
    void RemoveMaxPoint_i(const std::string& planUid);
    void DeleteDvhAndClearVoiDoseForPlan_i(const std::string& planUID);
    void ClearDoseInModelWarehouse_i(const std::string& planUID);
    void UpdateDoseInModelWarehouse_i(const std::string& planUID);
    void UpdateBeamsInModelWarehouse_i(const std::string& sNormgroupUid);
    void UpdatedMaxPointInModelWarehouse_i(const std::string &planUID, const std::string& poiUID);
    void InvalidateView_i();
    bool SavePlanBeam_i(const std::string& planUid, bool isSaveDosegrid = true);
    bool SaveBeamPoiDose_i(const std::string& planUid);
    bool SaveNormgroup_i(const std::string& ngUid);
    bool SaveNormgroupBeam_i(const std::string& ngUid, bool isSaveDosegrid = true);
    bool UpdatePoiDoseForImportedPlan_i(const std::string& planUid);
    bool UpdateMaxPoint_i(const std::string& planUID);
    bool UpdatePoiDoseAfterDoseCalculation_i(const std::string& planUID);
    void ClearBeamParams_i(const std::vector<RtBeam*> beamList);
    void UpdateBeamDoseValidationByNormalGroup_i(const std::string& ngUid);
    void UpdateBeamDoseValidationByPlan_i(const std::string& planUid);
    void UpdateNormalGroupDoseValidationByPlan_i(const std::string& planUid);

    TPS_DISALLOW_COPY_AND_ASSIGN(DoseCalculationImpl);
};

TPS_END_NAMESPACE

#endif  // TPS_COMCA_DOSECALCULATIONIMPL_H_
