////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_doselinegraphicobject.h
/// 
///  \brief class DoseLineGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/11
/// 
///  \modified by maoliang.gong(maoliang.gong@united-imaging.com)
///  \brief  refactor dose graphic object
///  \date    Jan. 13, 2014
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DOSELINEGRAPHICOBJECT_H
#define TPS_RL_DOSELINEGRAPHICOBJECT_H

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/vector3d.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"


TPS_BEGIN_NAMESPACE
class DoseGrid;

class TPS_RL_DECLSPEC TpsMPRDoseGraphicObject : public TpsGraphicObjectBase {
public:

    //the constructor and destructor
    TpsMPRDoseGraphicObject();
    ~TpsMPRDoseGraphicObject();

    //////////////////////////////////////////////////////////////////////////
    // \brief set dosegrid to dose line go;
    // \param[in] buffer  the dose grid buffer;
    // \param[in] height  the height of the dose grid buffer;
    // \param[in] width   the width of the dose grid buffer;
    // \return bool  true if succeeded, false otherwise
    // \note  this method will copy the memory of input |buffer|, 
    //        so release the buffer if needed.
    bool SetDoseGrid(float* buffer,int height, int width, 
                     const Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane);

    //////////////////////////////////////////////////////////////////////////
    // \brief get the dose grid buffer;
    // \param[out] buffer  the dose grid buffer
    // \param[out] height  the height of the dose grid buffer;
    // \param[out] width   the width of the dose grid buffer;
    void GetDoseGrid(float*& buffer,int& height, int& width, 
                     Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane) const;

private:
    float* mDosePlaneBuffer;
    int    mHeight;
    int    mWidth;

    Mcsf::MedViewer3D::Matrix4x4                mWorldToNormalPlaneTM;
};
TPS_END_NAMESPACE
#endif