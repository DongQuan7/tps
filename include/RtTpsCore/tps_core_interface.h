//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author chengcheng.rong chengcheng.rong@united-imaging.com
///
///  \file    
///  \brief   
///
///  \version 1.0
///  \date    Jul.8, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_CORE_ITPSBUSINESSCORE_H
#define TPS_CORE_ITPSBUSINESSCORE_H

#include "tps_core_common.h"
#include "tps_core_delegate_carrier.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;

namespace tps {

    public value struct CliHitResult
    {
      Int32   hitNum;
      String^ targetUid;
    };

    public ref class ITpsBusinessCore abstract
    {
    public:
        virtual bool Initialize() = 0;

        virtual void SetRedoUndoCallback(DelegateCarrier<CommonDelegate^>^ callback) = 0;
        virtual int TestCanSwitchToTps(System::String^ patientUid, int moduleType) = 0;
        virtual int GetRelatedPatientCountByCt2DensityUid(System::String^ ct2DensityUid,[Out] array<System::String^>^ %relatedPatientNameListSys )= 0;
        virtual int LoadSystemCT2DensityTables([Out] array<System::Byte>^% ct2DensityTables) = 0;
      
        virtual int LoadMachineTree([Out]IList<array<Byte>^>^ %protoMachineTree) = 0;
        virtual int LoadTables([Out]array<Byte>^ %protoTables) = 0;
        virtual int LoadVoiSettingTable([Out]array<Byte>^ %byteVoiSettingTable) = 0;
        virtual int LoadPatientTreeAsync(System::String^ patientUid) = 0;
        virtual void ClearPatientData() = 0;
        virtual int LoadSeries(IList<System::String^>^ seriesUidList, [Out]IList<array<Byte>^>^ %protoListByte) = 0;
        virtual int LoadRois(IList<System::String^>^ structureSetUidList, [Out]array<Byte>^ %protoByte) = 0;
        virtual int LoadPlans(IList<System::String^>^ planUidList, [Out]IList<array<Byte>^>^ %protoListByte) = 0;
        virtual int LoadDoseGrids(IList<System::String^>^ doseGridUidList, [Out]IList<System::String^>^ %affectedPlanUidList) = 0;
        virtual int UpdatePlanOrder(System::String^ patientUid, [Out]IList<array<Byte>^>^ %protoListByte) = 0;
        virtual bool LoadStandAloneEasyPlanData(System::String^ patientUid) = 0;
        virtual bool CreateViewModule(System::String^ controllerUid, int windowType, 
            int width, int height, DelegateCarrier<WindowUpdatedDelegate^>^ windowUpdatedCallback) = 0;
        virtual int DestoryViewModule(int windowType, String^ windowUid) = 0;
        virtual int ResizeViewModule(int wndType, System::String^ wndUid, 
            int displayHeight, int displayWidth) = 0;
        virtual void ActiveWindow(System::String^ windowUid, int windowType) = 0;
        virtual void DeactiveWindow(System::String^ windowUid, int windowType) = 0;
  
        virtual bool RemoveDataSource(int dataSourceGroupID, int dataSourceToRemove) = 0;
        virtual bool SwitchSeries(int groupType, System::String^ seriesUid, System::String^ floatSeriesUid) = 0;
        virtual int ChangePlanGoalSelection(int groupType, System::String^ uid, int level) = 0;

#pragma region Plan
        virtual int DeletePOI2BE_EasyPlan(System::String^ mEZPlanUid, System::String^ mPoiUid, int mPoiType) = 0;
        virtual int SendPOI2BE_EasyPlan(array<System::Byte>^ protoPoiBytes) = 0;
        virtual int DeleteImage_EasyPlan(System::String^ planUid) = 0;
        virtual int LoadImage_EasyPlan(System::String^ beamUID, 
            System::String^ localImagePath, [Out]IList<array<Byte>^>^ %protoRtImageByte) = 0;
        virtual int LoadRtImageInfo_EasyPlan(System::String^ localImagePath, [Out] array<System::Byte>^% protoOutput) = 0;
        virtual int CreatePlan(array<System::Byte>^ protoPlanBytes, [Out] array<System::Byte>^% protoPlan) = 0;
        virtual int EditPlan(System::String^ planuid, System::String^ planname, System::String^ plandescription, System::String^ setuppoiuid) = 0;
        virtual int EasyPlanBeamCreate(array<System::Byte>^ protoBeamBytes, [Out] array<System::Byte>^% protoBeamReturn) = 0;
        virtual int DeletePlan(System::String^ planUid) = 0;
        virtual int AddEasyPlan(int iWndType, System::String^ windowUid, array<System::Byte>^ easyPlanArray) = 0;
        virtual int DeleteEasyPlan(array<System::Byte>^ easyPlanArray) = 0;
        virtual int UpdateEasyPlan(array<System::Byte>^ easyPlanArray) = 0;
        virtual int EvaluationMultiPlanOperaton(int OperateType, System::String^ mergedPlanName, IList<System::String^>^ normgroupUids, [Out]array<System::Byte>^% mergedPlan) = 0;
        virtual int EasyPlanChangeSelectedPlan(System::String^ planUid) = 0;
        virtual int SaveEasyPlan(System::String^ easyPlanUid, System::String^ reviewerName) = 0;
        virtual int ApproveEasyPlan(array<System::Byte>^ byteApproveRecord) = 0;
        virtual int ApprovePlan(array<System::Byte>^ byteApproveRecord,
            [Out] array<System::Byte>^% pois, [Out] array<System::Byte>^% vois) = 0;
#pragma endregion

#pragma region Normgroup
        virtual int CreateNormgroup(array<System::Byte>^ protoInput, [Out] array<System::Byte>^% protoOutput) = 0;
        virtual int DeleteNormgroup(System::String^ normgroupUid) = 0;
        virtual int GetNormalgroupBeamOperateInfo(array<System::Byte>^ protoNormgroup, [Out] array<System::Byte>^% beamOperates) = 0;
        virtual int UpdateNormalgroupBeam(System::String^ normalgroupUid, array<System::Byte>^ protoBeamOperates, [Out] array<System::Byte>^% beams) = 0;
        virtual int EditNormgroup(array<System::Byte>^ protoNormgroup) = 0;
        virtual int ApproveNormgroup(array<System::Byte>^ byteApproveRecord, [Out] array<System::Byte>^% pois, [Out] array<System::Byte>^% vois) = 0;
#pragma endregion

#pragma region Beam
        virtual int CreateBeam(array<System::Byte>^ protoInput, [Out] array<System::Byte>^% protoOutput) = 0;
        virtual int DuplicateBeam(array<System::Byte>^ protoInput, [Out] array<System::Byte>^% protoOutput) = 0;
        virtual int OppositeBeam(array<System::Byte>^ protoInput, [Out] array<System::Byte>^% protoOutput) = 0;
        virtual int DeleteBeam(System::String^ beamUid) = 0;
        virtual int EditBeam(array<System::Byte>^ byteBeam, int editType, [Out] array<System::Byte>^% protoOutput) = 0;
        virtual int EditBeamOnTheFly(array<System::Byte>^ byteBeam)  = 0;
        virtual int SortBeamNumbers(System::String^ normgroupuid, [Out]System::String^% beamuids) = 0;
        virtual int RotateImrtBeamOnMpr(int wndType, System::String^ windowUid, 
            double curX, double curY, int mouseActionType,
            System::String^ beamUid, System::String^ segmentUid, 
            DelegateCarrier<SingleByteArrayDelegate^>^ callback) = 0;
        virtual int RotateArcBeamOnMpr(int wndType, System::String^ windowUid, 
            double preX, double preY, double curX, double curY, int mouseActionType,
            System::String^ beamUid, bool isToRotateStart,
            DelegateCarrier<SingleByteArrayDelegate^>^ callback) = 0;
        virtual int UpdateBeamAttributes(array<System::Byte>^ protoInput, int editType, [Out] array<System::Byte>^% protoSegmentList) = 0;
        virtual int SetMlcTransparency(float fTrans) = 0;
        virtual int DeleteEasyPlanBeam(System::String^ beamUid) = 0;
        virtual int UpdateEasyPlanBeam( System::String^ easyPlanUID,array<System::Byte>^ beamBytes) = 0;
        virtual int EasyPlanChangeSelectedBeam(int wndType, System::String^ wndUid, array<System::Byte>^ beamBytes) = 0;
        virtual int BeamBlockUpdate(System::String^ beamuid, array<System::Byte>^ protoByteArray, int blockType) = 0;
        virtual int BeamPreviewPageTurn(System::String^ windowUid, int windowType, int nPageTurn) = 0;
#pragma endregion

#pragma region Segment
        virtual int BeamSegmentUpdate(array<System::Byte>^ protoByteArray) = 0;
#pragma endregion

#pragma region POI
        virtual int PoiCreate3d(array<System::Byte>^ protoPoiBytes, [Out] array<System::Byte>^% outProtoPoi) = 0;
        virtual int PoiDelete(array<System::Byte>^ poiActionArray) = 0;
        virtual int PoiJumpTo(array<System::Byte>^ protoPoiBytes) = 0;
        virtual int PoiUpdateAttribute(array<System::Byte>^ protoPoiBytes, bool isUpdatePoiDose, [Out] array<System::Byte>^% outProtoPoi) = 0;
        //AddISOCenter in easy plan
        virtual array<System::Byte>^ AddISOCenter(array<System::Byte>^ protoPoiAction) = 0;
        virtual int CalculateSSD(System::String^ beamUid, System::String^ isocenterUid, System::String^ skinUid, bool isBeamSSD, [Out] array<System::Byte>^% protoSSD) = 0;
        virtual void ChangeAllPoiVisibility(System::String^ planUid, bool isVisible) = 0;
#pragma endregion

        virtual array<System::Byte>^ GetProtoPatientTreeData(System::String^ patientUid) = 0;


        virtual int UpdateCrosshairAction(double startx, double starty, double endx, double endy,
            double regionWidth, double regionHeight, int windowType, System::String^ windowUid, int crossmode, int hitmode,double rotateX,double rotateY) = 0;
        virtual bool ResetMPRObject(int windowType, System::String^ windowUid) = 0;
        virtual bool SetCrosshairHittedMode(int wndType, System::String^ wndUid, int mode) = 0;
        virtual void MPRPageTurn(int wndType, System::String^ windowUid, int iPageTurn) = 0;
        virtual void MPRReset(int windowType, System::String^ windowUid, int actionType) = 0;
        virtual void MPRPan(int windowType, System::String^ windowUid, double startx, double starty, 
            double stopx, double stopy) = 0;
        virtual void MPRZoom(int windowType, System::String^ windowUid, float startx, float starty, 
            float stopx, float stopy) = 0;
        virtual void MPRZoom(int windowType, System::String^ windowUid, float scaleFactor) = 0;
        virtual void MPRWindowingUseDelta(int windowType, System::String^ windowUid, float width,
            float center, bool needUpdateAll) = 0;
        virtual void MPRWindowingUseDirectWindowingPara(int windowType, System::String^ windowUid, float width,
            float center, bool needUpdateAll) = 0;
        virtual CliHitResult HitTest(int wndType, System::String^ wndUid, double x, double y) = 0;

#pragma region Bev
        virtual void BEVPan(int windowType, System::String^ windowUid, 
            double startx, double starty, double stopx, double stopy) = 0;
        virtual void BEVZoom(int windowType, float zoomFactor) = 0;
        virtual void BEVWindowing(int windowType, double startx, double starty, 
            double stopx, double stopy) = 0;
        virtual int BEVCEVSwitch(int windowType, bool isBev) = 0;
        virtual	int BEVReset(int windowType, int actionType) = 0;
        virtual int SetDRRVisibility(bool visibility) = 0;
        virtual int GetPixelSpacingIn10MMOnBEV(int windowType, System::String^ windowUid,
            double^% spacingx, double^% spacingy) = 0;
        virtual int BEVSwitchDisplayMode(int displayMode) = 0;
        virtual int SwitchSegment(System::String^ beamUid, int sequenceNum) = 0;
        virtual void DownSampleDrr(int windowType, System::String^ windowUid, float downSampleRate) = 0;
        virtual bool GetBevViewDirection(int windowType, System::String^ windowUid, 
            double^% viewX3D, double^% viewY3D,  double^% viewZ3D,  double^% upX3D, double^% upY3D, double^% upZ3D) = 0;
#pragma endregion


        virtual void EasyPlanRotate(int windowType, System::String^ wndUid, System::String^ beamUid,
            float startX, float startY, float stopX, float stopY) = 0;
        virtual void EasyPlanZoomImage(int windowType, System::String^ windowUid, System::String^ beamUid, float scaleFactor) = 0;
        virtual void EasyPlanSetPan(int windowType, System::String^ windowUid, System::String^ planUid, System::String^ beamUid,
            float startX, float startY, float stopX, float stopY, double viewerWidth, double viewerHeight) = 0;
        virtual void EasyPlanWindowing(int windowType, System::String^ windowUid, System::String^ beamUid, 
            int deltaWindowWidth, int deltaWindowCenter, [Out]int% currentWinWidth, [Out]int% currentWinCenter) = 0;
        virtual void EasyPlanMesureDistance(int mWindowType, System::String^ windowUid, System::String^ beamUid,
            float startX, float startY, float stopX, float stopY, bool actionStarted, bool actionDone, [Out] double% distanceInPatient) = 0;
        virtual void EasyPlanResetImage(int windowType, System::String^ windowUid, System::String^ beamUid) = 0;
        virtual int EasyPlanMesureAngle(int windowType, System::String^ windowUid, System::String^ beamUid, array<System::Byte>^ angleData) = 0;
        virtual int SendInverse_EasyPlan(int wndType, System::String^ windowUid, System::String^ beamUid) = 0;

#pragma region Dose
        virtual int UpdateDoseCalclateMode(System::String^ ngUid, int nDoseCalculateMode) = 0;
        virtual int RescaleDoseForBeamMU(System::String^ ngUid) = 0;
        virtual int RescaleDoseForBeamWeight(System::String^ ngUid) = 0;
        virtual int DoseClear(int level, System::String^ uid, bool isSave) = 0;
        virtual int DoseReComposeNgDose(System::String^ uid) = 0;
        virtual int RecomposePlanDose(System::String^ planUID) = 0;
        virtual int DoseCalculate(int level, System::String^ uid) = 0;
        virtual int DoseInfoRetrieve(int level, System::String^ uid, [Out] array<System::Byte>^% beams, 
            [Out] array<System::Byte>^% pois, [Out] array<System::Byte>^% vois, [Out] array<System::Byte>^% normgroups,
            [Out] array<System::Byte>^% plan) = 0;
        virtual int DVHInfoRetrieve(System::String^ planUid, [Out] array<System::Byte>^% dvhs, bool reCalculate) = 0;
        virtual int EzDoseCalculate(System::String^ easyPlanUID,System::String^ currentBeamUID, 
            [Out] array<System::Byte>^% outputBeamInfo, [Out] array<System::Byte>^% outputPointInfo) = 0;
        virtual bool DoseValidCheck(int level, System::String^ uid) = 0;
        virtual int RefreshNormGroupDVH(System::String^ normGroupUid, IList<System::String^>^ voiUidList,
            [Out] array<System::Byte>^% dvhs) = 0;
        virtual int CalculateBioParams(System::String^ normGroupUid, IList<System::String^>^ voiUidList,
            [Out] array<System::Byte>^% vois) = 0;
        virtual bool IsDoseValid(int level, System::String^ uid) = 0;
        virtual int UpdateNormVoi(System::String^ ngUid) = 0;
        virtual int UpdatePlanVoi(System::String^ planUid) = 0;
        virtual int DeleteNormVoi(System::String^ ngUid) = 0;
        virtual int DeletePlanVoi(System::String^ planUid) = 0;
#pragma endregion

#pragma region optGoal
		virtual int CreateOptGoal(array<System::Byte>^ protoOpt, [Out]System::String^% optGoalUid) = 0;
        virtual int CreateOptGoalFromTemplate(array<System::Byte>^ byteOptGoalCollection) = 0;
        virtual int RemoveOptGoal(System::String^ optGoalUid) = 0;
        virtual int UpdateOptGoal(array<System::Byte>^ protoOpt) = 0;
#pragma endregion

#pragma region Optimizer
        virtual int OptStart(array<System::Byte>^ protoOpt, DelegateCarrier<SingleByteArrayDelegate^>^ callback) = 0;
        virtual int OptAbort() = 0;
#pragma endregion

#pragma region Voi
        virtual int AutoContouring(array<System::Byte>^ protoAutoContouring, 
            DelegateCarrier<CommonDelegate^>^ callback, 
            [Out] array<System::Byte>^% vois) = 0;
        virtual int DeleteVoi(System::String^ voiUid) = 0 ;
        virtual int VoiUpdateAttribute(array<System::Byte>^ voi) = 0;//update voi exclude contours
        virtual int UpdateVoiInGridWithPlan(System::String^ planUid, [Out] array<System::Byte>^% vois) = 0;
        virtual float GetVoiInGrid( System::String^ planUid, System::String^ voiUid) = 0;
        virtual bool CheckPointInGrid(System::String^ planUid, float ptx, float pty, float ptz) = 0;
        virtual int VoiInfoRetrieve(System::String^ seriesUid, [Out] array<System::Byte>^% vois) = 0;
        virtual int VoiCreateAdvanced(array<System::Byte>^ voiCreateAdvanced, [Out] array<System::Byte>^ %voiReturned) = 0;
        virtual int VoiRemovePreviewWindow() = 0;
        virtual int VoiJumpTo(System::String^ strVoiUid) = 0;
        virtual int VoiPreviewPageTurn(System::String^ windowUid, int windowType, int iPageTurn) = 0;
        virtual int CreateVoi(array<System::Byte>^ byteVoi, [Out]array<System::Byte>^ %backByteVoi) = 0;
        virtual int CreateBolus(array<System::Byte>^ byteBolus,  [Out]array<System::Byte>^ %backbyteBolus) = 0 ;
        virtual array<System::String^>^ GetDensityOverlappedVoiList(System::String^ stringSeriesUid) = 0;
        virtual int VoiMansetDensity(System::String^ stringVoiUid, float fOverrideMassDensity, float fOverrideReferenceEDensity, int nOverrideSequence) = 0;
        virtual int VoiHotSpots(System::String^ stringPlanUid, System::String^ stringVoiUid, float fDoseLimit, int nHotFlag) = 0;
        virtual int VoiDoseVolumeValue(System::String^ stringPlanUid, array<System::String^>^ arrayVoiUid, array<int>^ arrayVoiVolume, [Out]array<float>^% arratVoiDose) = 0;
        virtual int ApproveVois(array<System::Byte>^ approveVois) = 0;
        virtual int UnApproveVois(array<System::String^>^ voiUids) = 0;
        virtual int GetDefaultDoseGridSize(System::String^ seriesUID, [Out] array<System::Byte>^% gridSizeByte) = 0;
#pragma endregion

        virtual void RefreshAllViews() = 0;

        virtual bool MapScreen2Patient(int windowType, System::String^ windowUid,
            double x2D, double y2D, float^% x3D, float^% y3D, float^% z3D) = 0;

        virtual bool MapPatient2Screen(int windowType, System::String^ windowUid,
            double x3D, double y3D, double z3D, float^% x2D, float^% y2D) = 0;

        virtual bool GetCrosshairProjectionPosition(int windowType, System::String^ windowUid, 
            float^% x2D, float^% y2D) = 0;
        virtual bool GetCrossXYPoints(int windowType, System::String^ windowUid,
            double^% x1, double^% y1, double^% x2, double^% y2, double^% x3, double^% y3, double^% x4, double^% y4) = 0;
        virtual double GetDistancePointToPlane(int windowType, System::String^ windowUid, 
            float x3D, float y3D, float z3D) = 0;
        virtual bool MapMilimeter2Pixel(int windowType, System::String^ windowUid,
            double lengthInmm, double^% pixelinXNP, double^% pixelinYNP) = 0;
        virtual int GetPatientToViewMatCore(int windowType, System::String^ windowUid, [Out] double% m11, [Out] double% m12, [Out] double% m13, [Out] double% m14, [Out] double% m21, [Out] double% m22, [Out] double% m23, [Out] double% m24, [Out] double% m31, [Out] double% m32, [Out] double% m33, [Out] double% m34, [Out] double% m41, [Out] double% m42, [Out] double% m43, [Out] double% m44) = 0;
        virtual bool GetMprViewDirection(int windowType, System::String^ windowUid, 
            double^% viewX3D, double^% viewY3D,  double^% viewZ3D,  double^% upX3D, double^% upY3D, double^% upZ3D) = 0;

#pragma region Fusion Methods
        virtual bool SetFixedImage(System::String^ seriesUid, [Out] array<System::Byte>^% feedbackByte) = 0;
        virtual bool SetFloatImage(System::String^ seriesUid, [Out] array<System::Byte>^% feedbackByte) = 0;
        virtual bool StartRegistration(System::String^ fixedSeriesUid, System::String^ floatSeriesUid, 
            int algorithm, 
            array<System::Byte>^ protoLandmarkPoints,
            DelegateCarrier<CommonDelegate^>^ callback, [Out] array<System::Byte>^% feedbackByte) = 0;
        virtual void ChangeFusionType(System::String^ fixedSeriesUid, 
            System::String^ floatSeriesUid, int fusionType) = 0;
        virtual void ChangeFusionFactor(System::String^ fixedSeriesUid, 
            System::String^ floatSeriesUid, double fusionFactor) = 0;
        virtual void ChangeFusionType(int fusionType) = 0;
        virtual void ChangeFusionFactor(double fusionFactor) = 0;
        virtual void ChangeRegistrationAlgorithm(int algorithm, 
            [Out] array<System::Byte>^% feedbackByte) = 0;
        virtual bool DisplayFusionRegion(bool editFixed, bool visible) = 0;
        virtual bool EditFusionRegion(int wndType, System::String^ wndUid, 
            double x, double y, int actionType) = 0;
        virtual void SetPseudoColor(System::String^ imageUid, System::String^ pseudoColorName) = 0;
        virtual bool AddLandmarkPoint(int wndType, System::String^ wndUid, 
            double x, double y, int index,double^% patX, double^% patY,double^% patZ, [Out] array<System::Byte>^% feedbackByte) = 0;
        virtual int ApproveRegistrationResult(array<System::Byte>^ byteApproveRecord, System::String^% fixedUid, System::String^% floatUid, System::String^% description, int registrationMethod, [Out] System::String^% uid) = 0;
        virtual int RemoveRegistrationRelationship(System::String^ fixedUid, 
            System::String^ floatUid) = 0;
        virtual bool ShowFusionImage(System::String^ fixedSeriesUid, 
            System::String^ floatSeriesUid, bool displayOrHide) = 0;
        //manual registration translate
        virtual int ManualRegistrationTranslate(int wndType, System::String^ wndUid, 
            float preX, float preY, float curX, float curY) = 0;
        //manual registration rotate
        virtual int ManualRegistrationRotate(int wndType, System::String^ wndUid, 
            float preX, float preY, float curX, float curY) = 0;
        virtual void EditChess(int wndType, System::String^ wndUid, 
            double x, double y, int mouseActionType) = 0;
        //virtual void EditLandmark(int wndType, System::String^ wndUid, 
        //    double x, double y, int mouseActionType, bool isToDelete, 
        //    [Out] array<System::Byte>^% feedbackByte) = 0;
#pragma endregion

        virtual int AdminStartup([Out]array<System::Byte>^ %adminTreeData) = 0;
        virtual int UpdatePhantom(System::String^ patientUid, System::String^ phantomSeriesUid, System::String^ phantomName, System::String^ phantomDescription) = 0;
        virtual int SetAdminPhantomImageDataSource(System::String^ phantomSeriesUid) = 0;
        virtual int CreatePhantom(array<System::Byte>^ byteCreatePhantom, [Out] array<System::Byte>^% protoOutput) = 0;
        virtual int CreateInternalPatient(array<System::Byte>^ byteCreatePatient, [Out] array<System::Byte>^% protoOutput) = 0;
        virtual int DeleteInternalPatient(System::String^ patientUid, System::String^ seriesUid) = 0;
       

#pragma region Edit VOI
        virtual int CombineNewContourToRoi(
            array<System::Byte>^ editVoiInfo, 
            DelegateCarrier<SingleByteArrayDelegate^>^ callback) = 0;
        virtual int SmoothVoi(System::String^ voiUid, DelegateCarrier<SingleByteArrayDelegate^>^ callback) = 0;
        virtual int RemoveSmallContours(System::String^ voiUid, double area, DelegateCarrier<SingleByteArrayDelegate^>^ callback) = 0;
        virtual int MoveVoi(array<System::Byte>^ moveVoiByte, [Out]array<System::Byte>^ %backMoveVoiByte, [Out]array<System::Byte>^ %backVoiByte) = 0;
        virtual int PanCurrentSliceContour(array<System::Byte>^ moveVoiByte, DelegateCarrier<SingleByteArrayDelegate^>^ callback) = 0;

        virtual bool AddTemplate(array<System::Byte>^ templateByte,int dataType) = 0;
        virtual bool UpdateTemplate(array<System::Byte>^ templateByte,int dataType) = 0;
        virtual bool RemoveTemplate(System::String^ uid) = 0;

        virtual int CutContours(array<System::Byte>^ cutContoursByte, [Out]array<System::Byte>^ %backVoiByte) = 0;
        virtual int RetrieveVoiDensity(System::String^ voiUID,[Out]array<System::Byte>^ %backVoiByte) = 0;
        virtual int SelectContourSlices(array<System::Byte>^ selectContourSlicesByte) = 0;
        virtual int SwitchTable(System::String^ seriesUid, System::String^ tableUid) = 0;
        virtual int UpdateTable(array<System::Byte>^ protoSeriesTable) = 0;
		virtual int SaveTableAsROI(System::String^ seriesUid, System::String^ tableUid, IList<float>^ voicolor, System::String^ voiName, [Out]array<System::Byte>^ % protoVOI) = 0;
        virtual int SaveVOIAsTable(System::String^ voiUid) = 0;
        virtual bool IsInVOI(int wndType, System::String^ wndUid, System::String^ voiUid, double x, double y) = 0;
        virtual bool IsInVolume(int wndType, System::String^ wndUid, double x, double y) = 0;
#pragma endregion

#pragma region ct2density
        virtual int CreateCT2DensityTable(array<System::Byte>^ c2dTable) = 0;
        virtual int RemoveCT2DensityTable(System::String^ c2dUid) = 0;
        virtual int UpdateCT2DensityTable(array<System::Byte>^ c2dTable, [Out] array<System::Byte>^% protoOutput) = 0;
        virtual bool ValidateCT2DensityOfSeries(System::String^ patientUid) = 0;
        virtual int ChangeCt2Density(System::String^ seriesUid, System::String^ ct2DensityUid) = 0;
#pragma endregion 

        virtual int AddQANormalGroup(System::String^ qaPlanUID, array<System::Byte>^ ngConfigByte, [Out]array<System::Byte>^ %backNormalizationGroupByte) = 0; 
        virtual int ChangeEvaluationOrder(int order,array<System::Byte>^ planByte) = 0;
        virtual int GetCurrentAxisSlice(int wndType, System::String^ wndUid, int^% currentSlice, int^%startSliceNumber, int^% totalSlice) = 0;

#pragma  region image related
        virtual int GenerateSkinMesh(System::String^ imageUID, [Out]array<System::Byte>^ %backMeshByte) = 0;
        virtual int GetCornerInfoOfMPR(int wndType, System::String^ wndUid, double normPosX, double normPosY, 
            int suvUnitType, [Out]array<System::Byte>^% cornerInfo) = 0;
        virtual int GetCornerInfoOfBEV(System::String^ imageUID, int wndType, System::String^ wndUid,
            [Out]array<System::Byte>^% cornerInfo) = 0;
        virtual int LoadIgrtImage(System::String^ planUid, array<System::Byte>^ protoRefImageBytes) = 0;

        virtual int GetDynamicCornerInfoOfMPR(
            int windowType, 
            System::String^ windowUid, 
            double normPosX, 
            double normPosY,
            int suvUnitType, 
            double^% density, 
            double^% value, 
            double^% doseValue,
            double^% positionx, 
            double^% positiony, 
            double^% positionz, 
            bool^% isValueValid) = 0;

        virtual int GetWindowCenterWidthOfMPR(int windowType, 
            System::String^ windowUid,
            int suvUnitType,
            double^% windowCenter, //Top if PT
            double^% windowWidth,  //Bottom if PT
            bool^% isValueValid) = 0;

        virtual int GetSliceInfoOfMPR(int windowType, 
            System::String^ windowUid,
            int^% sliceIndex,
			int^% startSliceNumber,
            int^% totalSlices,
            double^% sliceThickness,
            float^% sliceLocation) = 0;
#pragma  endregion

#pragma region Template
        virtual int LoadTemplate([Out]IList<array<Byte>^>^ %protoTemplateList) = 0;
        virtual int CreateDoseLineTemplate(array<Byte>^ byteDoseLineTemplate) = 0;
        virtual int DeleteDoseLineTemplate(String^ doseLineTemplateUID) = 0;
        virtual int CreateVoiTemplate(array<Byte>^ byteVoiTemplate) = 0;
        virtual int CreateBeamTemplate(array<Byte>^ byteBeamTemplate) = 0;
        virtual int DeleteBeamTemplate(String^ beamTemplateUID) = 0;
		virtual int CreateOptimizerTemplate(array<Byte>^ byteOptimizerTemplate) = 0;
#pragma endregion

        virtual int CompareDosegrid(array<System::Byte>^ primaryPlanArray, 
            array<System::Byte>^ secondaryPlanArray) = 0;

        virtual int ShowDoseGrid(array<System::Byte>^ doseGridByte) = 0;

        virtual int ChangeDoseGridPara(array<System::Byte>^ doseGridByte) = 0;

        virtual int UpdateDoseGridBoundary(array<System::Byte>^ doseGridBoundaryByte, [Out]array<System::Byte>^ %backPlanByte, [Out]array<System::Byte>^ %backVoiByte) = 0;

        virtual int ClearTempQAPoi() = 0;

        virtual int RefreshDVH(System::String^ planUid) = 0;

        virtual bool GetShadedSurfaceViewDirection(int windowType, System::String^ windowUid, 
            double^% viewX3D, double^% viewY3D,  double^% viewZ3D,  double^% upX3D, double^% upY3D, double^% upZ3D) = 0;

        virtual int ShadedRotate(array<System::Byte>^ shadedRotateByte) = 0;

        virtual int ShadedZoom(array<System::Byte>^ shadedZoomByte) = 0;

        virtual void ShadedPan(array<System::Byte>^ shadedPanByte) = 0;

        virtual int UpdateDrrSetting(array<System::Byte>^ drrSettingByte) = 0;

#pragma region Doseline
        virtual int UpdateDoseLineParams(array<Byte>^ protoDoseLineParams) = 0;
		virtual int ChangeDoselineDisplayMode(System::String^ planUID, bool isLine, bool isColorwash,
			float transparency, float transparency3D) = 0;

#pragma endregion 

        virtual int UpdateLightSetting(System::String^ sSeriesUid, array<System::Byte>^ lightSettingByte) = 0;

        virtual int ExportSliceDose(System::String^ planeType, System::String^ filePath, array<System::Byte>^ doseBytes) = 0;

        virtual int AddVoiByTemplate(String^ voiTemplateUid, String^ seriesUid, bool isAutoSegment, [Out]array<Byte>^ %outputVoiList) = 0;

        virtual void ChangeBevEditTarget(int target) = 0;

        virtual int CreateBeamsFromTemplate(array<System::Byte>^ protoInput, [Out] array<System::Byte>^% protoOutput) = 0;

        virtual void SetRulerVisibility(System::String^ strSeriesUid, bool isShow) = 0;

        virtual int DeleteQAPlan(System::String^ planUid, System::String^ seriesUid, System::String^ courseUid) = 0;

        virtual int PreviewBeamTemplate(String^ beamTemplateUid, String^ normGroupUid, String^ isocenterUid) = 0;

        virtual void Redo() = 0;
        virtual void Undo() = 0;

        virtual void PlaySeries(System::String^ stringSeriesUid, System::String^ stringPlanUid, int playState, int playSpeed, int playMode,
                                DelegateCarrier<SingleByteArrayDelegate^>^ callback,bool isCycli) = 0;
        virtual void SetMainPhase(System::String^ groupUid, System::String^ seriesUid) = 0;
        virtual void SendSeriesGroupInfo(System::String^ groupUid, array<System::String^>^ arraySeriesUid) = 0;
        virtual bool SwitchThumbnailPage(int deltaPage, System::String^ seriesUid, [Out] array<System::Byte>^% outSliceBuffer) = 0;

#pragma region machineSetting

        virtual int LoadMachineSetting() = 0;
        virtual int LoadMachineSetting(System::String^ machineUid, [Out]array<System::Byte>^ %machineSettingInfoBytes) = 0;
        virtual int AddTray(System::String^ machineUID,array<System::Byte>^trayInfoBytes,[Out]System::String^% trayUID) = 0;
		virtual int LoadMachineTemplate(System::String^ templateFilePath, [Out]IList<System::String^>^ % machineUids) = 0;
        virtual int AddCurveInfo(array<System::Byte>^curveInfoBytes,[Out]System::String^% curveUID) = 0;
        virtual int AddApplicatorInfo(array<System::Byte>^machineConfigBytes,[Out]System::String^% applicatorUID) = 0;
        virtual int AddCommissionInfo(array<System::Byte>^machineConfigBytes,[Out]System::String^% commissionUID,[Out]System::String^% measureUID,[Out]System::String^% phoCommissionUID,[Out]array<System::Byte>^%convolutionBytes) = 0;
        virtual int AddMachineSetting(array<System::Byte>^machineConfigBytes,[Out]System::String^% machineUID) = 0;
        virtual int AddWedgeInfo(array<System::Byte>^machineConfigBytes,[Out]System::String^% wedgeUID) = 0;
        virtual int ClearMeasureData(array<System::Byte>^clearCurveInfoBytes) = 0;
        virtual int CommonOperationCurveFunction(array<System::Byte>^curveListInfoBytes ,[Out]array<System::Byte>^ %curveListBytes) = 0;
        virtual int ComputerMeasureDataInfo(bool isCalcAbsOrRel, array<System::Byte>^curveListInfoBytes, [Out]array<System::Byte>^ %curveListBytes) = 0;
        virtual int RescaleMeasureDataInfo(bool isAbsMode, System::String^ commissionUID) =0;
        virtual int CreateIntersectionMatrix(System::String^ commissionUID) = 0;
        virtual int CurveErrorAnalyseFunction(array<System::Byte>^curveListInfoBytes ,[Out]array<System::Byte>^ %curveListBytes) = 0;
        virtual int DeleteApplicatorInfo(System::String^ applicatorUID,System::String^ objectType) = 0;
        virtual int DeleteCommissionInfo(System::String^ commissionUID,System::String^ objectType) = 0;
        virtual int DeleteMachineSettingInfo(System::String^ machineUID,System::String^ objectType) = 0;
		virtual int SaveMachineAsTemplate(System::String^ machineUid, System::String^ machineTemplateName, System::String^ tempFolderPath) = 0;
		virtual int CopyMachineSettingInfo(System::String^ machineUID, System::String^ machineName, System::String^ deviceId, System::String^ manufactory, [Out]System::String^% copyMachineUid) = 0;
        virtual int DeleteWedgeInfo(System::String^ wedgeUID,System::String^ objectType) = 0;
        virtual int DeleteTrayInfo(System::String^ trayUID,System::String^ objectType) = 0;
        virtual int DeleteSelectedCurves(array<System::Byte>^delecteCurveInfo) = 0;
        virtual int EnergyFlunenceCorrection(array<System::Byte>^fluenceBytes) = 0;
        virtual int DoFluenceCorrection(System::String^ commissionUID,System::String^ measureUID,float depth) = 0;
        virtual int GenerateFinalFile(System::String^ commissionUID) = 0;
        virtual int AutoCalPBSCorrection(System::String^ commissionUID) = 0;
        virtual int ImportDataFromFile(IList<System::String^>^fileList, array<System::Byte>^protoBytes,[Out] int %importedNumber) = 0;
        virtual int ImportDataFromGSTTFile(System::String^ filePath,array<System::Byte>^importGSTTFromDataBytes,[Out]array<System::Byte>^%importFromDataBytes) = 0;
        virtual int GetConvolutionDataInfo(array<System::Byte>^protoBytes,System::String^ commissionUID,[Out]array<System::Byte>^%convolutionBytes) = 0;
        virtual int GetImportFileDataInfo(System::String^ commissionUID,[Out]array<System::Byte>^%importFileInfoBytes,float depth,bool isNormalDepth) = 0;


        virtual int GetMachineSettingInfo([Out]array<System::Byte>^ %machineSettingInfoBytes) = 0;
        virtual int GetMonteCarloDataInfo(array<System::Byte>^protoBytes,System::String^commissionUID,[Out]array<System::Byte>^ %vCertifiedMonteCarloBytes) = 0;
        virtual int GetScpDataInfo(System::String^commissionUID,[Out]array<System::Byte>^ %scpInfoBytes) = 0;
        virtual int GetPencilBeamDataInfo(array<System::Byte>^protoBytes,System::String^commissionUID,[Out]array<System::Byte>^ %vCertifiedPencilBeamBytes) = 0;
        virtual int GetVirtualWedgeFactorDataInfo(array<System::Byte>^protoBytes,System::String^commissionUID,[Out]array<System::Byte>^ %gsttInfoBytes) = 0;
        virtual int GetWedgeFactorDataInfo(System::String^commissionUID,[Out]array<System::Byte>^%wedgeFactorBytes) = 0;
        virtual int InitilizePhoEngergyWeight(System::String^commissionUID,float minEnergy ,float maxEnergy,[Out]array<System::Byte>^ %vEnergyWeightBytes) = 0;
        virtual int NormMeasureDataInfo(array<System::Byte>^importFromFileBytes,[Out]array<System::Byte>^ %importFileBytes)  = 0;
        virtual int OptimizeFunction(bool bCalcAbsOrRel, array<System::Byte>^protoInfoBytes, [Out]array<System::Byte>^ %vEnergyWeightBytes) = 0;
        virtual int AddWedgeFieldInfo(IList<System::String^>^ wedgeUidList, System::String^ commissionUid, int algType) = 0;
        virtual int LoadPhysicalWedgeField(IList<System::String^>^ physicalWedgeNameList, System::String^ commissionUid, int algType, [Out]array<System::Byte>^ %PhysicalWedgeFieldListByte) = 0;
        virtual int LoadVirtualWedgeInfo(List<System::String^>^ virtualWedgeNameList, System::String^ commissionUid, int algType, [Out]array<System::Byte>^ %VirtualWedgeFieldListByte) = 0;
        virtual int ScpInfoFunction(System::String^commissionUID,array<System::Byte>^scpInfoDataBytes) = 0;
        virtual int UpdateApplicatorInfo(array<System::Byte>^machineConfigBytes) = 0;
        virtual int UpdateCommissionInfo(array<System::Byte>^machineConfigBytes) = 0;
        virtual int UpdateConvolution(array<System::Byte>^convolutionBytes) = 0;
        virtual int UpdateCurveProfile(array<System::Byte>^importFileBytes) = 0;
        virtual int UpdateLeafInfo(array<System::Byte>^machineConfigBytes) = 0;
        virtual int UpdateMachineSetting(array<System::Byte>^machineConfigBytes) = 0;
        virtual int UpdateMonteCarlo(array<System::Byte>^monteCarloBytes) = 0;
        virtual int UpdatePencilBeam(array<System::Byte>^pencilBeamBytes) = 0;
        //PMC part
        virtual bool UpdatePhotonMonteCarlo(array<System::Byte>^photonMonteCarloBytes) = 0;
        virtual int PrepareMonoPDD(System::String^commissionUID,DelegateCarrier<CommonDelegate^>^ callback) = 0;
        virtual int ClearMCPrePareFileFolder(System::String^machineUid,System::String^commissionUID) = 0;
        virtual int GetCompletePrepareFiles(System::String^machineUid,System::String^commissionUID, [Out]int iFileNum) = 0;
        virtual bool UpdatePDDThreeTables(System::String^commissionUID,array<System::Byte>^curveListInfoBytes,[Out]array<System::Byte>^ %primary, [Out]array<System::Byte>^ %secondary, [Out]array<System::Byte>^ %electron) = 0;
        virtual bool GetPhotoMonteCarloInfo(System::String^commissionUID,[Out]array<System::Byte>^ %photonMonteCarloByte) = 0;







        virtual int UpdateVirtualWedgeFieldInfo(array<System::Byte>^virtualWedgeFieldBytes) = 0;
        virtual int UpdateWedgeFactorInfo(array<System::Byte>^wedgeFactorBytes,[Out]System::String^% wedgeFactorUID) = 0;
        virtual int UpdateWedgeInfo(array<System::Byte>^machineConfigBytes) = 0;
        virtual int MonteCarloAlg(System::String^commissionUID) = 0;
        virtual int AddVirtualWedge(System::String^commissionUID, List<System::String^>^ accUIDList, int algType) = 0;
        virtual int DeleteVirtualWedge(System::String^commissionUID, List<System::String^>^ accUIDList, int algType) = 0;
        virtual int DeleteWedgeField(System::String^commissionUID, List<System::String^>^ accUIDList, int algType) = 0;
        virtual bool GetCorrectionFactor(System::String^commissionUID, int algType, System::String^selectedAcessoryUID, 
            [Out]array<System::Byte>^ %corrFactor) = 0;
        virtual int ApproveUserInfo(array<System::Byte>^ byteApproveRecord) = 0;
        virtual int UnApproveUserInfo(System::String^ objectuid) = 0;

#pragma endregion

#pragma region Gallery
        virtual bool LoadPatientForGalleryAsync(System::String^ patientUid) = 0;
        virtual array<System::Byte>^ GetProtoPatientTreeDataForGallery(System::String^ patientUid) = 0;
        virtual bool LoadSeriesForGallery(System::String^ seriesUid, [Out]System::String^% crosshairMode) = 0;
        virtual bool DeleteCurrentSlice(System::String^ seriesUid) = 0;
        virtual void LeavingModule() = 0;
        virtual void ClearImageData(System::String^ seriesUid) = 0;
        virtual bool GetDeletedImageUIDs(System::String^ seriesUid, [Out]System::String^% deletedImageUids) = 0;
        virtual bool BackupOriginalImage3DEntity(System::String^ seriesUid) = 0;
        virtual bool RestoreOriginalImage3DEntity(System::String^ seriesUid) = 0;

        virtual void GalleryMPRPageTurn(int windowType, System::String^ windowUid, int iPageTurn) = 0;
        virtual void GalleryMPRPan(int windowType, System::String^ windowUid, double startx, double starty, 
            double stopx, double stopy) = 0;
        virtual void GalleryMPRZoom(int windowType, System::String^ windowUid, float scaleFactor) = 0;
        virtual void GalleryMPRZoom(int windowType, System::String^ windowUid, double startx, double starty,
            double stopx, double stopy) = 0;
        virtual void GalleryMPRWindowing(int windowType, System::String^ windowUid, float deltaWidth,
            float deltaCenter, bool needUpdateAll) = 0;
        virtual void GalleryShadedSurfaceRotate(int windowType, array<System::Byte>^ protoBytes) = 0;

        virtual bool GetCrosshairProjectionPositionForGallery(int windowType, System::String^ windowUid, 
            float^% x2D, float^% y2D) = 0;
        virtual bool GetCrossXYPointsForGallery(int windowType, System::String^ windowUid,
            double^% x1, double^% y1, double^% x2, double^% y2, double^% x3, double^% y3, double^% x4, double^% y4) = 0;
        virtual int UpdateCrosshairActionForGallery(double startx, double starty, double endx, double endy,
            double regionWidth, double regionHeight, int windowType, System::String^ windowUid, int crossmode, int hitmode,double rotateX,double rotateY) = 0;
        virtual bool SetCrosshairHittedModeForGallery(int wndType, System::String^ wndUid, int mode) = 0;
        virtual int GetCornerInfoOfMPRForGallery(System::String^ imageUID, int wndType, System::String^ wndUid,
            /*System::String^ planUid,*/ double normPosX, double normPosY, bool bUpdateAll, int suvUnitType, 
            [Out]array<System::Byte>^% cornerInfo) = 0;
        virtual bool ResetMPRObjectForGallery(int windowType, System::String^ windowUid) = 0;
        virtual bool GetMprViewDirectionForGallery(int windowType, System::String^ windowUid, 
            double^% viewX3D, double^% viewY3D,  double^% viewZ3D,  double^% upX3D, double^% upY3D, double^% upZ3D) = 0;
        virtual bool GetShadedSurfaceViewDirectionForGallery(int windowType, System::String^ windowUid, 
            double^% viewX3D, double^% viewY3D,  double^% viewZ3D,  double^% upX3D, double^% upY3D, double^% upZ3D) = 0;
        virtual void PlaySeriesForGallery(System::String^ stringSeriesUid, int playState, int playSpeed, int playMode,
            DelegateCarrier<SingleByteArrayDelegate^>^ callback) = 0;

#pragma endregion

#pragma region PlanReport
        virtual int RequestSeriesForPlanReport(System::String^ planUID, System::String^ courseUID, IList<int>^ requestSliceNumList, int resolutionX, int resolutionY, DelegateCarrier<WindowUpdatedDelegate^>^ callback) = 0;
        virtual int GetDVHDataForPlanReport(System::String^ planUID, System::String^ courseUID, [Out] array<System::Byte>^% dvhs) = 0;
#pragma endregion

        virtual void ClearAllPatientLocks() = 0;

		virtual int CopyContour(System::String^ seriesUID, System::String^ voiUID) = 0;
        virtual int InterpolateContour(System::String^ voiUID,[Out]array<System::Byte>^ %voiBytes) = 0;
		virtual int CutContour(System::String^ seriesUID, System::String^ voiUID, [Out]array<System::Byte>^ %backVoiByte) = 0;
		virtual int PasteContour(System::String^ seriesUID, System::String^ voiUID, int editFunction, [Out]array<System::Byte>^ %backVoiByte) = 0;

		virtual int CopyPhantomToQA(System::String^ phantomUid, [Out]array<System::Byte>^ %qaPatientTreeData) = 0;

#pragma region [New Load Interface]
        virtual int LoadPatient(String^ patientUID, [Out]array<Byte>^ %protoByte) = 0;
        virtual int LoadSeries(String^ seriesUID, [Out]array<Byte>^ %protoByte) = 0;
        virtual int LoadPlan(String^ planUID, [Out]array<Byte>^ %protoByte) = 0;
#pragma endregion
    };
}

#endif
