////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_block_graphic_object_converter.h
/// 
///  \brief class BlockGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/17
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_BLOCK_GRAPHIC_OBJECT_CONVERTER_H_
#define TPS_RL_BLOCK_GRAPHIC_OBJECT_CONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps
class RtBlock;

/// \class BlockGOConverter
/// 
/// \brief *****
class TPS_RL_DECLSPEC BlockGOConverter : 
    public TpsGOConverterBase {
public:
    //the constructor and destructor
    BlockGOConverter(GO_TYPE goType);
    virtual ~BlockGOConverter();

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

    RtBlock* mApertureBlock;
    RtBlock* mShieldingBlock;
private:
    GO_TYPE mGoType;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(BlockGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif