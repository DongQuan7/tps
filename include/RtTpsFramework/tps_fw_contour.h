#ifndef TPS_FW_CONTOUR_H_
#define TPS_FW_CONTOUR_H_

#include "tps_fw_defs.h"

#include "RtTpsFramework/tps_fw_arithmetic.h"

#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_FW_DECLSPEC Contour
{
public:
    Contour();
    ~Contour();
    Contour(const Contour& contour);
    Contour& operator =(const Contour& contour);

    inline int VertexCount() const { return pt.size(); }

    //private:
    //int vertex_count;
    int slice_number;   /* number of associated CT slice, start with 0*/
    //::Mcsf::Point3f max;  /* extent of prism represented by this contour */
    //::Mcsf::Point3f min;
    std::vector<PT3> pt;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_FW_CONTOUR_H_