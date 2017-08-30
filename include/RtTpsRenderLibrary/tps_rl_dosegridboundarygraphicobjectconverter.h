////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  zuo kai  kai.zuo@united-imaging.com
/// 
///  \file tps_rl_dosegridboundarygraphicobjectconverter.h
/// 
///  \brief class TpsDoseGridBoundaryGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/11
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DOSEGRIDBOUNDARYGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_DOSEGRIDBOUNDARYGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

namespace RTFWK{
    class RTFWKDLDODoseGrid;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps
class RtDosegrid;
    /// \class TpsDoseGOConverter
    /// 
    /// \brief this class cut dose grid data to get a dose GO according to the MPR plane.
class TPS_RL_DECLSPEC TpsDoseGridBoundaryGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsDoseGridBoundaryGOConverter(GO_TYPE goType);
    ~TpsDoseGridBoundaryGOConverter();

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
    RtDosegrid*   mDoseGrid;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDoseGridBoundaryGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif