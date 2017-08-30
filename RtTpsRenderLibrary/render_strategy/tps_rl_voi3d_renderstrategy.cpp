

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_voi3d_renderstrategy.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"
//#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
/*#include "RtTpsRenderLibrary/tps_rl_bevwindowdisplaygraphicobject.h"*/
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_surface_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsFramework/tps_fw_contour_set.h"
#include "RtTpsFramework/tps_fw_contour.h"

TPS_BEGIN_NAMESPACE
VOI3DRenderStrategy::VOI3DRenderStrategy(WINDOW_TYPE windowType):
mVoiCollectionGO(nullptr), mStateGO(nullptr),
mCameraGO(nullptr), mVolumeGO(nullptr),mWindowType(windowType){

}

VOI3DRenderStrategy::~VOI3DRenderStrategy() {

}

void VOI3DRenderStrategy::Initialize() {

}

//改为从VOIGO中获取Ana数据
void VOI3DRenderStrategy::Render_i() {


    if(nullptr == mVoiCollectionGO || nullptr == mStateGO || 
       nullptr == mCameraGO || nullptr == mVolumeGO) {
        return;
    }
    
    //closed as the latest requirement by qiangqiang.zhou@20141212
    //bool bIsSurfaceWindow = (SURFACE == mWindowType);

    //if(FLUNCE_MAP == mStateGO->GetType() && !bIsSurfaceWindow) {
    //    TPS_LOG_TRACE_INFO<<"display fluence map!";
    //    return;
    //}

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);
    glViewport(0, 0, mWidth, mHeight);

    Mcsf::MedViewer3D::Matrix4x4 matP2W = mVolumeGO->GetWorld2Patient().Inverse();
    Mcsf::MedViewer3D::Matrix4x4 matProjection;
    mCameraGO->AdaptFovViaAspect((double)mWidth / mHeight);
    matProjection = mCameraGO->GetViewProjectionMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(matP2W._m);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(matProjection._m);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);

    Mcsf::Point3f pt;
    float color[4] = {0};
    auto voiList = mVoiCollectionGO->GetVOIList();
    for (auto it = voiList.begin(); it != voiList.end(); ++it)
    {
        if (it->second == nullptr || !it->second->GetVisibility() || 
            it->second->IsShaded())
        {
            continue;
        }
        
        it->second->GetColor(color);
        glColor4f(color[0], color[1],color[2], 1.0f/*color[3]*/);//alpha should be 1.0f

        auto cs = it->second->GetContourSet();
        for (auto itCS = cs->page_contours.begin(); itCS != cs->page_contours.end(); ++itCS)
        {
            for (int i = 0; i < itCS->second->contour_list.size(); ++i)
            {
                Contour& ctr = itCS->second->contour_list[i];
                if (ctr.pt.size() < 3) continue;

                glBegin(GL_LINE_LOOP);
                for (int j = 0; j < ctr.pt.size(); ++j)
                {
                    glVertex3f(ctr.pt[j].x, ctr.pt[j].y, ctr.pt[j].z);
                }
                glEnd();
            }
        }
    }

    //glDisable(GL_SMOOTH);
    //glDisable(GL_LINE_STIPPLE);
    glFlush();
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

void VOI3DRenderStrategy::Resize(int w, int h) {
    mHeight = h;
    mWidth = w;
    mDirty =true;
}

void VOI3DRenderStrategy::Finalize() {

}

int VOI3DRenderStrategy::CacheChanges() {
    if(nullptr == mVoiCollectionGO || nullptr == mStateGO
        || nullptr == mCameraGO || nullptr == mVolumeGO) {
            return -1;
    }

    //if (BEV == mStateGO->GetWindowType()){
    //    if(nullptr == mBEVDisplayGO) return -1;
    //    if(mBEVDisplayGO->IsDirty()) mDirty = true;
    //} 

//     if(nullptr != mBEVDisplayGO) {
//         if(mBEVDisplayGO->IsDirty() ) {
//             mDirty = true;
//         }
//     }
    if(mVoiCollectionGO->IsDirty() || mStateGO->IsDirty()
        || mCameraGO->IsDirty() || mVolumeGO->IsDirty()) {
         mDirty = true;
    }
    return 0;
}

void VOI3DRenderStrategy::SetCameraGO(std::shared_ptr<CameraGraphicObject> go) {
    if (mCameraGO != go){
        mCameraGO = go;
        mDirty = true;
    }
}

void VOI3DRenderStrategy::SetStateControlGO(
    std::shared_ptr<StateControlGraphicObject> go) {
    if (mStateGO != go){
        mStateGO = go;
        mDirty = true;
    }
}

void VOI3DRenderStrategy::SetVOICollectionGO(std::shared_ptr<TpsVOIGOCollection> voiCollection){
    if (mVoiCollectionGO != voiCollection){
        mVoiCollectionGO = voiCollection;
        mDirty = true;
    }
}

// void VOI3DRenderStrategy::SetBevDisplayGo(std::shared_ptr<BEVDisplayGraphicObject>
//     go) {
//     if (mBEVDisplayGO != go){
//         mBEVDisplayGO = go;
//         mDirty = true;
//     }
// }

void VOI3DRenderStrategy::SetVolumeGO(
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> go) {
    if (mVolumeGO != go){
        mVolumeGO = go;
        mDirty = true;
    }
}
TPS_END_NAMESPACE
