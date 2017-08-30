//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_dp_dosecalculation.h
/// 
///  \version 1.0
/// 
///  \date    2013/12/14
//////////////////////////////////////////////////////////////////////////

#ifndef	TPS_DP_DOSECALCULATION_H_
#define TPS_DP_DOSECALCULATION_H_

#include "tps_dp_defs.h"

namespace RTFWK
{
	class RTFWKDLDOBeam;
	class RTFWKDLDOPOI;
	class RTFWKDLDODoseGrid;
	class RTFWKDLDOVOI;
};

TPS_BEGIN_NAMESPACE

class TPS_DP_DECLSPEC TpsDoseCalculation : public TpsObject{
public:
	TpsDoseCalculation();
	~TpsDoseCalculation();
	int Calculation(
		RTFWK::DOSEALGORITHM_TYPE algorithmType,
		const std::string& ctImageSeriesUID,
		const std::vector<RTFWK::RTFWKDLDOVOI>& voilist,
		std::vector<RTFWK::RTFWKDLDOBeam>& beams,
		std::vector<RTFWK::RTFWKDLDOPOI>* poilist,
		RTFWK::RTFWKDLDODoseGrid* doseGrid);
};
	
TPS_END_NAMESPACE

#endif