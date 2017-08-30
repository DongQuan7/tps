////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_ap_simple_alg.h
/// 
///  \brief class SimpleAlg declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/07/28
////////////////////////////////////////////////////////////////

#ifndef TPS_AP_SIMPLE_ALG_H_
#define TPS_AP_SIMPLE_ALG_H_

#include "tps_ap_defs.h"

namespace Mcsf
{
    class Point2f;
    class Point3f;
    class Vector3f;
}

TPS_BEGIN_NAMESPACE

class TPS_AP_DECLSPEC TpsUtility
{
public:
    //An implementation of Ramer Douglas Peucker Algorithm
    //reduce the polygon size
    static void RDPPolygonReduce(std::vector<Mcsf::Point2f>& points, float epsilon = 1.0f);

    static void GaussianSmoothingWithoutShrinkage(std::vector<Mcsf::Point2f>& points);

    //inter_num means the number of points to be inserted into between every two points of original list
    static void PolygonCubicInterpolation(std::vector<Mcsf::Point2f>& points, int inter_num = 3);

    static void BezierSmoothing(std::vector<Mcsf::Point2f>& points, float angle_threshold = 3);

    static void SmoothCurve(std::vector<std::vector<Mcsf::Point2f> >& vvContour);

    static double ToClockPosition(double angle);

    //将逗号分隔的字符串分隔成vector<T>
    template <typename T>
    static bool StringToVector(std::string source, std::vector<T>& target){
        if (source == "")
        {
            return true;
        }

        target.clear();
        std::vector<std::string> vecValue;
        (void)boost::split(vecValue, source, boost::is_any_of(","));
        size_t iLen = vecValue.size();
        target.resize(iLen);
        for (int i = 0; i < iLen; i++)
        {
            if (vecValue[i] != "")
                target[i] = static_cast<T>(atof(vecValue[i].c_str()));
        }
        vecValue.clear();
        return true;
    }

    //calculate IEEE Float point value decimal epsilon
    static float Epsilon(float v);

    //calculate IEEE Float point value decimal epsilon
    static double Epsilon(double v);

    //ceil function exclude float point precision issue
    template <typename T>
    static T Ceil(T v)
    {
        T flr = floor(v);
        return (v - flr) <= Epsilon(flr) ? flr : ceil(v);
    }
    //floor function exclude float point precision issue
    template <typename T>
    static T Floor(T v)
    {
        T cl = ceil(v);
        return (cl - v) <= Epsilon(cl) ? cl : floor(v);
    }

    static float CalcPolygonArea(Mcsf::Point3f* V, int n, const Mcsf::Vector3f& N);

    static bool IsInPolygon(Mcsf::Point3f* V, int n, const Mcsf::Vector3f& N, const Mcsf::Point3f& p);
};

TPS_END_NAMESPACE

#endif