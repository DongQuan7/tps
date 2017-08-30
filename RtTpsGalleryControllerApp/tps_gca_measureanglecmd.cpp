//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  yuxuan.duan (mailto:yuxuan.duan@united-imaging.com)
///
///  \file   tps_gca_measureanglecmd.cpp 
///  \brief   measure angle
///
///  \version 1.0
///  \date  2015/03/06
///  \
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "tps_gca_measureanglecmd.h"

//ZHENGHE
#include "McsfGeometry/mcsf_geometry_sub_volume.h"

//TPS
#include "tps_logger.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_rl_modelwarehouse.h"
#include "RtTpsFramework/tps_rl_fire_operation.h"

#include "RtTpsFramework/tps_rl_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_rc_renderingcomponentrecorder.h"
#include "RtTpsFramework/tps_rc_renderproxybase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_rc_layoutmanager.h"
#include "tps_gca_mprpageturncmd.h" // define SelectedRenderProxyViaLayout
#include "RtTpsRenderLibrary/tps_rl_gallery_measureangle_operation.h"
#include "RtTpsRenderController/tps_rc_gallery_mprrendercontroller.h"

TPS_BEGIN_NAMESPACE
extern bool SelectRenderProxyViaLayout(TpsRenderProxyBase* renderProxy, void* para);

TpsGcaMeasureAngleCmd::TpsGcaMeasureAngleCmd(const tps::LAYOUT_UNIT &unit,
const std::list<Mcsf::MedViewer3D::Point2D>& anglePoints, 
const float cursorX, const float cursorY, 
const bool actionStarted, const bool firstSideDone, const bool secondSideDone)
: mUnit(unit), mAnglePoints(anglePoints),
mCursorX(cursorX), mCursorY(cursorY), 
mActionStarted(actionStarted), mFirstSideDone(firstSideDone), mSecondSideDone(secondSideDone){}

TpsGcaMeasureAngleCmd::~TpsGcaMeasureAngleCmd(void) {}

bool TpsGcaMeasureAngleCmd::PreExecute() {
    return true;
}

bool TpsGcaMeasureAngleCmd::Execute() {
    TPS_LOG_DEV_INFO<<"[INFO][MeasureAngleCmd][Execute]: "<<"MeasureAngleCmd execute!";

    try
    {
        // 1. get the series uid according to the layout information
        auto recorder = TpsRenderingComponentRecorder::GetInstance();
        TpsRenderProxyBase* renderProxy = recorder->SelectOne(
            (TpsRenderingComponentRecorder::Selector)SelectRenderProxyViaLayout, (void*)&mUnit);
        if(renderProxy == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to get related render proxy.";
            return false;
        }
        std::string seriesUID;
        if (!renderProxy->GetDataSource(mUnit, IMAGE, seriesUID)) {
            TPS_LOG_DEV_ERROR<<"Can not get the image data source of current layout: "
                <<"View Control ID: "<<mUnit.viewerControlID<<" Cell ID: "<<mUnit.cellID;
            return false;
        }

        // 2. get section type
        WINDOW_TYPE window;
        if (!renderProxy->GetLayoutManager()->GetWindowtype(mUnit, window)){
            TPS_LOG_DEV_ERROR<<"Failed to get the window type.";
            return false;
        }
        auto renderController = renderProxy->GetRenderController(window);
        auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
        if (mprRc == nullptr){
            TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
            return false;
        }
        FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();
        DISPLAY_SIZE dispsize;
        renderProxy->GetDisplaySize(mUnit, dispsize);
        if(dispsize.width <= 0 || dispsize.height <= 0){
            return false;
        }

        // 3. execute the Measure Angle command
        std::shared_ptr<TpsGalleryMeasureAngleOperation> measureAngleOperation(
            new TpsGalleryMeasureAngleOperation(dispsize, mUnit, mAnglePoints, mCursorX, mCursorY,
            mActionStarted, mFirstSideDone, mSecondSideDone));

        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
        std::string sMprKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MEASURE, section);
        std::string sMeasureKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MEASURE_VAL, section);
        std::string sMeasureValKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
        measureAngleOperation->SetGraphicObjectKeys(sMprKey, sMeasureKey, sMeasureValKey);
        ModelWarehouse::GetInstance()->ProcessOperation(measureAngleOperation);

        // 4. fire to render
        std::shared_ptr<TpsFireOperation> fire(new TpsFireOperation());
        ModelWarehouse::GetInstance()->ProcessOperation(fire);

    } catch (std::exception& ex) {
        ex;
        TPS_LOG_DEV_ERROR<<"Exception occurred during MeasureAngleCmd::Execute()";
        return false;
    }           

    TPS_LOG_DEV_INFO<<"[INFO][MeasureAngleCmd][Execute]: "<<"MeasureAngleCmd leave!";
    return true;
}

bool TpsGcaMeasureAngleCmd::PostExecute() {
    return true;
}

TpsGcaMeasureAngleCmd *TpsGcaMeasureAngleCmd::Clone() {
    TpsGcaMeasureAngleCmd *cmd = new TpsGcaMeasureAngleCmd(mUnit, mAnglePoints, mCursorX, mCursorY,
        mActionStarted, mFirstSideDone, mSecondSideDone);
    return cmd;
}

TPS_END_NAMESPACE
