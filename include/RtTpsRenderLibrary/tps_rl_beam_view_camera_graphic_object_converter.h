////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_beam_view_camera_graphic_object_converter.h
/// 
///  \brief class BeamViewCameraGoConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/29
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_BEAM_VIEW_CAMERA_GRAPHIC_OBJECT_CONVERTER_H_
#define TPS_RL_BEAM_VIEW_CAMERA_GRAPHIC_OBJECT_CONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

namespace Mcsf{
    namespace MedViewer3D{
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsSegmentGraphicObject;
class CameraGraphicObject;

/// \class BeamViewCameraGoConverter
/// 
/// \brief *****
class TPS_RL_DECLSPEC BeamViewCameraGoConverter : public TpsGOConverterBase {
public:
    //the constructor and destructor
    BeamViewCameraGoConverter(GO_TYPE goType);
    virtual ~BeamViewCameraGoConverter();

    inline void SetPlanUID(const std::string& planUID) { mPlanUID = planUID; }
    inline void SetImageUID(const std::string& imageUID) { mImageUID = imageUID; }
    inline void SetBevViewType(BEV_VIEW_TYPE viewType) { mViewType = viewType; }
protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief create new graphic object instance
    /// \return GraphicObjectPtr the new graphic object
    virtual GraphicObjectPtr CreateGraphicObject_i();

    //////////////////////////////////////////////////////////////////////////
    /// \brief get source data to be converted
    /// \param[in] uid  the UID of the TMS data
    /// \return bool  true if succeeded, false otherwise
    virtual bool PrepareSourceData_i(const GOConverterParameter& para);

    //////////////////////////////////////////////////////////////////////////
    /// \brief update graphic object according to source data
    /// \brief go  the graphic object pointer to be updated
    /// \return bool  true if succeeded, false otherwise
    virtual bool Update_i(GraphicObjectPtr go);

private:
    void SetupDefaultCameraLocation_i(
        const std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>& volume,
        const std::shared_ptr<CameraGraphicObject>& camera);

    std::string         mPlanUID;
    std::string         mImageUID;
    BEV_VIEW_TYPE       mViewType;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(BeamViewCameraGoConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
