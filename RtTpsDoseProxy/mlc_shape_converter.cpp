//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file mlc_shape_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/18
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mlc_shape_converter.h"

#include "alg/RtAlgDataLayer/rt_alg_dl_object_leafposition.h"

TPS_BEGIN_NAMESPACE

bool MlcShapeConverter::ConvertToALG(const RtMlcshape& tpsMlcShape, std::shared_ptr<RTFWK::RTFWKDLDOMLCShape>* algMlcShape)
{
    algMlcShape->reset(new RTFWK::RTFWKDLDOMLCShape(false));

    (*algMlcShape)->SetUID(tpsMlcShape.get_uid());
    (*algMlcShape)->SetIsStartMLCShape(tpsMlcShape.get_isstartmlcshape());
    (*algMlcShape)->SetJAWXPositions(false, tpsMlcShape.get_xlowerjawpos(), tpsMlcShape.get_xupperjawpos());
    (*algMlcShape)->SetJAWYPositions(false, tpsMlcShape.get_ylowerjawpos(), tpsMlcShape.get_yupperjawpos());
    (*algMlcShape)->SetMLCLeafDirection((RTFWK::MLC_LEAF_DIRECTION_TYPE)tpsMlcShape.get_mlcdirectiontype());
    std::vector<RTFWK::RTFWKDLDOLeafPosition> algLeafPositions;
    std::vector<db_Point2d> leafPositions = tpsMlcShape.get_leafpositions();
    std::vector<db_Point2d>::iterator itLeafPosistion = leafPositions.begin();
    for (; itLeafPosistion != leafPositions.end(); ++itLeafPosistion)
    {
        db_Point2d leafPos = *itLeafPosistion;
        RTFWK::RTFWKDLDOLeafPosition algLeafPos(leafPos.x, leafPos.y);
        algLeafPositions.push_back(algLeafPos);
    }
    (*algMlcShape)->SetLeafPositions(algLeafPositions);

    return true;
}

bool MlcShapeConverter::ConvertToTPS(RTFWK::RTFWKDLDOMLCShape& algMlcShape, RtMlcshape* tpsMlcShape)
{
    tpsMlcShape->set_uid(algMlcShape.GetUID());
    double xLowerPos = 0.0, xUppderPos = 0.0, yLowerPos = 0.0, yUpperPos = 0.0;
    algMlcShape.GetXLowerJawPosition(xLowerPos);
    tpsMlcShape->set_xlowerjawpos(xLowerPos);
    algMlcShape.GetXUpperJawPosition(xUppderPos);
    tpsMlcShape->set_xupperjawpos(xUppderPos);
    algMlcShape.GetYLowerJawPosition(yLowerPos);
    tpsMlcShape->set_ylowerjawpos(yLowerPos);
    algMlcShape.GetYUpperJawPosition(yUpperPos);
    tpsMlcShape->set_yupperjawpos(yUpperPos);
    tpsMlcShape->set_mlcdirectiontype(algMlcShape.GetMLCLeafDirection());
    // wzd 2017-02-10 计算优化后子野不会对称
    tpsMlcShape->set_jawxissymmetry(false);
    tpsMlcShape->set_jawyissymmetry(false);

    std::vector<db_Point2d> leafPositions;
    std::vector<RTFWK::RTFWKDLDOLeafPosition> algLeafPositions = algMlcShape.GetLeafPositions();
    std::vector<RTFWK::RTFWKDLDOLeafPosition>::iterator itLeafPos = algLeafPositions.begin();
    for (; itLeafPos != algLeafPositions.end(); ++itLeafPos)
    {
        RTFWK::RTFWKDLDOLeafPosition leafPos = *itLeafPos;
        db_Point2d tpsLeafPos(leafPos.GetLeafLowerPosition(), leafPos.GetLeafUpperPosition());
        leafPositions.push_back(tpsLeafPos);
    }
    tpsMlcShape->set_leafpositions(leafPositions);
    return true;
}

TPS_END_NAMESPACE