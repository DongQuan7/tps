//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_measurehittest_operation.cpp
///  \brief   Gallery Measure Hit Test declaration
///
///  \version 1.01
///  \date    May. 29, 2015
///  
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <boost/lexical_cast.hpp> 

// TPS
#include "RtTpsRenderLibrary/tps_rl_gallery_measurehittest_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
//#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobject.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
//#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

// ZHENGHE

#include "McsfViewer3DModel/mcsf_viewer3d_model.h" 

TPS_BEGIN_NAMESPACE   // begin namespace tps

    TpsGalleryMeasureHitTestOperation::TpsGalleryMeasureHitTestOperation(std::shared_ptr<GalleryMeasureGraphicObject> galleryMeasureGO,
    std::shared_ptr<GalleryTextGraphicObject> galleryTextGO,
    double x, double y, double width, double height/*, std::string &hitTestResult*/)
    :m_galleryMeasureGO(galleryMeasureGO), m_galleryTextGO(galleryTextGO),
    m_dX(x), m_dY(y), m_dWidth(width), m_dHeight(height), m_sHitTestResult("None|-1"/*hitTestResult*/){
}

TpsGalleryMeasureHitTestOperation::~TpsGalleryMeasureHitTestOperation(){

}

bool TpsGalleryMeasureHitTestOperation::ModifyGraphicObject() const {

    try
    {
        //auto go = mModelEntity->GetGraphicObject(m_measureGOKey);
        //auto measureGO = std::dynamic_pointer_cast<GalleryMeasureGraphicObject>(go);
        //if(measureGO == nullptr){
        //    TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to GalleryMeasureGraphicObject\n";
        //    return false;
        //}
        int hitIndex = -1;
        std::string hitType = "";

        //MeasureHitTest
        m_galleryMeasureGO->MeasureDistanceHitTest(m_dX, m_dY, m_dWidth, m_dHeight);
        m_galleryMeasureGO->GetMeasureDistanceHitIndex(&hitIndex);
        m_galleryMeasureGO->SetDirty(true);
        if (-1 != hitIndex)
        {
            hitType = "MeasureHit";
            m_sHitTestResult = hitType + "|" + boost::lexical_cast<std::string>(hitIndex);
            return true;
        }

        //TextHitTest
        m_galleryTextGO->TextHitTest(m_dX, m_dY, m_dWidth, m_dHeight);
        m_galleryTextGO->UpdateTextGO();
        m_galleryTextGO->GetTextHitIndex(&hitIndex);
        m_galleryTextGO->SetDirty(true);
        if (-1 != hitIndex)
        {
            hitType = "TextHit";
            m_sHitTestResult = hitType + "|" + boost::lexical_cast<std::string>(hitIndex);
            return true;
        }
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"CrosshairHitTestOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
