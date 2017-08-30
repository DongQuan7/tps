//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_generate_mesh.cpp
///  \brief   
///
///  \version 1.0
///  \date  2015/10/28  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_generate_mesh.h"

//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsAlgorithmProxy/tps_ap_masktomesh.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedvector3d.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedpoint3d.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedmesh.h"

#include "OpenMesh/Core/IO/MeshIO.hh"
#include "OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh"

TPS_BEGIN_NAMESPACE
    GenerateMeshCmd::GenerateMeshCmd( std::string &imageUID, int organType )
    : mImageUID(imageUID), mOrganType(organType) {

}

GenerateMeshCmd::~GenerateMeshCmd()
{

}

int GenerateMeshCmd::Execute()
{
    return TPS_ER_SUCCESS;

    TpsImageDataManager *imageDataManager = mDataRepository->GetImageDataManager();

    CertifiedMesh *skinMesh = nullptr;
    if(imageDataManager->GetSkinMesh(mImageUID, skinMesh) && skinMesh != nullptr){
        TPS_LOG_DEV_INFO<<"Skin mesh already existed in image data manager.";
        return TPS_ER_SUCCESS;
    }

    ROIEntity *skinEntity = mDataRepository->GetVoiManager()->GetExternal(mImageUID);
    if(skinEntity == nullptr) {
        TPS_LOG_DEV_ERROR<<"Fail to check skin voi entity before generate mesh ";
        return TPS_ER_FAILURE;
    }

    McsfGeometry::SubVolume* subVolume = nullptr;
    //auto subVolume = skinEntity->GetSubVolume();
    int iSize[3] ={0};
    subVolume->GetSize(iSize[0],iSize[1],iSize[2]);

    int iOffset[3]={0};
    subVolume->GetOffSet(iOffset[0],iOffset[1],iOffset[2]);
    unsigned char *buffer =(unsigned char*)(subVolume->GetData());

    const float fScale = 1.0f / 5;
    int sizeOfReSample[3] =
    {
        static_cast<int>(std::ceil(fScale* iSize[0])),
        static_cast<int>(std::ceil(fScale* iSize[1])), 
        static_cast<int>(std::ceil(fScale *iSize[2]))
    };
    const int iLengthResample = sizeOfReSample[0] * sizeOfReSample[1] * sizeOfReSample[2] * 
        sizeof(unsigned char);
    unsigned char *bufferResampled = new unsigned char[iLengthResample];
    memset(bufferResampled, 0, iLengthResample);

    SubVolumeHelperAPI::SubVolumeResample(buffer, iSize[0],iSize[1],iSize[2],
        fScale, fScale, fScale, bufferResampled);

    MyMesh* pMesh = new MyMesh();
    pMesh->clear();

    //TODO to be deleted if subvolume redefined
    for (int z = 0; z < sizeOfReSample[2] ; ++z){
        for (int y = 0; y < sizeOfReSample[1]; ++y){
            for (int x = 0; x < sizeOfReSample[0]; ++ x){
                if (x ==0 || x == sizeOfReSample[0] -1 
                    || y == 0 || y == sizeOfReSample[1] - 1 
                    /*|| z == 0 || z == sizeOfReSample[2] -1*/){
                        bufferResampled[z * sizeOfReSample[0] *
                            sizeOfReSample[1] + y * sizeOfReSample[0] + x] = 0;
                }
            }
        }
    }

    MaskToMeshByMarchingCube(bufferResampled,sizeOfReSample,1,*pMesh);
    //MaskToMeshByMarchingCube(buffer,iSize,1,*pMesh);

    delete bufferResampled; bufferResampled = nullptr;

    RtImage3DHeader *header = nullptr;
    if (mImageUID.empty() || !imageDataManager->
        GetImage3DHeader(mImageUID,header)) {
            TPS_LOG_DEV_ERROR<<"Fail to get image data during generate mesh ";
            return TPS_ER_FAILURE;
    }

    TpsImage3DEntity *imageEntity = nullptr;
    if (mImageUID.empty() || !imageDataManager->
        GetImage3DData(mImageUID, &imageEntity)) {
            TPS_LOG_DEV_ERROR<<"Fail to get image entity during generate mesh ";
            return TPS_ER_FAILURE;
    }

    Mcsf::Matrix4f matV2P;
    if(imageEntity == nullptr || !imageEntity->GetVolumeToPatientMatrix(matV2P)) {
        TPS_LOG_DEV_ERROR<<"Fail to get volume to patient matrix during generate mesh ";
        delete pMesh; pMesh = nullptr;
        return TPS_ER_FAILURE;
    }
    Mcsf::MedViewer3D::Matrix4x4 matVolume2Patient = TpsArithmeticConverter::ConvertToMatrix4x4(matV2P);

    size_t dim[3] = {header->m_iXDim, header->m_iYDim, header->m_iSliceCount};

    for(auto it = pMesh->vertices_begin(); it!= pMesh->vertices_end(); ++it) {
        auto point = pMesh->point(*it);
        float data[3];
        data[0] = point.data()[0] / fScale;
        data[1] = point.data()[1] / fScale;
        data[2] = point.data()[2] / fScale;
        float coordInVolume[3],coordInModel[3];
        coordInVolume[0] = data[0] + iOffset[0];
        coordInVolume[1] = data[1] + iOffset[1];
        coordInVolume[2] = data[2] + iOffset[2];

        coordInModel[0] = (coordInVolume[0] + 0.5) / dim[0];
        coordInModel[1] = (coordInVolume[1] + 0.5) / dim[1];
        coordInModel[2] = (coordInVolume[2] + 0.5) / dim[2];

        Mcsf::MedViewer3D::Point3D ptInPatient = matVolume2Patient.Transform(Mcsf::MedViewer3D::Point3D(coordInVolume[0], coordInVolume[1], coordInVolume[2]));
        data[0] = ptInPatient.x;
        data[1] = ptInPatient.y;
        data[2] = ptInPatient.z;

        MyMesh::Point pt(data[0],data[1],data[2]);
        pMesh->set_point(*it,pt);
    }

    //get normals
    pMesh->request_vertex_normals();
    pMesh->request_face_normals();
    pMesh->update_normals();

    CertifiedVector3D normalVector;
    CertifiedPoint3D point3D;
    CertifiedMesh *outMesh = new CertifiedMesh;
    for(auto it = pMesh->faces_begin(); it != pMesh->faces_end(); ++it) {

        for(auto itrr= pMesh->fv_iter(*it); itrr.is_valid(); ++itrr){

            normalVector.mX = pMesh->normal(*itrr).data()[0];
            normalVector.mY = pMesh->normal(*itrr).data()[1];
            normalVector.mZ = pMesh->normal(*itrr).data()[2];

            //glNormal3fv(mesh.normal(itrr.handle()).data());
            point3D.mX = pMesh->point(*itrr).data()[0];
            point3D.mY = pMesh->point(*itrr).data()[1];
            point3D.mZ = pMesh->point(*itrr).data()[2];

            outMesh->mPoints.push_back(point3D);
            outMesh->mNormals.push_back(normalVector);
        }

    }

    // add skin mesh into data manager 
    if(!imageDataManager->AddSkinMesh(mImageUID, *outMesh))
    {
        TPS_LOG_DEV_ERROR<<"Fail to add skin mesh data into image data manager.";
        delete pMesh; pMesh = nullptr;

        return TPS_ER_FAILURE;
    }

    delete pMesh; pMesh = nullptr;
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE