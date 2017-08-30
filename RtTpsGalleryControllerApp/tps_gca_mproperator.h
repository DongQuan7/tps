//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_mproperator.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GA_MPROPERATORIMPL_H_
#define TPS_GA_MPROPERATORIMPL_H_

//#include "RtTpsGalleryControllerApp/tps_comca_defs.h"
#include "RtTpsGalleryControllerApp/tps_gca_improperator.h"

TPS_BEGIN_NAMESPACE

struct CertifiedMousePosInfo;
struct CertifiedMovieAction;

class TpsGcaMPROperator : public IMPROperator {

public:
    TpsGcaMPROperator(){}
    ~TpsGcaMPROperator(){}

    /// \brief  Initialize
    ///  
    /// \return bool  [true] if succeeded, [false] otherwise
    virtual bool Initialize();

    /// \brief  Finalize
    ///  
    /// \return bool  [true] if succeeded, [false] otherwise
    virtual bool Finalize();

    /// \brief MPR page turn
    ///  
    /// \param[in] uint                     viewerControlID and cellID
    /// \param[in] iPageTurn                PageDown or PageUp
    /// \param[in] sectionType              MPR plane type
    /// \return int TPS_SUCCESS or TPS_FAILURE
    virtual int MPRPageturn(LAYOUT_UNIT unit, int iPageTurn);

    /// \brief MPR page turn 4D
    ///  
    /// \param[in] uint                     viewerControlID and cellID
    /// \param[in] iPageTurn                PageDown or PageUp
    /// \param[in] imageUid                 uid of image 3d dataset
    /// \param[in] sectionType              MPR plane type
    /// \return int TPS_SUCCESS or TPS_FAILURE
    //virtual int MPRPageturn4D(LAYOUT_UNIT unit, int iPageTurn, 
    //    const std::vector<std::string>& vecSeriesUidList, FIRST_POSTFIX_COMPONENT sectionType);

    /// \brief MPR pan
    ///  
    /// \param[in] uint                  viewerControlID and cellID
    /// \param[in] startx                normalized x pos for start in display region cs
    /// \param[in] starty                normalized y pos for start in display region cs
    /// \param[in] stopx                 normalized x pos for stop in display region cs
    /// \param[in] stopy                 normalized y pos for stop in display region cs
    /// \param[in] sectionType           MPR plane type
    /// \return int TPS_SUCCESS or TPS_FAILURE
    virtual int MPRPan(LAYOUT_UNIT unit, float startx, float starty, 
        float stopx, float stopy);

    /// \brief MPR zoom
    ///  
    /// \param[in] uint                  viewerControlID and cellID
    /// \param[in] zoomFactor            zoom factor
    /// \param[in] sectionType           MPR plane type
    /// \return int TPS_SUCCESS or TPS_FAILURE
    virtual int MPRZoom(LAYOUT_UNIT unit, float zoomFactor);

    /// \brief MPR zoom
    ///  
    /// \param[in] uint                  viewerControlID and cellID
    /// \param[in] startx                normalized x pos for start in display region cs
    /// \param[in] starty                normalized y pos for start in display region cs
    /// \param[in] stopx                 normalized x pos for stop in display region cs
    /// \param[in] stopy                 normalized y pos for stop in display region cs
    /// \param[in] sectionType           MPR plane type
    /// \return int TPS_SUCCESS or TPS_FAILURE
    virtual int MPRZoom(LAYOUT_UNIT unit, float startx, float starty, float stopx, 
        float stopy);

    /// \brief MPR windowing
    ///  
    /// \param[in] uint                  viewerControlID and cellID
    /// \param[in] deltaWidth              delta width of windowing
    /// \param[in] deltaCenter             delta center of windowing
    /// \param[in] sectionType           MPR plane type
    /// \return int TPS_SUCCESS or TPS_FAILURE
    virtual int MPRWindowing(LAYOUT_UNIT unit, float deltaWidth, float deltaCenter, 
         bool updateAll);

    /////////////////////////////////////////////////////////////////
    ///  \brief Set rotation for image display
    ///
    ///  \param[in]    1: tps::LAYOUT_UNIT) contains cellID(int) and viewerControlID(int)
    ///                2: drrImageUID) for now, the drrImageUID does not matter, system will use the
    ///                     current easy plan image uid stored in ImageDataManager instead.
    ///                3: startX, startY) mouse position when the mouse down event captured on media viewer.
    ///                4: stopX, stopY) real-time mouse position during mouse move on media viewer.
    ///  \param[out]   
    ///  \return       bool: true if set pan succeeded, false otherwise.
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    virtual int MPRRotate( const LAYOUT_UNIT &unit, /*const std::string &drrImageUID, */const float startX,
        const float startY, const float stopX, const float stopY);

    /////////////////////////////////////////////////////////////////
    ///  \brief Measure angle for image display
    ///
    ///  \param[in]    1: tps::LAYOUT_UNIT) contains cellID(int) and viewerControlID(int)
    ///                2: anglePoints) angle vertex coordinates captured on media viewer.
    ///                3: cursorX, cursorY) real-time mouse position during mouse move on media viewer.
    ///                4: actionStarted, firstSideDone, secondSideDone) flags to indicate which state
    ///                     the current operation is at.
    ///  \param[out]   
    ///  \return       bool: true if measure angle succeeded, false otherwise.
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    virtual int MeasureAngle(const tps::LAYOUT_UNIT &unit, const std::list<Mcsf::MedViewer3D::Point2D>& anglePoints, 
        const float cursorX, const float cursorY, 
        const bool actionStarted, const bool firstSideDone, const bool secondSideDone);

    /////////////////////////////////////////////////////////////////
    ///  \brief Measure distance for image display
    ///
    ///  \param[in]    1: tps::LAYOUT_UNIT) contains cellID(int) and viewerControlID(int)
    ///                2: startX, startY) mouse position when the mouse down event captured on media viewer.
    ///                3: stopX, stopY) real-time mouse position during mouse move on media viewer.
    ///                4: actionStarted, actionDone) flags to indicate which state the current operation is at.
    ///  \param[out]   
    ///  \return       bool: true if measure distance succeeded, false otherwise.
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    virtual int MeasureDistance(const tps::LAYOUT_UNIT &unit, const float startX, const float startY,
        const float stopX, const float stopY, const bool actionStarted, const bool actionDone);

    virtual HIT_TEST_RESULT HitTest(const CertifiedMousePosInfo& certifiedMousePosInfo);

    //virtual int UpdateCrosshair(LAYOUT_UNIT unit, int iMouseAction, 
    //    double xStart, double yStart, double xEnd, double yEnd, 
    //    int nHitTestMode);

    virtual int MPRUpdateCornerTextInfo(const CertifiedMousePosInfo& certifiedMousePosInfo);

    virtual int MPRCornerImageTextVisible(const std::string &sSeriesUID, const bool& bIsVisible);

    /// \brief MPR cursor pointer action
    ///  
    /// \param[in] normPlanX             normalized x in 2d plane
    /// \param[in] normPlanY             normalized y in 2d plane
    /// \param[in] imageUid              uid of image 3d dataset
    /// \param[in] planUid               uid of RT plan
    /// \param[in] sectionType           MPR plane type
    /// \return int TPS_SUCCESS or TPS_FAILURE
    //virtual int MPRCursorPointerAction(float normPlanX, float normPlanY,
    //    const std::string& seriesUid, const std::string& planUid,
    //    FIRST_POSTFIX_COMPONENT sectionType);

    //virtual int Movie(const CertifiedMovieAction &);

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGcaMPROperator);
};

TPS_END_NAMESPACE

#endif
