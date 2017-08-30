////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_graphicobjectconverterbase.h
/// 
///  \brief class TpsGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/18
////////////////////////////////////////////////////////////////
#ifndef TPS_FW_GRAPHICOBJECTCONVERTERBASE_H_
#define TPS_FW_GRAPHICOBJECTCONVERTERBASE_H_

#include "RtTpsFramework/tps_fw_defs.h"
#include "RtTpsFramework/tps_fw_go_converter_parameter.h"
#include <map>

namespace Mcsf{
    namespace MedViewer3D{
        class IGraphicObject;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class ModelWarehouse;
class ITpsDataRepository;
typedef std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> GraphicObjectPtr;

enum ConverterStatus
{
    Cvtr_Idle,
    Cvtr_Convert,
    Cvtr_Update,
};

/// \class TpsGOConverterBase
/// 
/// \brief this class converts tps data to graphic object of 3D render engine
class TPS_FW_DECLSPEC TpsGOConverterBase
{
public:
    //the constructor and destructor
    TpsGOConverterBase(GO_TYPE goType);
    virtual ~TpsGOConverterBase();

    //////////////////////////////////////////////////////////////////////////
    /// \brief convert data in TMS database to graphic object in 3D render engine
    /// \param[in] uid  the UID of the data to be converted
    /// \return GraphicObjectPtr  the result graphic object pointer
    ///         return NULL when any error happens
    GraphicObjectPtr Convert();
    virtual GraphicObjectPtr Convert(const GOConverterParameter& para);

    //////////////////////////////////////////////////////////////////////////
    /// \brief update the specified graphic object according TMS data
    /// \param[in] uid  the UID of the TMS data
    /// \param[out] go  the graphic object pointer to be updated
    /// /return bool  true if succeeded, false otherwise
    bool Update(GraphicObjectPtr go);
    virtual bool Update(const GOConverterParameter& para, GraphicObjectPtr go);

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
    virtual GraphicObjectPtr CreateGraphicObject_i() = 0;

    //////////////////////////////////////////////////////////////////////////
    /// \brief get source data to be converted
    /// \param[in] uid  the UID of the TMS data
    /// \return bool  true if succeeded, false otherwise
    virtual bool PrepareSourceData_i(const GOConverterParameter& para);

    //////////////////////////////////////////////////////////////////////////
    /// \brief update graphic object according to source data
    /// \brief go  the graphic object pointer to be updated
    /// \return bool  true if succeeded, false otherwise
    virtual bool Update_i(GraphicObjectPtr /*go*/);
protected:
    //the type of graphic object defined in "tps_fw_renderdefines.h"
    GO_TYPE             mGOType;
    //data managers
    ITpsDataRepository* mDataRepository;
    //graphic object warehouse
    ModelWarehouse*     mModelWarehouse;
    ConverterStatus     mStatus;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGOConverterBase);
};

template<class T>
class TpsGOCommonConverter : public TpsGOConverterBase{
public:
    TpsGOCommonConverter(GO_TYPE goType) : TpsGOConverterBase(goType){}
    virtual ~TpsGOCommonConverter() {}

private:
    virtual GraphicObjectPtr CreateGraphicObject_i() {
        std::shared_ptr<T> pT(new T());
        if (dynamic_pointer_cast<Mcsf::MedViewer3D::IGraphicObject>(pT))
        {
            return pT;
        }
        return nullptr;
    }
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGOCommonConverter);
};
TPS_END_NAMESPACE  // end namespace tps

#endif