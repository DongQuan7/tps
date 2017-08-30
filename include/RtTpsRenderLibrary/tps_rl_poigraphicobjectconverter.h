////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_poigraphicobjectconverter.h
/// 
///  \brief class PoiGraphicObjectConverter  
/// 
///  \version 1.0
/// 
///  \date    2014/1/06
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_POIGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_POIGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE;
class RtPoi;

class TPS_RL_DECLSPEC PoiGraphicObjectConverter : public TpsGOConverterBase {
public:
    //the constructor and destructor
    PoiGraphicObjectConverter(GO_TYPE goType);
    ~PoiGraphicObjectConverter();

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

    RtPoi*     mPoi;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(PoiGraphicObjectConverter);
};


class TPS_RL_DECLSPEC MaxPointGraphicObjectConverter : public PoiGraphicObjectConverter {
public:
    //the constructor and destructor
    MaxPointGraphicObjectConverter(GO_TYPE goType);
    ~MaxPointGraphicObjectConverter();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief get source data to be converted
    /// \param[in] uid  the UID of the TMS data
    /// \return bool  true if succeeded, false otherwise
    virtual bool PrepareSourceData_i(const GOConverterParameter& para);
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(MaxPointGraphicObjectConverter);
};


TPS_END_NAMESPACE



#endif
