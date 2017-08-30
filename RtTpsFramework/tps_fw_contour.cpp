#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_contour.h"

#include <string.h>

TPS_BEGIN_NAMESPACE

Contour::Contour() : slice_number(-1)
{

}

Contour::~Contour()
{
    pt.clear();
}

Contour::Contour(const Contour& contour) : pt(contour.pt), slice_number(contour.slice_number)
{
    //pt = new PT3[contour.vertex_count];
    //vertex_count = contour.vertex_count;
    //memcpy(pt, contour.pt, vertex_count * sizeof(PT3));

    pt = contour.pt;
    slice_number = contour.slice_number;
}

Contour& Contour::operator=(const Contour& contour)
{
//     if (vertex_count != contour.vertex_count || pt == nullptr)
//     {
//         if (pt) delete[] pt;
//         pt = new PT3[contour.vertex_count];
//     }
//     vertex_count = contour.vertex_count;
//     memcpy(pt, contour.pt, vertex_count * sizeof(PT3));

    pt = contour.pt;
    slice_number = contour.slice_number;
    return *this;
}

TPS_END_NAMESPACE