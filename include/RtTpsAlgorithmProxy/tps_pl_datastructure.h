////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file    tps_pl_datastructure.h
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    2014/06/06
////////////////////////////////////////////////////////////////

#ifndef TPS_PL_DATASTRUCTURE_H_
#define TPS_PL_DATASTRUCTURE_H_

#include "tps_defs.h"

TPS_PLUNC_BEGIN_NAMESPACE
typedef struct {
    float x;
    float y;
    float z;
 //   PNT3D():x(0),y(0),z(0){};
 //   PNT3D(float _x, float _y, float _z):x(_x),y(_y),z(_z){};    
}PNT3D;


typedef struct {
    int vertex_count;
    int slice_number;   /* number of associated CT slice */
    PNT3D max;          /* extent of prism represented by this contour */
    PNT3D min;
    float z;            /* nominal slice position  */
    float *x;
    float *y;
} CONTOUR_STC;

typedef struct ANASTRUCT_struct {
    int         contour_count;
    CONTOUR_STC *contours;

    PNT3D       max;
    PNT3D       min;
} ANASTRUCT;


struct LEAF_POSITION_STC{
    int independent;
    float pos_1;        // 1 is left or bottom, 2 is right or top 
    float pos_2;        // These are jaw positions (0.0 is at c-axis) 
    float min_w;
    float max_w;
};


typedef struct {
    int    x_dim, y_dim, z_dim;
    int    slicecnt;
    float  x_start, y_start, z_start;
    float  x_scale, y_scale, z_scale;
    int    slice_valid[3];
    float  inv_x_scale, inv_y_scale, inv_z_scale;
    char  *data;
} SCAN3D_MAP;

typedef struct edge_struct {
    int    iyo, iyf;
    float  yo, yf;
    float  xo, xf, dx;
    float  zo, zf, dz;
    struct 
        edge_struct  *next;
} edge_type;

TPS_PLUNC_END_NAMESPACE



#endif
