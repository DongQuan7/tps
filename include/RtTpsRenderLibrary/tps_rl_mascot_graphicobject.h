////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_mascot_graphicobject.h
/// 
///  \brief class MascotGraphicObejct declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/09/05
////////////////////////////////////////////////////////////////

#ifndef _TPS_RL_MASCOT_GRAPHICOBJECT_H_
#define _TPS_RL_MASCOT_GRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include <vector>

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC MascotGraphicObject : public TpsGraphicObjectBase {
public:

    typedef struct
    {
        float x;
        float y;
        float z;
    }PointStc, NormalStc, TexturePointStc;

    typedef struct
    {
        float r;
        float g;
        float b;
    }ColorStc;

    typedef struct
    {
        int p1;
        int p2;
        int p3;
    }FaceVertexStc, FaceNormalStc, FaceTextureStc;

    typedef struct
    {
        std::string strGroupName;
        std::vector<FaceVertexStc>  vecFaceVertex;
        std::vector<FaceNormalStc>  vecFaceNormal;
        std::vector<FaceTextureStc> vecFaceTexture;
        std::vector<std::string>    vecMaterialName;
        std::vector<int>            vecFaceCountForMaterial;
        int                         nTotalFacesNumber;
        bool                        hasMaterial;
    }GroupStc;

    typedef struct
    {
        std::string strMaterialName;
        ColorStc Ka;
        ColorStc Kd;
        ColorStc Ks;
        ColorStc Ke;
    }MaterialStc;

    class MascotData
    {
    public:
        std::vector<PointStc>       m_vecVertice;
        std::vector<NormalStc>       m_vecVertexNormals;
        std::vector<GroupStc>       m_vecGroups;
        std::vector<MaterialStc>    m_vecMaterials;
        PointStc                    m_ptLeftBottom;
        PointStc                    m_ptTopRight;
        float                       m_matModel2World[4][4];
        int                         m_nDisplayList;
    };

public:
    MascotGraphicObject();
    virtual ~MascotGraphicObject();
    //////////////////////////////////////////////////////////////////////////
    // virtual methods
    virtual bool Initialize();
    virtual bool Finalize();
    virtual bool HitTest(double ,double ,double ,double* ) {return false;}

    virtual void SetInitializedPostion(const Mcsf::MedViewer3D::Point3D& /*myCenterPoint*/,
        const Mcsf::MedViewer3D::Vector3D& /*mySize*/,
        const Mcsf::MedViewer3D::Quaternion& /*myQuaternion*/) { }
    /// \brief Rotate the graphic object
    virtual void CenterRotate(const Mcsf::MedViewer3D::Quaternion& /*myQuaternion*/) { }

    /// \brief Rotate the graphic object
    virtual void Rotate(const Mcsf::MedViewer3D::Quaternion& /*myQuaternion*/){ }

    /// \brief Translate the object
    virtual void Translate(const Mcsf::MedViewer3D::Vector3D& /*myTranslate*/) { }

    /// \brief Scale the object
    virtual void Scale(const Mcsf::MedViewer3D::Vector3D& /*myScale*/) { }

    bool Visible() const { return m_bVisible; }
    void SetVisible(bool bVisible) { m_bVisible = bVisible; }

    void Draw(float lightPosition[4]);

private:
    bool                m_bVisible;

    //static std::shared_ptr<MascotData>  m_spData;
    static MascotData*  m_spData;

    bool LoadOBJ(const char *path, 
                 std::vector<PointStc>& outVertices, 
                 std::vector<NormalStc>& outNormals, 
                 std::vector<GroupStc>& outGroups,
                 PointStc& leftButtom,
                 PointStc& topRight,
                 float matModel2World[4][4]);

    bool LoadMaterial(const char *path, 
                      std::vector<MaterialStc>& outMaterials);

    void Draw_i(std::vector<PointStc>& vecVertice, 
                std::vector<NormalStc>& vecNormals,
                std::vector<GroupStc>& vecGroups,
                float matModel2World[4][4],
                std::vector<MaterialStc>& vecMaterials);
};
TPS_END_NAMESPACE

#endif //_TPS_RL_CROSSHAIR_GRAPHICOBJECT_H_
