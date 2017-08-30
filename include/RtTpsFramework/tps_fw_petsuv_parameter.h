//////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2012
///  All rights reserved.
/// 
///  \author  SHI Chunhui  chunhui.shi@united-imaging.com
/// 
///  \file tps_fw_petsuv_parameter.h
/// 
///  \brief class  declaration 
/// 
///  \version 1.0
/// 
///  \date    2012/12/15
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_PET_SUV_PARAMETER_H_
#define TPS_FW_PET_SUV_PARAMETER_H_

TPS_BEGIN_NAMESPACE

enum CorrectedImage  // (0028, 0051)
{
    DECY,               // decay corrected
    ATTN,               // attenuation corrected
    SCAT,               // scatter corrected
    DTIM,               // dead time corrected
    MOTN,               // gantry motion corrected
    PMOT,               // patient motion corrected
    CLN,                // count loss normalization
    RAN,                // randoms corrected
    RADL,               // non-uniform radial sampling corrected
    DCAL,               // sensitivity calibrated using dose calibrator
    NORM                // detector normalization
};


enum  DecayCorrection   // (0054, 1102)
{
    NONECORRECTION,       // no decay correction
    START,                // acquisition start time
    ADMIN                 // radiopharmaceutical administration time
};

enum PixelValueUnits   // (0054, 1001)
{
    CNTS,
    NONEUNIT,
    CM2,
    PCNT,
    CPS,
    BQML, 
    MGMINML,
    UMOLMINML,
    MLMING,
    MLG, 
    CM1,
    UMOLML,
    PROPCNTS,
    PROPCPS, 
    MLMINML,
    MLML,
    GML,
    STDDEV 
};

enum PatientSex
{
    Male,
    Female
};

enum Manufacturer
{
    UIH,
    SIEMENS,
    PHILIPS,
    GE,
    UNDEFINED
};

TPS_END_NAMESPACE

#endif
