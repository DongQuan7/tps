////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_beam_transparency_rendering_node.cpp
/// 
///  \brief class BeamTransparencyRenderingNode implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/10/21
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_beam_transparency_rendering_node.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"
#include "../tps_rl_shaders.h"

//for color?
#include "RtTpsFramework/tps_fw_common_enums.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

BeamTransparencyRenderingNode::BeamTransparencyRenderingNode(int id)
    : PhongShadingTransprarencyNode(id)
{

}

BeamTransparencyRenderingNode::~BeamTransparencyRenderingNode()
{

}

bool BeamTransparencyRenderingNode::Initialize_i()
{
    glGenBuffers(1, &mVboId);
    glBindBuffer(GL_ARRAY_BUFFER, mVboId);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

void BeamTransparencyRenderingNode::Finalize()
{
    glDeleteBuffers(1, &mVboId);
}

//rendering logic is copied from old ShadedVOIStrategy, need refined later
void BeamTransparencyRenderingNode::PureRender_i(GLProgram* prog)
{
    if (mVolumeGO == nullptr || mNGCollectionGO == nullptr)
    {
        return;
    }

    glViewport(0, 0, mWidth, mHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    Matrix4x4 matProjection = mCameraGO->GetProjectionMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(matProjection._m);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mVboId);
    glVertexPointer(3, GL_DOUBLE, 0, NULL);
    glEnableClientState(GL_VERTEX_ARRAY);

    Matrix4x4 matModelView;
    Matrix4x4 matPat2View = mVolumeGO->GetWorld2Patient().Inverse();
    matPat2View.Prepend(mCameraGO->GetViewMatrix());

    double beam_near_scale = 0.5;
    double collimator_scale = 0.6;
    bool isActiveNG = false;
    std::map<std::string, std::shared_ptr<TpsBeamGraphicObject>> beamList;
    std::shared_ptr<TpsBeamGraphicObject> beamGO = nullptr;
    std::shared_ptr<TpsSegmentGraphicObject> segmentGO = nullptr;
    std::map<std::string, NormalGroupPtr> normalGroupMap =
        mNGCollectionGO->GetNormalGroupMap();
    for (auto itNG = normalGroupMap.begin(); itNG != normalGroupMap.end(); ++itNG)
    {
        if (itNG->second == nullptr) continue;
        isActiveNG = itNG->first == mNGCollectionGO->GetActiveNormalGroupUID();
        beamList = itNG->second->GetBeamMap();
        for (auto itBeam = beamList.begin(); itBeam != beamList.end(); ++itBeam)
        {
            beamGO = itBeam->second;
            if (beamGO == nullptr || !beamGO->GetVisible()) continue;

            segmentGO = beamGO->GetActiveSegmentGo();
            if (segmentGO == nullptr) continue;

            segmentGO->GetStartCollimatorToPatMatrix(&matModelView);
            matModelView.Prepend(matPat2View);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixd(matModelView._m);

            //TODO: add normal vectors for every vertex in geometries to get right lighting effect
            //TODO: draw block, wedge, fluence map
            DrawLeaves_i(prog, segmentGO, collimator_scale);
            DrawJaw_i(prog, segmentGO, itNG->second->GetSAD(), collimator_scale);
            bool isActiveBeam = isActiveNG && beamGO == itNG->second->GetActiveBeamGo();
            DrawBeamFace_i(prog, segmentGO, isActiveBeam, beam_near_scale);
            DrawIsoCenter_i();
            DrawCentralLine_i(itNG->second->GetSAD());
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void BeamTransparencyRenderingNode::BuildShaders_i()
{
    PhongShadingTransprarencyNode::BuildShaders_i();
    mStrShadingFragmentShaders.push_back(Shaders::Get(IDR_SHADER_DepthPeelingCalcFragColorFrag).c_str());
}

void BeamTransparencyRenderingNode::DrawLeaves_i(
    GLProgram* prog, SegmentGOPtr segmentGO, double scale)
{
    float color[4] = {0.38f, 0.3f, 0.26f, 0.6f};
    float edge_color[4] = {0.f, 0.f, 0.f, 0.4f};
    auto ptList = std::move(segmentGO->Get3DSegmentPtList());
    for (int i = 0; i < ptList.size(); ++i)
    {
        ptList[i] *= scale;
    }
    if (!ptList.empty())
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(double) * ptList.size() * 3, 
            ptList._Myfirst, GL_DYNAMIC_DRAW);

        if (prog)
        {
            prog->SetUniformf("vColor", edge_color, 4);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_QUADS, 0, ptList.size());
            prog->SetUniformf("vColor", color, 4);
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_QUADS, 0, ptList.size());
    }
}

void BeamTransparencyRenderingNode::DrawJaw_i(
    GLProgram* prog, SegmentGOPtr segmentGO, double sad, double scale)
{
    //draw jaw
    double scaled_sad = sad * scale;
    float displayRange[4], jawPos[4];
    segmentGO->GetJawPosition(jawPos);
    segmentGO->GetCollimatorDisplayRange(displayRange);
    for (int i = 0; i < 4; ++i)
    {
        jawPos[i] *= scale;
        displayRange[i] *= scale;
    }
    double jawRect[12] = { 
        jawPos[0], jawPos[2], -scaled_sad + 0.1, 
        jawPos[1], jawPos[2], -scaled_sad + 0.1, 
        jawPos[1], jawPos[3], -scaled_sad + 0.1, 
        jawPos[0], jawPos[3], -scaled_sad + 0.1 
    };
    double displayRect[12] = { 
        displayRange[0], displayRange[2], -scaled_sad + 0.1, 
        displayRange[1], displayRange[2], -scaled_sad + 0.1, 
        displayRange[1], displayRange[3], -scaled_sad + 0.1, 
        displayRange[0], displayRange[3], -scaled_sad + 0.1 
    };

    float color[4] = {0.09f, 0.41f, 0.54f, 0.4f};
    float edge_color[4] = {0.f, 0.f, 0.f, 1.f};
    if (prog)
    {
        prog->SetUniformf("vColor", edge_color, 4);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(jawRect), jawRect, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_QUADS, 0, 4);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glStencilMask(0xff);
    glClearStencil(0x0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glEnable(GL_STENCIL_TEST);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDrawArrays(GL_QUADS, 0, 4);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    if (prog)
    {
        prog->SetUniformf("vColor", color, 4);
    }
    glStencilFunc(GL_NOTEQUAL, 0x01, 0x01);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glBufferData(GL_ARRAY_BUFFER, sizeof(displayRect), displayRect, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisable(GL_STENCIL_TEST);
}

void BeamTransparencyRenderingNode::DrawBeamFace_i(
    GLProgram* prog, SegmentGOPtr segmentGO, bool isActive, double scale)
{
    float beam_line_normal_color[4] = { 0.f, 1.f, 0.f, 0.15f };
    float beam_line_active_color[4] = { 1.f, 1.f, 0.f, 0.15f };
    float beam_face_normal_color[4] = { 0.f, 6.f, 0.f, 0.1f };
    float beam_face_active_color[4] = { 6.f, 6.f, 0.f, 0.1f };

    float* beamLineColor = isActive ? 
        beam_line_active_color : beam_line_normal_color;
    float* beamFaceColor = isActive ? 
        beam_face_active_color : beam_face_normal_color;

    //draw beam line
    std::vector<Point3D> ptList;
    segmentGO->GetOutLine(&ptList);
    std::vector<Point3D> ptNearFace(ptList.size());
    std::transform(ptList.begin(), ptList.end(), ptNearFace.begin(), 
        [scale](const Point3D& point)->Point3D
    {
        return point * scale;
    });

    double* vertexBuffer = new double[ptList.size() * 12];
    for (int i = 0; i < ptList.size(); ++i)
    {
        int postIdx = i == ptList.size() - 1 ? 0 : i + 1;
        memcpy(vertexBuffer + 12 * i, &ptNearFace[i], 3 * sizeof(double));
        memcpy(vertexBuffer + 12 * i + 3, &ptNearFace[postIdx], 3 * sizeof(double));
        memcpy(vertexBuffer + 12 * i + 6, &ptList[postIdx], 3 * sizeof(double));
        memcpy(vertexBuffer + 12 * i + 9, &ptList[i], 3 * sizeof(double));
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * ptList.size() * 12, 
        vertexBuffer, GL_DYNAMIC_DRAW);
    if (prog)
    {
        prog->SetUniformf("vColor", beamLineColor, 4);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_QUADS, 0, ptList.size() * 4);
        prog->SetUniformf("vColor", beamFaceColor, 4);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glDrawArrays(GL_QUADS, 0, ptList.size() * 4);

    //draw outline
    float beam_out_line_normal_color[4] = { 0.f, 1.f, 0.f, 1.0f };
    float beam_out_line_active_color[4] = { 1.f, 1.f, 0.f, 1.0f };
    float* beamOutLineColor = isActive ? 
        beam_out_line_active_color : beam_out_line_normal_color;
    if (prog)
    {
        prog->SetUniformf("vColor", beamOutLineColor, 4);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point3D) * ptNearFace.size(), 
        ptNearFace._Myfirst, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINE_LOOP, 0, ptNearFace.size());
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point3D) * ptList.size(), 
        ptList._Myfirst, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINE_LOOP, 0, ptList.size());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    TPS_DEL_ARRAY(vertexBuffer);
}

void BeamTransparencyRenderingNode::DrawIsoCenter_i()
{
    //draw iso-center
    glPointSize(4);
    glBegin(GL_POINTS);
    glVertex3d(0.0, 0.0, 0.0);
    glEnd();
}

void BeamTransparencyRenderingNode::DrawCentralLine_i(double sad)
{
    //draw central line 
    glLineStipple(2, 0xFC30);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, -sad);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    CHECK_GL_ERRORS;
}
TPS_END_NAMESPACE