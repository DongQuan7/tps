////////////////////////////////////////////////////////////////////////////
/////  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
/////  All rights reserved.
/////
/////  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
/////
/////  \file   tps_epca_nudge_aperture_cmd.h 
/////  \brief   nudge mlc and block
/////
/////  \version 1.0
/////  \date  2015/04/09  
/////  \
////////////////////////////////////////////////////////////////////////////
//#ifndef TPS_EPCA_NUDGE_APERTURE_CMD_H_
//#define TPS_EPCA_NUDGE_APERTURE_CMD_H_
//
//#include <vector>
//#include "RtTpsFramework/tps_fw_command.h"
//#include "RtTpsCertifiedLib/rt_tps_certifiedmlceditinfo.h"
//#include "McsfMedViewer3DArithmetic/arithmetic.h"
//#include "McsfMedViewer3DCommon/mcsf_command_base.h"
//
//TPS_BEGIN_NAMESPACE
//
//typedef enum{
//    _IDLE = 0,
//    NUDGE_MASK,
//    DEMASK,
//} NUDGE_STATE;
//
//class RtBeam;
//struct CertifiedMlcEditInfo;
//
//class TpsNudgeApertureEasyPlanCmd : public TpsCommand{
//public:
//    TpsNudgeApertureEasyPlanCmd(const CertifiedMlcEditInfo &certMlc);
//    ~TpsNudgeApertureEasyPlanCmd();
//protected:
//    virtual int PreExecute();
//    virtual int Execute();
//    virtual int PostExecute();
//    TpsNudgeApertureEasyPlanCmd *Clone();
//
//private:
//    void NudgeBlock_i(RtBeam *beam);
//    bool PreNudgeBlock_i( RtBeam * beam, unsigned char *&blockMask,
//        int width,
//        int height);
//    bool ExecuteNudgeBlock_i(RtBeam * beam, 
//        unsigned char * blockMask,
//        unsigned int width, 
//        unsigned int height );
//    void SampleNudgePoints_i();
//    void CheckEditState_i( unsigned char * blockMask,
//        unsigned int width,
//        unsigned int height );
//    void ChangeMask_i( unsigned char * blockMask,
//        unsigned int width,
//        unsigned int height );
//    bool UpdateBlockContour_i(RtBeam * beam,
//        unsigned char * blockMask,
//        unsigned int width,
//        unsigned int height );
//
//private:
//    CertifiedMlcEditInfo                    mCertMlc;
//
//    static Mcsf::MedViewer3D::Point2D       mPrePoint;
//    static NUDGE_STATE                      mEditState;
//    Mcsf::MedViewer3D::Point2D              mHandlePoint;
//    std::vector<Mcsf::MedViewer3D::Point2D> mNudgePointList;
//private:
//    TPS_DISALLOW_COPY_AND_ASSIGN(TpsNudgeApertureEasyPlanCmd);
//};
//
//TPS_END_NAMESPACE
//
//#endif