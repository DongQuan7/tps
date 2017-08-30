//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_prca_mprprinter.h
///
/// \brief   MPRPrinter
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_MPRPRINTER_H_
#define TPS_BL_MPRPRINTER_H_

#include "tps_defs.h"

#include "tps_bl_printerbase.h"

#include <queue>

TPS_BEGIN_NAMESPACE

class FusionRenderStrategy;
class TpsBeamRenderStrategy;
class MprRoiRenderStrategy;
class TpsScaleRulerStrategy;
class TpsMPRDoseRenderStrategy;
class TpsColorSpectraRenderStrategy;
class TpsPoiRenderStrategy;
class MascotRenderStrategy;
class POITypeTextRenderStrategy;
class CornerTextInfoRenderStrategy;
class TpsColorSpectraTextRenderStrategy;
class AxialPageTurnOperation;
class TpsRefreshDoseOperation;

typedef std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> GraphicObjectPtr;

class MPRPrinter : public PrinterBase {
public:
	MPRPrinter(int width, int height, ITpsDataRepository* mDataRepository);
	virtual ~MPRPrinter();

	virtual void SetPlanUID(const std::string planUID);
	virtual void SetSeriesUID(const std::string seriesUID);
	virtual void SetSeriesSlices(std::queue<int>& seriesSlices);
    virtual void ClearModelWarehouse();

private:
	virtual bool PreparePrinting();
	virtual bool OnBeginPrinting();
	virtual PRINT_STATE OnPrinting();
	virtual bool OnEndPrinting();

private:
	std::shared_ptr<FusionRenderStrategy>                       mFusionStrategy;
	std::shared_ptr<TpsBeamRenderStrategy>                      mBeamStrategy;
	std::shared_ptr<MprRoiRenderStrategy>                       mVOIStrategy;
	std::shared_ptr<TpsScaleRulerStrategy>                      mScaleRulerStrategy;
	std::shared_ptr<TpsMPRDoseRenderStrategy>                   mDoseStrategy;
	std::shared_ptr<TpsColorSpectraRenderStrategy>              mColorSpectraStrategy;
	std::shared_ptr<TpsPoiRenderStrategy>                       m_spPoiStrategy;
	std::shared_ptr<MascotRenderStrategy>                       m_spMascotRenderStrategy;

	POITypeTextRenderStrategy*                                  mPoiTypeTextStrategy;
	CornerTextInfoRenderStrategy*                               mCornerTextInfoStrategy;

	std::shared_ptr<AxialPageTurnOperation>                     mPageTurnOperation;
	std::shared_ptr<TpsRefreshDoseOperation>                    mRefreshDoseOperation;

	std::string mPlanUID;
	std::string mSeriesUID;
	std::queue<int> mSeriesSlices;
};

TPS_END_NAMESPACE

#endif //TPS_PRCA_MPRPRINTER_H_
