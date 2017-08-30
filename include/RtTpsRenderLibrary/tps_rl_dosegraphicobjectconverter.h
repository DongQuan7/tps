////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_dosegraphicobjectconverter.h
/// 
///  \brief class TpsDoseGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/03
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DOSEGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_DOSEGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

namespace RTFWK{
    class RTFWKDLDOPlan;
    class RTFWKDLDODoseGrid;
}

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps
struct DoseLineAttribute;
class MPRGraphicObject;
class RtPlan;
class RtDosegrid;
/// \class TpsDoseGOConverter
/// 
/// \brief this class cut dose grid data to get a dose GO according to the MPR plane.
class TPS_RL_DECLSPEC TpsDoseGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsDoseGOConverter(GO_TYPE goType);
    ~TpsDoseGOConverter();

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

//private:
//   // this method will be removed when the dose line attribute becomes global.
//    bool ConvertDoseLineAttr_i(const std::string& planUid, 
//        std::vector<DoseLineAttribute>& vecDoseLineAttr);
//
protected:
    RtPlan*           mPlan;
    RtDosegrid*       mDoseGrid;

    std::shared_ptr<MPRGraphicObject>                       mMpr;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    mVolume;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDoseGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
