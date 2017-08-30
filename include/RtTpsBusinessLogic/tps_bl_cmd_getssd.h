////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  zuo kai kai.zuo@united-imaging.com
/// 
///  \file tps_bl_cmd_getssd.h
/// 
///  \brief class 
/// 
///  \version 1.0
/// 
///  \date    Feb.24,2016
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_GETSSD_H_
#define TPS_BL_CMD_GETSSD_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "McsfMedViewer3DArithmetic/point3d.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class Point3D;
    }
    class Point3f;
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE

class SignedDistanceField;

class TPS_BL_DECLSPEC GetSSDCmd : public TpsCommand{
public:
    GetSSDCmd(const std::string& beamUid, const std::string& isocenterUid, const std::string& skinUid, bool isBeamSSD);
    virtual ~GetSSDCmd(void);
    int GetSSD(double& ssd, double& rangeMin, double& rangeMax, double& ox, double& oy, double&oz);
protected:
    virtual int Execute();
    virtual int ComputeSSD(SignedDistanceField* sdf, Mcsf::Matrix4f matB2P, 
        Mcsf::Matrix4f matP2V, Mcsf::Matrix4f matV2P, size_t dim[3], float& ssd);

private:
    float CrossProduct(const Mcsf::MedViewer3D::Point3D &p1, const Mcsf::MedViewer3D::Point3D &p2) const{ 
        return (p1.x * p2.y - p1.y * p2.x); 
    } 
    bool Equal(float f1, float f2) const{ 
        return (abs(f1 - f2) < 1e-4f); 
    } 
    bool AbovePoint(const Mcsf::MedViewer3D::Point3D &p1, const Mcsf::MedViewer3D::Point3D &p2) const{ 
        return (p1.x > p2.x || (Equal(p1.x, p2.x) && p1.y > p2.y)); 
    } 
    int Intersection(Mcsf::MedViewer3D::Point3D p1, Mcsf::MedViewer3D::Point3D p2, Mcsf::MedViewer3D::Point3D p3, Mcsf::MedViewer3D::Point3D p4, Mcsf::MedViewer3D::Point3D& Int);
    std::vector<Mcsf::Point3f> GetIntersectionsOfLineAndVolume(
        const Mcsf::Point3f& p1, const Mcsf::Point3f& p2, double dimX, double dimY, double dimZ);
private:
    std::string                         mBeamUid;
    std::string                         mIsocenterUid;
    std::string                         mSkinUid;
    double                              mSSD;
    double                              mRangeMin;
    double                              mRangeMax;
    double                              mOriginalX;
    double                              mOriginalY;
    double                              mOriginalZ;
	bool                                mIsBeamSSD;
    bool                                mIsFirstSSD;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_GETSSD_H_
