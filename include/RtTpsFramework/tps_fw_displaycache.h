////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rc_displaycashe.h
/// 
///  \brief class TpsDisplayCashe declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/03
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_DISPLAYCACHE_H_
#define TPS_FW_DISPLAYCACHE_H_

#include "McsfMedViewer3DCommon/mcsf_viewer3d_observer_interface.h"
#include "McsfMedViewer3DCommon/mcsf_viewer3d_subject.h"

#include "RtTpsFramework/tps_fw_defs.h"

#include <map>

namespace Mcsf {
    namespace MedViewer3D{
        class ClsView;
    }
}
TPS_BEGIN_NAMESPACE  // begin namespace tps

class CPURenderStrategy;
class ModelWarehouse;

/// \class TpsDisplayCashe
/// 
/// \brief this class observes a render unit, ClsView in our case, and copy 
///        the render result(image buffer or no-image information) when Update() invoked;
///        it will take charge of the buffer of render result(malloc & free).
///        and it's not thread-safe either, so use it in one thread.
class TPS_FW_DECLSPEC TpsDisplayCache : 
    public Mcsf::MedViewer3D::IObserver, 
    public Mcsf::MedViewer3D::Subject{
    typedef std::map<std::string, CPURenderStrategy*> CPURenderMap;

public:
    //the constructor and destructor
    TpsDisplayCache();
    virtual ~TpsDisplayCache();

    //////////////////////////////////////////////////////////////////////////
    // \brief Set a render unit to be observed
    // \param[in] view  pointer to a ClsView object
    // \throw std::invalid_argument if view is empty
    void SetRenderUnit(std::shared_ptr<Mcsf::MedViewer3D::ClsView> view);

    //////////////////////////////////////////////////////////////////////////
    // \brief set cache id
    // \note  the cache id is same to the window type in current implementation
    //        so the window type must be unique for every window
    void SetCacheID(WINDOW_TYPE window_type);

    //////////////////////////////////////////////////////////////////////////
    // \brief get the cache id which can identify one render unit.
    // \return DISPLAYCASHE_UID  an integer different from every object.
    DISPLAY_CACHE_UID GetCacheID() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief  Get the image buffer pointer and size
    // \param[out] width   the width of the result image
    // \param[out] height  the height of the result image
    // \param[out] buffer  the memory pointer of the result image
    // \return bool  true when succeeded, false otherwise.
    // \note  the buffer of result image is 4 channels(RGBA), 
    //        so the real size of the buffer is 4 * width * height.
    //        in addition, it is dangerous to free the buffer outside.
    bool GetBuffer(int& width, int& height, unsigned char*& buffer) const;

    // observer interface
    // \brief  clear old buffer & copy new buffer from render unit
    virtual void Update(int iID = -1);

    //////////////////////////////////////////////////////////////////////////
    // \brief check resize flag
    // \return bool  true if cache resized, false otherwise
    bool IsResized();

    //////////////////////////////////////////////////////////////////////////
    // \brief register CPU rendering strategy onto the cache
    // \param[in]   strCPURenderName    name string for this CPU strategy
    // \param[in]   cpuRender           pointer to cpu render
    // \return 
    bool RegisterCPURenderStrategy(const std::string& strCPURenderName, 
        CPURenderStrategy* cpuRender);

    void DetachAllObserver();

    //need lock model when do CPU rendering, if we move text rendering to client,
    //remove this interface
    void SetModelWarehouse(ModelWarehouse* model);
private:
    void DoCPURendering_i(unsigned char*& buffer, int bufWidth, int bufHeight);

//     bool SaveToTrueColorBitmap(const std::string& path, unsigned char* buffer,
//         const int& height,
//         const int& width);
private:
    std::weak_ptr<Mcsf::MedViewer3D::ClsView> mView;
    DISPLAY_CACHE_UID            mID;
    unsigned char*              mBuffer;
    int                         mWidth;
    int                         mHeight;
    bool                        mIsResized;
    //static DISPLAY_CACHE_UID     cacheID;
    CPURenderMap                mmapCPURender;
    ModelWarehouse*             mModelWarehouse;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDisplayCache);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
