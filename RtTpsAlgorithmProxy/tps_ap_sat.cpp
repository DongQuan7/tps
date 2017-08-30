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

#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_ap_sat.h"
#include <math.h>
#include <malloc.h>
#include <stdarg.h>

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE


SatSeg Segment(Point2D p0, Point2D p1)
{
    Vector2D dir;
    dir.x = p1.x - p0.x;
    dir.y = p1.y - p0.y;
    SatSeg s = { p0, p1, dir };
    return s;
}

SatPolygon NewPolygon(std::vector<Point2D> verticeVec)
{
    int verticesNum = verticeVec.size();
    std::vector<SatSeg> edges;
    edges.reserve(verticesNum);
    for (int i = 0; i < verticesNum - 1; i++)
    {
        edges.push_back(Segment(verticeVec[i], verticeVec[i + 1]));
    }
    edges.push_back(Segment(verticeVec[0], verticeVec[verticesNum - 1]));
    SatPolygon shape = { verticeVec, edges };
    return shape;
}

void Project(SatPolygon a, Vector2D axis, float* arr)
{
    axis.Normalize();
    float min = a.verticeVec[0].x * axis.x + a.verticeVec[0].y * axis.y;
    float max = min; // min and max are the start and finish points
    for (int i = 1; i < a.verticeVec.size(); i++)
    {
        // find the projection of every point on the polygon onto the line.
        float proj = a.verticeVec[i].x * axis.x + a.verticeVec[i].y * axis.y;
        if (proj < min)
        {
            min = proj;
        }
        if (proj > max)
        {
            max = proj;
        }
    }
    arr[0] = min;
    arr[1] = max;
}

int Overlap(float* a_, float* b_)
{
    if (a_[0] >= b_[1])
    {
        return 0;
    }
    if (b_[0] >= a_[1])
    {
        return 0;
    }
    return 1;
}

bool TpsSat::Sat(const std::vector<Point2D>& verticeVecA, const std::vector<Point2D>& verticeVecB)
{
    float a_[2];
    float b_[2];
    SatPolygon a = NewPolygon(verticeVecA);
    SatPolygon b = NewPolygon(verticeVecB);
    for (int i = 0; i < a.verticeVec.size(); i++)
    {
        Vector2D axis = a.edges[i].dir; // Get the direction vector
        axis = Vector2D(axis.y, -axis.x); // Get the normal of the vector (90 degrees)
        Project(a, axis, a_);
        Project(b, axis, b_); // Find the projection of a and b onto axis
        if (!Overlap(a_, b_))
        {
            return false; // If they do not overlap, then no collision
        }
    }
    for (int i = 0; i < b.verticeVec.size(); i++)
    { // repeat for b
        Vector2D axis = b.edges[i].dir;
        axis = Vector2D(axis.y, -axis.x);
        Project(a, axis, a_);
        Project(b, axis, b_);
        if (!Overlap(a_, b_))
        {
            return false;
        }
    }
    return true;
}
TPS_END_NAMESPACE