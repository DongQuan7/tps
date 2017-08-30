////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_voigraphicobejctcollectionconverter.h
/// 
///  \brief class TpsVOIGOCollectionConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOIGRAPHICOBEJCTCOLLECTIONCONVERTER_H_
#define TPS_RL_VOIGRAPHICOBEJCTCOLLECTIONCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsVOIGOConverter;
class TpsImage3DEntity;
class ROIEntity;
/// \class TpsVOIGOCollectionConverter
/// 
/// \brief this class converts VOI entities of one image3d to VOI GO collection
class TPS_RL_DECLSPEC TpsVOIGOCollectionConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsVOIGOCollectionConverter(GO_TYPE goType, TpsVOIGOConverter* converter);
    ~TpsVOIGOCollectionConverter();

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
    TpsImage3DEntity*   mImage;
    std::vector<ROIEntity*> mRoiList;
    TpsVOIGOConverter*  mVOIConverter;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsVOIGOCollectionConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif