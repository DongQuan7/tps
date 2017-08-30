#ifndef TPS_RL_CONTOUR_SET_GRAPHIC_OBJECT_CONVERTER_H_
#define TPS_RL_CONTOUR_SET_GRAPHIC_OBJECT_CONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

class ROIEntity;

/// \class BlockGOConverter
/// 
/// \brief *****
class TPS_RL_DECLSPEC ContourSetGOConverter : public TpsGOConverterBase 
{
public:
    //the constructor and destructor
    ContourSetGOConverter(GO_TYPE goType);
    virtual ~ContourSetGOConverter();

    inline void SetROI(ROIEntity* roi) { mROI = roi; }

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
    ROIEntity*  mROI;
    int         mSection;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ContourSetGOConverter);
};

class TPS_RL_DECLSPEC CSGOCollectionConverter : public TpsGOConverterBase 
{
public:
    //the constructor and destructor
    CSGOCollectionConverter(GO_TYPE goType, ContourSetGOConverter* converter);
    virtual ~CSGOCollectionConverter();

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

    ContourSetGOConverter*  mCSGOConverter;
    std::vector<ROIEntity*> mROIList;
    int                     mSection;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(CSGOCollectionConverter);
};

TPS_END_NAMESPACE

#endif // TPS_RL_CONTOUR_SET_GRAPHIC_OBJECT_CONVERTER_H_
