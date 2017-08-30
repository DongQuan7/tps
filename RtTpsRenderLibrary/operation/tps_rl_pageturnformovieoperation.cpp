////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_pageturnformovieoperation.cpp
/// 
///  \brief class movie page turn op
/// 
///  \version 1.0
/// 
///  \date    2014/6/23
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_pageturnformovieoperation.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
//#include "McsfMedViewer3DArithmetic/arithmetic.h"


#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"




TPS_BEGIN_NAMESPACE

MoviePageTurnOperation::MoviePageTurnOperation(const std::string &imageUid, 
 int sliceNum) : mImageUID(imageUid),mSliceNum(sliceNum) {

};

MoviePageTurnOperation::~MoviePageTurnOperation() {

}

bool MoviePageTurnOperation::ModifyGraphicObject() const {
    using namespace Mcsf::MedViewer3D;

    // get volume GO
    std::string volumeKey = mImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto volumeGO = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(volumeGO);
    if(nullptr == volumeGO || nullptr == volume) {
        TPS_LOG_DEV_ERROR<<"failed to get volume go!";
        return false;
    }

    //get mpr GO
    auto goType = GOTypeHelper::ComposeAsGOType(MPR, AXIAL);
    std::string mprKey = mImageUID + "|" + GOTypeHelper::ToString(goType);
    auto mprGO = mModelWarehouse->GetModelObject(mprKey);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(mprGO);
    if(nullptr == mprGO || nullptr == mpr) {
        TPS_LOG_DEV_ERROR<<"failed to get mpr go!";
        return false;
    }

    double spacing[3];
    volume->GetSpacing(spacing);
    
    auto matP2W = volume->GetModel2PatientMat().Inverse();
    auto matM2W = volume->GetModel2World();
    matP2W.Prepend(matM2W);

    Vector3D vec(0,0,1);
    double lenth = -1 * matP2W.Transform(vec).Magnitude()*spacing[2];
    vec *= lenth;

    size_t dim[3];
    volume->GetDimension(dim);
    if(mSliceNum <0 || mSliceNum >= dim[2]) {
        TPS_LOG_DEV_ERROR<<"slice number out of range!";
        return false;
    }

    Matrix4x4 matModel2Volume(
        dim[0], 0, 0, 0,
        0, dim[1], 0, 0, 
        0, 0, dim[2], 0,
        -0.5, -0.5, -0.5, 1);

    Point3D vecticesInVolume[4] = {
        Point3D(0,dim[1],0),
        Point3D(dim[0],dim[1],0),
        Point3D(dim[0],0,0),
        Point3D(0,0,0)
    };

    Point3D vercticesInWorld[4];

    Matrix4x4 matVolume2World = matM2W * matModel2Volume.Inverse();

    for(int i=0; i <4; ++i) {
        vercticesInWorld[i] = matVolume2World.Transform(vecticesInVolume[i]);
    }

    Point3D vertices[4];
    for(int i=0; i<4; ++i) {
        vertices[i] = vercticesInWorld[i] + mSliceNum * vec;
    }

    MPRAPI::CalculateMPRMatrix(mpr,vertices);

    mpr->SetDirty(true);

    return true;
}



TPS_END_NAMESPACE