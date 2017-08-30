
#include "StdAfx.h" 

#include "RtTpsRenderLibrary/tps_rl_navigatecuberenderstrategy.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicUtils/camera.h"


#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"


TPS_BEGIN_NAMESPACE

MPRNavigateCubeRenderstrategy::MPRNavigateCubeRenderstrategy(){


}

MPRNavigateCubeRenderstrategy::~MPRNavigateCubeRenderstrategy() {

}

void MPRNavigateCubeRenderstrategy::Resize(int iWidth, int iHeight) {
    mHeight = iHeight;
    mWidth = iWidth;
}

void MPRNavigateCubeRenderstrategy::Initialize() {
    glGenVertexArrays(1,&mVertexArrayID);
    glBindVertexArray(mVertexArrayID);

    glGenBuffers(1,&mBufferVertexID);
    unsigned int index[] ={
        0,1,
        1,2,
        2,3,
        3,0,
        4,5,
        5,6,
        6,7,
        7,4,
        0,4,
        1,7,
        2,6,
        3,5

    };

    unsigned int indexBuferID;
    glGenBuffers(1,&indexBuferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBuferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*6*4,index,GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void MPRNavigateCubeRenderstrategy::Render_i() {

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);
    float areaLen = GetCubeArea_i();
    const float zInModel = 0.0;

    const double xOffset = 0.15f;
    const double yOffset = 0.2f;


    glViewport(0,0, areaLen, areaLen);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0x01);
    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    DrawCube_i(zInModel,-1.0f,xOffset,yOffset);

    DrawFace_i(zInModel,xOffset,yOffset);

    DrawCube_i(1.0f,zInModel,xOffset,yOffset);

    glDisable(GL_STENCIL_TEST);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);


}


float MPRNavigateCubeRenderstrategy::GetCubeArea_i() {
    return 200.0f;
}
void MPRNavigateCubeRenderstrategy::Finalize() {

}

int MPRNavigateCubeRenderstrategy::CacheChanges() {
    if(nullptr == mMprGraphicObject || nullptr == mVolumeGraphicObject) {
        return -1;
    }
    if(mMprGraphicObject->IsDirty() || mVolumeGraphicObject->IsDirty()) {
        mDirty = true;
    }
    return 0;
}

void MPRNavigateCubeRenderstrategy::SetVolumeGraphicObj(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume) {
    if (mVolumeGraphicObject != volume){
        mVolumeGraphicObject = volume;
        mDirty = true;
    }
}


void MPRNavigateCubeRenderstrategy::SetMprGraphicObject(std::shared_ptr<MPRGraphicObject> mpr) {
    if (mMprGraphicObject != mpr){
        mMprGraphicObject = mpr;
        mDirty = true;
    }
}

void MPRNavigateCubeRenderstrategy::DrawCube_i(float z1, float z2, float xOffset, float yOffset) {

    const float scale = 0.25;
    float verts[] ={
        -1*scale+xOffset*(1- z1)/2, -1*scale+yOffset*(1- z1)/2, z1*scale,   //0
           scale+xOffset*(1- z1)/2, -1*scale+yOffset*(1- z1)/2, z1*scale,   //1
           scale+xOffset*(1- z2)/2, -1*scale+yOffset*(1- z2)/2, z2*scale,   //2
        -1*scale+xOffset*(1- z2)/2, -1*scale+yOffset*(1- z2)/2, z2*scale,   //3
        -1*scale+xOffset*(1- z1)/2,    scale+yOffset*(1- z1)/2, z1*scale,   //4
        -1*scale+xOffset*(1- z2)/2,    scale+yOffset*(1- z2)/2, z2*scale,   //5
           scale+xOffset*(1- z2)/2,    scale+yOffset*(1- z2)/2, z2*scale,   //6
           scale+xOffset*(1- z1)/2,    scale+yOffset*(1- z1)/2, z1*scale    //7
    };
 

    glBindVertexArray(mVertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER,mBufferVertexID);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*3*8,verts,GL_STATIC_DRAW);
    glVertexPointer(3,GL_FLOAT,0,(GLfloat*)NULL);
    glEnableClientState(GL_VERTEX_ARRAY);

    glColor4f(1.0f,0.0f,0.0f,1.0f);

    glLineWidth(2.0f);
    glEnable(GL_SMOOTH);
    glDrawElements(GL_LINES,24,GL_UNSIGNED_INT,0);
    glLineWidth(1.0f); 
    glDisableClientState(GL_VERTEX_ARRAY);

    glBindVertexArray(0);


}

void MPRNavigateCubeRenderstrategy::DrawFace_i(float z, float xOffset, float yOffset) {
    const float scale = 0.25;
    glColor4f(0.0f,0.0f,1.0f,0.8f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glBegin(GL_QUADS);
    glVertex3f(-1*scale+xOffset*(1- z)/2, -1*scale+yOffset*(1- z)/2, z*scale);  
    glVertex3f(-1*scale+xOffset*(1- z)/2,  1*scale+yOffset*(1- z)/2, z*scale); 
    glVertex3f(   scale+xOffset*(1- z)/2,    scale+yOffset*(1- z)/2, z*scale);  
    glVertex3f(   scale+xOffset*(1- z)/2, -1*scale+yOffset*(1- z)/2, z*scale);
    glEnd();
    glDisable(GL_BLEND);

}


TPS_END_NAMESPACE

