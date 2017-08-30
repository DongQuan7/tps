//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  yuxuan.duan (mailto:yuxuan.duan@united-imaging.com)
///
///  \file   tps_gca_measureanglecmd.h 
///  \brief   Measure Angle Command 
///
///  \version 1.0
///  \date  2015/03/06
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_MEASUREANGLECMD_H_
#define TPS_GCA_MEASUREANGLECMD_H_

#include "RtTpsFramework/tps_fw_command.h"

//MCSF
#include "McsfMedViewer3DArithmetic/point2d.h"
#include <list>

TPS_BEGIN_NAMESPACE

class TpsGcaMeasureAngleCmd : public TpsCommand 
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
    TpsGcaMeasureAngleCmd(const tps::LAYOUT_UNIT &unit, const std::list<Mcsf::MedViewer3D::Point2D>& anglePoints, 
        const float cursorX, const float cursorY, const bool actionStarted, const bool firstSideDone, const bool secondSideDone);
    /////////////////////////////////////////////////////////////////
    ///  \brief Destructor
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return       
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    ~TpsGcaMeasureAngleCmd();
protected:
    virtual bool PreExecute();
    virtual bool Execute();
    virtual bool PostExecute();
    TpsGcaMeasureAngleCmd * Clone();

private:
    tps::LAYOUT_UNIT           mUnit;
    std::list<Mcsf::MedViewer3D::Point2D>    mAnglePoints;
    float                      mCursorX;
    float                      mCursorY;
    bool                       mActionStarted;
    bool                       mFirstSideDone;
    bool                       mSecondSideDone;
//private:
//    TPS_DISALLOW_COPY_AND_ASSIGN(MeasureAngleCmd);
};

TPS_END_NAMESPACE
#endif