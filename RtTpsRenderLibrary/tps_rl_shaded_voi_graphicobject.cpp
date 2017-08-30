//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file tps_rl_shaded_voi_graphicobject.cpp
///  \brief class ShadedVoiGraphicObject  
///
///  \version 1.0
///  \date    Dec. 22, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicUtils/camera.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_vertex_array_buffer.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_vertex_element_buffer.h"
                                       
#pragma warning(disable:4702)


TPS_BEGIN_NAMESPACE
ShadedVoiGraphicObject::ShadedVoiGraphicObject() :
mIsInRotateMode(false),/*mZoomFactorIn3D(1.0f),*/mFov(0.){

    /*mEye = Mcsf::MedViewer3D::Point3D(0.0,0.0,1.0);
    mLookAt = Mcsf::MedViewer3D::Point3D(0.0,0.0,0.0);
    mUp = Mcsf::MedViewer3D::Point3D(0.0,1.0,0.0);*/

    mMprTransparency3D = 0.0f;
    mrx = 0.0f;
    mry = 0.0f;
    mrz = 0.0f;
    mlightx = 0.0f;
    mlighty = 0.0f;
    mlightz = 0.0f;
    mscale = 1.0f;

	mspecf = 0.8f;
	mambf = 0.5f;
	mdifff = 0.35f;
	mshininess = 30.0f;
    msmoothness = 1;
    mDisplayMode = 1;
    museCameraLight = true;
    mdisplayVertexCube = false;
    mStartStripIndex = 0;
    mEndStripIndex = 1000;
    mMprSection = (int)CORONAL;
}

ShadedVoiGraphicObject::~ShadedVoiGraphicObject() {
    mVBOMap.clear();
    mNBOMap.clear();
    mEBOMap.clear();
    mTriangleCountMap.clear();
}

bool ShadedVoiGraphicObject::Initialize() {
  /*  PlaceCamera(Mcsf::MedViewer3D::Point3D(0,-20,0),
        Mcsf::MedViewer3D::Point3D(0,0,0),
        Mcsf::MedViewer3D::Vector3D(0,0,1));*/
     return true;
}

bool ShadedVoiGraphicObject::Finalize() {
    return true;
}

/*
TpsMesh* ShadedVoiGraphicObject::AddMesh(const std::string &voiUid) {

    if (mMeshMap.find(voiUid) == mMeshMap.end()){
        TpsMesh* pMesh = new TpsMesh();
        mMeshMap[voiUid] = pMesh;
    }
    return mMeshMap[voiUid];
}

const std::map<std::string,TpsMesh*> &ShadedVoiGraphicObject::GetMeshMap() const {

    return mMeshMap;
}
*/

void ShadedVoiGraphicObject::SetRotateMode(bool flag) {
    mIsInRotateMode = flag;
}

bool ShadedVoiGraphicObject::GetRoateMode() const {
    return mIsInRotateMode;
}

void ShadedVoiGraphicObject::SetVertexDirty(const std::string &/*voiUid*/, bool /*flag*/){
    //shaded voi
    /*
    auto itMesh = mMeshMap.find(voiUid);
    if(itMesh != mMeshMap.end()) {
        itMesh->second->is_dirty = flag;
    }
    else{
        TPS_LOG_DEV_ERROR<<"failed to find shaded voi from map:"<<voiUid;
    }
    */
}

void ShadedVoiGraphicObject::UpdateShadedVoiBuffersMap(
    std::string voiUid, float* vertexBuffer, 
    float* normalBuffer, unsigned int* indexBuffer, 
    int totalTriangleSize)
{
    int size = totalTriangleSize * 3 * 3 * sizeof(float);
    if (mVBOMap.find(voiUid) == mVBOMap.end())
    {
        mVBOMap[voiUid] = Mcsf::MedViewer3D::VertexArrayBuffer::Create();
    }
    mVBOMap[voiUid]->Load(size, vertexBuffer, GL_STATIC_DRAW);

    if (mNBOMap.find(voiUid) == mNBOMap.end())
    {
        mNBOMap[voiUid] = Mcsf::MedViewer3D::VertexArrayBuffer::Create();
    }
    mNBOMap[voiUid]->Load(size, normalBuffer, GL_STATIC_DRAW);

    size = totalTriangleSize * 3 * sizeof(unsigned int);
    if (mEBOMap.find(voiUid) == mEBOMap.end())
    {
        mEBOMap[voiUid] = Mcsf::MedViewer3D::VertexElementBuffer::Create();
    }
    mEBOMap[voiUid]->Load(size, indexBuffer, GL_STATIC_DRAW);
    mTriangleCountMap[voiUid] = totalTriangleSize;
}

//void ShadedVoiGraphicObject::GetShadedVoiBuffersMap(std::map<std::string, float*>& vertexBufferMap, std::map<std::string, float*>& normalBufferMap,
//    std::map<std::string, unsigned int*>& indexBufferMap, std::map<std::string, int>& totalTriangleSizeMap) {
//        vertexBufferMap = mVertexBufferMap;
//        normalBufferMap = mNormalBufferMap;
//        indexBufferMap = mIndexBufferMap;
//        totalTriangleSizeMap = mTotalTriangleSizeMap;
//}

void ShadedVoiGraphicObject::GetMeshData(
    const std::vector<std::string>& voiUIDList, 
    std::map<std::string, ArrayBufferPtr>& mapVBO, 
    std::map<std::string, ArrayBufferPtr>& mapNBO,
    std::map<std::string, ElementBufferPtr>& mapEBO,
    std::map<std::string, int>& mapTriangleCount) const
{
    mapVBO.clear();
    mapNBO.clear();
    mapEBO.clear();

    std::string uid;
    ArrayBufferPtr tmpVB, tmpNB;
    ElementBufferPtr tmpIB;
    int tmpCount;
    for (int i = 0; i < voiUIDList.size(); ++i)
    {
        uid = voiUIDList[i];
        auto itVB = mVBOMap.find(uid);
        if (itVB == mVBOMap.end())
        {
            continue;
        }
        tmpVB = itVB->second;
        auto itNB = mNBOMap.find(uid);
        if (itNB == mNBOMap.end())
        {
            continue;
        }
        tmpNB = itNB->second;
        auto itIB = mEBOMap.find(uid);
        if (itIB == mEBOMap.end())
        {
            continue;
        }
        tmpIB = itIB->second;
        auto itCount = mTriangleCountMap.find(uid);
        if (itCount == mTriangleCountMap.end())
        {
            continue;
        }
        tmpCount = itCount->second;
        mapVBO[uid] = tmpVB;
        mapNBO[uid] = tmpNB;
        mapEBO[uid] = tmpIB;
        mapTriangleCount[uid] = tmpCount;
    }
}

// void ShadedVoiGraphicObject::SetZoomFactorIn3D(float fZoomFactor){
//     mZoomFactorIn3D = fZoomFactor;
// }
// float ShadedVoiGraphicObject::GetZoomFactorIn3D() const{
//     return mZoomFactorIn3D;
// }

float VertexCube::mVertexIndex[36] = {0,1,2,
    2,3,0,
    4,7,6,
    6,5,4,
    8,11,10,
    10,9,8,
    12,13,14,
    14,15,12,
    16,17,18,
    18,19,16,
    20,23,22,
    22,21,20};

float VertexCube::mNormals[36] = {0.0f,-1.0f,0.0f,
    0.0f,-1.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,0.0f,1.0f,
    0.0f,0.0f,1.0f,
    0.0f,0.0f,-1.0f,
    0.0f,0.0f,-1.0f,
    -1.0f,0.0f,0.0f,
    -1.0f,0.0f,0.0f,
    1.0f,0.0f,0.0f,
    1.0f,0.0f,0.0f};

VertexCube::VertexCube()
{
    //float vertexIndex[36] = {0,1,2,
    //    2,3,0,
    //    4,7,6,
    //    6,5,4,
    //    8,11,10,
    //    10,9,8,
    //    12,13,14,
    //    14,15,12,
    //    16,17,18,
    //    18,19,16,
    //    20,23,22,
    //    22,21,20};
    //memcpy(mVertexIndex, vertexIndex, sizeof(float)*36);

    //float normals[36] = {0.0f,-1.0f,0.0f,
    //    0.0f,-1.0f,0.0f,
    //    0.0f,1.0f,0.0f,
    //    0.0f,1.0f,0.0f,
    //    0.0f,0.0f,1.0f,
    //    0.0f,0.0f,1.0f,
    //    0.0f,0.0f,-1.0f,
    //    0.0f,0.0f,-1.0f,
    //    -1.0f,0.0f,0.0f,
    //    -1.0f,0.0f,0.0f,
    //    1.0f,0.0f,0.0f,
    //    1.0f,0.0f,0.0f};
    //memcpy(mNormals, normals, sizeof(float)*36);
}

VertexCube::~VertexCube()
{

}

void VertexCube::ConstructCubeMesh_i()
{
    mVertice.clear();

    std::vector<Mcsf::Point3f> orignalVertex;
    orignalVertex.push_back(Mcsf::Point3f(mCenterX - mSizeX / 2, mCenterY + mSizeY / 2, mCenterZ + mSizeZ / 2));
    orignalVertex.push_back(Mcsf::Point3f(mCenterX + mSizeX / 2, mCenterY + mSizeY / 2, mCenterZ + mSizeZ / 2));
    orignalVertex.push_back(Mcsf::Point3f(mCenterX + mSizeX / 2, mCenterY - mSizeY / 2, mCenterZ + mSizeZ / 2));
    orignalVertex.push_back(Mcsf::Point3f(mCenterX - mSizeX / 2, mCenterY - mSizeY / 2, mCenterZ + mSizeZ / 2));
    orignalVertex.push_back(Mcsf::Point3f(mCenterX - mSizeX / 2, mCenterY + mSizeY / 2, mCenterZ - mSizeZ / 2));
    orignalVertex.push_back(Mcsf::Point3f(mCenterX + mSizeX / 2, mCenterY + mSizeY / 2, mCenterZ - mSizeZ / 2));
    orignalVertex.push_back(Mcsf::Point3f(mCenterX + mSizeX / 2, mCenterY - mSizeY / 2, mCenterZ - mSizeZ / 2));
    orignalVertex.push_back(Mcsf::Point3f(mCenterX - mSizeX / 2, mCenterY - mSizeY / 2, mCenterZ - mSizeZ / 2));

    mVertice.push_back(orignalVertex[0]); // 0
    mVertice.push_back(orignalVertex[1]); // 1
    mVertice.push_back(orignalVertex[2]); // 2
    mVertice.push_back(orignalVertex[3]); // 3
    mVertice.push_back(orignalVertex[4]); // 4
    mVertice.push_back(orignalVertex[5]); // 5
    mVertice.push_back(orignalVertex[6]); // 6
    mVertice.push_back(orignalVertex[7]); // 7
    mVertice.push_back(orignalVertex[0]); // 8
    mVertice.push_back(orignalVertex[4]); // 9
    mVertice.push_back(orignalVertex[7]); // 10
    mVertice.push_back(orignalVertex[3]); // 11
    mVertice.push_back(orignalVertex[1]); // 12
    mVertice.push_back(orignalVertex[5]); // 13
    mVertice.push_back(orignalVertex[6]); // 14
    mVertice.push_back(orignalVertex[2]); // 15
    mVertice.push_back(orignalVertex[0]); // 16
    mVertice.push_back(orignalVertex[4]); // 17
    mVertice.push_back(orignalVertex[5]); // 18
    mVertice.push_back(orignalVertex[1]); // 19
    mVertice.push_back(orignalVertex[3]); // 20
    mVertice.push_back(orignalVertex[7]); // 21
    mVertice.push_back(orignalVertex[6]); // 22
    mVertice.push_back(orignalVertex[2]); // 23
}

void VertexCube::Update(float centerX, float centerY, float centerZ, 
    float sizeX, float sizeY, float sizeZ)
{
    mCenterX = centerX;
    mCenterY = centerY;
    mCenterZ = centerZ;
    mSizeX = sizeX;
    mSizeY = sizeY;
    mSizeZ = sizeZ;

    ConstructCubeMesh_i();
}

void VertexCube::Update(float centerX, float centerY, float centerZ)
{
    mCenterX = centerX;
    mCenterY = centerY;
    mCenterZ = centerZ;

    ConstructCubeMesh_i();
}

void VertexCube::Render()
{
    // …Ë÷√≤ƒ÷ 
    float ambf = 1.0f;
    float diff = 1.0f;
    float color[4]={0.5,0.0,0.0,1.0};
    float amb[4] = {color[0]*ambf,color[1]*ambf,color[2]*ambf,color[3]};
    float dif[4] = {color[0]*diff,color[1]*diff,color[2]*diff,color[3]};
    float spec[4] ={1.0,1.0,1.0,1.0};
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,amb);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,dif);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,spec);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,50.f);
    glShadeModel(GL_SMOOTH);

    for(int n=0; n<mVertice.size(); n+=3)
    {
        int m = n / 3;
        glBegin(GL_TRIANGLES);
        glVertex3f(mVertice[mVertexIndex[n]].GetX(), mVertice[mVertexIndex[n]].GetY(), mVertice[mVertexIndex[n]].GetZ());
        glNormal3f(mNormals[m * 3], mNormals[m * 3 + 1], mNormals[m * 3 + 2]);
        glVertex3f(mVertice[mVertexIndex[n + 1]].GetX(), mVertice[mVertexIndex[n + 1]].GetY(), mVertice[mVertexIndex[n + 1]].GetZ());
        glNormal3f(mNormals[m * 3], mNormals[m * 3 + 1], mNormals[m * 3 + 2]);
        glVertex3f(mVertice[mVertexIndex[n + 2]].GetX(), mVertice[mVertexIndex[n + 2]].GetY(), mVertice[mVertexIndex[n + 2]].GetZ());
        glNormal3f(mNormals[m * 3], mNormals[m * 3 + 1], mNormals[m*3 + 2]);
        glEnd();
    }
}

TPS_END_NAMESPACE
