////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_applicator_graphic_object_converter.h
/// 
///  \brief class ApplicatorGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_APPLICATOR_GRAPHIC_OBJECT_CONVERTER_H_
#define TPS_RL_APPLICATOR_GRAPHIC_OBJECT_CONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class RtApplicator;

/// \class ApplicatorGOConverter
/// 
/// \brief *****
class TPS_RL_DECLSPEC ApplicatorGOConverter : 
    public TpsGOConverterBase {
public:
    //the constructor and destructor
    ApplicatorGOConverter(GO_TYPE goType);
    virtual ~ApplicatorGOConverter();

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

    RtApplicator* mApplicator;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ApplicatorGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif