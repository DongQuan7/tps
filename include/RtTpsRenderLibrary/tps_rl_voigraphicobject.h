////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_voigraphicobject.h
/// 
///  \brief class TpsVOIGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOIGRAPHICOBJECT_H_
#define TPS_RL_VOIGRAPHICOBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class VertexArrayBuffer;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class SignedDistanceField;
class ContourSet;

typedef std::shared_ptr<Mcsf::MedViewer3D::VertexArrayBuffer> ArrayBufferPtr;

/// \class TpsVOIGraphicObject
/// 
/// \brief this graphic object is corresponding to a VOI in tps data layer
class TPS_RL_DECLSPEC TpsVOIGraphicObject : public TpsGraphicObjectBase{
public:
    typedef enum{
        LINE,
        FILL,
    } VISION_MODE;

    //the constructor and destructor
    TpsVOIGraphicObject();
    virtual ~TpsVOIGraphicObject();

    //////////////////////////////////////////////////////////////////////////
    // \brief set vision mode of the VOI, line or fill mode
    // \param[in] mode the vision mode
    void SetVisionMode(VISION_MODE mode);

    //////////////////////////////////////////////////////////////////////////
    // \brief get vision of the VOI, line or fill mode
    // \return VISION_MODE  the vision mode
    VISION_MODE GetVisionMode() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief set the visibility of the VOI, true visible, false invisible
    // \param[in] visibility the visibility of VOI wanted
    void SetVisibility(bool visibility);

    //////////////////////////////////////////////////////////////////////////
    // \brief get the visibility of the VOI, true visible, false invisible
    // \return bool  the visibility of VOI
    bool GetVisibility() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief set color of voi
    void SetColor(const float* color);

    //////////////////////////////////////////////////////////////////////////
    // \brief get color buffer
    void GetColor(float* color) const;

    void SetSlices(int* slices, bool isCut);
    void GetSlices(int* slices) const;

    bool IsCutContour();

    void SetIsInterpolate(int slice, bool isInterpolate);

    bool GetIsInterpolate(int slice);

    void SetHasInterpolate(bool hasInterpolate);

    bool GetHasInterpolate();

    void SetInterpolateFlags(bool *flags);

    void GetInterpolateFlags(bool* &flags);

    void SetSliceCount(int sliceCount);

    int GetSliceCount();

    //废弃，即便缓存了avoid voi uid list，也无法判断avoid vois是否已经被修改
    //inline std::list<std::string> GetAvoidVoiUIDList() const { 
    //    return mAvoidVoisUid; }
    //inline void SetAvoidVoiUIDList(const std::list<std::string>& voiList) { 
    //    mAvoidVoisUid = voiList; }

    bool IsShaded() const;
    void SetIsShaded(bool isShaded);
    float GetAlpha3D() const;
    void SetAlpha3D(float alpha);
    bool IsMeshDataDirty() const;
    void SetMeshDataDirty(bool isMeshDataDirty);
    void GetMeshBuffer(unsigned int& vboID, unsigned int& nboID) const;
    void GetMeshVertexCount(unsigned int& vertexCount) const;
    void SetMeshVertexCount(unsigned int vertexCount);

    SignedDistanceField* GetSDF() const;
    ContourSet* GetContourSet() const;
private:
    SignedDistanceField*    mSDF;
    ContourSet*             mCS;
    VISION_MODE         mMode;
    float               mColor[4];
    bool                mVisible;
    int                 mSlices[2];
    bool                mIsCutContour;
    bool*               mIsInterpolate;
    bool                mHasInterpolate;
    int                 mSliceCount;

    bool                mIsShaded;
    float               mAlpha3D;
    ArrayBufferPtr      mMeshVBO;
    ArrayBufferPtr      mMeshNBO;
    unsigned int        mMeshVertexCount;
    bool                mIsMeshDataDirty;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsVOIGraphicObject);
};

TPS_END_NAMESPACE  // end namespace tps

#endif