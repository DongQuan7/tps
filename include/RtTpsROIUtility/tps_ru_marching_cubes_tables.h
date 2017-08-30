#ifndef TPS_RU_MARCHING_CUBES_TABLES_H_
#define TPS_RU_MARCHING_CUBES_TABLES_H_

#include "RtTpsROIUtility/tps_ru_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_RU_DECLSPEC MarchingCubesTables
{
public:
    static unsigned short mEdgeTable[256];
    static char mTriTable[256][16];
    static unsigned char mVertexCountTable[256];
    static unsigned char mEdgeVertexTable[12];
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RU_MARCHING_CUBES_TABLES_H_