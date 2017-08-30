#ifndef TPS_BL_CMD_EDIT_ROI_VIA_NEW_CONTOUR_H_
#define TPS_BL_CMD_EDIT_ROI_VIA_NEW_CONTOUR_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class Point3D;
    }
}


TPS_BEGIN_NAMESPACE

struct CertifiedVOIEditInfo;
struct CertifiedVOI;
class ContourSet;
class TpsImage3DEntity;
class ROIEntity;

class TPS_BL_DECLSPEC EditROIViaContourCmd : public TpsVOIRedoUndoCacheCmd
{
public:
    EditROIViaContourCmd(CertifiedVOIEditInfo& certifiedEditVOI);
    virtual ~EditROIViaContourCmd(void);

    virtual int Undo();
    virtual int Redo();
protected:
    virtual int Execute();
    virtual TpsRedoUndoCommand* Clone();
private:
    bool UpdateVOI_i(const std::string &voiUID);

    void CombineNewContour_i(
        const std::vector<Mcsf::MedViewer3D::Point3D>& points, 
        ContourSet* cs);
private:
    CertifiedVOIEditInfo& mCertifiedSmartContour;
    CertifiedVOI* mCertVOI;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_EDIT_ROI_VIA_NEW_CONTOUR_H_