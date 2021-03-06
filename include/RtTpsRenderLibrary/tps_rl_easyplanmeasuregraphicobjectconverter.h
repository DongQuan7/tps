////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  wenlong.zhao  mailto:wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplanmeasuregraphicobjectconverter.h
/// 
///  \brief class EasyPlanMeasureGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014-10-29
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANMEASUREGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_EASYPLANMEASUREGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE

class TpsImage2DEntity;
//IgrtDRR second GO Converter
class TPS_RL_DECLSPEC TpsEasyPlanDRRMeasureGOConverter
    :  public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsEasyPlanDRRMeasureGOConverter(GO_TYPE goType);
    virtual ~TpsEasyPlanDRRMeasureGOConverter ();

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

    TpsImage2DEntity*   mImage;
private:
    //std::string         mSection;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanDRRMeasureGOConverter );
};

TPS_END_NAMESPACE  

#endif