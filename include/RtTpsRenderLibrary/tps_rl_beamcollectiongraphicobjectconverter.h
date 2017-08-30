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

#ifndef TPS_RL_BEAMCOLLECTIONGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_BEAMCOLLECTIONGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include <vector>

TPS_BEGIN_NAMESPACE

class TpsBeamGOConverter;
class RtNormgroup;
class RtBeam;
class RtMachine;
class FluenceMapEntity;
class TpsIECCoordinateSystem;

/// \class TpsBeamGOConverter
/// 
/// \brief this class convert beam in data layer to beam graphic object
class TPS_RL_DECLSPEC TpsBeamCollectionGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsBeamCollectionGOConverter(GO_TYPE goType, TpsBeamGOConverter* beamConverter);
    ~TpsBeamCollectionGOConverter();

    inline void SetNormalgroup(RtNormgroup* ng) { mNormalGroup = ng; }
    inline void SetBeamList(const std::vector<RtBeam*> beamList){
        mBeamList = beamList;
    }
    inline void SetMachine(RtMachine* machine) {
        mMachine = machine; }
    inline void SetFluenceMapList(const std::vector<FluenceMapEntity*> fluenceMap){
        mFluenceMapList = fluenceMap;
    }
    inline void SetIecCSList(const std::vector<TpsIECCoordinateSystem*> iecCSList){
        mIecCSList = iecCSList;
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
    TpsBeamGOConverter*     mBeamConverter;
    RtNormgroup*            mNormalGroup;
    std::vector<RtBeam*>    mBeamList;
    std::vector<FluenceMapEntity*> mFluenceMapList;
    std::vector<TpsIECCoordinateSystem*> mIecCSList;
    RtMachine*              mMachine;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsBeamCollectionGOConverter);
};

TPS_END_NAMESPACE

#endif
