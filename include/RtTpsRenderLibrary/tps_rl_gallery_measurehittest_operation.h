//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_measurehittest_operation.h
///  \brief   Gallery Measure Hit Test declaration
///
///  \version 1.01
///  \date    May. 29, 2015
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_MEASUREHITTEST_OPERATION_H_
#define TPS_RL_GALLERY_MEASUREHITTEST_OPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_textgraphicobject.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_RL_DECLSPEC TpsGalleryMeasureHitTestOperation : public OperationBase{

public:
    //the constructor and destructor
    TpsGalleryMeasureHitTestOperation(std::shared_ptr<GalleryMeasureGraphicObject> galleryMeasureGO,
        std::shared_ptr<GalleryTextGraphicObject> galleryTextGO,
        double x, double y, double width, double height/*, std::string &hitTestResult*/);
    virtual ~TpsGalleryMeasureHitTestOperation();

    virtual bool ModifyGraphicObject() const;

    inline std::string GetHitTestResult() {return m_sHitTestResult;}

private:
    //std::string     m_measureGOKey;
    double          m_dX;
    double          m_dY;
    double          m_dWidth;
    double          m_dHeight;
    mutable std::string     m_sHitTestResult;
    std::shared_ptr<GalleryMeasureGraphicObject> m_galleryMeasureGO;
    std::shared_ptr<GalleryTextGraphicObject> m_galleryTextGO;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMeasureHitTestOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_CROSSHAIR_HITTEST_OPERATION_H_
