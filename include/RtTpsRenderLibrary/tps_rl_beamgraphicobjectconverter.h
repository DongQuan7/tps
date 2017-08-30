////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_beamgraphicobjectconverter.h
/// 
///  \brief class TpsBeamGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/25
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_BEAMGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_BEAMGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsWedgeGOConverter;
class ApplicatorGOConverter;
class BlockGOConverter;
class RtNormgroup;
class RtBeam;
class RtMachine;
class FluenceMapEntity;
class TpsIECCoordinateSystem;
/// \class TpsBeamGOConverter
/// 
/// \brief this class convert beam in data layer to beam graphic object
class TPS_RL_DECLSPEC TpsBeamGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsBeamGOConverter(GO_TYPE goType, TpsWedgeGOConverter* wedgeConverter, 
        ApplicatorGOConverter* applicatorConverter, 
        BlockGOConverter* apertureBlockConverter, 
        BlockGOConverter* shieldingBlockConverter);
    ~TpsBeamGOConverter();

    inline void SetBeam(RtBeam* beam){ 
        mBeam = beam; 
    }
    inline void SetNormalGroup(RtNormgroup* ng) { 
        mNormalGroup = ng; 
    }
    inline void SetMachine(RtMachine* machine) {
        mMachine = machine;
    }
    inline void SetFluenceMapEntity(FluenceMapEntity* fluenceMap) {
        mFluenceMapEntity = fluenceMap;
    }
    inline void SetIecCS(TpsIECCoordinateSystem* iecCs) {
        mIecCS = iecCs;
    }
    //////////////////////////////////////////////////////////////////////////
    // \brief  set model warehouse 
    virtual void SetModelWarehouse(ModelWarehouse* modelWarehouse);

    //////////////////////////////////////////////////////////////////////////
    // \brief set data repository
    virtual void SetDataRepository(ITpsDataRepository* dataRepository);

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
    TpsWedgeGOConverter*        mWedgeConverter;
    ApplicatorGOConverter*      mApplicatorConverter;
    BlockGOConverter*           mApertureBlockConverter;
    BlockGOConverter*           mShieldingBlockConverter;
    RtMachine*                  mMachine;
    RtBeam*                     mBeam;
    RtNormgroup*                mNormalGroup;
    FluenceMapEntity*           mFluenceMapEntity;
    TpsIECCoordinateSystem*     mIecCS;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsBeamGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
