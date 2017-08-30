//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Liwu Zuo liwu.zuo@united-imaging.com
//
/// \file    tps_prca_printerbase.h
///
/// \brief   PrinterBase
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_PRINTERBASE_H_
#define TPS_BL_PRINTERBASE_H_

#include "tps_defs.h"

#include "McsfMedViewer3DCommon/mcsf_viewer3d_observer_interface.h"

#include "RtTpsFramework/tps_fw_defs.h"

#include "boost/function.hpp"

#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include <memory>
#include <map>


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
class WindowActiveResponser;
typedef std::shared_ptr<Mcsf::MedViewer3D::ClsView> ClsViewPtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> IGraphicObjectPtr;
typedef std::map<std::string, CPURenderStrategy*> CPURenderMap;
typedef std::shared_ptr<WindowActiveResponser> WindowResponserPtr;

class ModelWarehouse;
class GOConverterFactoryBase;
struct GOConverterParameter;

class PrinterBase : public Mcsf::MedViewer3D::IObserver {
public:
	PrinterBase(int width, int height, ITpsDataRepository* dataRepository);
	virtual ~PrinterBase();

	virtual bool Print();
	virtual bool AttachListener(const WindowResponserPtr& responser);

protected:
	bool CheckGraphicObject(const std::string& baseUID,
		GO_TYPE goType, 
		IGraphicObjectPtr& go,
        const GOConverterParameter& para,
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
	
	ModelWarehouse* mModelWarehouse;
	GOConverterFactoryBase* mConverterFactory;
    ITpsDataRepository* mDataRepository;
	bool mIsInitialized;
	int mWidth;
	int mHeight;
	unsigned char* mBuffer;
	CPURenderMap mCPURender;
	WindowResponserPtr mResponser;
	HANDLE mDemaphore;
	ClsViewPtr mClsView;

	Mcsf::MedViewer3D::ClsView* mTest;
};

TPS_END_NAMESPACE

#endif //TPS_PRCA_PRINTERBASE_H_
