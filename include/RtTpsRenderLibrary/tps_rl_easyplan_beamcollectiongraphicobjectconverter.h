////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Cheng Yilong  yilong.cheng@united-imaging.com
/// 
///  \file tps_rl_easyplan_beamcollectiongraphicobjectconverter.h
/// 
///  \brief class TpsEasyPlanBeamCollectionGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/25
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLAN_BEAMCOLLECTIONGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_EASYPLAN_BEAMCOLLECTIONGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

    //class TpsEasyPlanBeamGOConverter;

    /// \class TpsEasyPlanBeamCollectionGOConverter
    /// 
    /// \brief this class convert segment in data layer to segment graphic object
class TPS_RL_DECLSPEC TpsEasyPlanBeamCollectionGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    //TpsEasyPlanBeamCollectionGOConverter(GO_TYPE goType, TpsEasyPlanSegmentGOConverter* segmentConverter);
    TpsEasyPlanBeamCollectionGOConverter(GO_TYPE goType);
    ~TpsEasyPlanBeamCollectionGOConverter();

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
    //TpsEasyPlanSegmentGOConverter*         mSegmentConverter;
    std::string                 mNormGroupUID;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanBeamCollectionGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif