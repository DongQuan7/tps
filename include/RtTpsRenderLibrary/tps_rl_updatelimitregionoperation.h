//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rl_updatelimitregionoperation.h 
///  \brief   limit region
///
///  \version 1.0
///  \date  2014/05/13  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RL_UPDATELIMITREGIONOPERATION_H_
#define TPS_RL_UPDATELIMITREGIONOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsUpdateRegionOperation : public OperationBase{
public:
    /////////////////////////////////////////////////////////////////
    ///  \brief invoke this constructor if want to update the region
    ///
    ///  \param[in]    startPoint(leftBottom Point)
    ///  \param[in]    endPoint(rightTop Point)
    ///  \param[in]    imageUid
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    TpsUpdateRegionOperation(const std::string &imageUid, 
        Mcsf::Point3f startPoint, 
        Mcsf::Point3f endPoint);

    /////////////////////////////////////////////////////////////////
    ///  \brief invoke this constructor if user want to show/hide 
    ///         the limit region
    ///  \param[in]    bool canShowRegion 
    ///  \param[in]    startPoint(leftBottom Point) default (0,0,0)
    ///  \param[in]    endPoint(rightTop Point)     default (0,0,0)
    ///  \param[in]    imageUid
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    TpsUpdateRegionOperation(const std::string &imageUid, bool canShowRegion,
        Mcsf::Point3f startPoint=Mcsf::Point3f(0,0,0), Mcsf::Point3f endPoint=Mcsf::Point3f(0,0,0));
    /////////////////////////////////////////////////////////////////
    ///  \brief Destructor
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    ~TpsUpdateRegionOperation();
    /////////////////////////////////////////////////////////////////
    ///  \brief modify go
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    virtual bool ModifyGraphicObject() const;

private:
    std::string                  mImageUid;
    Mcsf::Point3f                mStartPoint;
    Mcsf::Point3f                mEndPoint;
    bool                         mCanShowRegion;
    bool                         mIsFirstShow;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsUpdateRegionOperation);
};
TPS_END_NAMESPACE

#endif