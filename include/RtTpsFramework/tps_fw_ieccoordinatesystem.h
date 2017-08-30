////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_ieccoordinatesystem.h
/// 
///  \brief class TpsIECCoordinateSystem declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_IECCOORDINATESYSTEM_H_
#define TPS_FW_IECCOORDINATESYSTEM_H_

#include "tps_fw_defs.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

#include <memory>
#include <map>
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsCoordinateSystemTree;
class TpsCoordinateSystemNode;
typedef std::shared_ptr<TpsCoordinateSystemTree> CSTreePtr;
typedef std::shared_ptr<TpsCoordinateSystemNode> CSNodePtr;

typedef enum {
    _FIXED,                         //F
    _GANTRY,                        //G
    _BEAM_LIMITING_DEVICE,          //B
    _WEDGE_FILTER,                  //W
    _PATIENT_SUPPORT,               //S
    _TABLE_TOP_ECCENTRIC,           //E
    _TABLE_TOP,                     //T
    _PATIENT,                       //P: DICOM Patient Coordinate System, not IEC Patient C.S.
} IECCSType;

/// \class TpsIECCoordinateSystem
/// 
/// \brief implementation of IEC coordinate system
class TPS_FW_DECLSPEC TpsIECCoordinateSystem{
public:
    //the constructor and destructor
    TpsIECCoordinateSystem();
    ~TpsIECCoordinateSystem();

    bool Initialize(const Mcsf::Point3f& original, PatientPosition pp = HFS, 
        float sad = 0.0f, float gantryAngle = 0.0f, 
        float collimatorAngle = 0.0f, float tableAngle = 0.0f);
    bool GetTransformMatrix(IECCSType source, IECCSType target, 
        Mcsf::Matrix4f& matrix);

    bool SetSAD(float sad);
    bool RotateGantry(float gantryAngle);
    bool RotateTable(float tableAngle);
    bool RotateCollimator(float collimatorAngle);

    bool SetISOCenter(const Mcsf::Point3f& isoCenter);
    //bool TranslateTableTop(const Mcsf::Vector3f& translate);
    //if table supports self rotation
    bool SetLe(float le);
    bool SetTableSelfRotation(float tableSelfRotationAngle);

    //wedgeOrientation: unit degree
    bool SetWedgeOrientation(float wedgeOrientation);
    //swd: distance between radiation source and wedge filter c.s.'s origin, unit: mm
    bool SetSWD(float swd);
private:
    bool PrepareCSTree_i();

    bool SetupPatientCS_i(const Mcsf::Point3f& original, PatientPosition pp);
private:
    CSTreePtr                           mCSTree;
    //maybe, we can also use NodeID as value.
    std::map<IECCSType, CSNodePtr>      mCSNodeMap;

    //degree
    float           mGantryAngle;
    float           mCollimatorAngle;
    float           mTableAngle;
    //mm
    float           mSAD;
    //Tx, Ty, Tz, unit: mm, offset of table top to eccentric
    //Mcsf::Vector3f  mTableTopTranslation;
    //The ISOCenter location in DICOM patient c.s.
    ::Mcsf::Point3f mISOCenter;

    //table self rotation angle
    float           mTableSelfRotationAngle;
    //length between self rotation axis Ze and Zs
    float           mLe;

    //wedge filter part
    float           mWedgeOrientation;
    float           mSWD;

    bool            mInitialized;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsIECCoordinateSystem);
};

TPS_END_NAMESPACE  // end namespace tps

#endif