////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_start_registration.h
/// 
///  \brief class UpdateRegistrationCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/10/29
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_START_REGISTRATION_H_
#define TPS_BL_CMD_START_REGISTRATION_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsCertifiedLib/rt_tps_certified_registation_feedback.h"


namespace Mcsf{
    class Point3f;
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class ProgressActiveResponser;
class TpsImage3DEntity;
struct CertifiedLandmarkPoints;
/// \class StartRegistrationCmd
/// 
/// \brief *****
class TPS_BL_DECLSPEC StartRegistrationCmd : public TpsCommand {
public:
    //the constructor and destructor
    StartRegistrationCmd(const std::string& fixedUid, const std::string& floatUid, 
        int algorithm);
    virtual ~StartRegistrationCmd();

    CertifiedRegistrationFeedback GetFeedback() const;
    void SetProgressResponser(const std::shared_ptr<ProgressActiveResponser>& responser);
    int SetLandmarkPoints(const CertifiedLandmarkPoints& landmarkPointsList);
protected:
    virtual int Execute();

private:
    bool StartRegister_i(TpsImage3DEntity* fixedImage, TpsImage3DEntity* floatImage, int registrationAlgo);

    void SetupRoiMask_i(const int* fixedDim, const int* floatDim, 
        unsigned char* fixedRoiMask, unsigned char* floatRoiMask);

    bool RegisterLandmark_i();
    bool RegisterAlignment_i(TpsImage3DEntity* fixedImage, TpsImage3DEntity* floatImage);
    bool RegisterDeformable_i(TpsImage3DEntity* fixedImage, TpsImage3DEntity* floatImage);
    bool RegisterInitSetup_i(TpsImage3DEntity* fixedImage, TpsImage3DEntity* floatImage);

    bool IsLandmarkPointsInSamePlane()const;
private:
    std::string                     mFixedSeriesUid;
    std::string                     mFloatSeriesUid;
    int                             mAlgorithm;
    CertifiedRegistrationFeedback   mCertFeedback;
    //vector<Mcsf::Point3f>           mLandmarkPoints;
    Mcsf::Point3f* mLandmarkPoints;

    std::shared_ptr<ProgressActiveResponser> mProgressResponser;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(StartRegistrationCmd);
};

TPS_END_NAMESPACE  // end namespace tps

#endif