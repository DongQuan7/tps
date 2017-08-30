////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  yuxuan.duan  mailto:yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_measurevaluegraphicobjectconverter.h
/// 
///  \brief class GalleryMeasureValGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2015-03-23
////////////////////////////////////////////////////////////////


#ifndef TPS_RL_GALLERYMEASUREVALGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_GALLERYMEASUREVALGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsGalleryMeasureValGOConverter
    :  public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsGalleryMeasureValGOConverter(GO_TYPE goType);
    virtual ~TpsGalleryMeasureValGOConverter ();

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
    std::string                                        mSeries;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMeasureValGOConverter);
};

TPS_END_NAMESPACE

#endif