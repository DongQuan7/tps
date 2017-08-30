//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file contour_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/10/29
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "contour_converter.h"

#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"

TPS_BEGIN_NAMESPACE
bool ContourConverter::ConvertToALG(
    const RtContour& tpsContour,
    std::shared_ptr<RTFWK::RTFWKDLDOContour>* algContour)
{
    algContour->reset(new RTFWK::RTFWKDLDOContour(false));
    (*algContour)->SetUID(tpsContour.get_uid());
    (*algContour)->SetVOIUID(tpsContour.get_voiuid());
    std::vector<db_Point3f> contourPoints = tpsContour.get_contour_points();
    std::vector<Mcsf::Point3f> vAlgPoint3d;
    std::vector<db_Point3f>::iterator itPoint = contourPoints.begin();
    for (; itPoint != contourPoints.end(); ++itPoint)
    {
        db_Point3f tpsPoint3f = *itPoint;
        Mcsf::Point3f pt3d(tpsPoint3f.x, tpsPoint3f.y, tpsPoint3f.z);
        vAlgPoint3d.push_back(pt3d);
    }
    (*algContour)->SetPoints(vAlgPoint3d);
    (*algContour)->SetSliceIndex(tpsContour.get_sliceindex());
    (*algContour)->SetThickness(tpsContour.get_thickness());
    return true;
}

bool ContourConverter::CreateALGContourList(
    std::vector<RtContour*>& tpsContourList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOContour>>* algContourList)
{
    std::vector<RtContour*>::iterator itTpsContour = tpsContourList.begin();
    for (; itTpsContour != tpsContourList.end(); ++itTpsContour)
    {
        RtContour* tpsContour = *itTpsContour;
        std::shared_ptr<RTFWK::RTFWKDLDOContour> algContour;
        ContourConverter::ConvertToALG(*tpsContour, &algContour);
        algContourList->push_back(algContour);
    }
    return true;
}

TPS_END_NAMESPACE