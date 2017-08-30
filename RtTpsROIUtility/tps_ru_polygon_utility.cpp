#include "StdAfx.h"
#include "RtTpsROIUtility/tps_ru_polygon_utility.h"

#include "clipper.hpp"

using namespace ClipperLib;

#define PRECISION 1e6

TPS_BEGIN_NAMESPACE

Path ToPath(const POLYGON& p)
{
    Path path;
    path.reserve(p.size());
    for (int i = 0; i < p.size(); ++i)
    {
        path.push_back(IntPoint(
            static_cast<cInt>(p[i].x * PRECISION), 
            static_cast<cInt>(p[i].y * PRECISION)));
    }
    return path;
}

Paths ToPaths(const POLYGON_SET& ps)
{
    Paths paths;
    paths.reserve(ps.size());
    for (int i = 0; i < ps.size(); ++i)
    {
        paths.push_back(ToPath(ps[i]));
    }
    return paths;
}

POLYGON ToPolygon(const Path& path)
{
    POLYGON p;
    p.reserve(path.size());
    for (int i = 0; i < path.size(); ++i)
    {
        p.push_back(PT2(
            static_cast<double>(path[i].X) / PRECISION, 
            static_cast<double>(path[i].Y) / PRECISION));
    }
    return p;
}

POLYGON_SET ToPolygonSet(const Paths& paths)
{
    POLYGON_SET ps;
    ps.reserve(paths.size());
    for (int i = 0; i < paths.size(); ++i)
    {
        ps.push_back(ToPolygon(paths[i]));
    }
    return ps;
}

POLYGON_SET PolygonUtility::Logic(const POLYGON_SET& ps1, const POLYGON_SET& ps2, Operator op)
{
    Paths paths1 = ToPaths(ps1);
    Paths paths2 = ToPaths(ps2);

    Clipper clpr;
    clpr.AddPaths(paths1, ptSubject, true);
    clpr.AddPaths(paths2, ptClip, true);

    Paths res;
    clpr.Execute((ClipType)op, res, pftEvenOdd, pftEvenOdd);
    return ToPolygonSet(res);
}

POLYGON_SET PolygonUtility::Logic(const POLYGON_SET& ps, const POLYGON& p, Operator op)
{
    Paths paths = ToPaths(ps);
    Path path = ToPath(p);

    Clipper clpr;
    clpr.AddPaths(paths, ptSubject, true);
    clpr.AddPath(path, ptClip, true);

    Paths res;
    clpr.Execute((ClipType)op, res, pftEvenOdd, pftEvenOdd);
    return ToPolygonSet(res);
}

double PolygonUtility::Area(PT3* V, int n, const VEC3& N)
{
    double area = 0;
    double an, ax, ay, az; // abs value of normal and its coords
    int  coord;           // coord to ignore: 1=x, 2=y, 3=z

    if (n < 3) return 0;  // a degenerate polygon

    // select largest abs coordinate to ignore for projection
    ax = (N.x>0 ? N.x : -N.x);    // abs x-coord
    ay = (N.y>0 ? N.y : -N.y);    // abs y-coord
    az = (N.z>0 ? N.z : -N.z);    // abs z-coord

    coord = 3;                    // ignore z-coord
    if (ax > ay) {
        if (ax > az) coord = 1;   // ignore x-coord
    }
    else if (ay > az) coord = 2;  // ignore y-coord

    // compute area of the 2D projection
    switch (coord) {
    case 1:
        for (int i = 0; i < n; ++i)
            area += (Get(V[(i+1)%n], 1) * (Get(V[(i+2)%n], 2) - Get(V[i], 2)));
        break;
    case 2:
        for (int i = 0; i < n; ++i)
            area += (Get(V[(i+1)%n], 2) * (Get(V[(i+2)%n], 0) - Get(V[i], 0)));
        break;
    case 3:
        for (int i = 0; i < n; ++i)
            area += (Get(V[(i+1)%n], 0) * (Get(V[(i+2)%n], 1) - Get(V[i], 1)));
        break;
    }

    // scale to get area before projection
    an = sqrt( ax*ax + ay*ay + az*az); // length of normal vector
    switch (coord) {
    case 1:
        area *= (an / (2 * N.x));
        break;
    case 2:
        area *= (an / (2 * N.y));
        break;
    case 3:
        area *= (an / (2 * N.z));
    }
    return fabs(area);
}

bool PolygonUtility::IsIn(PT3* V, int n, const VEC3& N, const PT3& p)
{
    if (n < 3) return false;

    PT3 p1;
    VEC3 vx;
    for (int i = 0; i < n; ++i)
    {
        p1 = (V[i] + V[(i+1)%n]) * 0.5;
        vx = p1 - p;
        if(vx.Magnitude() <= 1e-6) continue;
        vx.Normalize();
        break;
    }
    VEC3 vy = N.CrossProduct(vx);
    vy.Normalize();

    VEC3 vec = V[0] - p;
    double x1 = vec.DotProduct(vx);
    double y1 = vec.DotProduct(vy);
    double x2, y2, lambda;
    int cn = 0;
    for (int i = 1; i <= n; ++i)
    {
        vec = V[i%n] - p;
        x2 = vec.DotProduct(vx);
        y2 = vec.DotProduct(vy);
        if ((y1 <= 0 && y2 > 0) || (y2 <= 0 && y1 > 0))
        {
            lambda = - y1 / (y2 - y1);
            if (x1 + lambda * (x2 - x1) > 0)
                ++cn;
        }
        x1 = x2;
        y1 = y2;
    }

    return (cn&1);    // 0 if even (out), and 1 if  odd (in)
}

std::pair<int, float> CalcMaxDist_i(const PT3* points, const VEC3& n, int start_idx, int end_idx)
{
    VEC3 vec_0 = points[end_idx] - points[start_idx];
    VEC3 vec_norm = vec_0.CrossProduct(n);
    double magnitude = vec_0.Magnitude();
    bool is_start_end_same = fabs(magnitude) <= 1e-6;
    if (!is_start_end_same) vec_norm.Normalize();

    std::pair<int, float> pr;
    double max_dist = 0.0, dist;
    VEC3 vec_1;
    for (int i = start_idx + 1; i < end_idx; ++i)
    {
        vec_1 = points[i] - points[start_idx];
        dist = is_start_end_same ? vec_1.Magnitude() : fabs(vec_norm.DotProduct(vec_1));
        if (dist > max_dist)
        {
            pr.first = i;
            max_dist = dist;
        }
    }
    pr.second = max_dist;
    return pr;
}

void RDPPolygonReduce_i(const PT3* points, const VEC3& n, 
    int start_idx, int end_idx, char* bit_flags, float epsilon)
{
    bit_flags[start_idx/8] |= (0x01 << (start_idx % 8));
    bit_flags[end_idx/8] |= (0x01 << (end_idx % 8));
    if (end_idx - start_idx < 2)
    {
        return;
    }

    auto maxDistPt = CalcMaxDist_i(points, n, start_idx, end_idx);
    if (maxDistPt.second < epsilon)
    {
        for (int i = start_idx + 1; i < end_idx; ++i)
        {
            bit_flags[i / 8] &= ~(0x01 << (i % 8));
        }
    }
    else
    {
        RDPPolygonReduce_i(points, n, start_idx, maxDistPt.first, bit_flags, epsilon);
        RDPPolygonReduce_i(points, n, maxDistPt.first, end_idx, bit_flags, epsilon);
    }
}

void PolygonUtility::RDPPolygonReduce(POLYGON3& polygon, const VEC3& n, float epsilon/* = 1.0f*/)
{
    if (polygon.size() < 3)
    {
        return;
    }

    char* bit_flags = new char[polygon.size() / 8 + 1];
    memset(bit_flags, 0, (polygon.size() / 8 + 1) * sizeof(char));
    RDPPolygonReduce_i(polygon._Myfirst, n, 0, polygon.size() - 1, bit_flags, epsilon);

    POLYGON3 result_list;
    for (int i = 0; i < polygon.size(); ++i)
    {
        if (bit_flags[i/8] & (0x01<<(i%8)))
        {
            result_list.push_back(polygon[i]);
        }
    }
    polygon = std::move(result_list);
    delete[] bit_flags;
}
TPS_END_NAMESPACE