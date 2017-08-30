//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  LI Song  song.li@united-imaging.com
///
///  \file tps_rl_dose_mesh_graphicobject.h
///  \brief class DoseMeshGraphicObject  
///
///  \version 1.0
///  \date    Jun. 8, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DOSE_MESH_GRAPHICOBJECT_H_
#define TPS_RL_DOSE_MESH_GRAPHICOBJECT_H_

#include "tps_rl_defs.h"

#undef min
#undef max

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsAlgorithmProxy//tps_ap_masktomesh.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsFramework/tps_fw_common_datastructure.h"

#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"

TPS_BEGIN_NAMESPACE

struct STC_STRIPS;

class TPS_RL_DECLSPEC DoseMeshGraphicObject : public TpsGraphicObjectBase{
public:
   
    DoseMeshGraphicObject();
    virtual ~DoseMeshGraphicObject();

    virtual bool Initialize();
    virtual bool Finalize();

    void SetDoseMesh(STC_STRIPS* pMesh);
    void SetDoseAnastruct(plunc::ANASTRUCT* pMesh);

    STC_STRIPS* GetDoseMesh(){return m_pStrips;}

    plunc::ANASTRUCT* GetDoseAnastruct() { return m_pAna; }

    void SetDoseColor(float fRed, float fGreen, float fBlue, float fAlpha);

    void GetDoseColor(float* fRed, float* fGreen, float* fBlue, float* fAlpha);

    void GetElementArray();

    void SetVisibility(const std::string &voiUid, bool flag);

    void SetVoiShaded(const std::string &voiUid, bool flag);
    //void SetVertexDirty(const std::string &voiUid, bool flag);

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

    void SetDoseRatio(float ratio) {mDoseRatio = ratio;}

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
    float GetDoseRatio() {return mDoseRatio;}

    void SetValidFlag(bool bValid) { m_bValid = bValid;}
    bool GetValidFlag() const { return m_bValid;}
    int GetElementBuffer(float** pVertexArray, float** pNormalArray, unsigned int** pIndexArray);
private:
    bool m_bValid;
    std::map<std::string,TPS_ANASTRUCT* >           mVOIGOMap;

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
    float mDoseRatio;

    STC_STRIPS* m_pStrips;
    plunc::ANASTRUCT* m_pAna;
    float m_fRed;
    float m_fGreen;
    float m_fBlue;
    float m_fAlpha;

    int   m_count;
    float *mpVertexArray;
    float *mpNormalArray;
    unsigned int *mpIndexArray;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(DoseMeshGraphicObject);
};

TPS_END_NAMESPACE

#endif //TPS_RL_DOSE_MESH_GRAPHICOBJECT_H_
