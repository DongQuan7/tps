//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_test_environment.h
///  \brief     rt tps test environment
///
///  \version 1.0
///  \date    April 28,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_TEST_ENVIRONMENT_H_
#define RT_TPS_TEST_ENVIRONMENT_H_

#include <string>
#include <memory>

#include "tps_defs.h"


TPS_BEGIN_NAMESPACE;

class IRtDatabaseWrapper;
class ITpsDataRepository;
class RtMachine;
class RtCommissionedunit;
class RtAccessory;
class RtCourse;
class RtPatient;
class RtSeries;
class RtPlan;
class RtNormgroup;
class RtBeam;
class RtPoi;
class ROIEntity;

/// \class TpsTestEnvironment 
/// \brief test environment for TPS
///
///
/// \par  Usage of this class:
/// \code
/// \endcode
////
class TPS_EXPORT_CLS TpsTestEnvironment
{
public:

    ///  \brief Constructor
    TpsTestEnvironment();

    ///  \brief destructor
    virtual ~TpsTestEnvironment();

    void SetPatientName(const std::string firstName, const std::string lastName);
    // included Machine, Unit, Patient, Series, Course
    bool LoadSystemData(void);
    
    // created a Plan, a Poi
    bool CreateTestData(void);

    IRtDatabaseWrapper*     GetDatabase();
    ITpsDataRepository*     GetRepository();
    RtMachine*              GetDefaultMachine();
    RtCommissionedunit*     GetDefaultUnit();
    RtAccessory*            GetDefaultAccessory();
    RtCourse*               GetDefaultCourse();
    RtPatient*              GetDefaultPatient();
    RtSeries*               GetDefaultSeries();
    RtPlan*                 GetDefaultPlan();
    RtNormgroup*            GetDefaultNormgroup();
    RtBeam*                 GetDefaultBeam();
    RtPoi*                GetDefaultPoi();
    ROIEntity*              GetDefaultVoi();

private:
    bool LoadROIs_i(ROIEntity* pVoiEntity, const std::string& sImageDataUID);

protected:
    std::string             mFirstName;
    std::string             mLastName;
    IRtDatabaseWrapper*     mDatabase;
    ITpsDataRepository*     mRepository;
    RtMachine*              mDefaultMachine;
    RtCommissionedunit*     mDefaultUnit;
    RtAccessory*            mDefaultAcc;
    RtCourse*               mDefaultCourse;
    RtPatient*              mDefaultPatient;
    RtSeries*               mDefaultSeries;
    RtPlan*                 mDefaultPlan;
    RtNormgroup*            mDefaultNg;
    RtBeam*                 mDefaultBeam;
    RtPoi*                mDefaultPoi;
    ROIEntity*              mDefaultVoi;
    std::string             mDefaultPrescriptionUid;
    bool                    mInitialized;
};

TPS_END_NAMESPACE;

#endif 

