////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  XU QIANG mailto:qiang.xu@united-imaging.com
/// 
///  \file tps_ap_sat.cpp
/// 
///  \brief class Seperating axis theorem
/// 
///  \version 1.0
/// 
///  \date    2016/12/09
////////////////////////////////////////////////////////////////

#ifndef TPS_AP_SAT_H_
#define  TPS_AP_SAT_H_

#include "tps_ap_defs.h"
#include "McsfMedViewer3DArithmetic/vector2d.h"
#include "McsfMedViewer3DArithmetic/point2d.h"


TPS_BEGIN_NAMESPACE

struct SatSeg
{
    Mcsf::MedViewer3D::Point2D p0;
    Mcsf::MedViewer3D::Point2D p1;
    Mcsf::MedViewer3D::Vector2D dir;
};

struct SatPolygon
{
    std::vector<Mcsf::MedViewer3D::Point2D> verticeVec;
    std::vector<SatSeg> edges;
};

class TPS_AP_DECLSPEC TpsSat
{
public:
    static bool TpsSat::Sat(
        const std::vector<Mcsf::MedViewer3D::Point2D>& verticeVecA, 
        const std::vector<Mcsf::MedViewer3D::Point2D>& verticeVecB);
};

TPS_END_NAMESPACE

#endif


