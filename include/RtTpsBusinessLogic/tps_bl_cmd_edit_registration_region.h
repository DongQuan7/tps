////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_cca_editregistrationregioncmd.h
/// 
///  \brief class EditRegistrationRegionCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/07/05
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_EDIT_REGISTRATION_REGION_H_
#define TPS_BL_CMD_EDIT_REGISTRATION_REGION_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

namespace Mcsf{
    class Point3f;
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsImage3DEntity;
/// \class EditRegistrationRegionCmd
/// 
/// \brief *****
class TPS_BL_DECLSPEC EditRegistrationRegionCmd : public TpsCommand {
public:
    //the constructor and destructor
    EditRegistrationRegionCmd(WINDOW_TYPE wndType, const std::string& wndUid, 
        double x, double y, int actionType);
    virtual ~EditRegistrationRegionCmd();

protected:
    virtual int Execute();
private:
    void HitRegionCube_i(const Mcsf::Point3f& point, 
        const int* region, const Mcsf::Matrix4f& matP2V, char& hitFlag);
    void UpdateRegionCube_i(const TpsImage3DEntity& image, 
        const Mcsf::Point3f& point, char editFlag, bool isEditingFixed, 
        const int* region, const Mcsf::Matrix4f& matP2V);
private:
    WINDOW_TYPE     mWndType;
    std::string     mWndUid;
    double          mX;
    double          mY;
    int             mActionType;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(EditRegistrationRegionCmd);
};

TPS_END_NAMESPACE  // end namespace tps

#endif