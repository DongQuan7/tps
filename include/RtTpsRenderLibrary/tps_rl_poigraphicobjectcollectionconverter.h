////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_poigraphicobjectcollectionconverter.h
/// 
///  \brief class PoiGraphicObjectCollectionConverter
/// 
///  \version 1.0
/// 
///  \date    2014/2/13
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_POIGRAPHICOBJECTCOLLECTIONCONVERTER_H_
#define TPS_RL_POIGRAPHICOBJECTCOLLECTIONCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

namespace RTFWK{
    //class RTFWKDLDOSeries;
    class RTFWKDLDOPlan;
}

TPS_BEGIN_NAMESPACE
class PoiGraphicObjectConverter;
class TPS_RL_DECLSPEC PoiGraphicObjectCollectionConverter : public TpsGOConverterBase {
public:
    //the constructor and destructor
    PoiGraphicObjectCollectionConverter(GO_TYPE goType, PoiGraphicObjectConverter* converter);
    ~PoiGraphicObjectCollectionConverter();

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
    PoiGraphicObjectConverter*  mPoiGoConverter;
    //RTFWK::RTFWKDLDOSeries*     mSeries;
    //RTFWK::RTFWKDLDOPlan*       mPlan;
    std::vector<std::string>    mPOIList;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(PoiGraphicObjectCollectionConverter);
};


class MaxPointGraphicObjectConverter;
class TPS_RL_DECLSPEC MaxPointGraphicObjectCollectionConverter : public PoiGraphicObjectCollectionConverter {
public:
    //the constructor and destructor
    MaxPointGraphicObjectCollectionConverter(GO_TYPE goType, MaxPointGraphicObjectConverter* converter);
    ~MaxPointGraphicObjectCollectionConverter();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief get source data to be converted
    /// \param[in] uid  the UID of the TMS data
    /// \return bool  true if succeeded, false otherwise
    virtual bool PrepareSourceData_i(const GOConverterParameter& para);
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(MaxPointGraphicObjectCollectionConverter);
};
//class TPS_RL_DECLSPEC QATempPoiCollectionConverter : public PoiGraphicObjectCollectionConverter {
//public:
//    //the constructor and destructor
//    QATempPoiCollectionConverter(GO_TYPE goType, PoiGraphicObjectConverter* converter);
//    ~QATempPoiCollectionConverter();
//
//    virtual GraphicObjectPtr Convert(const DataSourceMap& dataSourceMap);
//    virtual bool Update(const DataSourceMap& dataSourceMap, GraphicObjectPtr go);
//
//private:
//    // the default copy and assign method are not allowed.
//    TPS_DISALLOW_COPY_AND_ASSIGN(QATempPoiCollectionConverter);
//};
//
//class TPS_RL_DECLSPEC SeriesPoiCollectionConverter : public PoiGraphicObjectCollectionConverter {
//public:
//    //the constructor and destructor
//    SeriesPoiCollectionConverter(GO_TYPE goType, PoiGraphicObjectConverter* converter);
//    ~SeriesPoiCollectionConverter();
//
//    virtual GraphicObjectPtr Convert(const DataSourceMap& dataSourceMap);
//    virtual bool Update(const DataSourceMap& dataSourceMap, GraphicObjectPtr go);
//
//private:
//    // the default copy and assign method are not allowed.
//    TPS_DISALLOW_COPY_AND_ASSIGN(SeriesPoiCollectionConverter);
//};
TPS_END_NAMESPACE
#endif
