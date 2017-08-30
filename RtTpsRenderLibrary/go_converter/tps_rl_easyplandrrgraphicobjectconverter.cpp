////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  yilong.cheng  mailto:yilong.cheng@united-imaging.com
/// 
///  \file tps_rl_easyplangraphicobjectconverter.h
/// 
///  \brief class TDAIGRT EasyPlanGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014-10-29
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_easyplandrrgraphicobjectconverter.h"
#include <math.h>
//MCSF
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"

//TMS
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"
//TPS
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"


#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"


using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

TpsEasyPlanDrrGOConverter ::TpsEasyPlanDrrGOConverter (GO_TYPE goType)
: TpsGOConverterBase(goType),mImage(nullptr){

}

TpsEasyPlanDrrGOConverter ::~TpsEasyPlanDrrGOConverter (){

}

GraphicObjectPtr TpsEasyPlanDrrGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<EasyPlanDrrGraphicObejct>(
        new EasyPlanDrrGraphicObejct());
}

bool TpsEasyPlanDrrGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    mImage = nullptr;
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    if (true != mDataRepository->GetImageDataManager()->GetImage2DData(it->second, &mImage)){
        TPS_LOG_DEV_ERROR<<"Image 2d entity with UID: "<<it->second << " does not exist.";
        return false;
    }
    return true;
}

bool TpsEasyPlanDrrGOConverter::Update_i(GraphicObjectPtr go){
    auto easyPlanGO = std::dynamic_pointer_cast<EasyPlanDrrGraphicObejct>(go);
    if (mImage == nullptr || easyPlanGO == nullptr) return false;

    int width,height,winCenter,winWidth;
    unsigned long len;
    char* buffer;
    std::string imageUID;
    int bitsNumber = 0;
    std::string pixelRepresentation;
    DATATYPE dataType = NONE_DATA_TYPE;
    mImage->GetUID(imageUID);
    mImage->GetImageSize(width,height);
    mImage->GetImage2DBuffer(buffer);
    mImage->GetImageWindowingInfo(winCenter, winWidth);
    len = mImage->GetBufferLen();
    bitsNumber = mImage->get()->get_bitsallocated();
    pixelRepresentation = mImage->get()->get_pixelrepresentation();//0表示无符号 1表示有符号
    if (8 == bitsNumber && pixelRepresentation == "0"){
        dataType = DATA_TYPE_UNSIGNED_CHAR;
    } else if (8 == bitsNumber && pixelRepresentation == "1") {
        dataType = DATA_TYPE_CHAR;
    } else if (16 == bitsNumber && pixelRepresentation == "0") {
        dataType = DATA_TYPE_UNSIGNED_SHORT;
    } else if (16 == bitsNumber && pixelRepresentation == "1") {
        dataType = DATA_TYPE_SHORT;
    } else if (32 == bitsNumber) {
        dataType = DATA_TYPE_FLOAT;
    } else if (64 == bitsNumber) {
        dataType = DATA_TYPE_DOUBLE;
    }
    if (!easyPlanGO->Initialize()){
        TPS_LOG_DEV_ERROR<<"Failed to initialize DRR Fusion graphic object of \
                           image data with UID: "<<imageUID;
        return false;
    }
    //auto section = tps::GOTypeHelper::GetFirstPostfixComponent(mGOType);
    std::pair<float,float> imagePixalSpacing(
        mImage->get()->get_firstimageplanepixelspacing(), 
        mImage->get()->get_secondimageplanepixelspacing());
    float defaultUnitInDrrModel = max((width - 1) * imagePixalSpacing.first, 
            (height - 1) * imagePixalSpacing.second);
    float unitXInImgModel = width * imagePixalSpacing.first;
    float unitYInImgModel = height * imagePixalSpacing.second;
    Mcsf::Matrix3f matrix(Mcsf::Vector3f(0, 0, 1), Mcsf::Vector3f(1, 0, 1), Mcsf::Vector3f(0, 1, 1));
    Mcsf::Matrix3f matModel2W(Mcsf::Vector3f(0.5f, 0.5f, 1), 
                              Mcsf::Vector3f(defaultUnitInDrrModel / unitXInImgModel + 0.5f, 0.5f, 1), 
                              Mcsf::Vector3f(0.5f, 0.5f - 1 * defaultUnitInDrrModel / unitYInImgModel, 1));
    matModel2W.Append(matrix.Inverse());

    std::string planUID = mImage->get()->get_planuid();
    std::string opposedImageUID = "";
    mDataRepository->GetImageDataManager()->GetEasyPlanOpposedImageUID(planUID, &opposedImageUID);
    if(opposedImageUID == mImage->get()->get_uid()) {
        easyPlanGO->SetIsOpposedImage(true);
    }

    easyPlanGO->SetDrrModel2ImageModel(matModel2W);
    easyPlanGO->SetInitialDrrModel2ImageModel(matModel2W);
    easyPlanGO->SetImageBuffer(buffer,width,height,len,dataType);
    easyPlanGO->SetWindowingWinWidthAndWinCenter( winWidth, winCenter);

    return true;
}

void TpsEasyPlanDrrGOConverter::SetImage2DData(TpsImage2DEntity* image)
{
    if (image == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"SetImage2DData input image is null.";
        return;
    }
    mImage = image;
}
TPS_END_NAMESPACE  // end namespace tps
