
#ifndef TPS_RL_MASKTOMESH_H_
#define TPS_RL_MASKTOMESH_H_


#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "tps_ap_defs.h"

#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"

//using namespace Mcsf;

namespace OpenMesh{
    struct DefaultTraits;
    template <class T>
    class TriMesh_ArrayKernelT;
    struct EdgeHandle;
    struct VertexHandle;
}

TPS_BEGIN_NAMESPACE

typedef OpenMesh::TriMesh_ArrayKernelT<OpenMesh::DefaultTraits>  MyMesh;

typedef unsigned long       DWORD;
typedef unsigned int        UINT;

struct TriMeshFace
{
    TriMeshFace() {}
    TriMeshFace(DWORD I0, DWORD I1, DWORD I2)
    {
        I[0] = I0;
        I[1] = I1;
        I[2] = I2;
    }

    DWORD I[3];
};

bool TPS_AP_DECLSPEC MaskToMeshByMarchingCube(unsigned char* pAllMask, 
    int* iDim, unsigned char ucMaskID, MyMesh& mesh);


TPS_END_NAMESPACE
#endif
