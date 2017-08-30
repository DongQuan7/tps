////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file tps_rl_tablelineobjectconverter.h
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    2014/12/04
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_TABLELINEOBJECTCONVERTER_H_
#define TPS_RL_TABLELINEOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps
/// \class TpsDoseLineGOConverter
/// 
/// \brief this class cut dose line data to get a dose GO according to the MPR plane.
class TPS_RL_DECLSPEC TpsTableLineGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsTableLineGOConverter(GO_TYPE goType);
    ~TpsTableLineGOConverter();

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
    float mTableLien;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsTableLineGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif