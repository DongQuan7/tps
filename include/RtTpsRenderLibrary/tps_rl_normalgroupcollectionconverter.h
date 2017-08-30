////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_normalgroupcollectionconverter.h
/// 
///  \brief class NormalGroupCollectionConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/10
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_NORMALGROUPCOLLECTIONCONVERTER_H_
#define TPS_RL_NORMALGROUPCOLLECTIONCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

namespace RTFWK{
    class RTFWKDLDOPlan;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsBeamCollectionGOConverter;
class RtPlan;
class RtNormgroup;
/// \class NormalGroupCollectionConverter
/// 
/// \brief *****
class TPS_RL_DECLSPEC NormalGroupCollectionConverter : 
    public TpsGOConverterBase {
public:
    //the constructor and destructor
    NormalGroupCollectionConverter(GO_TYPE goType, TpsBeamCollectionGOConverter* converter);
    virtual ~NormalGroupCollectionConverter();

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
    RtPlan*           mPlan;
    std::vector<std::string>        mNormGroupUIDList;
    TpsBeamCollectionGOConverter*   mNormalGroupConverter;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(NormalGroupCollectionConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif