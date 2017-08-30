////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_ieccoordinatesystem.cpp
/// 
///  \brief class TpsIECCoordinateSystem implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"

#include "RtTpsFramework/tps_fw_coordinatesystemnode.h"
#include "RtTpsFramework/tps_fw_coordinatesystemtree.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

/* <key> \n
* PRA: Yes \n
* Name:void TpsIECCoordinateSystem \n
* Traced from: 46475 DS_PRA_TPS_CoordinateSystem \n
* Description: U-Plan system use DICOM patient coordinate system for \n
               patient and all other coordinate system follow IEC 61217 \n
* Short Description: IEC coordinate system & DICOM patient coordinate system\n
* Component: TPS \n
* </key> \n
*/
TpsIECCoordinateSystem::TpsIECCoordinateSystem()
    : mCSTree(nullptr), mGantryAngle(0.0f), 
    mCollimatorAngle(0.0f), mTableAngle(0.0f), 
    mSAD(0.0f), mLe(0.0f), mTableSelfRotationAngle(0.0f), 
    mWedgeOrientation(0.0f), mSWD(0.0f),
    /*mTableTopTranslation(0.0f, 0.0f, 0.0f), */mInitialized(false){

}

TpsIECCoordinateSystem::~TpsIECCoordinateSystem() {
    mCSNodeMap.clear();
}

bool TpsIECCoordinateSystem::Initialize(
    const Mcsf::Point3f& original, 
    PatientPosition pp /*= HFS */,
    float sad /* = 0.0f */, 
    float gantryAngle /* = 0.0f */, 
    float collimatorAngle /* = 0.0f */, 
    float tableAngle /* = 0.0f */){

    if(mInitialized) return true;
    try{
        if (!PrepareCSTree_i()){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][Initialize]: "
                <<"Failed to prepare coordinate system tree.";
            return false;
        }

        mInitialized = true;
        //set gantry angle
        RotateGantry(gantryAngle);
        //set collimator & sad
        SetSAD(sad);
        RotateCollimator(collimatorAngle);
        //set table angle
        RotateTable(tableAngle);
        mISOCenter = original;
        //set up DICOM patient c.s.
        if (!SetupPatientCS_i(original, pp))
        {
            TPS_LOG_DEV_ERROR<<"Failed to setup patient c.s.!";
            return false;
        }
        return true;
    }
    catch(const std::exception& ex){
        TPS_LOG_DEV_ERROR<<"[EXCEPTION][TpsIECCoordinateSystem][Initialize]: "
            <<"Exception happens: "<<ex.what();
        return false;
    }
}

bool TpsIECCoordinateSystem::GetTransformMatrix(
    IECCSType source, IECCSType target, Mcsf::Matrix4f& matrix){
    if(!mInitialized){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][GetTransformMatrix]: "
            <<"The IEC coordinate system has not been initialized.";
        return false;
    }

    return mCSTree->GetTransformMatrix(mCSNodeMap[source]->GetID(), 
        mCSNodeMap[target]->GetID(), matrix);
}

bool TpsIECCoordinateSystem::SetSAD(float sad){
    if(!mInitialized){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][SetSAD]: "
            <<"The IEC coordinate system has not been initialized.";
        return false;
    }

    if (sad < -1e-6){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][SetSAD]: "
            <<"The sad is negative.";
        return false;
    }

    float deltaSAD = sad - mSAD;
    //refresh BEAM_LIMITING_DEVICE node
    mCSNodeMap[_BEAM_LIMITING_DEVICE]->Translate(
        Mcsf::Vector3f(0.0f, 0.0f, deltaSAD));
    mSAD = sad;
    return true;
}

bool TpsIECCoordinateSystem::RotateGantry(float gantryAngle){
    if(!mInitialized){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][RotateGantry]: "
            <<"The IEC coordinate system has not been initialized.";
        return false;
    }
    float deltaGantryRadians = (gantryAngle - mGantryAngle)
        * float(PI) / 180.0f;
    //refresh GANTRY node
    mCSNodeMap[_GANTRY]->Rotate(_Y, deltaGantryRadians);
    mGantryAngle = gantryAngle;
    return true;
}

bool TpsIECCoordinateSystem::RotateTable(float tableAngle){
    if(!mInitialized){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][RotateTable]: "
            <<"The IEC coordinate system has not been initialized.";
        return false;
    }
    float deltaTableRadians = (tableAngle - mTableAngle)
        * float(PI) / 180.0f;
    //refresh PATIENT_SUPPORT node
    mCSNodeMap[_PATIENT_SUPPORT]->Rotate(_Z, deltaTableRadians);
    mTableAngle = tableAngle;
    return true;
}

bool TpsIECCoordinateSystem::RotateCollimator(float collimatorAngle){
    if(!mInitialized){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][RotateCollimator]: "
            <<"The IEC coordinate system has not been initialized.";
        return false;
    }
    float deltaCollimatorRadians = (collimatorAngle - mCollimatorAngle)
        * float(PI) /180.0f;
    //refresh BEAM_LIMITING_DEVICE node
    mCSNodeMap[_BEAM_LIMITING_DEVICE]->Rotate(_Z, deltaCollimatorRadians);
    mCollimatorAngle = collimatorAngle;
    return true;
}

//bool TpsIECCoordinateSystem::TranslateTableTop(
//    const Mcsf::Vector3f& translate){
//    if(!mInitialized){
//        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][TranslateTable]: "
//            <<"The IEC coordinate system has not been initialized.";
//        return false;
//    }
//
//    Mcsf::Vector3f deltaTranslation = translate - mTableTopTranslation;
//    mCSNodeMap[_TABLE_TOP]->Translate(deltaTranslation);
//    mTableTopTranslation = translate;
//    return true;
//}

bool TpsIECCoordinateSystem::SetISOCenter(
    const Mcsf::Point3f& isoCenter){

    if(!mInitialized){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][SetISOCenter]: "
            <<"The IEC coordinate system has not been initialized.";
        return false;
    }

    Mcsf::Vector3f deltaTranslation = mISOCenter - isoCenter;
    mCSNodeMap[_PATIENT]->Translate(deltaTranslation);
    mISOCenter = isoCenter;
    return true;
}

//if table supports self rotation
bool TpsIECCoordinateSystem::SetLe(float le){
    if(!mInitialized){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][SetLe]: "
            <<"The IEC coordinate system has not been initialized.";
        return false;
    }
    
    float deltaLe = le - mLe;
    mCSNodeMap[_PATIENT_SUPPORT]->Translate(Mcsf::Vector3f(0.0f, -1 * deltaLe, 0.0f));
    mCSNodeMap[_TABLE_TOP]->Translate(Mcsf::Vector3f(0.0f, deltaLe, 0.0f));
    mLe = le;
    return true;
}

bool TpsIECCoordinateSystem::SetTableSelfRotation(
    float tableSelfRotationAngle){
    if(!mInitialized){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][SetTableSelfRotation]: "
            <<"The IEC coordinate system has not been initialized.";
        return false;
    }

    float deltaSelfRotationRadians = (tableSelfRotationAngle - 
        mTableSelfRotationAngle) * float(PI) / 180.0f;;
    mCSNodeMap[_TABLE_TOP_ECCENTRIC]->Rotate(_Z, deltaSelfRotationRadians);
    mTableSelfRotationAngle = tableSelfRotationAngle;
    return true;
}

bool TpsIECCoordinateSystem::SetWedgeOrientation(float wedgeOrientation){
    if(!mInitialized){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][SetWedgeOrientation]: "
            <<"The IEC coordinate system has not been initialized.";
        return false;
    }

    float deltaRadians = (wedgeOrientation - 
        mWedgeOrientation) * float(PI) / 180.0f;
    mCSNodeMap[_WEDGE_FILTER]->Rotate(_Z, deltaRadians);
    mTableSelfRotationAngle = wedgeOrientation;
    return true;
}

bool TpsIECCoordinateSystem::SetSWD(float swd){
    if(!mInitialized){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][SetSWD]: "
            <<"The IEC coordinate system has not been initialized.";
        return false;
    }

    if (swd < -1e-6){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][SetSWD]: "
            <<"The swd is negative.";
        return false;
    }

    float deltaSWD = swd - mSWD;
    mCSNodeMap[_WEDGE_FILTER]->Translate(
        Mcsf::Vector3f(0.0f, 0.0f, -1 * deltaSWD));
    mSAD = swd;
    return true;
}

bool TpsIECCoordinateSystem::PrepareCSTree_i(){
    try{
        mCSTree.reset(new TpsCoordinateSystemTree());

        //fixed
        auto f = mCSTree->Root();
        if (f == nullptr) {
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][PrepareCSTree_i]: "
                <<"Failed to get root coordinate node: fixed c.s.";
            return false;
        }
        mCSNodeMap[_FIXED] = f;
        //gantry
        CSNodePtr g(new TpsCoordinateSystemNode());
        if (!f->AddChild(g)){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][PrepareCSTree_i]: "
                <<"Failed to add coordinate node: gantry c.s.";
            return false;
        }
        mCSNodeMap[_GANTRY] = g;

        //beam limiting device or delineator
        CSNodePtr b(new TpsCoordinateSystemNode());
        if (!g->AddChild(b)){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][PrepareCSTree_i]: "
                <<"Failed to add coordinate node: beam limiting device c.s.";
            return false;
        }
        mCSNodeMap[_BEAM_LIMITING_DEVICE] = b;

        //wedge filter
        CSNodePtr w(new TpsCoordinateSystemNode());
        if (!b->AddChild(w)){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][PrepareCSTree_i]: "
                <<"Failed to add coordinate node: wedge filter c.s.";
            return false;
        }
        mCSNodeMap[_WEDGE_FILTER] = w;
        //patient support
        CSNodePtr s(new TpsCoordinateSystemNode());
        if (!f->AddChild(s)){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][PrepareCSTree_i]: "
                <<"Failed to add coordinate node: patient support c.s.";
            return false;
        }
        mCSNodeMap[_PATIENT_SUPPORT] = s;

        //table top eccentric
        CSNodePtr e(new TpsCoordinateSystemNode());
        if (!s->AddChild(e)){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][PrepareCSTree_i]: "
                <<"Failed to add coordinate node: table top eccentric c.s.";
            return false;
        }
        mCSNodeMap[_TABLE_TOP_ECCENTRIC] = e;
        //table top
        CSNodePtr t(new TpsCoordinateSystemNode());
        if (!e->AddChild(t)){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][PrepareCSTree_i]: "
                <<"Failed to add coordinate node: table top c.s.";
            return false;
        }
        mCSNodeMap[_TABLE_TOP] = t;
        //patient
        CSNodePtr p(new TpsCoordinateSystemNode());
        if (!t->AddChild(p)){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][PrepareCSTree_i]: "
                <<"Failed to add coordinate node: patient c.s.";
            return false;
        }
        mCSNodeMap[_PATIENT] = p;

        return true;
    }
    catch (const std::exception& ex){
        TPS_LOG_DEV_ERROR<<"[EXCEPTION][TpsIECCoordinateSystem][PrepareCSTree_i]: "
            <<"Exception happens: "<<ex.what();
        return false;
    }

}

bool TpsIECCoordinateSystem::SetupPatientCS_i(const Mcsf::Point3f& original, PatientPosition pp){

    switch (pp){
    case HFS:
        mCSNodeMap[_PATIENT]->Rotate(_X, -1 * float(PI) / 2);
        break;
    case HFP:
        mCSNodeMap[_PATIENT]->Rotate(_X, -1 * float(PI) / 2);
        mCSNodeMap[_PATIENT]->Rotate(_Z, float(PI));
        break;
    case HFDR:
        mCSNodeMap[_PATIENT]->Rotate(_X, -1 * float(PI) / 2);
        mCSNodeMap[_PATIENT]->Rotate(_Z, -1 * float(PI) / 2);
        break;
    case HFDL:
        mCSNodeMap[_PATIENT]->Rotate(_X, -1 * float(PI) / 2);
        mCSNodeMap[_PATIENT]->Rotate(_Z, float(PI) / 2);
        break;
    case FFS:
        mCSNodeMap[_PATIENT]->Rotate(_X, float(PI) / 2);
        mCSNodeMap[_PATIENT]->Rotate(_Z, float(PI));
        break;
    case FFP:
        mCSNodeMap[_PATIENT]->Rotate(_X, float(PI) / 2);
        break;
    case FFDR:
        mCSNodeMap[_PATIENT]->Rotate(_X, float(PI) / 2);
        mCSNodeMap[_PATIENT]->Rotate(_Z, float(PI) / 2);
        break;
    case FFDL:
        mCSNodeMap[_PATIENT]->Rotate(_X, float(PI) / 2);
        mCSNodeMap[_PATIENT]->Rotate(_Z, -1 * float(PI) / 2);
        break;
    default:
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsIECCoordinateSystem][SetupPatientCS_i]: "
            <<"Unrecognized patient position: "<<pp;
        return false;
    }

    Mcsf::Vector3f vecOrigin(-1 * original.GetX(), 
        -1 * original.GetY(), -1 * original.GetZ());
    mCSNodeMap[_PATIENT]->Translate(vecOrigin);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps