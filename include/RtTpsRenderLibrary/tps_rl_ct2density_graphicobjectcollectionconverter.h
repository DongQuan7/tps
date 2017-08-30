////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Qiangqiang ZHOU qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_rl_ct2density_graphicobjectcollectionconverter.h
/// 
///  \brief class Ct2DensityGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    July 24, 2014
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CT2DENSITY_GRAPHICOBJECTCOLLECTIONCONVERTER_H_
#define TPS_RL_CT2DENSITY_GRAPHICOBJECTCOLLECTIONCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps


class TPS_RL_DECLSPEC Ct2DensityGOCollectionConverter : 
    public TpsGOConverterBase{
public:
    //the constructor and destructor
    Ct2DensityGOCollectionConverter(GO_TYPE goType);
    virtual ~Ct2DensityGOCollectionConverter();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief create new graphic object instance
    /// \return GraphicObjectPtr the new graphic object
    virtual GraphicObjectPtr CreateGraphicObject_i();

    //////////////////////////////////////////////////////////////////////////
    /// \brief update graphic object according to source data
    /// \brief go  the graphic object pointer to be updated
    /// \return bool  true if succeeded, false otherwise
    virtual bool Update_i(GraphicObjectPtr go);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(Ct2DensityGOCollectionConverter);
};

TPS_END_NAMESPACE  // TPS_RL_CT2DENSITY_GRAPHICOBJECTCOLLECTIONCONVERTER_H_

#endif