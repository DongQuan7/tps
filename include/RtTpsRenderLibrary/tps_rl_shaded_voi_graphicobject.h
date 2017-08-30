//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file tps_rl_shaded_voi_graphicobject.h
///  \brief class ShadedVoiGraphicObject declaration 
///
///  \version 1.0
///  \date    Dec. 22, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SHADED_VOI_GRAPHICOBJECT_H_
#define TPS_RL_SHADED_VOI_GRAPHICOBJECT_H_

#include "tps_rl_defs.h"

#undef min
#undef max

#include "McsfMedViewer3DArithmetic/arithmetic.h"
//#include "RtTpsAlgorithmProxy//tps_ap_masktomesh.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsFramework/tps_fw_common_datastructure.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class VertexArrayBuffer;
        class VertexElementBuffer;
    }
}

TPS_BEGIN_NAMESPACE

typedef std::shared_ptr<Mcsf::MedViewer3D::VertexArrayBuffer> ArrayBufferPtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::VertexElementBuffer> ElementBufferPtr;

/*
typedef struct TpsMesh{

    std::string mVoiUid;
    float   mColor[4];
    MyMesh*  mMesh;
    bool    is_visible;
    bool    is_shaded;
    bool    is_skin;
    bool    is_dirty;

    TpsMesh():is_visible(true),is_shaded(false),is_skin(false),is_dirty(true){
        memset(mColor,0.f, 4*sizeof(float));
        mMesh = new MyMesh();
        mMesh->clear();
        mVoiUid="";
    }
    ~TpsMesh(){
        if (nullptr == mMesh){
            mMesh->clear();
            delete mMesh;
            mMesh = nullptr;
        }
    }

    TpsMesh(const TpsMesh& mesh){
        memcpy(mColor, mesh.mColor, 4 * sizeof(float));
        is_visible = mesh.is_visible;
        is_shaded = mesh.is_shaded;
        is_skin = mesh.is_skin;
        is_dirty = mesh.is_dirty;
        mMesh = mesh.mMesh;
        mVoiUid = mesh.mVoiUid;
    }
    TpsMesh& operator =(const TpsMesh& mesh){
        memcpy(mColor, mesh.mColor, 4 * sizeof(float));
        is_visible = mesh.is_visible;
        is_shaded = mesh.is_shaded;
        is_skin = mesh.is_skin;
        is_dirty = mesh.is_dirty;
        mMesh = mesh.mMesh;
        mVoiUid = mesh.mVoiUid;
        return *this;
    }

}TpsMesh;
*/

class VertexCube
{
public:
    VertexCube();
    ~VertexCube();

    void Render();
    void Update(float centerX, float centerY, float centerZ, 
        float sizeX, float sizeY, float sizeZ);
    void Update(float centerX, float centerY, float centerZ);
    void SetSize(float x, float y, float z) {mSizeX=x;mSizeY=y;mSizeZ=z;}
private:
    void ConstructCubeMesh_i();

private:
    float mSizeX;
    float mSizeY;
    float mSizeZ;
    float mCenterX;
    float mCenterY;
    float mCenterZ;

    static float mNormals[36];
    static float mVertexIndex[36];
    std::vector<Mcsf::Point3f> mVertice;
};

class TPS_RL_DECLSPEC ShadedVoiGraphicObject :
   public TpsGraphicObjectBase{
public:
   
    ShadedVoiGraphicObject();
    virtual ~ShadedVoiGraphicObject();

    virtual bool Initialize();
    virtual bool Finalize();

    //TpsMesh* AddMesh(const std::string &voiUid);
    //const std::map<std::string,TpsMesh*> &GetMeshMap() const;

    bool GetRoateMode() const;

    void SetRotateMode(bool flag);

    void SetVertexDirty(const std::string &voiUid, bool flag);

    void SetMprSection(int section) {mMprSection = section;}
    void SetMprTransparency3D(float mprTransparency3D) {mMprTransparency3D = mprTransparency3D;}
    void SetRotateX(float rx) {mrx = rx;}
    void SetRotateY(float ry) {mry = ry;}
    void SetRotateZ(float rz) {mrz = rz;}
    void SetScale(float scale) {mscale = scale;}
    void SetShininess(float shininess) {mshininess = shininess;}
    void SetSpecularFactor(float specf) {mspecf = specf;}
    void SetAmbFactor(float ambf) {mambf = ambf;}
    void SetDiffFactor(float difff) {mdifff = difff;}
    void SetLightX(float lightx) {mlightx = lightx;}
    void SetLightY(float lighty) {mlighty = lighty;}
    void SetLightZ(float lightz) {mlightz = lightz;}
    void SetDisplayMode(int displayMode) {mDisplayMode = displayMode;}
    void SetSmoothness(int smoothness) {msmoothness = smoothness;}
    void SetUseCameraLight(bool useCameraLight) {museCameraLight = useCameraLight;}
    void SetDisplayVertexCube(bool flag) {mdisplayVertexCube=flag;}
    void SetStartStripIndex(int id) {mStartStripIndex=id;}
    void SetEndStripIndex(int id) {mEndStripIndex=id;}

    int GetMprSection(){return mMprSection;}
    float GetMprTransparency3D() {return mMprTransparency3D;}
    float GetRotateX() {return mrx;}
    float GetRotateY() {return mry;}
    float GetRotateZ() {return mrz;}
    float GetScale() {return mscale;}
    float GetShininess() {return mshininess;}
    float GetSpecularFactor() {return mspecf;}
    float GetAmbFactor() {return mambf;}
    float GetDiffFactor() {return mdifff;}
    float GetLightX() {return mlightx;}
    float GetLightY() {return mlighty;}
    float GetLightZ() {return mlightz;}
    int GetDisplayMode() {return mDisplayMode;}
    int GetSmoothness() {return msmoothness;}
    bool GetUseCameraLight() {return museCameraLight;}
    bool GetDisplayVertexCube() {return mdisplayVertexCube;}
    int GetStartStripIndex() {return mStartStripIndex;}
    int GetEndStripIndex() {return mEndStripIndex;}

    VertexCube& GetVertexCubeObject() {return mVertexCube;}

    void UpdateShadedVoiBuffersMap(std::string voiUid, float* vertexBuffer, float* normalBuffer, unsigned int* indexBuffer, int totalTriangleSize);

//     void GetShadedVoiBuffersMap(std::map<std::string, float*>& vertexBufferMap, std::map<std::string, float*>& normalBufferMap,
//                                 std::map<std::string, unsigned int*>& indexBufferMap, std::map<std::string, int>& totalTriangleSizeMap);

    void GetMeshData(const std::vector<std::string>& voiUIDList, 
        std::map<std::string, ArrayBufferPtr>& mapVBO, 
        std::map<std::string, ArrayBufferPtr>& mapNBO,
        std::map<std::string, ElementBufferPtr>& mapEBO, 
        std::map<std::string, int>& mapTriangleCount) const;
//     void    SetZoomFactorIn3D(float fZoomFactor);
//     float   GetZoomFactorIn3D() const;
private:

    //std::map<std::string,TPS_ANASTRUCT* >           mVOIGOMap;
    //std::map<std::string, TpsMesh*>                 mMeshMap;
    Mcsf::MedViewer3D::Point3D                      mEye;
    Mcsf::MedViewer3D::Point3D                      mLookAt;
    Mcsf::MedViewer3D::Point3D                      mUp;
    double                                          mFov;
    bool                                            mIsInRotateMode;
    float mMprTransparency3D;
    float mrx;
    float mry;
    float mrz;
    float mscale;
    float mspecf;
    float mambf;
    float mdifff;
    float mshininess;
    float mopacity;
    float mlightx;
    float mlighty;
    float mlightz;
    int   msmoothness;
    bool  museCameraLight;
    int   mDisplayMode;
    bool  mdisplayVertexCube;
    int   mStartStripIndex;
    int   mEndStripIndex;
    VertexCube mVertexCube;
    int   mMprSection;

//     std::map<std::string, float*> mVertexBufferMap;
//     std::map<std::string, float*> mNormalBufferMap;
//     std::map<std::string, unsigned int*> mIndexBufferMap;
    std::map<std::string, ArrayBufferPtr> mVBOMap;
    std::map<std::string, ArrayBufferPtr> mNBOMap;
    std::map<std::string, ElementBufferPtr> mEBOMap;
    std::map<std::string, int> mTriangleCountMap;

    //float                                           mZoomFactorIn3D;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(ShadedVoiGraphicObject );
};



TPS_END_NAMESPACE


#endif
