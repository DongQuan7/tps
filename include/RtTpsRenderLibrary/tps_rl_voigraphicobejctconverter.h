////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_voigraphicobejctconverter.h
/// 
///  \brief class TpsVOIGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOIGRAPHICOBEJCTCONVERTER_H_
#define TPS_RL_VOIGRAPHICOBEJCTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class ROIEntity;
class TpsImage3DEntity;
/// \class TpsVOIGOConverter
/// 
/// \brief this class convert a VOIEntity to VOI graphic object
class TPS_RL_DECLSPEC TpsVOIGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsVOIGOConverter(GO_TYPE goType);
    virtual ~TpsVOIGOConverter();

    inline void SetVoi(ROIEntity* voi){ mRoi = voi; }
    inline void SetSliceCount(int sliceCount){ mSliceCount = sliceCount; }
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
    ROIEntity*          mRoi;
    int                 mSliceCount;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsVOIGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif