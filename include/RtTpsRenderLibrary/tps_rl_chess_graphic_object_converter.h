////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_chess_graphic_object_converter.h
/// 
///  \brief class ChessGoConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/12/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CHESS_GRAPHIC_OBJECT_CONVERTER_H_
#define TPS_RL_CHESS_GRAPHIC_OBJECT_CONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class MPRGraphicObject;
/// \class ChessGoConverter
/// 
/// \brief *****
class TPS_RL_DECLSPEC ChessGoConverter : public TpsGOConverterBase 
{
public:
    //the constructor and destructor
    ChessGoConverter(GO_TYPE goType);
    virtual ~ChessGoConverter();

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
    virtual GraphicObjectPtr Convert(const std::string& /*uid*/) { return nullptr; }
    virtual bool Update(const std::string& /*uid*/, GraphicObjectPtr /*go*/) { return false; }

protected:
    std::shared_ptr<MPRGraphicObject>   mMpr;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ChessGoConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif