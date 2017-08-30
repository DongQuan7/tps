//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  yuxuan.duan (mailto:yuxuan.duan@united-imaging.com)
///
///  \file   tps_gca_measuredistancecmd.h 
///  \brief   Measure Distance Command 
///
///  \version 1.0
///  \date  2015/04/22
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_MEASUREDISTANCECMD_H_
#define TPS_GCA_MEASUREDISTANCECMD_H_

#include "RtTpsFramework/tps_fw_command.h"

//MCSF
#include "McsfMedViewer3DArithmetic/point2d.h"
#include <list>

TPS_BEGIN_NAMESPACE

class TpsGcaMeasureDistanceCmd : public TpsCommand 
{
public:
    /////////////////////////////////////////////////////////////////
    ///  \brief Constructor
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return       
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    TpsGcaMeasureDistanceCmd(const tps::LAYOUT_UNIT &unit, const float startX, const float startY,
        const float stopX, const float stopY, const bool actionStarted, const bool actionDone);
    /////////////////////////////////////////////////////////////////
    ///  \brief Destructor
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return       
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    ~TpsGcaMeasureDistanceCmd();
protected:
    virtual bool PreExecute();
    virtual bool Execute();
    virtual bool PostExecute();
    TpsGcaMeasureDistanceCmd * Clone();

private:
    tps::LAYOUT_UNIT           mUnit;
    //std::list<Mcsf::MedViewer3D::Point2D>    mAnglePoints;
    float                      mStartX;
    float                      mStartY;
    float                      mStopX;
    float                      mStopY;
    bool                       mActionStarted;
    bool                       mActionDone;
    //private:
    //    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGcaMeasureDistanceCmd);
};

TPS_END_NAMESPACE
#endif