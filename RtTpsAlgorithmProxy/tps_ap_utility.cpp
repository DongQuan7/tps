////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_ap_utility.cpp
/// 
///  \brief class TpsUtility implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/07/28
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

TPS_BEGIN_NAMESPACE

std::pair<int, float> CalcMaxDist_i(const Mcsf::Point2f* points, int start_idx, int end_idx)
{
    Mcsf::Vector2f vec_0 = points[end_idx] - points[start_idx];
    Mcsf::Vector2f vec_norm(-vec_0.GetY(), vec_0.GetX());
    double magnitude = vec_0.Magnitude();
    bool is_start_end_same = fabs(magnitude) <= 1e-6;
    if (!is_start_end_same) vec_norm.Normalize();

    std::pair<int, float> pr;
    double max_dist = 0.0, dist;
    Mcsf::Vector2f vec_1;
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

void RDPPolygonReduce_i(const Mcsf::Point2f* points, 
    int start_idx, int end_idx, char* bit_flags, float epsilon)
{
    bit_flags[start_idx/8] |= (0x01 << (start_idx % 8));
    bit_flags[end_idx/8] |= (0x01 << (end_idx % 8));
    if (end_idx - start_idx < 2)
    {
        return;
    }

    auto maxDistPt = CalcMaxDist_i(points, start_idx, end_idx);
    if (maxDistPt.second < epsilon)
    {
        for (int i = start_idx + 1; i < end_idx; ++i)
        {
            bit_flags[i / 8] &= ~(0x01 << (i % 8));
        }
    }
    else
    {
        RDPPolygonReduce_i(points, start_idx, maxDistPt.first, bit_flags, epsilon);
        RDPPolygonReduce_i(points, maxDistPt.first, end_idx, bit_flags, epsilon);
    }
}

void TpsUtility::RDPPolygonReduce(std::vector<Mcsf::Point2f>& points, float epsilon)
{
    if (points.size() < 3)
    {
        return;
    }

    char* bit_flags = new char[points.size() / 8 + 1];
    memset(bit_flags, 0, (points.size() / 8 + 1) * sizeof(char));
    RDPPolygonReduce_i(points._Myfirst, 0, points.size() - 1, bit_flags, epsilon);

    std::vector<Mcsf::Point2f> result_list;
    for (int i = 0; i < points.size(); ++i)
    {
        if (bit_flags[i/8] & (0x01<<(i%8)))
        {
            result_list.push_back(points[i]);
        }
    }
    points = std::move(result_list);
    delete[] bit_flags;
}

void TpsUtility::GaussianSmoothingWithoutShrinkage(std::vector<Mcsf::Point2f>& points)
{
    int size = points.size();
    std::vector<Mcsf::Vector2f> vectors;
    vectors.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        vectors.push_back(points[(i+1)%size] - points[i]);
    }

    float w = 0.5f;
    std::vector<Mcsf::Vector2f> vec_avg;
    vec_avg.reserve(size);
    vec_avg.push_back(vectors[0] - vectors[size - 1]);
    for (int i = 1; i < size; ++i)
    {
        vec_avg.push_back(w * (vectors[i] - vectors[i-1]));
    }

    float lambda = 0.33f;
    float mu = -0.34f;
    int iteration_no = 30;
    while(iteration_no--)
    {
        for (int j = 0; j < size; ++j)
        {
            points[j] += lambda * vec_avg[j];
        }
        for (int j = 0; j < size; ++j)
        {
            points[j] += mu * vec_avg[j];
        }
    }
}

void TpsUtility::PolygonCubicInterpolation(std::vector<Mcsf::Point2f>& points, int inter_num)
{
    if (points.size() <= 3 || inter_num < 1) return;
    //make a loop
    int n = points.size();
    points.push_back(points.front());
    //calculate differential
    float* diff_1st = new float[n];
    float* delta_x = new float[n];
    float* delta_y = new float[n];
    //float infinite_val = std::numeric_limits<float>::max();
    for (int i = 0; i < n; ++i)
    {
        delta_x[i] = points[i + 1].GetX() - points[i].GetX();
        delta_y[i] = points[i + 1].GetY() - points[i].GetY();
        diff_1st[i] = /*fabs(delta_x[i]) > 1e-6 ? */delta_y[i] / delta_x[i]/* : */
            /*TPS_FLOAT_SIGN(delta_y[i]) * infinite_val*/;
        if (fabs(delta_x[i]) <= 1e-6) printf("\nbad thing occurs!");
    }
    //construct the tridiagonal matrix
    float a,b;
    float* c = new float[n];
    float* d = new float[n + 1];
    a = 0; b = 2; c[0] = 1 / b; d[0] = 3 * diff_1st[0] / b;
    float temp;
    for (int i = 1; i < n; ++i)
    {
        a = diff_1st[i];
        b = 2 * (delta_x[i] + delta_x[i - 1]);
        temp = b - a * c[i - 1];
        c[i] = diff_1st[i - 1] / temp;
        d[i] = 3 * (delta_x[i] * diff_1st[i - 1] + delta_x[i - 1] * diff_1st[i]);
        d[i] = (d[i] - a * d[i - 1]) / temp;
    }
    a = 1; b = 2; temp = b - a * c[n - 1];
    d[n] = (3 * diff_1st[n - 1] - a * d[n - 1]) / temp;
    //get the k values
    float* k = new float[n + 1];
    k[n] = d[n];
    for (int i = n - 1; i >= 0; --i)
    {
        k[i] = d[i] - c[i] * k[i+1];
    }

    float* cubic_para_a = new float[n];
    float* cubic_para_b = new float[n];
    for (int i = 0; i < n; ++i)
    {
        cubic_para_a[i] = k[i] * delta_x[i] - delta_y[i];
        cubic_para_b[i] = -k[i+1] * delta_x[i] + delta_y[i];
    }

    //begin interpolation
    std::vector<Mcsf::Point2f> new_list;
    new_list.reserve(n * (inter_num + 1));
    float interval = 1.f / (inter_num + 1);
    float x, y, t;
    for (int i = 0; i < n - 1; ++i)
    {
        new_list.push_back(points[i]);
        if (fabs(delta_x[i]) < 1e-6) continue;
        for (int j = 1; j <= inter_num; ++j)
        {
            t = interval * j;
            x = points[i].GetX() + delta_x[i] * t;
            y = (1 - t) * points[i].GetY() + t * points[i+1].GetY() + 
                t * (1 - t) * (cubic_para_a[i] * (1 - t) + cubic_para_b[i] * t);
            new_list.push_back(Mcsf::Point2f(x, y));
        }
    }

    TPS_DEL_ARRAY(delta_x);
    TPS_DEL_ARRAY(delta_y);
    TPS_DEL_ARRAY(diff_1st);
    TPS_DEL_ARRAY(c);
    TPS_DEL_ARRAY(d);
    TPS_DEL_ARRAY(cubic_para_a);
    TPS_DEL_ARRAY(cubic_para_b);
    points = std::move(new_list);
}

void TpsUtility::BezierSmoothing(std::vector<Mcsf::Point2f>& points, float angle_threshold)
{
    if (points.size() < 3) return;

    points.push_back(points.front());

    int inter_num = 10;
    float interval = 1.f / (inter_num + 1), t = 0.f;
    float rad_threshold = angle_threshold * PI / 180;
    std::vector<Mcsf::Point2f> new_list;
    Mcsf::Vector2f vec1, vec2;
    Mcsf::Point2f p0, p1, p2, point;
    for (int i = 0; i < points.size() - 2; i += 3)
    {
        p0 = points[i]; p1 = points[i+1]; p2 = points[i+2];
        vec1 = p1 - p0; vec2 = p2 - p1;
        if (fabs(vec1.AngleBetween(vec2)) > rad_threshold)
        {
            new_list.push_back(p0);
            new_list.push_back(p1);
            new_list.push_back(p2);
            continue;
        }
        
        t = 0.f;
        for (int j = 0; j < inter_num + 2; ++j, t += interval)
        {
            point = (1 - t) * (1 - t) * p0 + 2 * (1 - t) * t * p1 + t * t * p2;
            new_list.push_back(point);
        }
    }
    points = std::move(new_list);
}

void RemoveSlivers(std::vector<std::vector<Mcsf::Point2f> >& vvContour)
{
    int i;
    float   dx1, dy1, dx2, dy2;
    float   len, dot;
    std::vector<Mcsf::Point2f>::iterator iter, pre, post;
    for (i = 0; i < vvContour.size(); ++i){
        if (vvContour[i].size() <= 3) continue;
        iter = vvContour[i].begin();
        for (; iter != vvContour[i].end();){
            if(iter == vvContour[i].begin()) pre = vvContour[i].end() - 1;
            else pre = iter - 1;
            if(iter == vvContour[i].end() - 1) post = vvContour[i].begin();
            else post = iter + 1;
            dx1 = iter->GetX() - pre->GetX();
            dy1 = iter->GetY() - pre->GetY();
            dx2 = post->GetX() - iter->GetX();
            dy2 = post->GetY() - iter->GetY();
            len = sqrt(dx1*dx1 + dy1*dy1);
            if (len == 0.0) {
                iter = vvContour[i].erase(iter); 
                continue;
            }
            dx1 /= len;
            dy1 /= len;
            len = sqrt(dx2*dx2 + dy2*dy2);
            if (len == 0.0) {
                iter = vvContour[i].erase(iter);
                continue;
            }
            dx2 /= len;
            dy2 /= len;

            dot = dx1*dx2 + dy1*dy2;
            if (dot > 0.999) {
                iter = vvContour[i].erase(iter);
            }
            else{
                iter++;
            }
        }
    }
}

void ResampleCurveByMeanValue(std::vector<std::vector<Mcsf::Point2f>>& vvContour)
{
    int sample = 0;
    float    dx1, dy1, dx2, dy2;
    std::vector<std::vector<Mcsf::Point2f> > tempVVContour;
    std::vector<Mcsf::Point2f> tempVContour;
    //RemoveSlivers(vvContour);
    for (int i = 0; i < vvContour.size(); ++i){
        tempVContour.clear();
        if (vvContour[i].size() <= 4) {
            tempVContour = vvContour[i];
            tempVVContour.push_back(tempVContour);
            continue;
        }
        else {
            tempVContour.push_back(vvContour[i][0]);
            for (int j = 1; j < vvContour[i].size() - 1; ++j){
                sample = j - 1;
                dx1 = vvContour[i][j].GetX() - vvContour[i][sample].GetX();
                dy1 = vvContour[i][j].GetY() - vvContour[i][sample].GetY();
                sample = j + 1;
                dx2 = vvContour[i][j].GetX() - vvContour[i][sample].GetX();
                dy2 = vvContour[i][j].GetY() - vvContour[i][sample].GetY();
                if (sqrt(dx1 * dx1 + dy1 * dy1) < 0.032 && sqrt(dx2 * dx2 + dy2 * dy2) < 0.032) {
                    tempVContour.push_back((vvContour[i][j] + vvContour[i][j + 1]) /2);
                }
                else
                {
                    tempVContour.push_back(vvContour[i][j]);
                }
            }
            tempVContour.push_back(vvContour[i][vvContour[i].size() - 1]);
            tempVVContour.push_back(tempVContour);
        }
    }
    vvContour.clear();
    vvContour = tempVVContour;
}

void TpsUtility::SmoothCurve(std::vector<std::vector<Mcsf::Point2f> >& vvContour)
{
    RemoveSlivers(vvContour);

    //Smooth Method 1:
    //int jaggedPixels = 16;
    //BezierCurve(vvContour, jaggedPixels);

    //Smooth Method 2:
    ResampleCurveByMeanValue(vvContour);

    // Remove Slivers
    RemoveSlivers(vvContour);
}

double TpsUtility::ToClockPosition(double angle)
{
    double res = angle - floor(angle / 360) * 360;
    if (fabs(res - 360) <= 1e-3)
    {
        res = 0.0;
    }
    return res;
}

float TpsUtility::Epsilon(float v)
{
    int* p = (int*)&v;
    unsigned char e = (*p & 0x7f800000) >> 23;
    if (e != 255)
    {
        //int e_bin = (e == 0 ? 1 : e ) - 127 - 23;
        int e_dec = (e == 0) ? -6 : (e - 125 - 23) / 10 * 3;
        return pow(10.f, e_dec);
    }
    //infinite or NAN
    else return v;
}

double TpsUtility::Epsilon(double v)
{
    long long* p = (long long*)&v;
    unsigned short e = (*p & 0x7ff0000000000000) >> 52;
    if (e != 2047)
    {
        //int e_bin = (e == 0 ? 1 : e ) - 1023 - 52;
        int e_dec = (e == 0) ? -15 : (e - 1023 - 52) / 10 * 3;
        return pow(10.0, e_dec);
    }
    //infinite or NAN
    else return v;
}

float TpsUtility::CalcPolygonArea(Mcsf::Point3f* V, int n, const Mcsf::Vector3f& N)
{
    float area = 0;
    float an, ax, ay, az; // abs value of normal and its coords
    int  coord;           // coord to ignore: 1=x, 2=y, 3=z

    if (n < 3) return 0;  // a degenerate polygon

    // select largest abs coordinate to ignore for projection
    ax = (N[0]>0 ? N[0] : -N[0]);    // abs x-coord
    ay = (N[1]>0 ? N[1] : -N[1]);    // abs y-coord
    az = (N[2]>0 ? N[2] : -N[2]);    // abs z-coord

    coord = 3;                    // ignore z-coord
    if (ax > ay) {
        if (ax > az) coord = 1;   // ignore x-coord
    }
    else if (ay > az) coord = 2;  // ignore y-coord

    // compute area of the 2D projection
    switch (coord) {
    case 1:
        for (int i = 0; i < n; ++i)
            area += (V[(i+1)%n][1] * (V[(i+2)%n][2] - V[i][2]));
        break;
    case 2:
        for (int i = 0; i < n; ++i)
            area += (V[(i+1)%n][2] * (V[(i+2)%n][0] - V[i][0]));
        break;
    case 3:
        for (int i = 0; i < n; ++i)
            area += (V[(i+1)%n][0] * (V[(i+2)%n][1] - V[i][1]));
        break;
    }

    // scale to get area before projection
    an = sqrt( ax*ax + ay*ay + az*az); // length of normal vector
    switch (coord) {
    case 1:
        area *= (an / (2 * N[0]));
        break;
    case 2:
        area *= (an / (2 * N[1]));
        break;
    case 3:
        area *= (an / (2 * N[2]));
    }
    return fabs(area);
}

bool TpsUtility::IsInPolygon(Mcsf::Point3f* V, int n, 
    const Mcsf::Vector3f& N, const Mcsf::Point3f& p)
{
    if (n < 3) return false;

    Mcsf::Point3f p1;
    Mcsf::Vector3f vx;
    for (int i = 0; i < n; ++i)
    {
        p1 = (V[i] + V[(i+1)%n]) * 0.5;
        vx = p1 - p;
        if(vx.Length() <= 1e-6) continue;
        vx.Normalize();
        break;
    }
    Mcsf::Vector3f vy = N.CrossProduct(vx);
    vy.Normalize();

    Mcsf::Vector3f vec = V[0] - p;
    float x1 = vec.DotProduct(vx);
    float y1 = vec.DotProduct(vy);
    float x2, y2, lambda;
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
TPS_END_NAMESPACE