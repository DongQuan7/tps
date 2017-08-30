////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_series_table_cache_graphic_object_converter.h
/// 
///  \brief class SeriesTableCacheConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/07/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SERIES_TABLE_CACHE_GRAPHIC_OBJECT_CONVERTER_H_
#define TPS_RL_SERIES_TABLE_CACHE_GRAPHIC_OBJECT_CONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class RtTable;
class SeriesTable;

/// \class SeriesTableCacheConverter
/// 
/// \brief *****
class TPS_RL_DECLSPEC SeriesTableCacheConverter : 
    public TpsGOConverterBase 
{
public:
    //the constructor and destructor
    SeriesTableCacheConverter(GO_TYPE goType);
    virtual ~SeriesTableCacheConverter();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief create new graphic object instance
    /// \return GraphicObjectPtr the new graphic object
    virtual GraphicObjectPtr CreateGraphicObject_i();

    virtual bool PrepareSourceData_i(const GOConverterParameter& para);

    //////////////////////////////////////////////////////////////////////////
    /// \brief update graphic object according to source data
    /// \brief go  the graphic object pointer to be updated
    /// \return bool  true if succeeded, false otherwise
    virtual bool Update_i(GraphicObjectPtr go);

    RtTable* mRtTable;
    SeriesTable* mSeriesTable;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(SeriesTableCacheConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif