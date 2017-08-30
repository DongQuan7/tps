#ifndef TPS_RU_COMMON_H_
#define TPS_RU_COMMON_H_

#include "RtTpsROIUtility/tps_ru_defs.h"
#include "cuda_runtime.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

#define BLOCK_SIZE 16
#define NEIGHBOR_SHIFT 2

struct MS_MAP
{
    int2 map_size;

    //0x01 row, 0x02 column, 0x04 center belongs to flag, square id for high 4 bits.
    char* label_map;
    size_t l_pitch;

    float3* row_vertex_map;
    float3* column_vertex_map;
    size_t v_pitch;
};

struct contour 
{
    float2* points;
    int count;
    //0 no hat, 1 down hat, 2 up hat, 3 up & down hat
    char hat_flag;
    bool is_dirty;
};
TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RU_COMMON_H_