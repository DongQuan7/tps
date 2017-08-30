#ifndef TPS_RU_POLYGON_UTILITY_H_
#define TPS_RU_POLYGON_UTILITY_H_

#include "tps_ru_defs.h"
#include <vector>
#include "RtTpsFramework/tps_fw_arithmetic.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

typedef std::vector<PT2> POLYGON;
typedef std::vector<POLYGON> POLYGON_SET;
typedef std::vector<PT3> POLYGON3;

class TPS_RU_DECLSPEC PolygonUtility
{
public:
    enum Operator { INTER, UNION, SUB, XOR };
    static POLYGON_SET Logic(const POLYGON_SET& ps1, const POLYGON_SET& ps2, Operator op);
    static POLYGON_SET Logic(const POLYGON_SET& ps, const POLYGON& p, Operator op);

    static double Area(PT3* V, int n, const VEC3& N);
    static bool IsIn(PT3* V, int n, const VEC3& N, const PT3& p);

    //An implementation of Ramer Douglas Peucker Algorithm
    //reduce the polygon size
    static void RDPPolygonReduce(POLYGON3& polygon, const VEC3& n, float epsilon = 1.0f);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RU_POLYGON_UTILITY_H_