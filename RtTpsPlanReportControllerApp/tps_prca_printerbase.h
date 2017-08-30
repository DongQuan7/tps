//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_prca_printerbase.h
///
/// \brief   PrinterBase
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_PRCA_PRINTERBASE_H_
#define TPS_PRCA_PRINTERBASE_H_

#include "tps_defs.h"

#include "McsfMedViewer3DCommon/mcsf_viewer3d_observer_interface.h"

#include "RtTpsFramework/tps_fw_defs.h"

#include "boost/function.hpp"

namespace Mcsf {
    namespace MedViewer3D {
        class ClsView;
        class IGraphicObject;
    }
}

TPS_BEGIN_NAMESPACE

typedef enum {
    CONTINUE = 0,
    FINISH,
} PRINT_STATE;

class CPURenderStrategy;

typedef std::shared_ptr<Mcsf::MedViewer3D::ClsView> ClsViewPtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> IGraphicObjectPtr;
typedef std::map<std::string, CPURenderStrategy*> CPURenderMap;

class PrinterBase : public Mcsf::MedViewer3D::IObserver {
public:
    PrinterBase(int width, int height);
    virtual ~PrinterBase();

    virtual bool Print();
    virtual bool AttachListener(boost::function<void (unsigned char*, int, int)> listener);

protected:
    bool CheckGraphicObject(const std::string& baseUID,
        GO_TYPE goType, 
        IGraphicObjectPtr& go,
        bool forceToUpdate = false);
    bool RegisterCPURenderStrategy(const std::string& strCPURenderName,
        CPURenderStrategy* cpuRender);

private:
    bool OnPreparePrinting();
    virtual bool PreparePrinting();
    virtual bool OnBeginPrinting();
    virtual PRINT_STATE OnPrinting();
    virtual bool OnEndPrinting();

    virtual void Update(int iID = -1);

    bool SaveToTrueColorBitmap(const std::string& path, 
        unsigned char* buffer,
        const int& height,
        const int& width);

    void DoCPURendering(unsigned char*& buffer, int bufWidth, 
        int bufHeight);

protected:
    ClsViewPtr mClsView;
    bool mIsInitialized;
    int mWidth;
    int mHeight;
    unsigned char* mBuffer;
    CPURenderMap mCPURender;
    boost::function<void (unsigned char*, int, int)> mListener;
};

TPS_END_NAMESPACE

#endif //TPS_PRCA_PRINTERBASE_H_
