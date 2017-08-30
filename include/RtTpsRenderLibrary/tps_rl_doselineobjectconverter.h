////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  liu linqi  linqi.liu@united-imaging.com
/// 
///  \file tps_rl_doselineobjectconverter.h
/// 
///  \brief class TpsDoseLineGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/08/04
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DOSELINEGRAPHICALOBJECTCONVERTER_H_
#define TPS_RL_DOSELINEGRAPHICALOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps
class RtPlan;
class RtDoseline;
class RtNormgroup;

/// \class TpsDoseLineGOConverter
/// 
/// \brief this class cut dose line data to get a dose GO according to the MPR plane.
class TPS_RL_DECLSPEC TpsDoseLineGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsDoseLineGOConverter(GO_TYPE goType);
    ~TpsDoseLineGOConverter();

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

    std::vector<RtDoseline*> mDoseLineList;
    RtPlan* mPlan;
    std::vector<RtNormgroup*> mNormGroupList;
    //std::string mDoseCalcUid;
    //DOSE_CALCULATION_LEVEL mDoseCalcLevel;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDoseLineGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
