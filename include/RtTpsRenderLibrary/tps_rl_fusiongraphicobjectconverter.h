////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_fusiongraphicobjectconverter.h
/// 
///  \brief class FusionGoConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/20
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_FUSIONGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_FUSIONGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"

namespace Mcsf{
    namespace MedViewer3D{
        class Texture3D;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsImage3DEntity;
typedef std::shared_ptr<Mcsf::MedViewer3D::Texture3D> Tex3DPtr;
/// \class FusionGoConverter
/// 
/// \brief *****
class TPS_RL_DECLSPEC FusionGoConverter : public TpsGOConverterBase {
public:
    //the constructor and destructor
    FusionGoConverter(GO_TYPE goType);
    virtual ~FusionGoConverter();

//     inline void SetFixedImage(TpsImage3DEntity* fixedImage){
//         mFixedImage = fixedImage;
//     }
//     inline void SetFloatImageUID(const std::string& floatImageUID){
//         mFloatImageUID = floatImageUID;
//     }
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
    virtual GraphicObjectPtr Convert(const std::string& /*uid*/) { return nullptr; }
    virtual bool Update(const std::string& /*uid*/, GraphicObjectPtr /*go*/) { return false; }

protected:
    bool LoadTexture_i(Tex3DPtr texture, void* buffer);

    TpsImage3DEntity*       mFixedImage;
    std::string             mFloatImageUID;
    int                     mAlg;
    void*                   mNonRigidBuffer;
    Mcsf::Matrix4f          mRigidMat;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(FusionGoConverter);
};

class TPS_RL_DECLSPEC VaryingFusionGoConverter : public FusionGoConverter 
{
public:
    //the constructor and destructor
    VaryingFusionGoConverter(GO_TYPE goType);
    virtual ~VaryingFusionGoConverter();

protected:
    virtual bool PrepareSourceData_i(const GOConverterParameter& para);
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(VaryingFusionGoConverter);
};
TPS_END_NAMESPACE  // end namespace tps

#endif
