////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_shaded_voi_goconverter.h
/// 
///  \brief class ShadedVoiGraphicObjectConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/03/05
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SHADED_VOI_GOCONVERTER_H_
#define TPS_RL_SHADED_VOI_GOCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include <vector>

namespace Mcsf{
    namespace MedViewer3D{
        class GraphicObjVolume;
    }
}
class ShadedVoiGraphicObject;

TPS_BEGIN_NAMESPACE  // begin namespace tps
class VOIEntity;
struct CONTOUR_struct;
typedef CONTOUR_struct TPS_CONTOUR_STC;
struct Contour;
class RtImage3DHeader;

class TPS_RL_DECLSPEC ShadedVoiGraphicObjectConverter : 
    public TpsGOConverterBase{
public:
    //the constructor and destructor
    ShadedVoiGraphicObjectConverter(GO_TYPE goType);
    virtual ~ShadedVoiGraphicObjectConverter();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief create new graphic object instance
    /// \return GraphicObjectPtr the new graphic object
    virtual GraphicObjectPtr CreateGraphicObject_i();

    //////////////////////////////////////////////////////////////////////////
    /// \brief update graphic object according to source data
    /// \brief go  the graphic object pointer to be updated
    /// \return bool  true if succeeded, false otherwise
    virtual bool Update_i(GraphicObjectPtr go);

protected:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ShadedVoiGraphicObjectConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
