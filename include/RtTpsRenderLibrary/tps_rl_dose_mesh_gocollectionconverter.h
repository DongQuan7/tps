////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_dose_mesh_goconverter.h
/// 
///  \brief class DoseMeshGraphicObjectConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/06/08
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DOSE_MESH_GOCONVERTER_H_
#define TPS_RL_DOSE_MESH_GOCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include <vector>

namespace Mcsf{
    namespace MedViewer3D{
        class GraphicObjVolume;
    }
}

class DoseMeshGraphicObject;

TPS_BEGIN_NAMESPACE  // begin namespace tps
struct CONTOUR_struct;
typedef CONTOUR_struct TPS_CONTOUR_STC;
class RtPlan;
class RtNormgroup;

class TPS_RL_DECLSPEC DoseMeshGraphicObjectCollectionConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    DoseMeshGraphicObjectCollectionConverter(GO_TYPE goType);
    virtual ~DoseMeshGraphicObjectCollectionConverter();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief create new graphic object instance
    /// \return GraphicObjectPtr the new graphic object
    virtual GraphicObjectPtr CreateGraphicObject_i();

    //////////////////////////////////////////////////////////////////////////
    /// \brief get source data to be converted
    /// \param[in] uid  the UID of the TMS data
    /// \return bool  true if succeeded, false otherwise
    virtual bool PrepareSourceData_i(const GOConverterParameter& para);
    
    //////////////////////////////////////////////////////////////////////////
    /// \brief update graphic object according to source data
    /// \brief go  the graphic object pointer to be updated
    /// \return bool  true if succeeded, false otherwise
    virtual bool Update_i(GraphicObjectPtr go);

private:
    void sample_ii(unsigned char *bufferSrc,int xSrc,int ySrc,int zSrc,
        float &xScale, float &yScale, float &zScale,unsigned char*& bufferDes,
        int xDes, int yDes, int zDes);

    /// \brief update graphic object according to source data
    /// \brief go  the graphic object pointer to be updated
    /// \return bool  true if succeeded, false otherwise
    void UpdateInLineMode(GraphicObjectPtr go);

    bool ReduceContourByPlunc_i(TPS_CONTOUR_STC& contour, int smoothness);
    void CopyTpsContourSTC_i(TPS_CONTOUR_STC& target, TPS_CONTOUR_STC& contour);
    void ReleaseTpsContourSTC_i(TPS_CONTOUR_STC& target);
    void SplineInterpolation(TPS_CONTOUR_STC& contour);

protected:
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    mVolume;
    std::string                                             mImageUID;
    std::string                                             mPlanUID;
    RtPlan*                                                 mPlan;
    std::vector<RtNormgroup*>                               mNormgroupList;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(DoseMeshGraphicObjectCollectionConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_DOSE_MESH_GOCONVERTER_H_
