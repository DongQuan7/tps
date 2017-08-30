////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_mprgraphicobjectconverter.h
/// 
///  \brief class TpsMPRGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_MPRGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_MPRGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include <map>

namespace Mcsf{
    namespace MedViewer3D{
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class MPRGraphicObject;

/// \class TpsMPRGOConverter
/// 
/// \brief this class cut image 3d data to get a MPR graphic object.
class TPS_RL_DECLSPEC TpsMPRGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsMPRGOConverter(GO_TYPE goType);
    ~TpsMPRGOConverter();

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

protected:
    //////////////////////////////////////////////////////////////////////////
    //this function modify MPRAPI::PositionMPRPlane method in 3D render engine
    // \brief position MPR plane according a given volume data and 
    //        section type(axial, coronal, sagittal)
    // \param[in] volume  the volume graphic object
    // \param[in] section  the section type
    // \param[out] mpr  output MPR graphic object
    // \return bool  true when succeeded, false otherwise
    bool PositionMPRPlane_i(
        std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume,
        FIRST_POSTFIX_COMPONENT section, SECOND_POSTFIX_COMPONENT location,
        std::shared_ptr<MPRGraphicObject> mpr);

    int ToPatientPosition_i(std::string sPP);
protected:
    enum MPR_VIEW_DIRECTION{
        F2H = 0x00,//default AXIAL
        H2F = 0x04,
        L2R = 0x01,//default SAGITTAL
        R2L = 0x05,
        A2P = 0x02,//default CORONAL
        P2A = 0x06
    };

protected:
    std::map<FIRST_POSTFIX_COMPONENT, MPR_VIEW_DIRECTION>   mGOSectionToPatienCSAxisMap;
    std::map<MPR_VIEW_DIRECTION, MPR_VIEW_DIRECTION>        mVecRightMap;
    std::map<MPR_VIEW_DIRECTION, MPR_VIEW_DIRECTION>        mVecUpMap;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    mVolume;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsMPRGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif