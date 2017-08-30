////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_business_logic_controller.h
/// 
///  \brief class TpsBLController declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/09/28
////////////////////////////////////////////////////////////////

#ifndef TPS_BUSINESS_LOGIC_COMPONENTS_MANAGER_H_
#define TPS_BUSINESS_LOGIC_COMPONENTS_MANAGER_H_

#include <boost/function.hpp>

#include "tps_bl_defs.h"
#include "RtTpsRenderController\tps_rc_renderproxy.h"
#include "RtTpsFramework\tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_prepare_notifier.h"
#include "RtTpsProtoLib/rt_tps_optoutput.pb.h"
#include "RtTpsProtoLib/rt_ms_photonsmontecarlo.pb.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedimportfromfile.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedmachinesetting.h"

#include "RtTpsDataAccess/tps_da_watercarriageparsefile.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_commissionedunit_tray.h"

//#include "alg/alg_commission.h"

namespace Mcsf
{
    class Point2f;
}

namespace RTFWK
{
    class PointDose;
    class RTFWKDLDOMeasuredProfile;
    class RTFWKDLDOGoldenSTT;
    class  RTFWKDLDOMcPhoCommissionedUnit;
    typedef struct FieldSizeKey FieldSize;
    class RTFWKDLDOCommissionedunit;
    class RTFWKDLDOMachineUnit;
    class RTFWKDLDOConvolutionData;
    class RTFWKDLDOSartable;
    class RTFWKDLDOMachine;
    class RTFWKDLDOMLCSetting;
    class RTFWKDLDOAccessory;

}

namespace ms{
    namespace proto{
        class RT_MS_MachineConfigInfo;
        class RT_MS_Proto;
        class RT_MS_PencilBeam;
        class RT_MS_Convolution;
        class RT_MS_ImportFromFileList;
        class RT_MS_SCPInfoList;
        class RT_MS_MonteCarlo;
        class RT_MS_ImportGSTTFromFile;
        class RT_MS_WedgeFactorSettingInfoList;
        class RT_MS_PhysicalWedgeField;
        class RT_MS_VirtualWedgeInfo;
        class RT_MS_ImportFromFileInfo;
        class RT_MS_MachineTray;
    }
}

namespace RTALG
{
    class AlgCommission;
}

namespace boost
{
    class thread;
    class mutex;
}

namespace tps {
    namespace proto {
        class RtTpsProtoRepeatedBeam;
        class RtTpsProtoRepeatedBeamOperate;
    }
}

namespace McsfGeometry
{
    class SubVolume;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsRenderProxy;
class ITpsDataRepository;
class TpsDataRepositoryFactory;
class TpsCommonControllerFactory;
class ITpsCommonController;
class IRtDatabaseWrapper;
class TpsCommand;
class TpsDoseProxy;
class TpsOptProxy;
class DoseCalculationImpl;
class TpsSaveObjects;
class TpsLoadObjects;
class MPRPrinter;

class RtPlan;
class RtNormgroup;
class RtBeam;
class RtPoi;
class RtVoi;
class RtDvh;
class RtCt2density;
class RtDoseoptgoalbase;
class RtMeasuredprofile;
class RtGoldenstt;
class RtMcphocommissionedunit;
class RtTray;
class RtWedgefactor;
class RtCourse;
class RtDoseline;
class RtApproverecord;
class RtMachine;
class RtCommissionedunit;
class RtAccessorychunk;
class RtGoldenstt;
class RtAccessory;
class RtPrescription;
class RtConstraint;
class RtVoisetting;
struct RtTpsDoselineSetting;
class RtMeasureddata;
class RtTemplate;

class TpsImage3DEntity;
class ROIEntity;
class PointDose;

class VOITemplate;
class DoseLineTemplate;
class OptimizerTemplate;
class BeamTemplate;
class DoseLineTemplate;
class VOITemplate;

class TpsPlanningCtrlOptObserver;
class RedoUndoActiveResponser;
class WindowActiveResponser;
class OptActiveResponser;
class SimpleGuiResponser;
typedef std::shared_ptr<RedoUndoActiveResponser> RedoUndoResponserPtr;
typedef std::shared_ptr<WindowActiveResponser> WindowResponserPtr;
typedef std::shared_ptr<OptActiveResponser> OptResponserPtr;


struct CertifiedVOI;
struct CertifiedPlan;
struct CertifiedRegistrationFeedback;
struct DoseOptimizationParam;
struct CornerTextMPR;
struct CornerTextBEV;
struct CornerTextGallery;
struct CertifiedMlcEditInfo;
struct CertifiedPoiDose;
struct TpsCertifiedSegment;
struct CertifiedImportFileInfo;
struct CertifiedEnergyWeight;
struct CommonPhysicalWedgeField;
struct CertifiedElectronContInfo;
struct CertifiedGSTTInfo;
struct CertifiedMachineSetting;
struct CertifiedScpInfo;
struct CertifiedWedgeFactorInfoList;
struct CertifiedConvolution;
struct CertifiedPencilBeam;
struct CertifiedMonteCarlo;
struct CoordinateData;
struct CertifiedVirtualWedgeInfo;
struct CertifiedGridSize;

struct FieldSize;
struct TpsSeriesGroup4d;
class MPRPrinter;
class RtTable;
class AlgCommissionProxy;

struct AddRoiBasedOnDosePara;
struct AddRoiBasedOnRoiPara;
struct MirrorRoiPara;
struct ExpandRoiPara;
struct CombineRoiPara;

/// \class TpsBLComponentsManager
/// 
/// \brief this class will manage all the BL components, including DataRepository,
///        RenderProxy, DatabaseWrapper, CommonController, etc., and also wrap
///        some business logic interfaces
class TPS_BL_DECLSPEC TpsBLController{
public:
    //the constructor and destructor
    TpsBLController();
    ~TpsBLController();

    ///initialize all components
    bool Initialize();
    ///delete all resources
    void Finalize();

    ///get the pointer to render proxy
    TpsRenderProxy* GetRenderProxy();
    ///get the pointer to data repository
    ITpsDataRepository* GetDataRepository();
    ///get the pointer to database wrapper
    IRtDatabaseWrapper* GetDatabaseWrapper();

    void SetDatabaseWrapper(IRtDatabaseWrapper* dbWrapper);      // use test.

    void SetRedoUndoActiveResponser(const RedoUndoResponserPtr& responser);

    int ProcessCommand(TpsCommand* cmd);

    void RefreshAll();

    TPS_ERROR_CODES CanSwitchToTps(const std::string& patientUid, int targetModule);
    int TpsBLController::GetRelatedPatientCountByCt2DensityUid(const std::string& ct2DensityUid, std::vector<std::string>& relatedPatientNameList);

	int CalculateDVH(const std::string& planUid);
    int CalculateNormGroupDVH(const std::string& normGroupUid,const std::vector<std::string>& voiUidList,
        std::vector<RtDvh*>& tpsDVHList);

    int CalculateBioParams(const std::string& normGroupUid,
        const std::vector<std::string>& voiUidList,
        std::vector<ROIEntity*>& vVois);

    int CalculateDVHForPlanReport(const std::string& planUid);

    ///create view module
    bool CreateCoreViewModule(const std::string& wndUid, int windowType, 
        int width, int height, const WindowResponserPtr& activeResponser);
    int DestoryCoreViewModule(WINDOW_TYPE windowType, const std::string& windowUid);
    void ActiveWindow(const std::string& wndUid, int wndType);
	TPS_ERROR_CODES SaveTableAsROI(const std::string& seriesUid, const std::string& tableUid, const std::vector<float>& voicolor, const std::string & name, ROIEntity *&tableVoi);
	void DeactiveWindow(const std::string& wndUid, int wndType);

    bool StartOptimize(const std::string& ngUid, 
        const std::vector<std::string>& vecBasedOnNgUids, 
        const std::vector<RtDoseoptgoalbase*> optGoals, 
        const DoseOptimizationParam optParam, 
        const OptResponserPtr& optResponser);

    bool OptimizeAbort();

    ///change current image of specified window group, floatUID is the registration image
    ///empty string means display fixed image only
    bool SwitchSeries(int groupType, const std::string& targetUID, 
        const std::string& floatUID);

    bool SaveMainPhaseToDBSeriesGroup(const std::string& SeriesGroupUid, const std::string& MainPhaseUid);

    bool RemoveDataSource(DATA_SOURCE_GROUP_ID dataSourceGroupID, DATA_SOURCE_ID dataSourceToRemove);
    ///change plan/normal group/beam active status, this interface won't change the current image
    ///if current image showing in window is not the one the target plan belongs to,
    ///an error will be returned
    int ChangePlanGoalSelection(int groupType, const std::string& uid,
        PLAN_GOAL_SELECTION_LEVEL level);

    //\brief    Compare dosegird size and spacing while compare/merge plan
    //param[in]  two plans used in evaluation
    //param[in]  
    //Out        string differenceInfo: discribe the difference from two dosegrids
    int CompareDoseGrid(CertifiedPlan &primaryPlan, CertifiedPlan &secondaryPlan);



    ///translate crosshair
    int UpdateCrosshair(WINDOW_TYPE windowType, const std::string& windowUid,
        double xStart, double yStart, double xEnd, double yEnd, int crossMode, int hitMode, double rotateX, double rotateY);

    bool ResetMPRObject(
        WINDOW_TYPE windowType, const std::string& windowUid);

    int GetNormalgroupBeamOperateInfo(RtNormgroup* normgroup, proto::RtTpsProtoRepeatedBeamOperate& beamOperateList);

    bool SetCrosshairHittedMode(int wndType, const std::string& wndUid, int mode);

    int RotateShadedSurface(const std::string& seriesUID, 
        float prePtX, float prePtY, float curPtX, float curPtY);

    int ShadedSurfaceZoom(const std::string& seriesUID, float fZoomFactor);

    int Pan3DWindow(WINDOW_TYPE window, const std::string& wndUid, 
        float startx, float starty, float stopx, float stopy);

	// UT 3D WZD
    bool GetShadedSurfaceViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid,
        double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);

    bool TransformPoint(WINDOW_TYPE window, const std::string& wndUid, 
        float ptInX, float ptInY, float* ptOutX, float* ptOutY, float* ptOutZ);

    bool TransformPoint(WINDOW_TYPE window, const std::string& wndUid, 
        float ptInX, float ptInY, float ptInZ, float* ptOutX, float* ptOutY);

    bool GetCrosshairProjectionPosition(WINDOW_TYPE windowType, const std::string& windowUid,
        float& x2D, float& y2D);

    bool GetCrossXYPoints(WINDOW_TYPE windowType,
        const std::string& windowUid, double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4);

	// UT MPR WZD
    bool GetMprViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid,
        double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);

	// UT MPR WZD
    double DistancePointToPlane(WINDOW_TYPE windowType, const std::string& windowUid, 
        float x3D, float y3D, float z3D);

    bool MapMilimeter2Pixel(int wndType, const std::string& wndUid,
        double lengthInmm, double* pixelinXNP, double* pixelinYNP);

    bool LoadSystemCT2DensityTables(std::vector<RtCt2density*>& vRtCt2densityList);

    #pragma region Fusion Methods
    ///set image data source of fusion-fixed & fusion-mixed group, if images all ready, 
    ///initialize the registration relationship
    ///when imageUID is empty, remove image data source of fusion-fixed & fusion-mixed group
    bool SetFixedImage(const std::string& imageUID, CertifiedRegistrationFeedback& feedback);
    ///set image data source of fusion-float & fusion-mixed group, if images all ready, 
    ///initialize the registration relationship
    ///when imageUID is empty, remove image data source of fusion-float & fusion-mixed group
    bool SetFloatImage(const std::string& imageUID, CertifiedRegistrationFeedback& feedback);
    ///display fusion region of fixed image or float image,
    ///the region defines the grid to do registration in,
    ///the part outside the region will be neglected during registration
    bool DisplayFusionRegion(bool editFixed, bool visible);
    void ChangeFusionType(const std::string& fixedUid, const std::string floatUid, 
        int fusionType);
    void ChangeFusionFactor(const std::string& fixedUid, const std::string& floatUid, 
        double fusionFactor);
    void ChangeFusionType(int fusionType);
    void ChangeFusionFactor(double fusionFactor);
    void ChangeRegistrationAlgorithm(int registrationAlgorithm, 
        CertifiedRegistrationFeedback& feedback);
    void SetPseudoColor(const std::string& imageUid, const std::string& pseudoColorName);
    int AddLandmarkPoint(WINDOW_TYPE wndType, const std::string& wndUid, 
        double x, double y, int index, double& patX, double& patY, double& patZ, CertifiedRegistrationFeedback& feedback);
    ///display/hide fusion image in common group windows
    bool ShowFusionImage(const std::string& fixedSeriesUid, 
        const std::string& floatSeriesUid, bool displayOrHide);
    void EditChess(WINDOW_TYPE wndType, const std::string& wndUid, 
        double x, double y, int mouseActionType);
    #pragma endregion

    bool SetAdminPhantomImageDataSource(const std::string& phantomSeriesUid);
    //bool LoadPhantomSeriesInQAMoudule(const std::string& phantomSeriesUid);

    bool LoadQASeries(const std::string& qaSeriesUid);
	//UT EasyPlan ZWL
	bool LoadEasyPlanData(const std::string &patientUid);

    /// dose calc impl
    int UpdateDoseCalclateMode(const std::string& ngUid, int nDoseCalculateMode);
    int RescaleDoseForBeamMU(const std::string ngUid);
    int RescaleDoseForBeamWeight(const std::string ngUid);
    int CalculateDose(const int targetLevel, const std::string uid);
    int ClearDose(const int targetLevel, const std::string uid, const bool isSave = true);
    bool IsDoseValid(const int targetLevel, const std::string uid);
    bool ReComposeNgDose(const std::string uid);
    int RecomposePlanDose(const std::string& planUID);
    int DoseHotSpots(const std::string& strPlanUid, const std::string& strVoiUid, float fDoseLimit, int iHotFlag);
    int UpdateNormVoi(const std::string& ngUid);
    int DeleteNormVoi(const std::string& ngUid);
    int UpdatePlanVoi(const std::string& planUid);
    int DeletePlanVoi(const std::string& planUid);

    ///data access interfaces
    bool GetBeamsInfo(const int targetLevel, const std::string uid, std::vector<RtBeam*>& vBeams);
    bool GetVoisInfo(const int targetLevel, const std::string uid, std::vector<ROIEntity*>& vVois);
    void GetVoisInfoBySerisesUid(const std::string& seriesUid, std::vector<ROIEntity*>& vVois);
    bool GetPoisInfo(const int targetLevel, const std::string uid, std::vector<RtPoi*>& vPois);
    bool GetNormGroupsInfo(const int targetLevel, const std::string uid, std::vector<RtNormgroup*>& vNormGroups);
    bool GetPlanInfo(const int targetLevel, const std::string uid, RtPlan*& vPlan);
    bool GetDvhsInfo(const std::string planUid, std::vector<RtDvh*>& vDvhs, bool reCalculate = false);
    bool GetDvhsInfoForPlanReport(const std::string planUid, std::vector<RtDvh*>& vDvhs);
    int GetDefaultDoseGridSize(const std::string seriesUID, CertifiedGridSize& pocoGridSize);

    ///MPR operators
	// UT MPR WZD
    bool MprPageTurn(WINDOW_TYPE wndType, const std::string& wndUid, int iPageturn);
	// UT MPR WZD
    bool MprReset(WINDOW_TYPE wndType, const std::string& wndUid, int acType);
	// UT MPR WZD
    bool MprZoom(WINDOW_TYPE wndType, const std::string& wndUid, float scaleFactor);
	// UT MPR WZD
	bool MprZoom(WINDOW_TYPE wndType, const std::string& wndUid,
		float startx, float starty, float stopx, float stopy);
	// UT MPR WZD
    bool MprPan(WINDOW_TYPE wndType, const std::string& wndUid, 
        float startX, float startY, float stopX, float stopY);
	// UT MPR WZD
    bool MprWindowing(WINDOW_TYPE wndType, const std::string& wndUid, 
        float deltaWidth, float deltaCenter, bool updateAll);
	//UT EasyPlan ZWL
    bool EasyPlanEditMlc(CertifiedMlcEditInfo& certInfo, 
        /*const std::string& beamuid, 
        const std::string& segmentuid,*/
        int apertureType);
    //UT EasyPlan ZWL
    bool EasyPlanNudge(CertifiedMlcEditInfo& certInfo,
        /*const std::string& beamuid, 
        const std::string& segmentuid,*/
        int apertureType);
	//UT EasyPlan ZWL
    bool EasyPlanDragMLC(CertifiedMlcEditInfo& certInfo, 
        /*const std::string& beamuid, 
        const std::string& segmentuid,*/
        int apertureType);
	//UT EasyPlan ZWL
	bool EasyPlanZoomImage(WINDOW_TYPE wndType, const std::string& wndUid, const std::string& beamUid, float scaleFactor);
	//UT EasyPlan ZWL
	bool EasyPlanRotateImage(WINDOW_TYPE wndType, const std::string& wndUid, const std::string& beamUid,
		float startX, float startY, float stopX, float stopY);
	//UT EasyPlan ZWL
	bool EasyPlanSetPanImage( int windowType, std::string &windowUid, std::string &planUid, std::string &beamUid,
		float startX, float startY, float stopX, float stopY,
		double viewerWidth, double viewerHeight );
	//UT EasyPlan ZWL
	bool EasyPlanWindowingImage( int windowType, std::string &windowUid, std::string &beamUid,
		int deltaWindowWidth, int deltaWindowCenter, int& currentWinWidth, int& currentWinCenter);
	//UT EasyPlan ZWL
    bool EasyPlanMesureDistance(WINDOW_TYPE windowType, std::string &windowUid, std::string &beamUID,
        float startX, float startY, float stopX, float stopY, bool actionStarted, bool actionDone, double& distanceInPatient);
	//UT EasyPlan ZWL
	bool EasyPlanResetImage( int windowType, std::string &windowUid, std::string &beamUid );
	//UT EasyPlan ZWL
	//bool EasyPlanChangeApertureType( std::string &strPlanUid, std::string &strBeamUid, int apertureType );
	//UT EasyPlan ZWL
	void EasyPlanChangeSelectedPlan(std::string &strPlanUid);
	//UT EasyPlan ZWL
    bool EasyPlanMesureAngle(int windowType, const std::string& windowUid, const std::string& beamUid, 
        const std::list<std::pair<double, double>>& points,
		float cursorX, float cursorY, bool actionStarted, bool firstSideDone, bool secondSideDone);
    //UT EasyPlan ZWL
    bool EasyPlanInverseImage( int windowType, const std::string &windowUid, const std::string &beamUid );
	//UT EasyPlan ZWL
	bool SaveEasyPlan( std::string planUid, std::string reviewerName );

    int ApproveEasyPlan(RtApproverecord& approverecord);


	//Bev operators
	// UT BEV WZD
	bool BevPan(WINDOW_TYPE wndType, const std::string& wndUid, 
		float startX, float startY, float stopX, float stopY);
	// UT BEV WZD
	bool BevZoom(WINDOW_TYPE wndType, float zoomFactor);
	// UT BEV WZD
	bool BevWindowing(WINDOW_TYPE wndType, float startX, float startY, 
		float stopX, float stopY);
	// UT BEV WZD
	int BevCevSwitch(WINDOW_TYPE wndType, bool isBev);
	int SetDrrVisibility(bool visibility);
	// UT BEV WZD
	int BevReset(WINDOW_TYPE wndType, int actionType);
	// UT BEV WZD
    bool BevSwitchDisplayMode(int displayMode);
	// UT BEV WZD
	int SwitchSegment(const std::string& beamUid, int page);
    void DowmSampleDrr(WINDOW_TYPE wndType, const std::string& wndUid, float downSampleRate);
    // VOI

    //////////////////////////////////////////////////////////////////////////
    // add roi advanced
    int AddRoiBasedOnRoi(
        const AddRoiBasedOnRoiPara& para, ROIEntity*& roi);
    int AddRoiBasedOnDose(
        const AddRoiBasedOnDosePara& para, ROIEntity*& roi);
    int VoiRemovePreviewWindow();
    //////////////////////////////////////////////////////////////////////////

    // Ê¹ÓÃ¸ÃplanµÄdosegrid¸üÐÂ¸ÃplanÏà¹ØµÄËùÓÐvoiµÄIngridÊý¾Ý
    void UpdateVoiInGridWithPlan(const std::string &planUid);
    // ¸üÐÂÄ³¸övoiÔÚÄ³¸öplanµÄdosegridµÄingrid°Ù·Ö±È
    float GetVoiInGrid(const std::string &planUid, const std::string &voiUid);
    bool CheckPointInGrid(const std::string &planUid, float ptx, float pty, float ptz);

    void Redo();
    void Undo();

    //param[in] x, y, normalized screen coordinate
    //return int  hit test number
    HitTestResult HitTest(WINDOW_TYPE wndType, const std::string& wndUid, double x, double y);

    bool PreviewTurnPage(int turnDirection, const std::string& imageUid, 
        const void*& buffer, int& bufferSize);

    virtual int PlaySeries(const std::string& strSeriesUid, const std::string& strPlanUid, PLAY_STATE playState, PLAY_SPEED playSpeed, PLAY_MODE playMode,
                           std::shared_ptr<SimpleGuiResponser> spResponser,bool isCyclic = true);
    virtual void Play3DSeries(PLAY_STATE playState);
    virtual void Play4DSeries(PLAY_STATE playState);
    virtual void SetMainPhase(std::string strGroupUid, std::string strSeriesUid);
    virtual void SetSeriesGroup(std::string groupUid, std::vector<std::string> vecSeriesUid);

    void ChangeBevEditTarget(BEV_EDIT_TARGET target);

    int AddCT2DensityTable(RtCt2density& ct2densityTable);
    int RemoveCT2DensityTable(const std::string c2dUid);
    int UpdateCT2DensityTable(RtCt2density& ct2densityTable);

    bool AddTemplate(RtTemplate* sTemplate);
    bool UpdateTemplate(RtTemplate* sTemplate);
    bool RemoveTemplate(std::string& templateUid);

    bool CreateOptGoal(RtDoseoptgoalbase& optGoal);
    bool ValidateCT2DensityOfSeries(const std::string& strPatientUid);
	int CreateOptGoal(const std::vector<RtDoseoptgoalbase*>& vecOptGoal);
    bool RemoveOptGoal(const std::string optGoalUid);
    bool UpdateOptGoal(RtDoseoptgoalbase& optGoal);
    int LoadDoseLineTemplate(std::vector<DoseLineTemplate*>& doseLineTemplateList);
    int LoadTemplate(std::vector<VOITemplate*>& voiTemplateList,
        std::vector<DoseLineTemplate*>& doseLineTemplateList,
        std::vector<BeamTemplate*>& beamTemplateList,
        std::vector<OptimizerTemplate*>& optimizerTemplateList,
        std::vector<RtTemplate*>& windowingTemplateList);

    int UpdateDoseLineParams(std::string planUID, std::vector<RtDoseline*>& doseLineList,
        RtTpsDoselineSetting& doselineSetting, bool isTemporaryUpdated);

	int ChangeDoselineDisplayMode(string planUid, bool isLine, bool isColorwash,
		float transparency,float transparency3D);


    int LoadSeries(const std::vector<std::string>& seriesUIDList,
        std::vector<TpsImage3DEntity*>& seriesList,
        std::vector<std::shared_ptr<TpsSeriesGroup4d>>& groupList,
        std::vector<ROIEntity*>& voiList);

    int LoadRois(const std::vector<std::string>& structureSetUIDList,
        std::vector<ROIEntity*>& voiList);

    int LoadPlans(const std::vector<std::string>& planUIDList,
        std::vector<RtPlan*>& planList,
        std::vector<RtPoi*>& poiList,
        std::vector<RtDoseline*>& doseLineList,
        std::vector<RtNormgroup*>& normGroupList,
        std::vector<RtBeam*>& beamList);

    int LoadDoseGrids(const std::vector<std::string>& doseGridUIDList, 
        std::vector<std::string>& affectedPlanUidList);

    int UpdatePlanOrder(const std::string& patientUid,
        std::vector<RtCourse*>& courseList, 
        std::vector<RtPrescription*>& prescriptionList,
        std::vector<RtConstraint*>& constraintList);

    int GetMachineTreeFromManagers(
        std::vector<RtApproverecord*>& vApproverecordList,
        std::vector<RtMachine*>& vMachineList,
        std::vector<RtCommissionedunit*>& vCommissionedunitList,
        std::vector<RtAccessorychunk*>& vRtAccessorychunkList,
        std::vector<RtGoldenstt*>& vRtGoldensttList,
        std::vector<RtAccessory*>& vRtAccessoryList);

    int GetAllRtTables(std::vector<RtTable*>& vecRtTables);
    int GetVoiSettingTable(std::vector<RtVoisetting*>& voiSettingTable);

    int RotateShadedSurface(WINDOW_TYPE windowType, const float& preX, const float& preY, const float& curX, const float& curY);

    int ResizeViewModule(WINDOW_TYPE window, 
        const std::string& wndUid, 
        DISPLAY_SIZE displaySize);
    int ZoomShadedSurface(WINDOW_TYPE windowType, const float& zoomFactor);

    int UpdateDrrSetting(std::string seriesUid, std::string normUid, float ctThresholdMin, float ctThresholdMax, std::string ct2densityName, std::string ct2densityUid, float electronEnergy);

    int CreateDoseLineTemplate(const DoseLineTemplate& doseLineTemplate);

    int DeleteDoseLineTemplate(const std::string& doseLineTemplateUID);

    //void ResetMlcShape(const std::string& beamUID);

    int CreateVoiTemplate(VOITemplate& voiTemplate);

    int CreateBeamTemplate(BeamTemplate& beamTemplate);

	int CreateOptimizerTemplate(OptimizerTemplate& optimizerTemplate);

    int AddVoiByTemplate(std::string voiTemplateUid,
        std::string seriesUid,
        bool isAutoSegment,
        std::vector<CertifiedVOI>& voiList);

    int UpdateLightSetting(std::string seriesUid, int mprSection, float mprTransparency3D, float rx, float ry, float rz, 
        float scale, float specf, float ambf, float difff, float shininess, 
        float opacity, float lightx, float lighty, float lightz,int smoothness, 
        bool useCameraLight, int displayMode, bool displayVertexCube,
        int startStripIndex, int endStripIndex);

    int AddBeamFromTemplate(std::string beamTemplateUid, std::string normGroupUid, std::string commissionedUnitUid, std::string isocenterUid, proto::RtTpsProtoRepeatedBeam& outputBeamList);

    int DeleteBeamTemplate(const std::string& beamTemplateUID);

    int SortBeamNumbers(const std::string& normgroupuid, std::string* pBeamUids);

	// UT MPR WZD
    int GetCornerInfoOfMPR(
        int wndType, 
        const std::string& wndUid,
        double normPosX, 
        double normPosY,
        int suvUnitType, //for PT
        CornerTextMPR* cornerInfo);

    int GetSliceInfoOfMPR(int windowType, 
        const std::string& windowUid,
        int& sliceIndex,
		int& startSliceNumber,
        int& totalSlices,
        double& sliceThickness,
        float& slcieLocation);

    int GetWindowCenterWidthOfMPR( int windowType, 
        const std::string& windowUid,
        int suvUnitType,
        double& windowCenter, //Top if PT
        double& windowWidth,  //Bottom if PT
        bool& isValueValid);

    int GetDynamicCornerInfoOfMPR(
        int windowType, 
        const std::string& windowUid, 
        double normPosX, 
        double normPosY,
        int suvUnitType, 
        double& density, 
        double& value, 
        double& doseValue,
        double& positionx, 
        double& positiony, 
        double& positionz, 
        bool& isValueValid);
    
    int GetCornerInfoOfBEV(const std::string& imageUID, int wndType, const std::string& wndUid,
        CornerTextBEV* cornerInfo);

    int SetRulerVisibility(std::string strSeriesUid, bool visible);

	// UT BEV WZD
    int GetPixelSpacingIn10MMOnBEV(int windowType, const std::string& windowUid,
        double& spacingx, double& spacingy);

	// UT BEV WZD
    bool GetBevViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid,
        double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);

    int PreviewBeamTemplate(std::string beamTemplateUid, std::string normGroupUid, std::string isocenterUid);

    int EditSegments(const std::vector<TpsCertifiedSegment>& segments);

    //Gallery operators
    bool LoadSeriesForGallery(std::string seriesUID);
    void RenderForInValidSeiresUID();
    int GetCrosshairMode(std::string seriesUID, int& iCrosshairMode);
    bool ClearImageDatas();
    bool ClearImageData(std::string seriesUID);
    void GetCurrentSeriesUID(std::string& currentSeriesUID);
    bool GetCurrentMPRSlice(WINDOW_TYPE strWidowType, const std::string& wndUid, int& currentSlice, int& totalSlice);
    bool DeleteSliceInLocal(const std::string& sSeriesUID, int& currentSlice);
    bool GetDeletedImageUIDs(const std::string& sSeriesUID, std::vector<std::string>& vImageUIDs);
    bool BackupOriginalImage3DEntity(const std::string& sSeriesUID);
    bool RestoreOriginalImage3DEntity(const std::string& sSeriesUID);

    bool GalleryMprPageTurn(WINDOW_TYPE wndType, const std::string& wndUid, int iPageturn);
    bool GalleryMprPan(WINDOW_TYPE wndType, const std::string& wndUid, 
        float startX, float startY, float stopX, float stopY);
    bool GalleryMprZoom(WINDOW_TYPE wndType, const std::string& wndUid, float scaleFactor);
    bool GalleryMprZoom(WINDOW_TYPE wndType, const std::string& wndUid, 
        float startX, float startY, float stopX, float stopY);
    bool GalleryMPRWindowing(WINDOW_TYPE wndType, const std::string& wndUid, float deltaWidth,
        float deltaCenter, bool needUpdateAll);
    bool GalleryShadedSurfaceRotate(WINDOW_TYPE wndType, const float& prePtX, const float& prePtY,
        const float& curPtX, const float& curPtY);

    bool GetCrosshairProjectionPositionForGallery(WINDOW_TYPE windowType, const std::string& windowUid,
        float& x2D, float& y2D);
    bool GetCrossXYPointsForGallery(WINDOW_TYPE windowType, const std::string& windowUid, 
        double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4);
    //int TranslateCrosshairForGallery(WINDOW_TYPE windowType, const std::string& windowUid, 
    //    double xStart, double yStart, double xEnd, double yEnd);
    //translate crosshair
    int UpdateCrosshairForGallery(WINDOW_TYPE windowType, const std::string& windowUid,
        double xStart, double yStart, double xEnd, double yEnd, int crossMode, int hitMode, double rotateX, double rotateY);
    bool SetCrosshairHittedModeForGallery(int wndType, const std::string& wndUid, int mode);
    int GetCornerInfoOfMPRForGallery(const std::string& imageUID, int wndType, const std::string& wndUid,
        /*const std::string& planUid, */double normPosX, double normPosY,
        bool bUpdateAll, int suvUnitType,//for PT
        CornerTextGallery* cornerInfo);
    bool ResetMPRObjectForGallery(WINDOW_TYPE windowType, const std::string& windowUid);
    bool GetMprViewDirecrionForGallery(WINDOW_TYPE windowType, const std::string& windowUid,
        double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);
    bool GetShadedSurfaceViewDirecrionForGallery(WINDOW_TYPE windowType, const std::string& windowUid,
        double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);

    int PlaySeriesForGallery(const std::string& strSeriesUid, PLAY_STATE playState, PLAY_SPEED playSpeed, PLAY_MODE playMode,
        std::shared_ptr<SimpleGuiResponser> spResponser);
    void Play3DSeriesForGallery(PLAY_STATE playState);
    void Play4DSeriesForGallery(PLAY_STATE playState);

    //plan report operators
    int InitializeMPRPrinter(const int width, const int height, const WindowResponserPtr& responser);
    int PrintMPR(std::string planUID, const std::vector<int> sliceNums);
    int LoadPlanReportData(std::string planUID, const std::string& courseUID);
    int ComposeDoseForPlanReport(std::string planUID);
    int GenerateDVH(std::string planUID);

    int ClearPatientDataForPlanReport();
	int ClearPatientData();

    bool IsInVOI(WINDOW_TYPE windowType, const std::string& windowUid, 
        const std::string& voiUid, double x, double y);
    bool IsInVolume(WINDOW_TYPE windowType, const std::string& windowUid, double x, double y);
    int UnApproveVois(std::vector<std::string> voiUidList);
    void ChangeAllPoiVisibility(const std::string& planUid, bool visible);
private:
    bool InitDatabaseWrapper_i();
    bool InitDataRepository_i();
    bool InitRenderModule_i();
    bool InitDoseProxy_i();
    bool InitFreeType_i();
    bool InitDoseCalculationImpl_i();
    bool InitDoseOpt_i();
    void InitPlayMovie_i();
    bool UpdateMLCNBlockShape(const std::string& beamuid, 
        //const std::string& segmentuid,
        int apertureType);
    bool ClearManagerData_i();
    bool ClearManagerDataForPlanReport_i();

    std::string GetPlanUid_i(const int targetLevel, const std::string uid);

    int AddVoiList_i(std::vector<CertifiedVOI>& voiList);

    bool IsAutoSegmentSupported_i(const std::string voiName);

    //////////////////////////////////////////////////////////////////////////
    //add voi advanced
    int MirrorRoi_i(
        TpsImage3DEntity* image3D, 
        McsfGeometry::SubVolume* srcSV, 
        const MirrorRoiPara& para, 
        McsfGeometry::SubVolume* tgtSV);
    int ExpandRoi_i(
        TpsImage3DEntity* image3D, 
        McsfGeometry::SubVolume* srcSV, 
        const ExpandRoiPara& para, 
        bool isExpansion, 
        McsfGeometry::SubVolume* tgtSV);
    int CombineRoi_i(
        McsfGeometry::SubVolume* srcSV, 
        const CombineRoiPara& para, 
        McsfGeometry::SubVolume* tgtSV);
    int PreviewRoi_i(
        TpsImage3DEntity* image3D, 
        const std::string& targetUID, 
        const float* color,
        McsfGeometry::SubVolume*& sv);
    int AddRoi_i(
        TpsImage3DEntity* image3D, 
        const std::string& targetUID, 
        const std::string& roiName,
        int roiPlanningRole,
        int roiSubRole,
        const float* color,
        McsfGeometry::SubVolume*& sv, 
        ROIEntity*& roi);
    //////////////////////////////////////////////////////////////////////////

    TpsRenderProxy              *mRenderProxy;
    TpsRenderProxy              *mRenderProxyForPlanReport;
    ITpsDataRepository          *mDataRepository;
    ITpsDataRepository          *mDataRepositoryForPlanReport;
    IRtDatabaseWrapper          *mDatabaseWrapper;
    TpsDataRepositoryFactory    *mDataRepositoryFactory;
    TpsDoseProxy                *mDoseProxy;
    TpsDoseProxy                *mDoseProxyForPlanReport;
    DoseCalculationImpl         *mDoseCalculator;
    DoseCalculationImpl         *mdoseCalculatorForPlanReport;
    TpsSaveObjects              *mSaveObjects;
    TpsLoadObjects              *mLoadObjects;
    TpsLoadObjects              *mLoadObjectsForPlanReport;
    TpsOptProxy                 *mOptProxy;
    TpsPlanningCtrlOptObserver  *mCtrlOptObserver;
    bool                         mInitialized;
    MPRPrinter                  *mMPRPrinter;

    // movie
    void PlayerThreadCallback();
    std::string                 m_strMovieSeriesUid;
    std::string                 m_strMoviePlanUid;
    PLAY_STATE                  mPlayState;
    PLAY_SPEED                  mPlaySpeed;
    PLAY_MODE                   mPlayMode;
    bool                        mIsCyclic;
    std::shared_ptr<SimpleGuiResponser> m_spResponser3d;
    std::shared_ptr<SimpleGuiResponser> m_spResponser4d;
    boost::thread               *m_thread;
    boost::mutex                *m_mutex;

    void GalleryPlayerThreadCallback();
    std::string                 m_strGalleryMovieSeriesUid;
    PLAY_STATE                  mGalleryPlayState;
    PLAY_SPEED                  mGalleryPlaySpeed;
    PLAY_MODE                   mGalleryPlayMode;
    std::shared_ptr<SimpleGuiResponser> m_spGalleryResponser3d;
    std::shared_ptr<SimpleGuiResponser> m_spGalleryResponser4d;
    boost::thread               *m_galleryThread;
    boost::mutex                *m_galleryMutex;

    // temporary storage for preview voi
    std::vector<ROIEntity*>     m_vecPreviewVOI;

    //Gallery
    std::string                 mCurrentImageDataUid;

    //TpsMCPrepareNotifier        *pNotifier;

public:

     int LoadMachineSetting(); 
     int UpdateMachineSetting(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig);
     int AddMachineSetting(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig,string& machineUID);
     int MachineProtoToRTMachineConverter(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig,RtMachine* machine);
     int DeleteObject(std::string& uid,std::string& objectType);
     int AddTray( std::string machineUID,ms::proto::RT_MS_MachineTray* protoMachineTray,std::string& trayUID);
     int AddCommissionInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig , std::string & commissionUID,std::string& measureUID,std::string& phoCommissionUID,CertifiedConvolution& vCertifiedConvolution);
     int UpdateCommissionInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig );

     int ImportDataFromFile(std::vector<std::string>& fileList, ms::proto::RT_MS_Proto* proto, float& minAxisX, float& maxAxisX, float& minAxisY, float& maxAixsY ,int& importedNumber);
     int UpdatePencilBeam(ms::proto::RT_MS_PencilBeam* protoPencilBeam);

     //PMC part
     int GetPrepareProgress();
     bool UpdatePhotonMonteCarlo(ms::proto::RT_MS_PhotonMonteCarloInformation* protoPhotonMonteCarlo);
     int PrepareMonoPDD(std::string& commissionUID, TpsMCPrepareNotifier* pNotifier);
     int ClearMCPrePareFileFolder(std::string& machineUID, std::string& commissionUID);
     int GetCompletePrepareFiles(std::string& machineUID, std::string& commissionUID, int& iFileNum);
     bool UpdatePDDThreeTables(std::string commissionUID,ms::proto::RT_MS_ImportFromFileList* measureUIDList,
         std::vector<EnergyAndWeight>& primarySpectrumCollection,std::vector<EnergyAndWeight>& secondarySpectrumCollection,
         std::vector<EnergyAndWeight>& electronSpectrumCollection);
     //to be deleted
     void testFunction(std::vector<RTFWK::RTFWKDLDOMeasuredProfile*> aLGmeasuredProfileList,std::vector<EnergyAndWeight>&out1,
         std::vector<EnergyAndWeight>&out2,std::vector<EnergyAndWeight>&out3);








     int UpdateConvolution(ms::proto::RT_MS_Convolution* protoConvolution);
     int UpdateMonteCarlo(ms::proto::RT_MS_MonteCarlo* protoMonteCarlo);
     int UpdateVirtualWedgeFieldInfo(ms::proto::RT_MS_VirtualWedgeInfo* protoVirtualWedgeField);

     int AddWedgeInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig ,std::string& wedgeUID);
     int UpdateWedgeInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig);

     int AddApplicatorInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig,std::string& applicatorUID);
     int UpdateApplicatorInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig);

     int UpdateLeafInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig);
     int ComputerMeasureDataInfo(ms::proto::RT_MS_ImportFromFileList* measureUIDList, const bool& isCalcAbsOrRel, std::vector<shared_ptr<RtMeasuredprofile>>& importFromDataList, float& minAxisX, float& maxAxisX, float& minAxisY, float& maxAxisY);
     int NormMeasureDataInfo(ms::proto::RT_MS_ImportFromFileList* measureUIDList, std::list<CertifiedImportFileInfo>& importFromDataList);
     int CommonOperationCurveFunction(ms::proto::RT_MS_ImportFromFileList* measureUIDList, std::vector<std::shared_ptr<RtMeasuredprofile>>& importFromDataList);
     int OptimizeFunction(bool bCalcAbsOrRel, ms::proto::RT_MS_Proto* protoInfo, CertifiedElectronContInfo& electronContInfo);
     int ScpInfoFunction(const std::string& commissionUID, const RtMeasureddata& measureData);
     int UpdateWedgeFactorInfo(RtWedgefactor& wedgefactor, std::string& wedgeFactorUID, const float& wedgefactorDepth);
     int UpdateCurveProfile(const RtMeasuredprofile& measurePro);
     int AddaCurveProfile(ms::proto::RT_MS_ImportFromFileInfo* protoImportData, std::string& measureDataUID);
     int DeleteSelectedCurves(ms::proto::RT_MS_ImportFromFileList* protoImportDataList);
     int ClearMeasureData(ms::proto::RT_MS_ImportFromFileInfo* protoImportData);
     int  RescaleMeasureDataInfo(bool isAbsMode, std::string commissionUID);

     int CurveErrorAnalyseFunction(ms::proto::RT_MS_ImportFromFileList* measureUIDList, std::vector<shared_ptr<RtMeasuredprofile>>& vMeasuredprofileList);
     int ImportDataFromGSTTFile(std::string& filePath,ms::proto::RT_MS_ImportGSTTFromFile* importGSTTInfo,CertifiedGSTTInfo& gsttInfo);

     int CreateIntersectionMatrix(std::string& commissionUID);
     int InitilizePhoEngergyWeight(std::string& commissionUID,float& minEnergy, float& maxEnergy, std::vector<CertifiedEnergyWeight>&vEnergyWeight);
     int DoFluenceCorrection(std::string& commissionUID,std::string& measureDataUID,float& depth);
     int EnergyFlunenceCorrection(std::string commissionUID,std::string measureUID, float depth ,std::vector<float>& energyList);
     int GenerateFinalFile(std::string& commissionUID);
     int MonteCarloAlg(std::string& commissionUID);
     int AddVirtualWedge(std::string& commissionUID, std::string& accUID, int algType);
     int DeleteVirtualWedge(std::string& commissionUID, std::string& accUID, int algType);
     int AddWedgeFieldInfo(std::string& accessoryUid, std::string& commissionUid, int algType);
     int DeleteWedgeField(std::string& commissionUID, std::string& accUID, int algType);
//LOAD.....
	 bool CopyMachineAndCommission(std::string machineUid, std::string machineName, std::string deviceId, std::string manufactory, std::string& copyMachineUid);
	 bool GetMachineSettingInfomation(std::vector<RtMachine>& machineInfoList);
     bool GetTrayListByMachineUid(std::string machineUid,std::vector<RtTray>& vTrayList);
     bool GetAccessoryListByMachineUid(std::string machineUid,std::vector<RtAccessory>& accessoryList);
     bool GetCommissionListByMachineUid(std::string machineUid,std::vector<RtCommissionedunit>& commissionUnitList);
     bool GetCommissionedunit_trayListByUnitUid(const std::vector<RtCommissionedunit>& commissionUnitList, std::vector<RtCommissionedunit_tray>& trayFactorList);


     bool GetMachineSettingInfoByMachineUid(const std::string& machineUid ,RtMachine& machine);

     bool GetImportFileDataInfo(std::string& uid, std::vector<RtMeasuredprofile>& measureProfileList, float &depth, bool &isNormalDepth, float& minAxisX, float& maxAxisX, float& minAxisY, float& maxAxisY);
     bool GetScpDataInfo(std::string& commissionUID, RtMeasureddata& measureData);
     bool GetWedgeFactorDataInfo(std::string& commissionUID, RtWedgefactor& wedgeFactor);
     bool GetVirtualWedgeFactorDataInfo(std::string& commission, std::list<std::string>&virtualWedgeUID, std::vector<CertifiedGSTTInfo>& gsttInfo);
     bool GetAccessoryListByCommissionUid(std::string& commissionUID, std::vector<RtAccessory>& accessoryList);
     bool GetConvolutionDataInfo(std::list<std::string>&physicalWedgeUID,std::list<std::string>&virtualWedgeUID,std::string& commissionUID,CertifiedConvolution& vCertifiedConvolution);

     bool GetPencilBeamDataInfo(std::list<std::string>&physicalWedgeUID,std::list<std::string>&virtualWedgeUID,std::string& commissionUID,CertifiedPencilBeam& cerPencilBeam);

     bool GetMonteCarloDataInfo(std::list<std::string>&physicalWedgeUID,std::list<std::string>&virtualWedgeUID,std::string commissionUID,CertifiedMonteCarlo& monteCarlo);
     
     //PMC
     bool GetPhotonMonteCarloDataInfo(std::string commissionUID, shared_ptr<ms::proto::RT_MS_PhotonMonteCarloInformation>& protoPhotonMonteCarlo);
     bool GetMCPhysicalWedgeFieldInfo_i(std::string& commissionUID, shared_ptr<ms::proto::RT_MS_PhotonMonteCarloInformation> protoPhotonMonteCarlo);
     bool GetMCVirtualWedgeFieldInfo_i(std::string& commissionUID,shared_ptr<ms::proto::RT_MS_PhotonMonteCarloInformation> protoPhotonMonteCarlo);


     bool GenerateCorrectionFactor( std::string commissionUID,int algType,std::string selectedAcessoryUID,
         std::vector<float>&correctionFactor);


     int ApproveUserInfo(RtApproverecord& approverecord);
     int UnApproveUserInfo(const std::string& objectuid);
     void ClearAllPatientLocks();

	 int CopyContour(const std::string& seriesUID,const std::string& voiUID);
	 int CutContour(const std::string& seriesUID,const std::string& voiUID);
     int RetrieveVoiDensity(const std::string& voiUID, ROIEntity*& tpsVOIEntity);
	 int PasteContour(const std::string seriesUID, const std::string voiUID, EDIT_FUNCTION editFunction);
     bool AutoCalPBSCorrection(std::string commissionUID);
     bool GetPhysicalWedgeFieldInfo_i(std::list<std::string>& physicalWedgeUID, std::string& commissionUID, std::vector< std::tr1::shared_ptr<CommonPhysicalWedgeField> >& physicalWedgeList, int algType);
     bool GetVirtualWedgeFieldInfo_i(std::list<std::string>& virtualWedgeUID, std::string& commissionUID, int algType, std::vector< std::tr1::shared_ptr<CertifiedVirtualWedgeInfo>>& virtualWedgeList);
     
	 bool SaveMachineAsTemplate(const std::string& machineUid, std::string machineTemplateName, const std::string& savePath);
	 int LoadMachineTemplate(const string& templateFilePath, std::vector<std::string>& machineUids);
	 bool GetAccessoryByUid(const std::string& uid, RtAccessory& accessory);

     TPS_ERROR_CODES GetPatient2ViewMat(const WINDOW_TYPE& wndType, const std::string& wndUid, Mcsf::MedViewer3D::Matrix4x4& patient2ViewMat);
private:
    bool DeletePhysicalWedge(std::string& commissionUID,std::string& accUID);
    bool GetGoldensttListByAccUidAndUnitUid(std::string commissionUID,std::string selectedAcessoryUID,std::string& goldenUID);
    int DeleteCommission_i(std::string& uid);
    int DeleteMachine_i(std::string& uid);
    int DeleteWedgeInfo_i(std::string& uid);
    int DeleteApplicatorInfo_i(std::string& uid);
    int DeleteTrayInfo_i(std::string& uid);
    bool StringSplit_i(std::string str,char a,std::vector<float>num);
    bool GetOffSetDose_i(std::vector<CoordinateData>& coordinateData,std::vector<float>& vecDose, float& doseMax, int coodiDir,FieldSize fieldSize, float& offX,float& offY);
    int IsNormalSucess_i(int& type, int& depth, std::vector<PointDose*>& pointDoseList,float& kk, int& failureDepthValue );
    int GetMeasureDataDepthMaxAndMin_i(std::vector<RtMeasuredprofile*>& measureProfileList, int pointDoseType,float& min,float& max);
    float GetPos_i(CoordinateData& oPoint, int& coodiDir);

    int SortToCollectionData(int curveType,std::vector<PointDose*>& pointDoseList,std::vector<PointDose*>& sortPointDoseList);
    bool UpdatePhysicalWedgeField_i( std::vector<std::tr1::shared_ptr<CommonPhysicalWedgeField >> physicalWedgeList,std::string& commissionUID, int& algType);


    bool GetMeasureDataMaxandMinXY(std::vector<RtMeasuredprofile*>& measureProfileList, float& minX,float& maxX,float& minY,float& maxY);

    bool FloatToStringFunc(std::vector<float> vfCorrectionFactor,std::string & str);
    bool IntToStringFunc(std::vector<int> vfCorrectionFactor,std::string & str);

    bool StringToVectorFunc( std::string& strMuDataX, std::vector<float>& DataList);

    bool DBGoldenSttConvertToAlgType(RtGoldenstt* dBGoldenSTT,RTFWK::RTFWKDLDOGoldenSTT& goldenSTT);
    bool AlgTypeConvertToDBGoldenStt(RTFWK::RTFWKDLDOGoldenSTT* algGolden,RtGoldenstt& goldenSTT);

    bool DbMcphocommissionConvertToAlgCommission(RtMcphocommissionedunit* dbphoCommUnit,
        RTFWK::RTFWKDLDOMcPhoCommissionedUnit& phoCommUnit);

    bool DbCommissionConvertToAlgCommission(RTFWK::RTFWKDLDOCommissionedunit* algCommissionedUnit,
        RtCommissionedunit& tpsCommissionedUnit);
    void SetMachineUnit_i(RTFWK::RTFWKDLDOMachineUnit* machineUnit, 
        RtCommissionedunit& tpsCommissionedUnit);

    bool SetConvolutionData_i(RTFWK::RTFWKDLDOConvolutionData* convolutionData,
        RtCommissionedunit& tpsCommissionedUnit);

    bool SetSarTable_i(RTFWK::RTFWKDLDOSartable* sar, RtCommissionedunit& tpsCommissionedUnit);

    bool DbMachineConvertToAlgMachine(RTFWK::RTFWKDLDOMachine* algMachine, RtMachine& tpsMachine, std::vector<RtTray>& vTray);

    bool SetMLCSetting_i(RTFWK::RTFWKDLDOMLCSetting* algMlcSetting, RtMachine& tpsMachine);

    bool GetAccessoryConvertToAlgData( RtAccessory* accessory, RTFWK::RTFWKDLDOAccessory* algAccessory );
   

    bool GenerateCurveProfilePath(RtMeasuredprofile& profile);
    bool UpdateFileList(RtMeasuredprofile * profile, RtMeasuredprofile& retProfile);
    bool WriteFile_i(std::string sFilePath,string& fileContent);
    bool ReadFile_i(std::string sFilePath,string& fileContent);
    bool ConfigLog();
    bool LoadMachineSettingFromDB(const std::string& machineUid);
    int AlgCommissionProxyInit(std::string& commissionUID, AlgCommissionProxy* mAlgCommissionProxy);
private:
    CertifiedImportFileInfo mDataList;
    std::list<CertifiedImportFileInfo> mFileInfoList;
    AlgCommissionProxy* mAlgCommissionProxy;
    //RTFWK::RTFWKDLDOMeasuredProfile temMeasureProfile;


private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsBLController);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
