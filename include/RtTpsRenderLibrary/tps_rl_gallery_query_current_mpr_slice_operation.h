////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  Duan Yuxuan  yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_query_current_mpr_slice_operation.h
/// 
///  \brief class TpsGalleryQueryMprSliceInfoOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/03/13
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_QUERY_CURRENT_MPR_SLICE_OPERATION_H_
#define TPS_RL_GALLERY_QUERY_CURRENT_MPR_SLICE_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsRenderLibrary/tps_rl_query_current_mpr_slice_operation.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

    /// \class TpsGalleryQueryMprSliceInfoOperation
    /// 
    /// \brief *****
class TPS_RL_DECLSPEC TpsGalleryQueryMprSliceInfoOperation
    : public QueryMprSliceInfoOperation{
public:
    //the constructor and destructor
    TpsGalleryQueryMprSliceInfoOperation(const std::string& seriesUID, 
        FIRST_POSTFIX_COMPONENT section, bool isViewOrder = false);
    virtual ~TpsGalleryQueryMprSliceInfoOperation();

    virtual bool ModifyGraphicObject() const;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryQueryMprSliceInfoOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
