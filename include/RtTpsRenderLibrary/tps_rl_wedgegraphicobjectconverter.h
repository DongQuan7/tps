////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_wedgegraphicobjectconverter.h
/// 
///  \brief class TpsWedgeGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_WEDGEGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_WEDGEGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps
class RtMachine;
class RtAccessory;

/// \class TpsWedgeGOConverter
/// 
/// \brief *****
class TPS_RL_DECLSPEC TpsWedgeGOConverter : 
    public TpsGOConverterBase {
public:
    //the constructor and destructor
    TpsWedgeGOConverter(GO_TYPE goType);
    virtual ~TpsWedgeGOConverter();

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

    double ConvertToDisplayAngle_i(double angle) const;

    RtAccessory* mAccessory;
    RtMachine* mMachine;


private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsWedgeGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif