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
 
#ifndef TPS_RC_RENDERPROXY_H_
#define TPS_RC_RENDERPROXY_H_

#include "tps_rc_defs.h"
#include <list>

#include "McsfMedViewer3DArithmetic/point2d.h"
#include "RtTpsFramework/tps_fw_renderproxybase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
        class Matrix4x4;
    }
    class Vector3f;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps
namespace plunc {
    struct LEAF_POSITION_STC;
}
class TpsRenderProxyBase;
class TpsRenderControllerBase;
class MPRGraphicObject;
class ROIEntity;
class RtDoseline;
class RtDosegrid;
class TpsVOIEditingCache;
struct CertifiedMlcEditInfo;
class TpsImage2DEntity;

static const std::string sTpsConfigPath = "RtTpsConfigPath";
/// \class TpsRenderProxy
/// 
/// \brief this class is the render module of APP controller
///        it is responsible for the initialization of LayoutManager & DisplayDelegate
///        it is also in charge of the management of render controllers & LayoutManager
///        overall, if wanna render something, get the TpsRenderManager instance
class TPS_RC_DECLSPEC TpsRenderProxy : public TpsRenderProxyBase 
{
    typedef std::vector<Mcsf::MedViewer3D::Point2D> Contour2DPointList;
public:
    //the constructor and destructor
    TpsRenderProxy();
    virtual ~TpsRenderProxy();

    //////////////////////////////////////////////////////////////////////////
    // \brief  initialize the render proxy, setup data source group
    // \return bool  true when succeeded, false otherwise
    virtual bool Initialize(ITpsDataRepository* dataRepository);

    TPS_ERROR_CODES GetPatient2ViewMat(const WINDOW_TYPE wndType, const std::string& wndUid, Mcsf::MedViewer3D::Matrix4x4& mPatient2View);
    bool RefreshDose(const std::string& planUID);
    bool RefreshDoseLine(std::string planUid);

    bool GetCurrentMprSlice(WINDOW_TYPE window, const std::string& wndUid, 
        int& slice, int& startSliceNumber, int& totalSlice);

    bool SetCrosshairHittedMode(int wndType, const std::string& wndUid, int mode);

    virtual bool CheckDataSourceGroup(WINDOW_TYPE window, DATA_SOURCE_GROUP_ID& id);

	// true:supported FBO false: unsupported FBO
	bool CheckFBO();

    bool CreateUpdateNormalGroup(const std::string& planUid, const std::string& normGroupUid);

	//easy plan
	bool EasyPlanChangeSelectedBeam(const std::string &planUid,const std::string &normGroupUid,const std::string &beamUid);

    bool AddBeam(const std::string& planUID, const std::string& beamUID);

    bool UpdateBeam(const std::string& ngUid, const std::string& beamUID);

    bool DeleteBeam(const std::string& planUid, const std::string &beamUid);

	void EasyPlanModifyBeamInfo(const std::string &beamUID);

    bool AddVOI(const std::string& voiUID, const std::string& imageUID);

    bool PreviewVOI(ROIEntity* pVOI, std::string strImageUid);

    void DeleteVoi(const std::string& imageUid, const std::string& voiUid);

    //update voi graphic object
    void UpdateVOI(const std::string &imageUid, const std::string &voiUid);
    //update voi attributes including: color, visibility, interpolation flags, 
    //if operation does not affect the shape of the voi, use this interface.
    void UpdateVOIAttribute(const std::string &imageUid, const std::string &voiUid);

    //the point is in patient c.s.
    void JumpTo(const std::string& seriesUid, float x, float y, float z);

    void VoiPreviewJumpTo(const std::string& seriesUid, float x, float y, float z);

    bool EditDoseGridBoundary(const std::string& planUID, Mcsf::Point3f startPoint, Mcsf::Point3f endPoint, Mcsf::Point3f spacing);

    bool ShowDoseGridBoundary(const std::string& planUID, bool isShowBoundary);

    bool AddNormalGroup(const std::string& planUid, const std::string& normGroupUid);

    bool UpdateNormalGroup(const std::string& planUid, const std::string& normGroupUid);

    bool DeleteNormalGroup(const std::string& planUid, const std::string& normGroupUid);

    bool AddPoi(const std::string &strPlanUid, const std::string& strPoiUid);

    bool DeletePoi(const std::string &strPlanUid, const std::string& strPoiUid);

    bool DeleteMaxPoint(const std::string &strPlanUid, const std::string& strPoiUid);

    void UpdatePOI(const std::string &strPlanUid, const std::string &strPoiUid );

    void ResetBeamViewCameras(const std::string& planUID);

    bool EasyPlanSetInverse(int wndType, const std::string& wndUid, const std::string& beamUid);

    //bool ResetMlcShape(const std::string& beamUID);

    void UpdateSetupCrossHair(const std::string& wndUid, WINDOW_TYPE wndType,
        const std::string &imageuid, const Mcsf::Point2f &mousePosition, bool isVisible);

	// get image uid from data source with window type
	void EasyPlanUpdateSetupCrossHair(const std::string& wndUid, WINDOW_TYPE wndType,
		const Mcsf::Point2f &mousePosition, bool isVisible);

    bool SetEasyPlanPixelSpace();

    bool SetEasyPlanPixelSpace(float spacingX, float spacingY);

    void UpdateMPRCornerInformation(WINDOW_TYPE windowType, 
        const std::string& windowUid, double scrX, double scrY);

    bool ResetMPRObject(WINDOW_TYPE windowType, const std::string& windowUid);

	void UpdateCrosshair(WINDOW_TYPE windowType, const std::string& windowUid,
        double xStart, double yStart, double xEnd, double yEnd, int crossMode, int hitMode, double rotateX, double rotateY);

    void RotateShadedSurface(const std::string seriesUID, const Mcsf::Point2f& prePt, 
        const Mcsf::Point2f& curPt);

    void ShadedSurfaceZoom(const std::string seriesUID, float fZoomFactor);

    void Pan3DWindow(WINDOW_TYPE wndType, const std::string& wndUid, 
        float startX, float startY, float stopX, float stopY);

    bool GetShadedSurfaceViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid, 
        double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);

    void UpdateLightSetting(const std::string& strImageUid, int mprSection, float mprTransparency3D, float rx, float ry, float rz, 
        float scale, float specf, float ambf, float difff, float shininess, 
        float opacity, float lightx, float lighty, float lightz,int smoothness, 
        bool useCameraLight, int displayMode, bool displayVertexCube,
        int startStripIndex, int endStripIndex);

    bool GetCrosshairProjectionPosition(WINDOW_TYPE windowType, 
        const std::string& windowUid, float& x2D, float& y2D);

    bool GetCrossXYPoints(WINDOW_TYPE windowType,
        const std::string& windowUid, double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4);

    bool GetMprViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid, 
        double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);

    double DistancePointToPlane(WINDOW_TYPE windowType, const std::string& windowUid,
    float x3D, float y3D, float z3D);

    ////////////////////////Fusion Part///////////////////////////////////////
    // \brief start register
    bool StartRegister();
    void RefreshRegistrationResult();
    void RefreshApprovedRegistrationResult(const std::string& fixedUid, const std::string& floatUid);
    void RefreshFusionFactor(const std::string& fixedUid, const std::string& floatUid, double factor);
    void RefreshFusionType(const std::string& fixedUid, const std::string& floatUid, int type);
    void RefreshFusionFactor(double factor);
    void RefreshFusionType(int type);
    void EditChessBoard(WINDOW_TYPE wndType, const std::string& wndUid, 
        double x, double y, int actionType);
//    void UpdateLandmark();
    /// refresh type 0: all regions, 1 only fixed, 2 only float
    void UpdateFusionRegion(FusionRegionUpdateType refreshType = AllRegion);
    void SetPseudoColor(const std::string& imageUID, const std::string& pseudoColorName);
    ///////////////////////////////////////////////////////////////////////////
    
    bool MprPageTurn(WINDOW_TYPE wndType, const std::string& wndUid, int iPageturn);
    bool MprReset(WINDOW_TYPE wndType, const std::string& wndUid, int acType);
    bool MprZoom(WINDOW_TYPE wndType, const std::string& wndUid, float scaleFactor);
	bool MprZoom(WINDOW_TYPE wndType, const std::string& wndUid, 
		float startX, float startY, float stopX, float stopY);
    bool MprPan(WINDOW_TYPE wndType, const std::string& wndUid, 
        float startX, float startY, float stopX, float stopY);
    bool MprWindowing(WINDOW_TYPE wndType, const std::string& wndUid, 
        float deltaWidth, float deltaCenter, bool updateAll);
    //bool EasyPlanDrawMlcSmartAperture(const CertifiedMlcEditInfo& certMlc);
    bool EasyPlanUpdateBlock(const CertifiedMlcEditInfo &certNudgeInfo);
    //bool EasyPlanShowNudgeCircle(double x, double y, double radius, const std::string &beamUID);
    //bool EasyPlanMlcNudge(const CertifiedMlcEditInfo &certMlc);
    //bool EasyPlanDragMlc(const CertifiedMlcEditInfo &certMlc);
    bool EasyPlanGetCurrentMlcData(const std::string& planUID, plunc::LEAF_POSITION_STC*& leafData, int& leafNum);
    bool EasyPlanGetCurrentBlockData(const std::string& planUID, std::vector<Mcsf::MedViewer3D::Point2D>& vecData);
    void EasyPlanSetISOCenter(const std::string &planUID, const std::string &poiUID, 
        const std::string& currentImageUID, const float& currentIsoInImgX, const float& currentIsoInImgY, 
        const float& previousIsoInImgCoorX, const float& previousIsoInImgCoorY,
        const float& viewerWidth, const float& viewerHeight, const int& imageWidth, const int& imageHeigh,
        const float& firstImagePlanePixelSpacing, const float& secondImagePlanePixelSpacing);
    void EasyPlanSetNormPoint(const std::string &planUID, const std::string &poiUID, const std::string &imageUid);
    bool EasyPlanZoomImage(WINDOW_TYPE wndType, const std::string& wndUid, const std::string& beamUid, float scaleFactor);
	bool EasyPlanRotateImage( WINDOW_TYPE wndType, const std::string& wndUid, const std::string& beamUid, float startX, float startY, float stopX, float stopY );
	bool EasyPlanSetPanImage( int windowType, std::string& windowUid, std::string &planUid, std::string &beamUid, float startX, float startY, float stopX, float stopY, double viewerWidth, double viewerHeight );
	bool EasyPlanWindowingImage( int windowType, std::string &windowUid, std::string &beamUid, int windowWidth, int windowCenter, int& currentWinWidth, int& currentWinCenter);
    bool EasyPlanMesureDistance(WINDOW_TYPE windowType, std::string &windowUid, std::string &beamUid, float startX, float startY, float stopX, float stopY, bool actionStarted, bool actionDone);
	bool EasyPlanResetImage( int windowType, std::string & windowUid, std::string & beamUid );
    bool EasyPlanLoadImage( int windowType, const std::string& windowUid );
	//bool EasyPlanChangeApertureType( std::string & strPlanUid, std::string & strBeamUid, int apertureType );
    bool EasyPlanMesureAngle(int windowType,const std::string& windowUid, const std::string& beamUid, 
        const std::list<Mcsf::MedViewer3D::Point2D>& points, float cursorX, float cursorY,
		bool actionStarted, bool firstSideDone, bool secondSideDone );

	////////////////////////Bev Pan, Zoom, Windowing, Reset, Bev2Cev///////////////////////////////
	bool BEVPan(WINDOW_TYPE wndType, const std::string& wndUid, 
		float startX, float startY, float stopX, float stopY);
	bool BEVZoom(WINDOW_TYPE wndType, float zoomFactor);
	bool BEVWindowing(WINDOW_TYPE wndType, const int &windowWidth, const int &windowCenter);
	bool BEVCEVSwitch(WINDOW_TYPE wndType);
	bool BEVReset(WINDOW_TYPE wndType, int acType);
    bool BevSwitchDisplayMode(int displayMode);
    void SwitchSegment(const std::string& beamUid, int page);
    bool UpdateDrrSetting(std::string seriesUid, std::string normUid, float ctThresholdMin, 
        float ctThresholdMax, std::string ct2densityUid, float electronEnergy);
    bool GetPixelSpacingIn10MMOnBEV(WINDOW_TYPE windowType, const std::string& windowUid,
        double& spacingx, double& spacingy);
    bool GetBevViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid, 
        double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);
	///////////////////////////////////////////////////////////////////////////////////////////////

    bool ChangeCT2DensityTable(const std::string& seriesUid, const std::string& normUid, const std::string& ct2densityUid);

    bool SetRulerVisibility(const std::string& strImageUid, bool isVisible);
    bool SetDrrVisibility(bool isVisible);
    bool SetMlcTransparency(float fTransparency);
    bool DeleteDrrImage(const std::string& beamUid, const std::string imageUID);

    bool ChangePlanGoalSelection(DATA_SOURCE_GROUP_ID groupType, 
        int level, const std::string& uid);
    bool ActivePlan(DATA_SOURCE_GROUP_ID groupType, const std::string& planUID);
    bool ActiveNormGroup(DATA_SOURCE_GROUP_ID groupType, const std::string& normalUID);
    bool ActiveBeam(DATA_SOURCE_GROUP_ID groupType, const std::string& beamUID);

	/////////////////////////////////////////////////////////////////
	///  \brief Set result window
	///
	///  \param[in]    
	///  \param[out]   
	///  \return	   
	///  \exceptions:   none  
	/////////////////////////////////////////////////////////////////
	void SetEvaluationResult(const std::string &resultPlanUid,
		const std::string &resultImageUid);

	// easy plan(normpoint + isocenter)
	void DeleteISOCenter(const std::string &planUID, const std::string &poiUID);

    bool DoseHotSpots(const std::string& strPlanUID, const std::string& strVoiUid, float fDoseLimit, int iHotFlag);

   
    bool OperateContourSlices(WINDOW_TYPE windowType, std::string windowUid, std::string vOIUID, int firstSlice, int secondSlice, int editMode, int subEditMode);

    bool SelectContourSlices(WINDOW_TYPE windowType, std::string windowUid, Mcsf::MedViewer3D::Point2D currentPoint,
        int actionType, std::string vOIUID,int firstSlice, int secondSlice);

    bool GetSliceDoseGird(RtDosegrid *&doseGrid, TPS_FIRST_POSTFIX_COMPONENT section, std::string planUid, std::string imageUid);

    bool GetMprCornerInfo(WINDOW_TYPE windowType, const std::string& windowUid, double normPosX, double normPosY,
        int suvUnitType, double& density, double& value, double& doseValue,
        double& windowcenter, double& windowwidth, int& sliceIndex, int& startSliceNumber, double& sliceThickness,
        double& positionx, double& positiony, double& positionz, bool& isValueValid, 
        std::string& modality);

    bool GetMprDynamicCornerInfo(WINDOW_TYPE windowType, 
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

    bool TpsRenderProxy::GetMprSliceInfo(WINDOW_TYPE windowType, 
        const std::string& windowUid, int& sliceIndex, int& startSliceNumber, 
        int& totalSlices, double& sliceThickness, float& sliceLocation);

    bool GetMprWindowCenterWidth(
        WINDOW_TYPE windowType, 
        const std::string& windowUid,
        int suvUnitType,
        double& windowCenter, //Top if PT
        double& windowWidth,  //Bottom if PT
        bool& isValueValid);

    bool VoiRedoUndo(std::shared_ptr<TpsVOIEditingCache> cache);
    void PreviewBeamTemplate(std::string beamTemplateUid, std::string normGroupUid, std::string isocenterUid);

    /*void UpdatePhantomPOI(const std::string& sImageUid, const std::string& sPoiUid);
    bool DeletePhantomPoi(const std::string& sImageUid, const std::string& sPoiUid);*/
    //bool DeleteQAPlanPoi();

    void UpdateSegment(const std::string& normGroupUID, const std::string& beamUID);

    //this interface will dis/enable the non-image render strategies, 
    //such as beam line, dose line, voi, and so on.
    //all the windows which has same IMAGE data source uid with the specified window
    //will be affected by this interface
    void HideMprNonImage(WINDOW_TYPE windowType, const std::string& windowUid, bool isToHide);

    //enable or disable arc edit mode on mpr
    void EnableArcEditModeOnMprPlane(const std::string& planUID, 
        const std::string& ngUID, const std::string& beamUID, bool isEnabled);
    void SetArcEditCache(
        const std::string& planUID, 
        const std::string& ngUID, 
        const std::string& beamUID, 
        bool isClockwise, bool isSimpleArc, bool isToRotateStart,
        double minAngle, double maxAngle,
        double cwRange, double ccwRange,
        double startAngle, double arcLength,
        const Mcsf::MedViewer3D::Matrix4x4& matStartGantry2Patient, 
        const Mcsf::MedViewer3D::Matrix4x4& matEndGantry2Patient,
        const Mcsf::MedViewer3D::Matrix4x4& matPatient2OrgGantry);
    void EditArcBeamCache(
        WINDOW_TYPE windowType, 
        const std::string& windowUid,
        double preX, double preY,
        double currX, double currY, 
        const std::string& planUid,
        const std::string& ngUid, 
        const std::string& beamUid);
    bool GetArcGantryAnglesFromCache(
        const std::string& planUID, 
        const std::string& ngUID, 
        const std::string& beamUID, 
        double& start, double& arcLength);

    bool GetNormalOfMPR(WINDOW_TYPE windowType, const std::string& windowUid, Mcsf::Vector3f& normal);

    ///////////////////////////////////////Gallery Part////////////////////////////////////////////
    int GetCrosshairMode(std::string seriesUID, int& iCrosshairMode);
    //用到这两个方法的地方，在前段是否要删除相应的GO，By DYX
    bool RemoveGraphicObjects();
    bool RemoveGraphicObjects(std::string seriesUID);
    // currentslice [0, totalSlice)
    bool GetCurrentMPRSlice(WINDOW_TYPE strWidowType, const std::string& wndUid, int& currentSlice, int& startSliceNumber, int& totalSlice);

    bool GalleryMprPageTurn(WINDOW_TYPE wndType, const std::string& wndUid, int iPageturn);
    bool GalleryMprPan(WINDOW_TYPE wndType, const std::string& wndUid, 
        float startX, float startY, float stopX, float stopY);
    bool GalleryMprZoom(WINDOW_TYPE wndType, const std::string& wndUid, float scaleFactor);
    bool GalleryMprZoom(WINDOW_TYPE wndType, const std::string& wndUid, 
        float startX, float startY, float stopX, float stopY);
    bool GalleryMprWindowing(WINDOW_TYPE wndType, const std::string& wndUid, 
        float deltaWidth, float deltaCenter, bool updateAll);
    bool GalleryShadedSurfaceRotate(const std::string seriesUID, const Mcsf::Point2f& prePt, const Mcsf::Point2f& curPt);

    bool GetCrosshairProjectionPositionForGallery(WINDOW_TYPE windowType, const std::string& windowUid, 
        float& x2D, float& y2D);
    bool GetCrossXYPointsForGallery(WINDOW_TYPE windowType, const std::string& windowUid, 
        double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4);
    bool SetCrosshairHittedModeForGallery(int wndType, const std::string& wndUid, int mode);
    //void TranslateCrosshairForGallery(WINDOW_TYPE windowType, const std::string& windowUid, 
    //    double xStart, double yStart, double xEnd, double yEnd);
    void UpdateCrosshairForGallery(WINDOW_TYPE windowType, const std::string& windowUid,
        double xStart, double yStart, double xEnd, double yEnd, int crossMode, int hitMode, double rotateX, double rotateY);
    void HideMprNonImageForGallery(WINDOW_TYPE windowType, bool isToHide);
    bool GetMprCornerInfoForGallery(WINDOW_TYPE windowType, const std::string& windowUid, double normPosX, double normPosY,
        int suvUnitType, double& density, double& value, double& windowcenter, double& windowwidth, 
		int& sliceIndex, int& startSliceNumber, int& totalSliceNum, double& sliceThickness,
        double& positionx, double& positiony, double& positionz, bool& isValueValid, 
        std::string& modality, double& zoomFactor, double& rotate);
    bool ResetMPRObjectForGallery(WINDOW_TYPE windowType, const std::string& windowUid);
    bool GetMprViewDirecrionForGallery(WINDOW_TYPE windowType, const std::string& windowUid, 
        double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);
    bool GetShadedSurfaceViewDirecrionForGallery(WINDOW_TYPE windowType, const std::string& windowUid, 
        double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    void DownSampleDrr(WINDOW_TYPE windowType, const std::string& windowUid, float downSampleRate);

    bool EditContour(const std::string seriesUID, const std::string voiUID, EDIT_MODE editMode);

    bool PasteContour(const std::string seriesUID, const std::string voiUID, EDIT_FUNCTION editFunction);

    void ClearGraphicObjectsViaDataSource(DATA_SOURCE_ID sourceID, const DATA_SOURCE_UID& uid);

    void RemoveRestrationRelationship(const std::string& fixedUid, const std::string& floatUid);

    bool IsInVolume(WINDOW_TYPE windowType, const std::string& windowUid, float x, float y);

    void ChangePoiVisibilityOfSeries(const std::string& seriesUid, bool isVisible);

    void UpdateSeriesTable(const std::string& seriesUid);

    //test if the specified graphic object exists
    bool ContainsGraphicObject(const std::string& baseUid, GO_TYPE goType);
private:
    std::shared_ptr<MPRGraphicObject> GetMprPlaneGraphicObject(const std::string& seriesUid, int wndType);

    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> GetVolumeGraphicObject(const std::string& seriesUid);

private:
    //////////////////////////////////////////////////////////////////////////
    // \brief create render controller instance according to specified 
    //        window type & data source(optional)
    // \param[in] type  the unique id for windows, 
    //            it also indicates the rendering content
    // \return TpsRenderControllerBase instance pointer
    virtual TpsRenderControllerBase* CreateRenderController_i(WINDOW_TYPE type);

    //////////////////////////////////////////////////////////////////////////
    // \brief create graphic object converter factory
    virtual GOConverterFactoryBase* CreateGOConverterFactory_i();

	//virtual bool GenerateCommonGraphicObjects_i();

    // 一个光栅化圆形的算法
    void CirclePoints_i(int cx, int cy, int x, int y, 
        std::vector<Mcsf::MedViewer3D::Point2D>& vecCirclePts);
    void CircleMidpoint_i(int xCenter, int yCenter, int radius, 
        std::vector<Mcsf::MedViewer3D::Point2D>& vecCirclePts);
    void CircleSimple_i(int xCenter, int yCenter, int radius,
    std::vector<Mcsf::MedViewer3D::Point2D>& vecCirclePts);

    void RearrangeCirclePoints_i(std::vector<Mcsf::MedViewer3D::Point2D>& vecCirclePts);

    bool DeactiveCurrentPlan_i(int groupType);
    bool SetPlanAsDataSource_i(int groupType, const std::string& planUID, 
        const std::string& imageUID, bool& isPlanChanged);

    enum PageTurnRefreshType
    {
        Refresh_Voi         = 0x01,
        Refresh_Dose        = 0x02,
        Refresh_Table       = 0x04,
    };
    void MprPageTurn_i(const std::string& imageUid, FIRST_POSTFIX_COMPONENT section, 
        SECOND_POSTFIX_COMPONENT extension, int steps, int refreshType);
    void GalleryMprPageTurn_i(const std::string& imageUid, FIRST_POSTFIX_COMPONENT section, int steps);
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsRenderProxy);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
