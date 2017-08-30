////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_ap_dose_mesh_helper.h
/// 
///  \brief class DoseMesh helper declaration 
/// 
///  \version 1.0
/// 
///  \date    Jun.10,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_AP_DOSE_MESH_HELPER_H_
#define TPS_AP_DOSE_MESH_HELPER_H_

#include "tps_ap_defs.h"
#include "tps_pl_datastructure.h"

namespace Mcsf
{
    //namespace MedViewer3D
    //{
    //    class Matrix4x4;
    //}
    class Matrix4f;
};

TPS_BEGIN_NAMESPACE  // begin namespace TPS

class TpsImage3DEntity;
class RtDosegrid;

struct STC_LINE_SEGMENT
{
    plunc::PNT3D p1;
    plunc::PNT3D p2;
};

//typedef struct struct_STRIP
struct STC_STRIP
{
    int             count;
    int             con_num1;
    int             con_num2;
    int*            vert;
    plunc::PNT3D*   pt;
    plunc::PNT3D*   dpt;
    plunc::PNT3D*   norm;
    ~STC_STRIP()
    {
        free(pt);
        free(dpt);
        free(norm);
        free(vert);
    }
};

//typedef struct struct_STRIPS
struct STC_STRIPS
{
    int         nCount;
    STC_STRIP*  pStrip;

    ~STC_STRIPS()
    {
        for(int i = 0; i < nCount; i++)
        {
            STC_STRIP* strip = pStrip + i;
            strip->~STC_STRIP();
        }
        free(pStrip);
    }
} ;


class TPS_AP_DECLSPEC TpsDoseMeshHelper{
public:
    static int GenerateMeshFromDose(TpsImage3DEntity* pImage3D, RtDosegrid* pDoseGrid,
        float fDoseMin, Mcsf::Matrix4f& matGrid2World, STC_STRIPS* pMesh, plunc::ANASTRUCT* pAna);
};

TPS_END_NAMESPACE  // end namespace TPS

#endif  //TPS_AP_DOSE_MESH_HELPER_H_
