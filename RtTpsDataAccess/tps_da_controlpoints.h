#ifndef TPS_DA_CONTROLPOINTS_H_
#define TPS_DA_CONTROLPOINTS_H_

#include "McsfMedViewer3DArithmetic/arithmetic.h"
TPS_BEGIN_NAMESPACE  // begin namespace tps


TPS_DA_DECLSPEC class TpsControlPoints
{
public:
    //the constructor and destructor
    TpsControlPoints();
    ~TpsControlPoints();

    Vector3D<Point3D> GetPoint();
    Vector3D<Point3D> SetPoint();

private:
    Vector3D<Point3D> controlpoint;
    int pCount;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsControlPoints);
};

TPS_END_NAMESPACE  // end namespace tps

#endif