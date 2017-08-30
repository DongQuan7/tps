////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Chen Xi  xi.chen@united-imaging.com
/// 
///  \file tps_rl_updatesegmentoperation.cpp
/// 
///  \brief class TpsUpdateSegmentOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/10/08
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_updatesegmentoperation.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"


TPS_BEGIN_NAMESPACE

TpsUpdateSegmentOperation::TpsUpdateSegmentOperation(const std::string& normGroupUID,
	const std::string& beamUID)
	: mNormGroupUID(normGroupUID), mBeamUID(beamUID) {

}

TpsUpdateSegmentOperation::~TpsUpdateSegmentOperation() {

}

bool TpsUpdateSegmentOperation::ModifyGraphicObject() const{

	RtNormgroup* normgroup = nullptr;
	if (!mDataRepository->GetNormgroupManager()->GetNormGroup(mNormGroupUID, &normgroup)
		|| normgroup == nullptr) {
		TPS_LOG_DEV_ERROR << "The normgroup does not exist which uid: " << mNormGroupUID;
		return false;
	}

	RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mBeamUID);
	if (beam == nullptr) {
		TPS_LOG_DEV_ERROR << "The beam does not exist which uid: " << mBeamUID;
		return false;
	}

	std::string planUID = normgroup->get_planuid();
	if (planUID.empty()) {
		TPS_LOG_DEV_ERROR << "The plan uid is empty.";
		return false;
	}

	auto go = mModelWarehouse->GetModelObject(planUID + "|" +
		GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
	auto goNgCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
	if (goNgCollection == nullptr){
		TPS_LOG_DEV_ERROR << "Failed get normalgroup collection of plan: " << planUID;
		return false;
	}

	auto goBeamCollection = goNgCollection->GetNormalGroup(mNormGroupUID);
	if (goBeamCollection == nullptr){
		TPS_LOG_DEV_ERROR << "Failed get beam collection of normalgroup: " << mNormGroupUID;
		return false;
	}

	auto goBeam = goBeamCollection->SelectBeam(mBeamUID);
	if (goBeam == nullptr){
		TPS_LOG_DEV_ERROR << "Failed get beam go of beam: " << mBeamUID;
		return false;
	}

	auto goSegmentList = goBeam->GetSegmentGoList();
	auto segmentList = beam->get_beamsegments();
	if (segmentList.size() != goSegmentList.size()) {
		TPS_LOG_DEV_ERROR << "Not Support different count segments";
		return false;
	}

	double jawPos;
	float aJawPos[4];

	for (int index = 0; index < segmentList.size(); index++) {
		auto mlcShape = segmentList[index]->get_startmlcshape();
		auto goSegment = goSegmentList[index];

        jawPos = mlcShape->get_xlowerjawpos();
		aJawPos[0] = static_cast<float>(jawPos);
        jawPos = mlcShape->get_xupperjawpos();
		aJawPos[1] = static_cast<float>(jawPos);
		jawPos = mlcShape->get_ylowerjawpos();
		aJawPos[2] = static_cast<float>(jawPos);
		jawPos = mlcShape->get_yupperjawpos();
		aJawPos[3] = static_cast<float>(jawPos);
		goSegment->SetJawPosition(aJawPos);

		auto leafPositionList = mlcShape->get_leafpositions();

		for (int leafIndex = 0; leafIndex < leafPositionList.size(); leafIndex++) {
			goSegment->SetLeafPosition(leafIndex,
				leafPositionList[leafIndex].x,
				leafPositionList[leafIndex].y,
				goSegment->GetMLC()->m_pLeaf[leafIndex].min_w,
				goSegment->GetMLC()->m_pLeaf[leafIndex].max_w);
		}
	}

    if (!goBeam->UpdateCombinationFieldEdges())
    {
        TPS_LOG_DEV_ERROR<<"Failed to update combined fields outline.";
        return false;
    }

	goBeam->SetDirty(true);
	goBeamCollection->SetDirty(true);

	return true;
}

TPS_END_NAMESPACE