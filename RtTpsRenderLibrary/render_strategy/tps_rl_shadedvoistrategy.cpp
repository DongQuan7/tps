//
//////////////////////////////////////////////////////////////////
/////  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
/////  All rights reserved.
///// 
/////  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
///// 
/////  \file tps_rl_shadedvoistrategy.cpp
///// 
/////  \brief class TpsShadedVoiStrategy definition 
///// 
/////  \version 1.0
///// 
/////  \date    2013/2/12
//////////////////////////////////////////////////////////////////
//
#include "StdAfx.h"
//
//#pragma warning(disable:4996)
//#pragma warning(disable:4505)
//#pragma warning(disable:4702)
//
//#include "RtTpsRenderLibrary/tps_rl_shadedvoistrategy.h"
//
//#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
//#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/graphic_object_plane.h"
//
//#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
//#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
//#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
//#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
//#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_shaders.h"
//#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
//#include "McsfMedViewer3DDataRepresentation/image_data.h"
//
//#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobject.h"      // for Dose surface
//#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobjectcollection.h"
//
////#include "RtTpsDataAccess/tps_da_voimanager.h"
//#include "RtTpsFramework/tps_fw_voientity.h"
//#include "RtTpsDataAccess/tps_da_idatarepository.h"
//#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
//
//#include "RtTpsFramework/tps_fw_modelwarehouse.h"
//#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
//#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
//#include "RtTpsRenderLibrary/GLSLProgramObject.h"
//#include "RtTpsFramework/tps_fw_common_enums.h"
//
//#include "tps_logger.h"
//
////#include "RtTpsRenderLibrary/nvModel.h"
////#include <RtTpsRenderLibrary/nvShaderUtils.h>
////#include <RtTpsRenderLibrary/nvSDKPath.h>
//#include "GL/glew.h"
//#include "GL/glut.h"
////D:/UIH/include/rt/tps/RtTpsRenderLibrary/
//#include <iostream>
//#include <sstream>
//#include <fstream>
//#include <string>
//
//#define FOVY 30.0
//#define ZNEAR 0.0001
//#define ZFAR 10.0
//#define MAX_DEPTH 1.0
//
////#define SHADER_PATH "\\appdata\\rt\\tps\\shaders\\"
//
//GLenum g_drawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT,
//    GL_COLOR_ATTACHMENT1_EXT,
//    GL_COLOR_ATTACHMENT2_EXT,
//    GL_COLOR_ATTACHMENT3_EXT,
//    GL_COLOR_ATTACHMENT4_EXT,
//    GL_COLOR_ATTACHMENT5_EXT,
//    GL_COLOR_ATTACHMENT6_EXT
//};
//
//#define  MAX_VERT_NUM   512*512 *80
//
//TPS_BEGIN_NAMESPACE
//
//TpsShadedVoiStrategy::TpsShadedVoiStrategy(WINDOW_TYPE windowType) :needRefreshBeamBuffer(true),
//    mCameraGO(nullptr), mVolumeGO(nullptr),mStateGO(nullptr),m_spDoseMeshGOCollection(nullptr),mWindowType(windowType){
//    g_shaderDualInit = new GLSLProgramObject();
//    g_shaderDualPeel = new GLSLProgramObject();
//    g_shaderDualFinal = new GLSLProgramObject();
//}
//
//TpsShadedVoiStrategy::~TpsShadedVoiStrategy() {
//    delete g_shaderDualInit; g_shaderDualInit = nullptr;
//    delete g_shaderDualPeel; g_shaderDualPeel = nullptr;
//    delete g_shaderDualFinal; g_shaderDualFinal = nullptr;
//}
//
//void TpsShadedVoiStrategy::Initialize() {
//
//    //create VBOs 
//    glGenVertexArrays(1,&mVertexArrayID);
//    glBindVertexArray(mVertexArrayID);
//
//    glGenBuffers(1,&mVertexBufferID);
//    glGenBuffers(1,&mNormalBufferID);
//    glGenBuffers(1,&mIndexBufferID);
//    glGenBuffers(1,&mColorBufferID);
//    glGenBuffers(1, &mMPRTextureVertID);
//
//    glBindVertexArray(0);
//
//    InitDualPeelingRenderTargets();
//
//    BuildShaders();
//}
//
//void TpsShadedVoiStrategy::Finalize() {
//    DestroyShaders();
//
//}
//
//void TpsShadedVoiStrategy::Render_i() {
//
//    if(mVolumeGO == nullptr || mCameraGO == nullptr 
//        || mShadedSurfaceGO == nullptr || mVoiCollectionGO == nullptr
//        /*|| mNormalGroupGO == nullptr*/) {
//            return;
//    }
//    //GenerateMprTextureBuffer();
//
//    //closed as the latest requirement by qiangqiang.zhou@20141212
//    //bool bIsSurfaceWindow = (SURFACE == mWindowType);
//
//    using namespace Mcsf::MedViewer3D;
//
//    mCameraGO->AdaptFovViaAspect((double)mWidth / mHeight);
//    Matrix4x4 matModelView = mCameraGO->GetViewMatrix();
//    Matrix4x4 matProjection = mCameraGO->GetProjectionMatrix();
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
//    glPushAttrib(GL_ENABLE_BIT);
//    glViewport(0, 0, mWidth, mHeight);
//
//    Matrix4x4 matP2W = mVolumeGO->GetWorld2Patient().Inverse();
//
//    matModelView.Append(matP2W);
//
//    //mScreenToTextMat = matProjection;
//    //mScreenToTextMat.Append(matModelView);
//    //mScreenToTextMat = mScreenToTextMat.Inverse();
//
//    //Matrix4x4 projection2Screen = Matrix4x4(sizeX / 2, 0 ,0, sizeX / 2 + startX,
//    //                                        0, sizeY / 2, 0, sizeY / 2 + startY,
//    //                                        0, 0, 1, 0, 
//    //                                        0, 0, 0 , 1);
//    //mScreenToTextMat.Append(projection2Screen.Inverse());
//
//    glEnable(GL_DEPTH_TEST);
//    glDepthMask(GL_TRUE);
//
//    glMatrixMode(GL_MODELVIEW);
//    glPushMatrix();
//    glLoadMatrixd(matModelView._m);
//
//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//    glLoadMatrixd(matProjection._m);
//
//    // parameters are from plunc!
//    Point3D ptEye = mCameraGO->GetEye();
//    Point3D lookAt = mCameraGO->GetLookAt();
//    Point3D eyeLight = ptEye * 0.9 +  lookAt * 0.1; // 让光源在眼睛附近
//    Point3D ptLight;
//    ptLight.x = mShadedSurfaceGO->GetLightX();
//    ptLight.y = mShadedSurfaceGO->GetLightY();
//    ptLight.z = mShadedSurfaceGO->GetLightZ();
//    bool useFixedLight = !mShadedSurfaceGO->GetUseCameraLight();
//
//    //printf("eye:%.2f, %.2f, %.2f\n", ptEye.x,ptEye.y,ptEye.z);
//    //printf("lookAt:%.2f, %.2f, %.2f\n", lookAt.x,lookAt.y,lookAt.z);
//    //printf("eye light dir:%.2f, %.2f, %.2f\n", eyeLight.x,eyeLight.y,eyeLight.z);
//    //printf("fix light dir:%.2f, %.2f, %.2f\n", ptLight.x,ptLight.y,ptLight.z);
//    // set light property
//    float point[4]={eyeLight.x,eyeLight.y,eyeLight.z,1.0f};
//    if (useFixedLight) {
//        point[0] = ptLight.x; point[1] = ptLight.y; point[2] = ptLight.z;
//    }
//
//    glFrontFace(GL_CCW);
//    glLightfv(GL_LIGHT0,GL_POSITION,point);
//    glEnable(GL_LIGHT0);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//    RenderPrism_i();
//
//    glFlush();
//    glDisable(GL_LIGHT0);
//    glDisable(GL_BLEND);
//    glDisable(GL_LIGHTING);
//    glDisable(GL_DEPTH_TEST);
//    glPopMatrix();
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//    glPopAttrib();
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//
//}
//
//void TpsShadedVoiStrategy::RenderPrism_i()
//{
//    // factors
//    //float specf = mShadedSurfaceGO->GetSpecularFactor();
//    //float ambf = mShadedSurfaceGO->GetAmbFactor();
//    //float diff = mShadedSurfaceGO->GetDiffFactor();
//    //float shininess = mShadedSurfaceGO->GetShininess();
//    bool displayLine = (mShadedSurfaceGO->GetDisplayMode()==0)?true:false;
//    bool displayVertexCube = mShadedSurfaceGO->GetDisplayVertexCube();
//    int startStripIndex = mShadedSurfaceGO->GetStartStripIndex();
//    int endStripIndex = mShadedSurfaceGO->GetEndStripIndex();
//
//    auto stripMap = mShadedSurfaceGO->GetPrismStripsMap();
//    auto voiMap = mShadedSurfaceGO->GetVoiMap();
//
//    using namespace Mcsf;
//    using namespace Mcsf::MedViewer3D;
//
//    // 设置用来显示网格点的小正方体的尺寸
//    VertexCube& vertexCube = mShadedSurfaceGO->GetVertexCubeObject();
//    vertexCube.SetSize(0.002f, 0.002f, 0.002f);
//
//    std::vector<std::string> renderVoiList;
//    renderVoiList.clear();
//
//    bool showBeams = GenerateBeamBuffers();
//
//    // 处理显示所有的strip
//    std::shared_ptr<TpsVOIGraphicObject> voiGo = nullptr;
//    tps::VOIEntity* pVoi = nullptr;
//    for (auto itr=stripMap.begin(); itr!=stripMap.end(); ++itr) {
//
//        // get voi color from voi manager
//        float color[4] ={0.5,0.5,0.5,0.5};
//        voiGo = mVoiCollectionGO->GetVOI(itr->first);
//        if (voiGo == nullptr) continue;
//        pVoi = voiGo->GetVoiEntity();
//        if (pVoi == nullptr) continue;
//        if (!pVoi->GetVoiVisibilty()) continue;
//        if (!pVoi->IsShaded()) continue;
//        voiGo->GetColor(color);
//        if(!displayLine) {
//            renderVoiList.push_back(pVoi->GetVoiUid());
//            continue;
//        }
//
//        if (displayLine){
//            glColor4f(color[0], color[1], color[2], 1.0f);
//        }
//
//        // 获得该VOI的所有strip指针
//        CON_STRIPS& strips = (*itr->second);
//        if (strips.size()<=0) {
//            TPS_LOG_DEV_ERROR<<"strips is empty. skip";
//            continue;
//        }
//        int nStart = (startStripIndex>=0)?startStripIndex:0;
//        int nEnd = (endStripIndex<strips.size())?endStripIndex:strips.size()-1;
//        //float dirfactor = 1.0f;
//
//        int nextIndex = 0;
//        int nextnextIndex = 0;
//
//        for (int n=nStart; n<=nEnd; n++) {
//            CON_STRIP& strip = strips[n];
//            if(strip.mPtArray.size() == 0) continue;
//            if(strip.mContourIndex1 == strip.mContourIndex2) {
//                for (int m=0; m<strip.mPtArray.size(); m+=1) {
//                    nextIndex = m + 1;
//                    nextnextIndex = m+ 2;
//                    if(nextIndex >= strip.mPtArray.size()) {
//                        nextIndex = nextIndex - strip.mPtArray.size();
//                    }
//                    if(nextnextIndex >= strip.mPtArray.size()) {
//                        nextnextIndex = nextnextIndex - strip.mPtArray.size();
//                    }
//                    Mcsf::Point3f& line1veterx1 = strip.mPtArray[m];
//                    Mcsf::Point3f& line1veterx2 = strip.mPtArray[nextIndex];
//                    Mcsf::Point3f& line2veterx1 = strip.mPtArray[nextnextIndex];
//                    if (displayVertexCube) {
//                        vertexCube.Update(line1veterx1.GetX(), line1veterx1.GetY(), line1veterx1.GetZ());
//                        vertexCube.Render();
//                        vertexCube.Update(line1veterx2.GetX(), line1veterx2.GetY(), line1veterx2.GetZ());
//                        vertexCube.Render();
//                        vertexCube.Update(line2veterx1.GetX(), line2veterx1.GetY(), line2veterx1.GetZ());
//                        vertexCube.Render();
//                    }
//
//                    if (displayLine) {
//                        // 绘制网格线
//                        glBegin(GL_LINE_LOOP);
//                        glVertex3f(line1veterx1.GetX(), line1veterx1.GetY(), line1veterx1.GetZ());
//                        glVertex3f(line1veterx2.GetX(), line1veterx2.GetY(), line1veterx2.GetZ());
//                        glVertex3f(line2veterx1.GetX(), line2veterx1.GetY(), line2veterx1.GetZ());
//                        glEnd();
//                    }
//                }
//                continue;
//            }
//            else {
//                for (int m=0; m<strip.mPtArray.size()-2; m+=2) {
//                    // 每两个点是一条线段
//                    Mcsf::Point3f& line1veterx1 = strip.mPtArray[m];
//                    Mcsf::Point3f& line1veterx2 = strip.mPtArray[m+1];
//                    Mcsf::Point3f& line2veterx1 = strip.mPtArray[m+2];
//                    Mcsf::Point3f& line2veterx2 = strip.mPtArray[m+3];
//
//                    // 确定第三个点
//                    Point3f vertex3rdPt;
//                    if (line2veterx1!=line1veterx1&&line2veterx1!=line1veterx2) {
//                        vertex3rdPt = line2veterx1;
//                    }
//                    else {
//                        vertex3rdPt = line2veterx2;
//                    }
//
//                    // 在网格点上显示小正方体
//                    if (displayVertexCube) {
//                        vertexCube.Update(line1veterx1.GetX(), line1veterx1.GetY(), line1veterx1.GetZ());
//                        vertexCube.Render();
//                        vertexCube.Update(line1veterx2.GetX(), line1veterx2.GetY(), line1veterx2.GetZ());
//                        vertexCube.Render();
//                        vertexCube.Update(vertex3rdPt.GetX(), vertex3rdPt.GetY(), vertex3rdPt.GetZ());
//                        vertexCube.Render();
//                    }
//
//                    if (displayLine) {
//                        // 绘制网格线
//                        glBegin(GL_LINE_LOOP);
//                        glVertex3f(line1veterx1.GetX(), line1veterx1.GetY(), line1veterx1.GetZ());
//                        glVertex3f(line1veterx2.GetX(), line1veterx2.GetY(), line1veterx2.GetZ());
//                        glVertex3f(vertex3rdPt.GetX(), vertex3rdPt.GetY(), vertex3rdPt.GetZ());
//                        glEnd();
//                    }
//                }
//            }
//        }
//    }
//
//    //render
//
//    mIndexBufferMap.clear();
//    mVertexBufferMap.clear();
//    mNormalBufferMap.clear();
//    mTotalTriangleSizeMap.clear();
//    mVoiMap.clear();
//        
//    std::map<std::string, float*> vertexBufferMap;
//    std::map<std::string, float*> normalBufferMap;
//    std::map<std::string, unsigned int*> indexBufferMap;
//    std::map<std::string, int> totalTriangleSizeMap;
//
//    mShadedSurfaceGO->GetShadedVoiBuffersMap(vertexBufferMap, normalBufferMap, indexBufferMap, totalTriangleSizeMap);
//    for(auto it = renderVoiList.begin(); it != renderVoiList.end(); ++it) {
//        mIndexBufferMap[*it] = indexBufferMap[*it];
//        mVertexBufferMap[*it] = vertexBufferMap[*it];
//        mNormalBufferMap[*it] = normalBufferMap[*it];
//        mTotalTriangleSizeMap[*it] = totalTriangleSizeMap[*it];
//        mVoiMap.push_back(*it);
//    }
//
//    RenderDualPeeling(showBeams);
//
//}
//
//void TpsShadedVoiStrategy::RenderOpenMesh_i()
//{
//
//}
//
//void TpsShadedVoiStrategy::Resize(int iWidth, int iHeight) {
//    mHeight = iHeight;
//    mWidth = iWidth;
//    mDirty =true;
//
//    for (int i = 0; i < 2; i++)
//    {
//        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualDepthTexId[i]);
//        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_FLOAT_RG32_NV, mWidth, mHeight,
//            0, GL_RGB, GL_FLOAT, 0);
//
//        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualFrontBlenderTexId[i]);
//        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, mWidth, mHeight,
//            0, GL_RGBA, GL_FLOAT, 0);
//    }
//}
//
//int TpsShadedVoiStrategy::CacheChanges() {
//    if(mVolumeGO == nullptr || mShadedSurfaceGO == nullptr || mCameraGO == nullptr || mVoiCollectionGO == nullptr) {
//        return -1;
//    }
//    if(mVolumeGO->IsDirty() || mShadedSurfaceGO->IsDirty() || mCameraGO->IsDirty() || mVoiCollectionGO->IsDirty() || (mMprGO != nullptr && mMprGO->IsDirty())) {
//        mDirty = true;
//    }
//    if (m_spDoseMeshGOCollection != nullptr)
//    {
//        if (m_spDoseMeshGOCollection->IsDirty())
//            mDirty = true;
//    }
//
//    if(mNormalGroupCollection == nullptr) return -1;
//    std::string activeNGUid = mNormalGroupCollection->GetActiveNormalGroupUID();
//    auto activeNG = mNormalGroupCollection->GetNormalGroup(activeNGUid);
//    if (activeNG == nullptr){
//        TPS_LOG_DEV_ERROR<<"There is no active normal group!";
//        return -1;
//    }
//
//    std::map<std::string, std::shared_ptr<TpsBeamGraphicObject>> beamMap = activeNG->GetBeamMap();
//
//    for(auto itr = beamMap.begin(); itr != beamMap.end(); ++itr) {
//
//        std::shared_ptr<TpsBeamGraphicObject> beamGo = itr->second;
//        if (beamGo == nullptr){
//            TPS_LOG_DEV_ERROR<<"TpsBeamGraphicObj is null!";
//            continue;
//        }
//        if(beamGo->IsDirty()) {
//            needRefreshBeamBuffer = true;
//            mDirty = true;
//            break;
//        }
//    }
//
//    if(beamMap.size() == 0 || activeNG->IsDirty())
//    {
//        needRefreshBeamBuffer = true;
//        mDirty = true;
//    }
//
//
//    return -1;
//}
//
//void TpsShadedVoiStrategy::SetShadedSurfaceGraphicObject(std::shared_ptr<
//    ShadedVoiGraphicObject> go) {
//    if (mShadedSurfaceGO != go){
//        mShadedSurfaceGO = go;
//        mDirty = true;
//    }
//}
//
//void TpsShadedVoiStrategy::SetVolumeGraphicObject(std::shared_ptr<
//    Mcsf::MedViewer3D::GraphicObjVolume> volume) {
//    if (mVolumeGO != volume){
//        mVolumeGO = volume;
//        mDirty = true;
//    }
//}
//
//void TpsShadedVoiStrategy::SetCameraGO(std::shared_ptr<CameraGraphicObject>
//    go) {
//    if (mCameraGO != go){
//        mCameraGO = go;
//        mDirty = true;
//    }
//}
//
//void TpsShadedVoiStrategy::SetStateControlGO(
//    std::shared_ptr<StateControlGraphicObject> stateControlGO){
//    if (mStateGO != stateControlGO){
//        mStateGO = stateControlGO;
//        mDirty = true;
//    }
//}
//
//void TpsShadedVoiStrategy::SetNormalGroupCollection(
//    std::shared_ptr<NormalGroupCollection> normalGroupCollection){
//    if (mNormalGroupCollection != normalGroupCollection){
//        mNormalGroupCollection = normalGroupCollection;
//        mDirty = true;
//    }
//}
//
//void TpsShadedVoiStrategy::SetVOICollectionGO(
//    std::shared_ptr<TpsVOIGOCollection> voiCollection){
//    if (mVoiCollectionGO != voiCollection){
//        mVoiCollectionGO = voiCollection;
//        mDirty = true;
//    }
//}
//
//void TpsShadedVoiStrategy::InitDualPeelingRenderTargets()
//{
//    glGenTextures(2, g_dualDepthTexId);
//    glGenTextures(2, g_dualFrontBlenderTexId);
//    glGenFramebuffersEXT(1, &g_dualPeelingSingleFboId);
//    for (int i = 0; i < 2; i++)
//    {
//        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualDepthTexId[i]);
//        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
//        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
//        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_FLOAT_RG32_NV, mWidth, mHeight,
//            0, GL_RGB, GL_FLOAT, 0);
//
//        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualFrontBlenderTexId[i]);
//        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
//        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
//        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, mWidth, mHeight,
//            0, GL_RGBA, GL_FLOAT, 0);
//    }
//    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_dualPeelingSingleFboId);
//
//    int j = 0;
//    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
//        GL_TEXTURE_RECTANGLE_ARB, g_dualDepthTexId[j], 0);
//    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,
//        GL_TEXTURE_RECTANGLE_ARB, g_dualFrontBlenderTexId[j], 0);
//
//    j = 1;
//    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT3_EXT,
//        GL_TEXTURE_RECTANGLE_ARB, g_dualDepthTexId[j], 0);
//    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT4_EXT,
//        GL_TEXTURE_RECTANGLE_ARB, g_dualFrontBlenderTexId[j], 0);
//
//    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
//
//    glGenTextures(1, &mMPRTextureID);
//    glBindTexture(GL_TEXTURE_2D, mMPRTextureID);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//
//
//    //glBindBuffer(GL_ARRAY_BUFFER, mMPRTextureVertID);
//    //glBufferData(GL_ARRAY_BUFFER, sizeof(texVert), texVert, GL_STATIC_DRAW);
//
//    //CHECK_GL_ERRORS;
//}
////--------------------------------------------------------------------------
//void TpsShadedVoiStrategy::DeleteDualPeelingRenderTargets()
//{
//    glDeleteFramebuffersEXT(1, &g_dualPeelingSingleFboId);
//    glDeleteTextures(2, g_dualDepthTexId);
//    glDeleteTextures(2, g_dualFrontBlenderTexId);
//    glDeleteTextures(1, &mMPRTextureID); 
//}
//
//void TpsShadedVoiStrategy::GenerateMprTextureBuffer()
//{
//    currentSection = mShadedSurfaceGO->GetMprSection();
//    if (currentSection == 4) return; //临时解决方案：由于是临时屏蔽Oblique下的MPR，代码写死4代表oblique模式下，直接return不勾画
//
//    if(currentSection == AXIAL)
//    {
//        mMprGO = mMprAxialGO;
//    }
//    else if(currentSection == CORONAL)
//    {
//        mMprGO = mMprCoronalGO;
//    }
//    else
//    {
//        mMprGO = mMprSagittalGO;
//    }
//    if(mMprGO == nullptr || mVolumeGO == nullptr) return;
//
//    using namespace Mcsf::MedViewer3D;
//    Point3D vertices[4];
//    Point3D verticesVolume[4];
//    mMprGO->GetVertices(vertices);
//
//    Matrix4x4 world2PatientMatrix = mVolumeGO->GetWorld2Patient();
//    size_t dim[3];
//    mVolumeGO->GetDimension(dim);
//
//    
//
//    Mcsf::MedViewer3D::Matrix4x4 modelToVolume(dim[0], 0, 0, 0,
//        0, dim[1], 0, 0,
//        0, 0, dim[2], 0,
//        -0.5, -0.5, -0.5, 1);
//    Mcsf::MedViewer3D::Matrix4x4 matWorld2Volume = modelToVolume * mVolumeGO->GetModel2World().Inverse();
//
//    Point3D point;
//    for (int i = 0; i < 4; ++i)
//    {
//        point = world2PatientMatrix.Transform(vertices[i]);
//        verticesVolume[i] = matWorld2Volume.Transform(vertices[i]);
//        vertices[i] = point;
//    }
//
//    std::shared_ptr<MedViewer3DImageData> imageData = mVolumeGO->GetImageData();
//    int dataType = imageData->GetDataType();
//    double slope = imageData->GetSlope();
//    double intercept = imageData->GetIntercept();
//    double windowWidth, windowCenter;
//    imageData->GetWindowCenterWindowWidth(&windowCenter, &windowWidth);
//
//    void *buffer = mVolumeGO->GetVolumeData();
//    int planeCount = dim[0] * dim[1];
//
//    int currentSlice = 0;
//    int bufferCount = 0;
//    int bufferXCount,bufferYCount;
//    if(currentSection == AXIAL)
//    {
//       currentSlice = verticesVolume[0].z + 0.5;
//       bufferCount = dim[0] * dim[1];
//       bufferXCount = dim[0];
//       bufferYCount = dim[1];
//    }
//    else if(currentSection == CORONAL)
//    {
//        currentSlice = verticesVolume[0].y + 0.5;
//        bufferCount = dim[0] * dim[2];
//        bufferXCount = dim[0];
//        bufferYCount = dim[2];
//    }
//    else
//    {
//        currentSlice = verticesVolume[0].x + 0.5;
//        bufferCount = dim[1] * dim[2];
//        bufferXCount = dim[1];
//        bufferYCount = dim[2];
//    }
//
//    float *currentBuffer = new float[bufferCount];
//    float ct = 0.0f;
//    float gray = 0.0f;
//    if(currentSection == AXIAL)
//    {
//        for(int j = 0; j < dim[1]; ++j) {
//            for(int k = 0; k < dim[0]; ++k) {
//                if(dataType == 0) {
//                    unsigned char *volumeData = nullptr;
//                    volumeData = (unsigned char*)(buffer);
//                    ct = (float)volumeData[currentSlice * planeCount + j * dim[0] + k] * slope + intercept;
//                }
//                else if (dataType == 3) {
//                    short *volumeData = nullptr;
//                    volumeData = (short*)(buffer);
//                    ct = (float)volumeData[currentSlice * planeCount + j * dim[0] + k] * slope + intercept;
//                }
//                else if (dataType == 1) {
//                    unsigned short *volumeData = nullptr;
//                    volumeData = (unsigned short*)(buffer);
//                    ct = (float)volumeData[currentSlice * planeCount + j * dim[0] + k] * slope + intercept;
//                }
//                else if (dataType == 2) {
//                    char *volumeData = nullptr;
//                    volumeData = (char*)(buffer);
//                    ct = (float)volumeData[currentSlice * planeCount + j * dim[0] + k] * slope + intercept;
//                }
//                else if (dataType == 4) {
//                    float *volumeData = nullptr;
//                    volumeData = (float*)(buffer);
//                    ct = (float)volumeData[currentSlice * planeCount + j * dim[0] + k] * slope + intercept;
//                }
//                else if (dataType == 5) {
//                    double *volumeData = nullptr;
//                    volumeData = (double*)(buffer);
//                    ct = (float)volumeData[currentSlice * planeCount + j * dim[0] + k] * slope + intercept;
//                }
//                gray = float(ct - windowCenter) / windowWidth + 0.5f;
//                gray = TPS_CLAMP(gray, 0.f, 1.f);
//                currentBuffer[j * dim[1] + k]= gray;
//            }
//        }
//    }
//    else if(currentSection == CORONAL)
//    {
//        for(int j = 0; j < dim[2]; ++j) {
//            for(int k = 0; k < dim[0]; ++k) {
//                if(dataType == 0) {
//                    unsigned char *volumeData = nullptr;
//                    volumeData = (unsigned char*)(buffer);
//                    ct = (float)volumeData[j * planeCount + currentSlice * dim[0] + k] * slope + intercept;
//                }
//                else if (dataType == 3) {
//                    short *volumeData = nullptr;
//                    volumeData = (short*)(buffer);
//                    ct = (float)volumeData[j * planeCount + currentSlice * dim[0] + k] * slope + intercept;
//                }
//                else if (dataType == 1) {
//                    unsigned short *volumeData = nullptr;
//                    volumeData = (unsigned short*)(buffer);
//                    ct = (float)volumeData[j * planeCount + currentSlice * dim[0] + k] * slope + intercept;
//                }
//                else if (dataType == 2) {
//                    char *volumeData = nullptr;
//                    volumeData = (char*)(buffer);
//                    ct = (float)volumeData[j * planeCount + currentSlice * dim[0] + k] * slope + intercept;
//                }
//                else if (dataType == 4) {
//                    float *volumeData = nullptr;
//                    volumeData = (float*)(buffer);
//                    ct = (float)volumeData[j * planeCount + currentSlice * dim[0] + k] * slope + intercept;
//                }
//                else if (dataType == 5) {
//                    double *volumeData = nullptr;
//                    volumeData = (double*)(buffer);
//                    ct = (float)volumeData[j * planeCount + currentSlice * dim[0] + k] * slope + intercept;
//                }
//                gray = float(ct - windowCenter) / windowWidth + 0.5f;
//                gray = TPS_CLAMP(gray, 0.f, 1.f);
//                currentBuffer[j * dim[1] + k]= gray;
//            }
//        }
//    }
//    else
//    {
//        for(int j = 0; j < dim[2]; ++j) {
//            for(int k = 0; k < dim[1]; ++k) {
//                if(dataType == 0) {
//                    unsigned char *volumeData = nullptr;
//                    volumeData = (unsigned char*)(buffer);
//                    ct = (float)volumeData[j * planeCount + k * dim[0] + currentSlice] * slope + intercept;
//                }
//                else if (dataType == 3) {
//                    short *volumeData = nullptr;
//                    volumeData = (short*)(buffer);
//                    ct = (float)volumeData[j * planeCount + k * dim[0] + currentSlice] * slope + intercept;
//                }
//                else if (dataType == 1) {
//                    unsigned short *volumeData = nullptr;
//                    volumeData = (unsigned short*)(buffer);
//                    ct = (float)volumeData[j * planeCount + k * dim[0] + currentSlice] * slope + intercept;
//                }
//                else if (dataType == 2) {
//                    char *volumeData = nullptr;
//                    volumeData = (char*)(buffer);
//                    ct = (float)volumeData[j * planeCount + k * dim[0] + currentSlice] * slope + intercept;
//                }
//                else if (dataType == 4) {
//                    float *volumeData = nullptr;
//                    volumeData = (float*)(buffer);
//                    ct = (float)volumeData[j * planeCount + k * dim[0] + currentSlice] * slope + intercept;
//                }
//                else if (dataType == 5) {
//                    double *volumeData = nullptr;
//                    volumeData = (double*)(buffer);
//                    ct = (float)volumeData[j * planeCount + k * dim[0] + currentSlice] * slope + intercept;
//                }
//                gray = float(ct - windowCenter) / windowWidth + 0.5f;
//                gray = TPS_CLAMP(gray, 0.f, 1.f);
//                currentBuffer[j * dim[1] + k]= gray;
//            }
//        }
//    }
//
//    glActiveTexture(GL_TEXTURE0 + 2);
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, mMPRTextureID);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, bufferXCount, bufferYCount, 0, 
//        GL_LUMINANCE, GL_FLOAT, currentBuffer);
//    glDisable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    glActiveTexture(GL_TEXTURE0);
//
//    delete []currentBuffer; currentBuffer = nullptr;
//}
////--------------------------------------------------------------------------
//
//void TpsShadedVoiStrategy::DrawModel(std::string uid, bool showBeams)
//{
//    for(auto it = mVoiMap.begin(); it != mVoiMap.end(); ++it) {
//        std::string voiUid = *it;
//
//        if(uid == "pass") {
//            auto voiGo = mVoiCollectionGO->GetVOI(voiUid);
//            if (voiGo == nullptr) continue;
//            tps::VOIEntity* pVoi = voiGo->GetVoiEntity();
//            if (pVoi == nullptr) continue;
//            float color[4];
//            pVoi->GetColor(color);
//            color[3] = pVoi->GetAlpha3D();  //use alpha3d
//            g_shaderDualPeel->setUniform("vColor", color, 4);
//        }
//
//        glBindVertexArray(mVertexArrayID);
//
//        glBindBuffer(GL_ARRAY_BUFFER,mVertexBufferID);
//        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*mTotalTriangleSizeMap[voiUid]*3*3,mVertexBufferMap[voiUid],GL_STATIC_DRAW);
//        glVertexPointer(3,GL_FLOAT,0,(GLfloat*)NULL);
//        glEnableClientState(GL_VERTEX_ARRAY);
//
//        glBindBuffer(GL_ARRAY_BUFFER,mNormalBufferID);
//        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*mTotalTriangleSizeMap[voiUid]*3*3,mNormalBufferMap[voiUid],GL_STATIC_DRAW);
//        glNormalPointer(GL_FLOAT,0,(GLfloat*)NULL);
//        glEnableClientState(GL_NORMAL_ARRAY);
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mIndexBufferID);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*mTotalTriangleSizeMap[voiUid]*3,mIndexBufferMap[voiUid],GL_STATIC_DRAW);
//
//        glBindVertexArray(0);
//
//        glBindVertexArray(mVertexArrayID);
//        glDrawElements(GL_TRIANGLES, mTotalTriangleSizeMap[voiUid] * 3, GL_UNSIGNED_INT, NULL);
//        glBindVertexArray(0);
//    }
//
//    DrawDoseMesh(uid); 
//
//    DrawMpr(uid);
//
//    //DrawVolume(uid);
//
//    if(!showBeams) return;
//    Draw3DSegment(uid);
//    for (auto it = mBeamColorMap.begin(); it != mBeamColorMap.end(); ++it) {
//        float *color = (*it).second;
//        std::string beamUid = (*it).first;
//
//        if(uid == "pass") {
//            g_shaderDualPeel->setUniform("vColor", color, 4);
//            glEnable(GL_STENCIL_TEST);
//            glStencilMask(0x01);
//            glStencilFunc(GL_ALWAYS, 0x01, 0x01);
//            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//
//            glDisable(GL_DEPTH_TEST);
//            glEnable(GL_LINE_SMOOTH);
//            glEnable(GL_BLEND);
//            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//        }
//        //draw edge
//        glBindVertexArray(mVertexArrayID);
//
//        glBindBuffer(GL_ARRAY_BUFFER,mVertexBufferID);
//        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*mBeamEdgeFloatNumMap[beamUid],mBeamEdgeMap[beamUid],GL_STATIC_DRAW);
//        glVertexPointer(3,GL_FLOAT,0,(GLfloat*)NULL);
//        glEnableClientState(GL_VERTEX_ARRAY);
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mIndexBufferID);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*mBeamEdgeFloatNumMap[beamUid] / 3,mBeamEdgeIndexMap[beamUid],GL_STATIC_DRAW);
//
//        glBindVertexArray(0);
//
//        glBindVertexArray(mVertexArrayID);
//        glDrawElements(GL_LINE_LOOP, mBeamEdgeFloatNumMap[beamUid] / 3, GL_UNSIGNED_INT, NULL);
//        glBindVertexArray(0);
//
//
//        //draw further edge
//        glBindVertexArray(mVertexArrayID);
//
//        glBindBuffer(GL_ARRAY_BUFFER,mVertexBufferID);
//        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*mBeamFurtherEdgeFloatNumMap[beamUid],mBeamFurtherEdgeMap[beamUid],GL_STATIC_DRAW);
//        glVertexPointer(3,GL_FLOAT,0,(GLfloat*)NULL);
//        glEnableClientState(GL_VERTEX_ARRAY);
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mIndexBufferID);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*mBeamFurtherEdgeFloatNumMap[beamUid] / 3,mBeamFurtherEdgeIndexMap[beamUid],GL_STATIC_DRAW);
//
//        glBindVertexArray(0);
//
//        glBindVertexArray(mVertexArrayID);
//        glDrawElements(GL_LINE_LOOP, mBeamFurtherEdgeFloatNumMap[beamUid] / 3, GL_UNSIGNED_INT, NULL);
//        glBindVertexArray(0);
//
//		//draw center line
//		glLineStipple(1,0xF18F);
//		glEnable(GL_LINE_STIPPLE);
//		int centerLineIndex[2];
//		centerLineIndex[0] = 0;
//		centerLineIndex[1] = 1;
//		glBindVertexArray(mVertexArrayID);
//
//		glBindBuffer(GL_ARRAY_BUFFER,mVertexBufferID);
//		glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*6,mBeamCenterLineMap[beamUid],GL_STATIC_DRAW);
//		glVertexPointer(3,GL_FLOAT,0,(GLfloat*)NULL);
//		glEnableClientState(GL_VERTEX_ARRAY);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mIndexBufferID);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*6 / 3,centerLineIndex,GL_STATIC_DRAW);
//
//		glBindVertexArray(0);
//
//		glBindVertexArray(mVertexArrayID);
//		glDrawElements(GL_LINE_LOOP, 6 / 3, GL_UNSIGNED_INT, NULL);
//		glBindVertexArray(0);
//
//		glDisable(GL_LINE_STIPPLE);
//
//        //draw side line
//
//        //glBindVertexArray(mVertexArrayID);
//
//        //glBindBuffer(GL_ARRAY_BUFFER,mVertexBufferID);
//        //glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*mBeamSideLineFloatNumMap[beamUid],mBeamSideLineMap[beamUid],GL_STATIC_DRAW);
//        //glVertexPointer(3,GL_FLOAT,0,(GLfloat*)NULL);
//        //glEnableClientState(GL_VERTEX_ARRAY);
//
//        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mIndexBufferID);
//        //glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*mBeamSideLineFloatNumMap[beamUid] / 3,mBeamSideLineIndexMap[beamUid],GL_STATIC_DRAW);
//
//        //glBindVertexArray(0);
//
//        //glBindVertexArray(mVertexArrayID);
//        //glDrawElements(GL_LINES, mBeamSideLineFloatNumMap[beamUid] / 3, GL_UNSIGNED_INT, NULL);
//        //glBindVertexArray(0);
//
//        if(uid == "pass") {
//            float tempColor[4];
//            tempColor[0] = color[0];
//            tempColor[1] = color[1];
//            tempColor[2] = color[2];
//            tempColor[3] = 0.3;
//            g_shaderDualPeel->setUniform("vColor", tempColor, 4);
//        }
//
//        glBindVertexArray(mVertexArrayID);
//
//        glBindBuffer(GL_ARRAY_BUFFER,mVertexBufferID);
//        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*mBeamSideWallFloatNumMap[beamUid],mBeamSideWallMap[beamUid],GL_STATIC_DRAW);
//        glVertexPointer(3,GL_FLOAT,0,(GLfloat*)NULL);
//        glEnableClientState(GL_VERTEX_ARRAY);
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mIndexBufferID);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*mBeamSideWallFloatNumMap[beamUid] / 3,mBeamSideWallIndexMap[beamUid],GL_STATIC_DRAW);
//
//        glBindVertexArray(0);
//
//        glBindVertexArray(mVertexArrayID);
//        glDrawElements(GL_QUADS, mBeamSideWallFloatNumMap[beamUid] / 3, GL_UNSIGNED_INT, NULL);
//        glBindVertexArray(0);
//
//    }
//}
////--------------------------------------------------------------------------
//void TpsShadedVoiStrategy::BuildShaders()
//{
//    g_shaderDualInit->attachVertexShader(Shaders::strDepthPeelingInitVertShader.c_str());
//    g_shaderDualInit->attachFragmentShader(Shaders::strDepthPeelingInitFragShader.c_str());
//    g_shaderDualInit->link();
//
//    g_shaderDualPeel->attachVertexShader(Shaders::strPhongShadingVertShader.c_str());
//    g_shaderDualPeel->attachVertexShader(Shaders::strDepthPeelingPeelVertShader.c_str());
//    g_shaderDualPeel->attachFragmentShader(Shaders::strPhongShadingFragShader.c_str());
//    g_shaderDualPeel->attachFragmentShader(Shaders::strDepthPeelingPeelFragShader.c_str());
//    g_shaderDualPeel->link();
//
//    g_shaderDualFinal->attachVertexShader(Shaders::strDepthPeelingFinalVertShader.c_str());
//    g_shaderDualFinal->attachFragmentShader(Shaders::strDepthPeelingFinalFragShader.c_str());
//    g_shaderDualFinal->link();
//}
//
////--------------------------------------------------------------------------
//void TpsShadedVoiStrategy::DestroyShaders()
//{
//    g_shaderDualInit->destroy();
//    g_shaderDualPeel->destroy();
//    g_shaderDualFinal->destroy();
//}
//
////--------------------------------------------------------------------------
//void TpsShadedVoiStrategy::RenderDualPeeling(bool showBeams)
//{
//    GenerateMprTextureBuffer();
//    //float matS2T[16];
//    //for (int i = 0; i < 4; i++)
//    //    for (int j = 0; j < 4; j++)
//    //    {
//    //        matS2T[i * 4 + j] = (mScreenToTextMat.m)[i][j];
//    //    }
//    showBeams;
//    using namespace Mcsf::MedViewer3D;
//    Matrix4x4 matModelView = mCameraGO->GetViewMatrix();
//    Point3D ptEye = mCameraGO->GetEye();
//    Point3D lookAt = mCameraGO->GetLookAt();
//    Point3D eyeLight = ptEye * 0.9 +  lookAt * 0.1; // 让光源在眼睛附近
//    Point3D viewEyeLight = matModelView.Transform(eyeLight);
//    Point3D ptLight, viewPtLight;
//    ptLight.x = mShadedSurfaceGO->GetLightX();
//    ptLight.y = mShadedSurfaceGO->GetLightY();
//    ptLight.z = mShadedSurfaceGO->GetLightZ();
//    viewPtLight = matModelView.Transform(ptLight);
//    bool useFixedLight = !mShadedSurfaceGO->GetUseCameraLight();
//    float shiness = mShadedSurfaceGO->GetShininess();
//    float amb = mShadedSurfaceGO->GetAmbFactor();
//    float diff = mShadedSurfaceGO->GetDiffFactor();
//    float spec = mShadedSurfaceGO->GetSpecularFactor();
//    float sads[4] = {spec, amb, diff, shiness};
//    int useLight = 1;
//    int useText = -1;
//    float point[4]={viewEyeLight.x,viewEyeLight.y,viewEyeLight.z,1.0f};
//    if (useFixedLight) {
//        point[0] = viewPtLight.x; point[1] = viewPtLight.y; point[2] = viewPtLight.z;
//    }
//
//    float vEyePos[4];
//    float vLightPos[4];
//
//    vEyePos[0] = point[0];
//    vEyePos[1] = point[1];
//    vEyePos[2] = point[2];
//    vEyePos[3] = point[3];
//
//    vLightPos[0] = point[0];
//    vLightPos[1] = point[1];
//    vLightPos[2] = point[2];
//    vLightPos[3] = point[3];
//
//    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
//    glBlendEquationEXT(GL_MAX_EXT);
//    // ---------------------------------------------------------------------
//    // 1. Initialize Min-Max Depth Buffer
//    // ---------------------------------------------------------------------
//
//   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_dualPeelingSingleFboId);
//
//    // Render targets 1 and 2 store the front and back colors
//    // Clear to 0.0 and use MAX blending to filter written color
//    // At most one front color and one back color can be written every pass
//    glDrawBuffers(2, &g_drawBuffers[1]);
//    glClearColor(0, 0, 0, 0);
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    // Render target 0 stores (-minDepth, maxDepth, alphaMultiplier)
//    glDrawBuffer(g_drawBuffers[0]);
//    glClearColor(-MAX_DEPTH, MAX_DEPTH, 0, 0);
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    g_shaderDualInit->bind();
//    DrawModel("All", showBeams);
//    g_shaderDualInit->unbind();
//
//   // CHECK_GL_ERRORS;
//    // ---------------------------------------------------------------------
//    // 2. Dual Depth Peeling + Blending
//    // ---------------------------------------------------------------------
//    int currId = 0;
//    for (int pass = 1;pass < 5; pass++) 
//    {
//        currId = pass % 2;
//        int prevId = 1 - currId;
//        int bufId = currId * 3;	
//
//        glDrawBuffers(2, &g_drawBuffers[bufId+1]);
//        glClearColor(0, 0, 0, 0);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        glDrawBuffer(g_drawBuffers[bufId+0]);
//        glClearColor(-MAX_DEPTH, MAX_DEPTH, 0, 0);
//        glClear(GL_COLOR_BUFFER_BIT);
//        // Render target 0: RG32F MAX blending
//        // Render target 1: RGBA MAX blending
//        // Render target 2: RGBA MAX blending
//        glDrawBuffers(3, &g_drawBuffers[bufId+0]);
//        glBlendEquationEXT(GL_MAX_EXT);
//        g_shaderDualPeel->bind();
//        g_shaderDualPeel->bindTextureRECT("DepthBlenderTex", g_dualDepthTexId[prevId], 0);
//        g_shaderDualPeel->bindTextureRECT("FrontBlenderTex", g_dualFrontBlenderTexId[prevId], 1);
//        g_shaderDualPeel->bindTexture2D("textMpr", mMPRTextureID, 2);
//        //g_shaderDualPeel->setUniform("matScreen2text", matS2T, 16);
//        g_shaderDualPeel->setUniform("vLightpos", vLightPos, 4);
//        g_shaderDualPeel->setUniform("vEyepos", vEyePos, 4);
//        g_shaderDualPeel->setUniform("sads", sads, 4);
//        g_shaderDualPeel->setUniform("useLight", &useLight, 1);
//        g_shaderDualPeel->setUniform("useText", &useText, 1);
//        DrawModel("pass", showBeams);
//        g_shaderDualPeel->unbind();
//        //CHECK_GL_ERRORS;
//    }
//    glDisable(GL_BLEND);
//    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
//
//    // ---------------------------------------------------------------------
//    // 3. Final Pass
//    // ---------------------------------------------------------------------
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
//    glDrawBuffer(GL_COLOR_ATTACHMENT0);
//
//    glDrawBuffer(GL_BACK);
//    g_shaderDualFinal->bind();
//    g_shaderDualFinal->bindTextureRECT("FrontBlenderTex", g_dualFrontBlenderTexId[currId], 1);
//    DrawModel("All", showBeams);
//    g_shaderDualFinal->unbind();
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//    glFlush();
//
//    //clear maps
//    //ReleaseBeamBuffers();
//    glBlendEquationEXT(GL_FUNC_ADD_EXT);
//    //CHECK_GL_ERRORS;
//}
//
////--------------------------------------------------------------------------
//void TpsShadedVoiStrategy::reshape(int w, int h)
//{
//    if (mWidth != w || mHeight != h)
//    {
//        mWidth = w;
//        mHeight = h;
//        DeleteDualPeelingRenderTargets();
//        InitDualPeelingRenderTargets();
//    }
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(FOVY, (float)mWidth/(float)mHeight, ZNEAR, ZFAR);
//    glMatrixMode(GL_MODELVIEW);
//    glViewport(0, 0, mWidth, mHeight);
//}
//
////--------------------------------------------------------------------------
//void TpsShadedVoiStrategy::idle()
//{
//    glutPostRedisplay();
//}
//
//bool TpsShadedVoiStrategy::GenerateBeamBuffers() {
//
//    if(needRefreshBeamBuffer)
//    {
//        ReleaseBeamBuffers();
//    }
//    if(mNormalGroupCollection == nullptr) return false;
//    if(!needRefreshBeamBuffer) return true;
//    needRefreshBeamBuffer = false;
//
//    using namespace Mcsf::MedViewer3D;
//
//    double nearScale = 0.25, farScale = 1.25;
//
//    Matrix4x4 matPat2World = mVolumeGO->GetWorld2Patient().Inverse();
//    Matrix4x4 matViewProjection = mCameraGO->GetViewProjectionMatrix();
//
//    Matrix4x4 matWorld2Pat = mVolumeGO->GetWorld2Patient();
//
//    std::string activeNGUid = mNormalGroupCollection->GetActiveNormalGroupUID();
//    auto activeNG = mNormalGroupCollection->GetNormalGroup(activeNGUid);
//    if (activeNG == nullptr){
//        TPS_LOG_DEV_ERROR<<"There is no active normal group!";
//        return false;
//    }
//
//    std::map<std::string, std::shared_ptr<TpsBeamGraphicObject>> beamMap = activeNG->GetBeamMap();
//
//    auto activeBeam = activeNG->GetActiveBeamGo();
//
//    float fColor[4] = {0.};
//    TPS_COLOR colorType = GREEN;
//    GetClorFromEnum(colorType, fColor);
//
//    float fColorSelected[4] = {0.};
//    TPS_COLOR colorTypeSelected = YELLOW;
//    GetClorFromEnum(colorTypeSelected, fColorSelected);
//
//    for(auto itr = beamMap.begin(); itr != beamMap.end(); ++itr) {
//
//        std::shared_ptr<TpsBeamGraphicObject> beamGo = itr->second;
//        if (beamGo == nullptr){
//            TPS_LOG_DEV_ERROR<<"TpsBeamGraphicObj is null!";
//            continue;
//        }
//        if(beamGo->GetVisible() == false) continue;
//        std::shared_ptr<TpsSegmentGraphicObject> segment = beamGo->GetActiveSegmentGo();
//        if(segment == nullptr) {
//            TPS_LOG_DEV_ERROR<<"segment is null!";
//            continue;
//        }
//        std::vector<Point3D> ptList;
//        segment->GetOutLine(&ptList);
//        if (ptList.empty()){
//            continue;
//        }
//
//        //delete the same point first
//        std::vector<Point3D> ptList1;
//        int size = ptList.size();
//        int prei = 0;
//        for (int i = 0; i < size; ++i) {
//            prei = i - 1;
//            if(i == 0) prei = size - 1;
//            if((ptList[i] - ptList[prei]).Magnitude() < 1e-6) continue;
//            ptList1.push_back(ptList[i]);
//        }
//
//        std::vector<Point3D> ptListTemp;
//
//        size = ptList1.size();
//        prei = 0;
//        int nexti = 0;
//
//        for(int i = 0; i < size; ++i) {
//            prei = i - 1;
//            nexti = i + 1;
//            if(i == 0) prei = size - 1;
//            if(i == size - 1) nexti = 0;
//            Vector3D v1(ptList1[nexti].x - ptList1[i].x, ptList1[nexti].y - ptList1[i].y, ptList1[nexti].z - ptList1[i].z);
//            Vector3D v2(ptList1[i].x - ptList1[prei].x, ptList1[i].y - ptList1[prei].y, ptList1[i].z - ptList1[prei].z);
//            if(v2.Magnitude() < 1e-6) continue;
//            double angle = v1.AngleBetween(v2);
//            if(abs(angle) < 1e-6) continue;
//            ptListTemp.push_back(ptList1[i]);
//        }
//
//        Matrix4x4 matBeam2Patient;
//        segment->GetStartCollimatorToPatMatrix(&matBeam2Patient);
//
//        Point3D ptSource = matBeam2Patient.Transform(Point3D(0.0,0.0,0.0));
//		Point3D ptIsocenter = matBeam2Patient.Transform(Point3D(0.0,0.0,-1000.0));
//		float *beamCenterLine = new float[6];;
//		beamCenterLine[0] = ptSource.x;
//		beamCenterLine[1] = ptSource.y;
//		beamCenterLine[2] = ptSource.z;
//		beamCenterLine[3] = ptIsocenter.x;
//		beamCenterLine[4] = ptIsocenter.y;
//		beamCenterLine[5] = ptIsocenter.z;
//		mBeamCenterLineMap[beamGo->GetUID()] = beamCenterLine;
//
//        Point3D ptCurrent(0.0,0.0,0.0);
//        if (beamGo == activeBeam){
//            float *beamColor = new float[4];
//            beamColor[0] = fColorSelected[0];
//            beamColor[1] = fColorSelected[1];
//            beamColor[2] = fColorSelected[2];
//            beamColor[3] = fColorSelected[3];
//            mBeamColorMap[beamGo->GetUID()] = beamColor;
//        }
//        else{
//            float *beamColor = new float[4];
//            beamColor[0] = fColor[0];
//            beamColor[1] = fColor[1];
//            beamColor[2] = fColor[2];
//            beamColor[3] = fColor[3];
//            mBeamColorMap[beamGo->GetUID()] = beamColor;
//        }
//
//        float *beamEdgeLine = new float[ptListTemp.size() * 3];
//        unsigned int *beamEdgeIndex = new unsigned int[ptListTemp.size()];
//        int edgeIndex = 0;
//        for(std::vector<Point3D>::iterator itrr = ptListTemp.begin(); itrr != ptListTemp.end(); ++itrr) {
//            Point3D ptTemp = matBeam2Patient.Transform(*itrr);
//            beamEdgeLine[3 * edgeIndex] = nearScale * ptTemp.x + (1 - nearScale) * ptSource.x;
//            beamEdgeLine[3 * edgeIndex + 1] = nearScale * ptTemp.y + (1 - nearScale) * ptSource.y;
//            beamEdgeLine[3 * edgeIndex + 2] = nearScale * ptTemp.z + (1 - nearScale) * ptSource.z;
//            beamEdgeIndex[edgeIndex] = edgeIndex;
//            edgeIndex++;
//        }
//        mBeamEdgeMap[beamGo->GetUID()] = beamEdgeLine;
//        mBeamEdgeFloatNumMap[beamGo->GetUID()] = ptListTemp.size() * 3;
//        mBeamEdgeIndexMap[beamGo->GetUID()] = beamEdgeIndex;
//
//        float *beamFurtherEdgeLine = new float[ptListTemp.size() * 3];
//        unsigned int *beamFurtherEdgeIndex = new unsigned int[ptListTemp.size()];
//        edgeIndex = 0;
//        for(std::vector<Point3D>::iterator itrr = ptListTemp.begin(); itrr != ptListTemp.end(); ++itrr) {
//            Point3D ptTemp = matBeam2Patient.Transform(*itrr);
//            beamFurtherEdgeLine[3 * edgeIndex] = farScale * ptTemp.x + (1 - farScale) * ptSource.x;
//            beamFurtherEdgeLine[3 * edgeIndex + 1] = farScale * ptTemp.y + (1 - farScale) * ptSource.y;
//            beamFurtherEdgeLine[3 * edgeIndex + 2] = farScale * ptTemp.z + (1 - farScale) * ptSource.z;
//            beamFurtherEdgeIndex[edgeIndex] = edgeIndex;
//            edgeIndex++;
//        }
//        mBeamFurtherEdgeMap[beamGo->GetUID()] = beamFurtherEdgeLine;
//        mBeamFurtherEdgeFloatNumMap[beamGo->GetUID()] = ptListTemp.size() * 3;
//        mBeamFurtherEdgeIndexMap[beamGo->GetUID()] = beamFurtherEdgeIndex;
//
//
//        float *beamSideLine = new float[ptListTemp.size() * 2 * 3];
//        unsigned int*beamSideLineIndex = new unsigned int[ptListTemp.size() * 2];
//        int sideLineIndex = 0;
//        for(std::vector<Point3D>::iterator itrr = ptListTemp.begin(); itrr != ptListTemp.end(); ++itrr) {
//
//            ptCurrent.x = matBeam2Patient.Transform(*itrr).x;
//            ptCurrent.y = matBeam2Patient.Transform(*itrr).y;
//            ptCurrent.z = matBeam2Patient.Transform(*itrr).z;
//
//            beamSideLine[sideLineIndex * 6] = (1 - nearScale) * ptSource.x + nearScale * ptCurrent.x;
//            beamSideLine[sideLineIndex * 6 + 1] = (1 - nearScale) * ptSource.y + nearScale * ptCurrent.y;
//            beamSideLine[sideLineIndex * 6 + 2] = (1 - nearScale) * ptSource.z + nearScale * ptCurrent.z;
//            beamSideLine[sideLineIndex * 6 + 3] = (1 - farScale) * ptSource.x + farScale * ptCurrent.x;
//            beamSideLine[sideLineIndex * 6 + 4] = (1 - farScale) * ptSource.y + farScale * ptCurrent.y;
//            beamSideLine[sideLineIndex * 6 + 5] = (1 - farScale) * ptSource.z + farScale * ptCurrent.z;
//            beamSideLineIndex[sideLineIndex * 2] = sideLineIndex * 2;
//            beamSideLineIndex[sideLineIndex * 2 + 1] = sideLineIndex * 2 + 1;
//            sideLineIndex++;
//        }
//        mBeamSideLineMap[beamGo->GetUID()] = beamSideLine;
//        mBeamSideLineFloatNumMap[beamGo->GetUID()] = ptListTemp.size() * 2 * 3;
//        mBeamSideLineIndexMap[beamGo->GetUID()] = beamSideLineIndex;
//
//        float *beamSideWall = new float[ptListTemp.size() * 4 * 3];
//        unsigned int *beamSideWallIndex = new unsigned int[ptListTemp.size() * 4];
//        int count = ptListTemp.size();
//        int sideWallIndex = 0;
//        for(int i = 0 ; i < count; ++i) {
//            int j = i+1;
//            if(i == count - 1) {
//                j = 0;
//            }
//            Point3D ptFirst = matBeam2Patient.Transform(ptListTemp[i]);
//            Point3D ptSecond = matBeam2Patient.Transform(ptListTemp[j]);
//
//            Point3D pt1 = ptFirst * nearScale + ptSource * (1 - nearScale);
//            Point3D pt2 = ptFirst * farScale + ptSource * (1 - farScale);
//            Point3D pt3 = ptSecond * farScale + ptSource * (1 - farScale);
//            Point3D pt4 = ptSecond * nearScale + ptSource * (1 - nearScale);
//
//            beamSideWall[sideWallIndex * 12] = pt1.x;
//            beamSideWall[sideWallIndex * 12 + 1] = pt1.y;
//            beamSideWall[sideWallIndex * 12 + 2] = pt1.z;
//            beamSideWall[sideWallIndex * 12 + 3] = pt2.x;
//            beamSideWall[sideWallIndex * 12 + 4] = pt2.y;
//            beamSideWall[sideWallIndex * 12 + 5] = pt2.z;
//            beamSideWall[sideWallIndex * 12 + 6] = pt3.x;
//            beamSideWall[sideWallIndex * 12 + 7] = pt3.y;
//            beamSideWall[sideWallIndex * 12 + 8] = pt3.z;
//            beamSideWall[sideWallIndex * 12 + 9] = pt4.x;
//            beamSideWall[sideWallIndex * 12 + 10] = pt4.y;
//            beamSideWall[sideWallIndex * 12 + 11] = pt4.z;
//            beamSideWallIndex[sideWallIndex * 4] = sideWallIndex * 4;
//            beamSideWallIndex[sideWallIndex * 4 + 1] = sideWallIndex * 4 + 1;
//            beamSideWallIndex[sideWallIndex * 4 + 2] = sideWallIndex * 4 + 2;
//            beamSideWallIndex[sideWallIndex * 4 + 3] = sideWallIndex * 4 + 3;
//            sideWallIndex++;
//        }
//        mBeamSideWallMap[beamGo->GetUID()] = beamSideWall;
//        mBeamSideWallFloatNumMap[beamGo->GetUID()] = ptListTemp.size() * 4 * 3;
//        mBeamSideWallIndexMap[beamGo->GetUID()] = beamSideWallIndex;
//    }
//    return true;
//}
//
//void TpsShadedVoiStrategy::ReleaseBeamBuffers() {
//
//    //clear beam buffers
//    for (auto it = mBeamColorMap.begin();it!=mBeamColorMap.end(); ++it) {
//        if((*it).second == nullptr) continue;
//        delete [](*it).second;
//        (*it).second = nullptr;
//    }
//    mBeamColorMap.clear();
//
//    for (auto it = mBeamEdgeMap.begin();it!=mBeamEdgeMap.end(); ++it) {
//        if((*it).second == nullptr) continue;
//        delete [](*it).second;
//        (*it).second = nullptr;
//    }
//    mBeamEdgeMap.clear();
//
//    for (auto it = mBeamFurtherEdgeMap.begin();it!=mBeamFurtherEdgeMap.end(); ++it) {
//        if((*it).second == nullptr) continue;
//        delete [](*it).second;
//        (*it).second = nullptr;
//    }
//    mBeamFurtherEdgeMap.clear();
//
//    for (auto it = mBeamSideLineMap.begin();it!=mBeamSideLineMap.end(); ++it) {
//        if((*it).second == nullptr) continue;
//        delete [](*it).second;
//        (*it).second = nullptr;
//    }
//    mBeamSideLineMap.clear();
//
//    for (auto it = mBeamSideWallMap.begin();it!=mBeamSideWallMap.end(); ++it) {
//        if((*it).second == nullptr) continue;
//        delete [](*it).second;
//        (*it).second = nullptr;
//    }
//    mBeamSideWallMap.clear();
//
//    for (auto it = mBeamEdgeIndexMap.begin();it!=mBeamEdgeIndexMap.end(); ++it) {
//        if((*it).second == nullptr) continue;
//        delete [](*it).second;
//        (*it).second = nullptr;
//    }
//    mBeamEdgeIndexMap.clear();
//
//    for (auto it = mBeamFurtherEdgeIndexMap.begin();it!=mBeamFurtherEdgeIndexMap.end(); ++it) {
//        if((*it).second == nullptr) continue;
//        delete [](*it).second;
//        (*it).second = nullptr;
//    }
//    mBeamFurtherEdgeIndexMap.clear();
//
//    for (auto it = mBeamSideLineIndexMap.begin();it!=mBeamSideLineIndexMap.end(); ++it) {
//        if((*it).second == nullptr) continue;
//        delete [](*it).second;
//        (*it).second = nullptr;
//    }
//    mBeamSideLineIndexMap.clear();
//
//    for (auto it = mBeamSideWallIndexMap.begin();it!=mBeamSideWallIndexMap.end(); ++it) {
//        if((*it).second == nullptr) continue;
//        delete [](*it).second;
//        (*it).second = nullptr;
//    }
//    mBeamSideWallIndexMap.clear();
//
//	for (auto it = mBeamCenterLineMap.begin();it!=mBeamCenterLineMap.end(); ++it) {
//		if((*it).second == nullptr) continue;
//		delete [](*it).second;
//		(*it).second = nullptr;
//	}
//	mBeamCenterLineMap.clear();
//}
//
//void TpsShadedVoiStrategy::SetDoseMeshGOCollection(std::shared_ptr<DoseMeshGraphicObjectCollection> spDoseMeshGOCollection){
//    if (m_spDoseMeshGOCollection!= spDoseMeshGOCollection){
//        m_spDoseMeshGOCollection = spDoseMeshGOCollection;
//        mDirty = true;
//    }
//}
//
//void TpsShadedVoiStrategy::SetMprGo(std::shared_ptr<MPRGraphicObject> mprGoAxial, std::shared_ptr<MPRGraphicObject> mprGoCoronal,
//    std::shared_ptr<MPRGraphicObject> mprGoSagittal) {
//    if (mMprAxialGO != mprGoAxial){
//        mMprAxialGO = mprGoAxial;
//        mDirty = true;
//    }
//    if (mMprCoronalGO != mprGoCoronal){
//        mMprCoronalGO = mprGoCoronal;
//        mDirty = true;
//    }
//    if (mMprSagittalGO != mprGoSagittal){
//        mMprSagittalGO = mprGoSagittal;
//        mDirty = true;
//    }
//
//    mMprGO = mprGoCoronal;
//    currentSection = CORONAL;
//}
//
//void TpsShadedVoiStrategy::DrawDoseMesh(std::string uid)
//{
//    if (m_spDoseMeshGOCollection != nullptr)
//    {
//        auto doseMeshCollection = m_spDoseMeshGOCollection->GetAllDoseMeshGo();
//        for (int index = 0; index < doseMeshCollection.size();++index)
//        {
//            auto m_spDoseMeshGO = doseMeshCollection[index];
//            if(!m_spDoseMeshGO->GetValidFlag()) continue;
//            int nQuadPoint = 0;
//            float* pVertex = nullptr;
//            float* pNormal = nullptr;
//            unsigned int* pIndex = nullptr;
//            nQuadPoint = m_spDoseMeshGO->GetElementBuffer(&pVertex, &pNormal, &pIndex);
//            if (nQuadPoint > 0)
//            {
//                if(uid == "pass") 
//                {
//                    float color[4];
//                    float fRed, fGreen, fBlue, fAlpha;
//                    m_spDoseMeshGO->GetDoseColor(&fRed, &fGreen, &fBlue, &fAlpha);
//                    color[0] = fRed;
//                    color[1] = fGreen;
//                    color[2] = fBlue;
//                    color[3] = fAlpha;
//                    g_shaderDualPeel->setUniform("vColor", color, 4);
//                }
//
//                glBindVertexArray(mVertexArrayID);
//
//                glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
//                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nQuadPoint * 3, pVertex, GL_STATIC_DRAW);
//                glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)NULL);
//                glEnableClientState(GL_VERTEX_ARRAY);
//
//                glBindBuffer(GL_ARRAY_BUFFER, mNormalBufferID);
//                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nQuadPoint * 3, pNormal, GL_STATIC_DRAW);
//                glNormalPointer(GL_FLOAT, 0, (GLfloat*)NULL);
//                glEnableClientState(GL_NORMAL_ARRAY);
//
//                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);
//                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* nQuadPoint, pIndex, GL_STATIC_DRAW);
//
//                glDrawElements(GL_QUAD_STRIP, nQuadPoint, GL_UNSIGNED_INT, NULL);
//                glBindVertexArray(0);
//            }
//        }
//    }
//}
//void TpsShadedVoiStrategy::Draw3DSegment(std::string uid)
//{
//    using namespace Mcsf::MedViewer3D;
//    std::string activeNGUid = mNormalGroupCollection->GetActiveNormalGroupUID();
//    auto activeNG = mNormalGroupCollection->GetNormalGroup(activeNGUid);
//    if (activeNG == nullptr){
//        TPS_LOG_DEV_ERROR<<"There is no active normal group!";
//        return;
//    }
//    int useLight = -1;
//    std::map<std::string, std::shared_ptr<TpsBeamGraphicObject>> beamMap = activeNG->GetBeamMap();
//
//    auto activeBeam = activeNG->GetActiveBeamGo();
//
//    if(uid == "pass") {
//        float tempColor[4];
//        tempColor[0] = 0.3765f;
//        tempColor[1] = 0.3020f;
//        tempColor[2] = 0.2549f;
//        tempColor[3] = 1.0;
//        g_shaderDualPeel->setUniform("vColor", tempColor, 4);
//        g_shaderDualPeel->setUniform("useLight", &useLight, 1);
//    }
//
//    for(auto itr = beamMap.begin(); itr != beamMap.end(); ++itr) {
//
//        std::shared_ptr<TpsBeamGraphicObject> beamGo = itr->second;
//        if (beamGo == nullptr){
//            TPS_LOG_DEV_ERROR<<"TpsBeamGraphicObj is null!";
//            continue;
//        }
//        if(beamGo->GetVisible() == false) continue;
//        std::shared_ptr<TpsSegmentGraphicObject> segment = beamGo->GetActiveSegmentGo();
//        if(segment == nullptr) {
//            TPS_LOG_DEV_ERROR<<"segment is null!";
//            continue;
//        }
//
//        std::vector<Point3D> ptList = segment->Get3DSegmentPtList();
//
//        if(ptList.size() <= 0) return;
//
//        float *floatList = new float[ptList.size() * 3];
//
//        unsigned int* pIndex = new unsigned int[ptList.size()];
//
//        for(int i = 0; i < ptList.size(); ++i) {
//            floatList[3 * i + 0] = ptList[i].x;
//            floatList[3 * i + 1] = ptList[i].y;
//            floatList[3 * i + 2] = ptList[i].z;
//            pIndex[i] = i;
//        }
//
//        glBindVertexArray(mVertexArrayID);
//
//        glBindBuffer(GL_ARRAY_BUFFER,mVertexBufferID);
//        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*ptList.size() * 3,floatList,GL_STATIC_DRAW);
//        glVertexPointer(3,GL_FLOAT,0,(GLfloat*)NULL);
//        glEnableClientState(GL_VERTEX_ARRAY);
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mIndexBufferID);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*ptList.size(),pIndex,GL_STATIC_DRAW);
//
//        glBindVertexArray(0);
//
//        glBindVertexArray(mVertexArrayID);
//        glDrawElements(GL_QUADS, ptList.size(), GL_UNSIGNED_INT, NULL);
//        glBindVertexArray(0);
//    }
//
//    if(uid == "pass") {
//        useLight = 1;
//        g_shaderDualPeel->setUniform("useLight", &useLight, 1);
//    }
//}
//
//void TpsShadedVoiStrategy::DrawMpr(std::string uid)
//{
//    currentSection = mShadedSurfaceGO->GetMprSection();
//    if (currentSection == 4) return; //临时解决方案：由于是临时屏蔽Oblique下的MPR，代码写死4代表oblique模式下，直接return不勾画
//
//    if(mMprGO == nullptr || mVolumeGO == nullptr) return;
//    using namespace Mcsf::MedViewer3D;
//    int useLight = -1;
//    int useText = 1;
//    float mprTransparency3D = 1.0f - mShadedSurfaceGO->GetMprTransparency3D();
//    if(mprTransparency3D < 1e-6)
//    {
//        return;
//    }
//    if(uid == "pass") {
//        g_shaderDualPeel->setUniform("useLight", &useLight, 1);
//        g_shaderDualPeel->setUniform("useText", &useText, 1);
//        g_shaderDualPeel->setUniform("mprTransparency3D", &mprTransparency3D, 1);
//    }
//    Point3D vertices[4];
//    mMprGO->GetVertices(vertices);
//    Matrix4x4 world2PatientMatrix = mVolumeGO->GetWorld2Patient();
//
//    using namespace Mcsf::MedViewer3D;
//    std::shared_ptr<MedViewer3DImageData> imageData = mVolumeGO->GetImageData();
//    Vector3D axis[3];
//    double spacing[3], size[3];
//    Point3D originPoint, lastPoint;
//    size_t dim[3];
//    imageData->GetImageOrientationInPatientCoordinate(axis[0], axis[1], axis[2]);
//    imageData->GetImageOrientationX(&axis[0]);
//    imageData->GetImageOrientationY(&axis[1]);
//    imageData->GetImagePositionFirst(&originPoint);
//    imageData->GetImagePositionLast(&lastPoint);
//    mVolumeGO->GetSpacing(spacing);
//    mVolumeGO->GetDimension(dim);
//    size[0] = spacing[0] * dim[0];
//    size[1] = spacing[1] * dim[1];
//    size[2] = spacing[2] * dim[2];
//    axis[2] =  lastPoint - originPoint;
//    axis[2].Normalize();
//
//    Point3D vertex[8];
//    Point3D vertex4[4];
//    //float vertexf[24];
//    vertex[0] = Point3D(originPoint.x, originPoint.y, originPoint.z);
//    vertex[1] = vertex[0] + axis[0] * size[0];
//    vertex[2] = vertex[1] + axis[1] * size[1];
//    vertex[3] = vertex[0] + axis[1] * size[1];
//    vertex[4] = vertex[0] + axis[2] * size[2];
//    vertex[5] = vertex[1] + axis[2] * size[2];
//    vertex[6] = vertex[2] + axis[2] * size[2];
//    vertex[7] = vertex[3] + axis[2] * size[2];
//    Point3D point;
//    if(currentSection == SAGITTAL)
//    {
//        vertex4[0] = vertex[0]; vertex4[1] = vertex[3]; vertex4[2] = vertex[7];
//        vertex4[3] = vertex[4];
//        for (int i = 0; i < 4; ++i)
//        {
//            point = world2PatientMatrix.Transform(vertices[i]);
//            vertices[i] = point;
//            vertices[i].y = vertex4[i].y;
//            vertices[i].z = vertex4[i].z;
//        }
//    }
//    else if(currentSection == AXIAL)
//    {
//        vertex4[0] = vertex[0]; vertex4[1] = vertex[1]; vertex4[2] = vertex[2];
//        vertex4[3] = vertex[3];
//        for (int i = 0; i < 4; ++i)
//        {
//            point = world2PatientMatrix.Transform(vertices[i]);
//            vertices[i] = point;
//            vertices[i].x = vertex4[i].x;
//            vertices[i].y = vertex4[i].y;
//        }
//    }
//    else
//    {
//        vertex4[0] = vertex[0]; vertex4[1] = vertex[1]; vertex4[2] = vertex[5];
//        vertex4[3] = vertex[4];
//        for (int i = 0; i < 4; ++i)
//        {
//            point = world2PatientMatrix.Transform(vertices[i]);
//            vertices[i] = point;
//            vertices[i].x = vertex4[i].x;
//            vertices[i].z = vertex4[i].z;
//        }
//    }
//
//    float floatList[12];
//    int pIndex[4];
//    for(int i = 0; i < 4; ++i) {
//        floatList[3 * i + 0] = vertices[i].x;
//        floatList[3 * i + 1] = vertices[i].y;
//        floatList[3 * i + 2] = vertices[i].z;
//        pIndex[i] = i;
//    }
//
//    //float texVert[] = {0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f};
//
//    glBindVertexArray(mVertexArrayID);
//
//    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, floatList, GL_STATIC_DRAW);
//    glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)NULL);
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mIndexBufferID);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*4,pIndex,GL_STATIC_DRAW);
//
//    g_shaderDualPeel->setTextureVert("vInTexCoord", mMPRTextureVertID);
//
//    glBindVertexArray(0);
//
//    glBindVertexArray(mVertexArrayID);
//    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
//    glBindVertexArray(0);
//
//    if(uid == "pass") {
//        useLight = 1;
//        useText = -1;
//        g_shaderDualPeel->setUniform("useLight", &useLight, 1);
//        g_shaderDualPeel->setUniform("useText", &useText, 1);
//    }
//}
//
//void TpsShadedVoiStrategy::DrawVolume(std::string uid) {
//    if(uid == "pass") {
//        float tempColor[4];
//        tempColor[0] = 1.0f;
//        tempColor[1] = 0.0f;
//        tempColor[2] = 0.0f;
//        tempColor[3] = 1.0;
//        g_shaderDualPeel->setUniform("vColor", tempColor, 4);
//    }
//    using namespace Mcsf::MedViewer3D;
//    std::shared_ptr<MedViewer3DImageData> imageData = mVolumeGO->GetImageData();
//    Vector3D axis[3];
//    double spacing[3], size[3];
//    Point3D originPoint, lastPoint;
//    size_t dim[3];
//    imageData->GetImageOrientationInPatientCoordinate(axis[0], axis[1], axis[2]);
//    imageData->GetImageOrientationX(&axis[0]);
//    imageData->GetImageOrientationY(&axis[1]);
//    imageData->GetImagePositionFirst(&originPoint);
//    imageData->GetImagePositionLast(&lastPoint);
//    mVolumeGO->GetSpacing(spacing);
//    mVolumeGO->GetDimension(dim);
//    size[0] = spacing[0] * dim[0];
//    size[1] = spacing[1] * dim[1];
//    size[2] = spacing[2] * dim[2];
//    axis[2] =  lastPoint - originPoint;
//    axis[2].Normalize();
//
//    Point3D vertex[8];
//    float vertexf[24];
//    vertex[0] = Point3D(originPoint.x, originPoint.y, originPoint.z);
//    vertex[1] = vertex[0] + axis[0] * size[0];
//    vertex[2] = vertex[1] + axis[1] * size[1];
//    vertex[3] = vertex[0] + axis[1] * size[1];
//    vertex[4] = vertex[0] + axis[2] * size[2];
//    vertex[5] = vertex[1] + axis[2] * size[2];
//    vertex[6] = vertex[2] + axis[2] * size[2];
//    vertex[7] = vertex[3] + axis[2] * size[2];
//    for(int i = 0; i < 8; ++i) {
//        vertexf[3 * i + 0] = vertex[i].x;
//        vertexf[3 * i + 1] = vertex[i].y;
//        vertexf[3 * i + 2] = vertex[i].z;
//    }
//
//    unsigned int vertexIndex[24] = {0,1,1,2,2,3,3,0,0,4,1,5,2,6,3,7,4,5,5,6,6,7,7,4};
//
//    glBindVertexArray(mVertexArrayID);
//
//    glBindBuffer(GL_ARRAY_BUFFER,mVertexBufferID);
//    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*24,vertexf,GL_STATIC_DRAW);
//    glVertexPointer(3,GL_FLOAT,0,(GLfloat*)NULL);
//    glEnableClientState(GL_VERTEX_ARRAY);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mIndexBufferID);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*24,vertexIndex,GL_STATIC_DRAW);
//
//    glBindVertexArray(0);
//
//    glBindVertexArray(mVertexArrayID);
//    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, NULL);
//    glBindVertexArray(0);
//}
//
//TPS_END_NAMESPACE
