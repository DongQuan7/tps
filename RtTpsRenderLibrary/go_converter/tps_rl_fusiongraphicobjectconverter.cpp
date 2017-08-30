////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_fusiongraphicobjectconverter.cpp
/// 
///  \brief class FusionGoConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/20
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_fusiongraphicobjectconverter.h"

#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"

#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_texture_3d.h"
#include "McsfMedViewer3DDataRepresentation/data_config.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_fusiongraphicobject.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

//#include "RtTpsFramework/tps_fw_common_enums.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

FusionGoConverter::FusionGoConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), 
    mFixedImage(nullptr), 
    mFloatImageUID(""), 
    mNonRigidBuffer(nullptr)
{
    mRigidMat.SetIdentity();
}

FusionGoConverter::~FusionGoConverter(){

}

GraphicObjectPtr FusionGoConverter::CreateGraphicObject_i(){

    return std::shared_ptr<FusionGraphicObject>(new FusionGraphicObject());
}

bool FusionGoConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()){
        TPS_LOG_DEV_ERROR<<"Can not find IMAGE data source";
        return false;
    }
    std::string imageUid = it->second;
    it = para.data_source_map.find(FLOAT_IMAGE);
    if (it == para.data_source_map.end()){
        TPS_LOG_DEV_ERROR<<"Can not find FLOAT_IMAGE data source.";
        return false;
    }
    mFloatImageUID = it->second;
    //get fixed image entity
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        imageUid, &mFixedImage) || mFixedImage == nullptr){
        TPS_LOG_DEV_ERROR<<"The fixed image does not exist which uid: "<<imageUid;
        return false;
    }

    if (mFixedImage->GetDeformableRegistrationResult(mFloatImageUID, mNonRigidBuffer)){
        mAlg = NonRigid;
    }
    else if (mFixedImage->GetRegistrationMatrix(mFloatImageUID, mRigidMat)){
        mAlg = Rigid;
    }
    else{
        TPS_LOG_DEV_ERROR<<"Failed to get registration relationship.";
        return false;
    }

    return true;
}

bool FusionGoConverter::Update_i(GraphicObjectPtr go){
    auto fusionGo = std::dynamic_pointer_cast<FusionGraphicObject>(go);
    if (mFixedImage == nullptr || mFloatImageUID.empty() || fusionGo == nullptr) return false;

    fusionGo->SetRegAlg((RegAlg)mAlg);
    if (mAlg == NonRigid){
        if (mNonRigidBuffer == nullptr) {
            TPS_LOG_DEV_ERROR<<"The non rigid buffer is empty.";
            return false;
        }
        Tex3DPtr texVolume = fusionGo->GetFusionVolumeTexture();
        if (texVolume == nullptr){
            texVolume = Mcsf::MedViewer3D::Texture3D::Create();
        }
        if (!LoadTexture_i(texVolume, mNonRigidBuffer)){
            TPS_LOG_DEV_ERROR<<"Failed to load texture.";
            return false;
        }
        fusionGo->SetFusionVolumeTexture(texVolume);
    }
    else{
        fusionGo->SetRegistrationMatrix(
            tps::TpsArithmeticConverter::ConvertToMatrix4x4(mRigidMat));
    }

    fusionGo->SetFixedImageUID(mFixedImage->GetUID());
    fusionGo->SetFloatImageUID(mFloatImageUID);
    //fusionGo->SetFusionType(FUSION_TYPE_BLEND);
    return true;
}

bool FusionGoConverter::LoadTexture_i(Tex3DPtr texture, void* buffer){
    //TODO improve the performance here

    RtImage3DHeader* header = nullptr;
    mFixedImage->GetImage3DHeader(header);

    size_t dim[3] = { header->m_iXDim, header->m_iYDim, header->m_iSliceCount };
    int iBitsAllc = header->m_iBitsAllocated;
    int iPixelRepresentation = header->m_iDataType;
    int iDataType;
    int iStride = static_cast<int>(dim[0]);
    int iBytesPerPixel = 1;
    double minScalar = 0.0;
    if (iBitsAllc == 8 && iPixelRepresentation == 0){//8
        iDataType = MEDVIEWER3D_UCHAR;
        iStride *= sizeof(unsigned char);
        iBytesPerPixel *= sizeof(unsigned char);
    }
    else if (iBitsAllc == 16 && iPixelRepresentation == 0){//16
        iDataType = MEDVIEWER3D_USHORT;
        iStride *= sizeof(unsigned short);
        iBytesPerPixel *= sizeof(unsigned short);
    }
    else if (iBitsAllc == 16 && iPixelRepresentation == 1){
        iDataType = MEDVIEWER3D_SHORT;
        iStride *= sizeof(short);
        iBytesPerPixel *= sizeof(short);
        short* sBuffer = (short*)buffer;
        minScalar = 32767.0;
        for (size_t i = 0; i < dim[0] * dim[1] * dim[2]; ++i){
            if (sBuffer[i] < minScalar) minScalar = sBuffer[i];
        }
    }
    else{
        TPS_LOG_DEV_ERROR<<"Unsupported data type which bit allocated: "
            <<iBitsAllc<<" pixel representation: "<<iPixelRepresentation;
        return false;
    }

    char* pPackedData = nullptr;
    if (iStride % 4 != 0) {
        iStride = (iStride + 4) / 4 * 4;
        // Allocate a new block of memory
        pPackedData = new char[iStride * dim[1] * dim[2]];
        char* pSrc = (char*)buffer; char* pDst = pPackedData;
        for (int i = 0; i < dim[1] * dim[2];
            pSrc += dim[0] * iBytesPerPixel,
            pDst += iStride, ++i) {
            memcpy(pDst, pSrc, dim[0] * iBytesPerPixel);
        }
    }
    else {
        // Just use the raw data
        pPackedData = static_cast<char*>(buffer);
    }
    size_t size = iStride / iBytesPerPixel * dim[1] * dim[2];

    switch(iDataType)
    {
    case MEDVIEWER3D_UCHAR: 
        texture->Load(0,
            GL_LUMINANCE8, 
            static_cast<GLsizei>(dim[0]),
            static_cast<GLsizei>(dim[1]),
            static_cast<GLsizei>(dim[2]),
            0,
            GL_LUMINANCE,
            GL_UNSIGNED_BYTE,
            pPackedData);
        break;
    case MEDVIEWER3D_USHORT:
        texture->Load(0, 
            GL_LUMINANCE16,
            static_cast<GLsizei>(dim[0]),
            static_cast<GLsizei>(dim[1]),
            static_cast<GLsizei>(dim[2]),
            0,
            GL_LUMINANCE,
            GL_UNSIGNED_SHORT,
            pPackedData);
        break;
    case MEDVIEWER3D_SHORT:
        if (minScalar < 0.0)
        {
            unsigned short* p = new unsigned short[size];
            short* pImage = (short *)pPackedData;
            for(int i = 0;i < size; i++) {
                *(p + i) = static_cast<unsigned short>(static_cast<float>(*(pImage + i)) - minScalar);
            }
            texture->Load(0, 
                GL_LUMINANCE16,
                static_cast<GLsizei>(dim[0]),
                static_cast<GLsizei>(dim[1]),
                static_cast<GLsizei>(dim[2]),
                0,              
                GL_LUMINANCE,
                GL_UNSIGNED_SHORT,
                p);
            delete []p;
            p = nullptr;
        }
        else
        {
            texture->Load(0, 
                GL_LUMINANCE16,
                static_cast<GLsizei>(dim[0]),
                static_cast<GLsizei>(dim[1]),
                static_cast<GLsizei>(dim[2]),
                0,              
                GL_LUMINANCE,
                GL_UNSIGNED_SHORT,
                pPackedData);
        }
        break;
    }
    if (pPackedData != buffer) {
        delete[] pPackedData;
    }
    return true;
}

VaryingFusionGoConverter::VaryingFusionGoConverter(GO_TYPE goType)
: FusionGoConverter(goType)
{

}

VaryingFusionGoConverter::~VaryingFusionGoConverter()
{

}

bool VaryingFusionGoConverter::PrepareSourceData_i(const GOConverterParameter& /*para*/)
{
    auto mng = mDataRepository->GetFusionSecondaryDataManager();
    if (mng == nullptr){
        TPS_LOG_DEV_ERROR<<"The fusion secondary data manager is null.";
        return false;
    }
    std::string imageUid = mng->GetFixedImageUID();
    mFloatImageUID = mng->GetFloatImageUID();
    //get fixed image entity
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        imageUid, &mFixedImage) || mFixedImage == nullptr){
        TPS_LOG_DEV_ERROR<<"The fixed image does not exist which uid: "<<imageUid;
        return false;
    }
    mAlg = Rigid;
    mRigidMat = mng->GetRigidResult();
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
