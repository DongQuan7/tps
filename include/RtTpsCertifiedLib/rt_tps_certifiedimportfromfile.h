//////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    rt_ms_certifiedimprotfile.h
///  \brief   CertifiedImportFileInfo
///
///  \version 1.0
///  \date    1. 15, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef RT_MS_CERTIFIEDIMPORTFILE_H_
#define RT_MS_CERTIFIEDIMPORTFILE_H_

#include "tps_defs.h"

#include "RtTpsProtoLib/rt_ms_importfromfile.pb.h"

TPS_BEGIN_NAMESPACE



struct ErrorResult
{
    std::string errorName;
    float averageerror;
    float maxerror;
    float errorlimit;
    int failurepoits; 
};

struct CoodrinationPoint{
    float x;
    float y;
    float z;
    float dose;
};

struct CertifiedImportFileInfo{
    std::string uID;
    std::string parentUID;
    bool isDisplay;
    std::string fieldSize;
    float fieldSizeX;
    float fieldSizeY;
    int type;
    float  offsetX;
    float  offsetY;
    float  depth;
    float degree;
    int modulation;
    std::string wedgeACCID;
    int status;
    bool isAccDisplay;

    std::vector< std::tr1::shared_ptr<CoodrinationPoint> > pointList;
    std::vector< std::tr1::shared_ptr<CoodrinationPoint> > calcpointList;

    std::vector< std::tr1::shared_ptr<CoodrinationPoint> > normPointList;
    std::vector< std::tr1::shared_ptr<CoodrinationPoint> > normCalcPointList;

    std::vector< std::tr1::shared_ptr<ErrorResult> > errorResultList;

    void Convert(const ms::proto::RT_MS_ImportFromFileInfo &importFileInfo) {

        if (importFileInfo.has_uid()) {
            uID = importFileInfo.uid();
        }
        if (importFileInfo.has_parentuid()) {
            parentUID = importFileInfo.parentuid();
        }
        if (importFileInfo.has_isdisplay()){
            isDisplay = importFileInfo.isdisplay();
        }
        if (importFileInfo.has_fieldsize()){
            fieldSize = importFileInfo.fieldsize();
        }
        if (importFileInfo.has_fieldsizex())
        {
            fieldSizeX = importFileInfo.fieldsizex();
        }
        if (importFileInfo.has_fieldsizey())
        {
            fieldSizeY = importFileInfo.fieldsizey();
        }
        if (importFileInfo.has_type()){
            type = importFileInfo.type();
        }
        if (importFileInfo.has_offsetx()){
            offsetX = importFileInfo.offsetx();
        }
        if (importFileInfo.has_offsety()){
            offsetY = importFileInfo.offsety();
        }
        if (importFileInfo.has_depth()){
            depth = importFileInfo.depth();
        }
        if (importFileInfo.has_degree()){
            degree = importFileInfo.degree();
        }
        if (importFileInfo.has_modulation()){
            modulation = importFileInfo.modulation();
        }
        if (importFileInfo.has_status()){
            status = importFileInfo.status();
        }
        if (importFileInfo.has_iswedgeaccid()){
            isAccDisplay = importFileInfo.iswedgeaccid();
        }
        if (importFileInfo.has_wedgeaccid()){
            wedgeACCID = importFileInfo.wedgeaccid();
        }

        //if (importFileInfo.has_minaxisx()){
        //    minAxisX = importFileInfo.minaxisx();
        //}
        //if (importFileInfo.has_maxaxisx()){
        //    maxAxisX = importFileInfo.maxaxisx();
        //}
        //if (importFileInfo.has_minaxisy()){
        //    minAxisY = importFileInfo.minaxisy();
        //}
        //if (importFileInfo.has_maxaxisy()){
        //    maxAxisY = importFileInfo.maxaxisy();
        //}
        //if(importFileInfo.has_mincalaxisx()){
        //    minCalAxisX = importFileInfo.mincalaxisx();
        //}
        //if(importFileInfo.has_maxcalaxisx()){
        //    maxCalAxisX = importFileInfo.maxcalaxisx();
        //}
        //if(importFileInfo.has_mincalaxisy()){
        //    minCalAxisY = importFileInfo.mincalaxisy();
        //}
        //if(importFileInfo.has_maxcalaxisy()){
        //    maxCalAxisY = importFileInfo.maxcalaxisy();
        //}

        ms::proto::RT_MS_CoodrinatePoint point,protocalpoint;
        for (int i = 0; i < importFileInfo.pointlist_size(); ++i){
             std::tr1::shared_ptr<CoodrinationPoint> ppoint(new CoodrinationPoint());
            point = importFileInfo.pointlist(i);
            ppoint->dose = point.dose();
            ppoint->x = point.x();
            ppoint->y = point.y();
            ppoint->z = point.z();
            pointList.push_back(ppoint);
        }

        for (int i = 0; i < importFileInfo.calpointlist_size(); ++i){
            std::tr1::shared_ptr<CoodrinationPoint> calpoint(new CoodrinationPoint());
            protocalpoint = importFileInfo.calpointlist(i);
            calpoint->dose = protocalpoint.dose();
            calpoint->x = protocalpoint.x();
            calpoint->y = protocalpoint.y();
            calpoint->z = protocalpoint.z();
            calcpointList.push_back(calpoint);
        }

        for (int i = 0; i < importFileInfo.normpointlist_size(); ++i){
            std::tr1::shared_ptr<CoodrinationPoint> normppoint(new CoodrinationPoint());
            protocalpoint = importFileInfo.normpointlist(i);
            normppoint->dose = protocalpoint.dose();
            normppoint->x = protocalpoint.x();
            normppoint->y = protocalpoint.y();
            normppoint->z = protocalpoint.z();
            normPointList.push_back(normppoint);
        }

        for (int i = 0; i < importFileInfo.normcalpointlist_size(); ++i){
            std::tr1::shared_ptr<CoodrinationPoint> normcalpoint(new CoodrinationPoint());
            protocalpoint = importFileInfo.normcalpointlist(i);
            normcalpoint->dose = protocalpoint.dose();
            normcalpoint->x = protocalpoint.x();
            normcalpoint->y = protocalpoint.y();
            normcalpoint->z = protocalpoint.z();
            normCalcPointList.push_back(normcalpoint);
        }

        ms::proto::RT_MS_ErrorResult errorResult;
        for (int i = 0; i < importFileInfo.errorresultlist_size(); ++i){
            std::tr1::shared_ptr<ErrorResult> pErrorResult(new ErrorResult());
            errorResult = importFileInfo.errorresultlist(i);
            pErrorResult->errorName = errorResult.errorname();
            pErrorResult->averageerror = errorResult.averageerror();
            pErrorResult->maxerror = errorResult.maxerror();
            pErrorResult->errorlimit = errorResult.errorlimit();
            pErrorResult->failurepoits = errorResult.failurepoits();
            errorResultList.push_back(pErrorResult);
        }

    }

    void ConvertBack( ms::proto::RT_MS_ImportFromFileInfo* importFileInfo) const{
        importFileInfo->Clear();
        importFileInfo->set_uid(uID);
        importFileInfo->set_parentuid(parentUID);
        importFileInfo->set_isdisplay(isDisplay);
        importFileInfo->set_fieldsize(fieldSize);
        importFileInfo->set_type(type);
        importFileInfo->set_offsetx(offsetX);
        importFileInfo->set_offsety(offsetY);
        importFileInfo->set_fieldsizex(fieldSizeX);
        importFileInfo->set_fieldsizey(fieldSizeY);
        importFileInfo->set_depth(depth);
        importFileInfo->set_modulation(modulation);
        importFileInfo->set_status(status);
        importFileInfo->set_iswedgeaccid(isAccDisplay);
        importFileInfo->set_degree(degree);
        importFileInfo->set_wedgeaccid(wedgeACCID);

        //importFileInfo->set_minaxisx(minAxisX);
        //importFileInfo->set_maxaxisx(maxAxisX);
        //importFileInfo->set_minaxisy(minAxisY);
        //importFileInfo->set_maxaxisy(maxAxisY);

        //importFileInfo->set_mincalaxisx(minCalAxisX);
        //importFileInfo->set_maxcalaxisx(maxCalAxisX);
        //importFileInfo->set_mincalaxisy(minCalAxisY);
        //importFileInfo->set_maxcalaxisy(maxCalAxisY);

        ms::proto::RT_MS_CoodrinatePoint* ppoint = nullptr;
        for (auto point = pointList.begin(); point != pointList.end(); ++point ){
            ppoint = importFileInfo->add_pointlist();
            ppoint->set_dose((*point)->dose);
            ppoint->set_x((*point)->x);
            ppoint->set_y((*point)->y);
            ppoint->set_z((*point)->z);
        }

        for (auto point = calcpointList.begin(); point != calcpointList.end(); ++point ){
            ppoint = importFileInfo->add_calpointlist();
            ppoint->set_dose((*point)->dose);
            ppoint->set_x((*point)->x);
            ppoint->set_y((*point)->y);
            ppoint->set_z((*point)->z);
        }

        for (auto point = normPointList.begin(); point != normPointList.end(); ++point ){
            ppoint = importFileInfo->add_normpointlist();
            ppoint->set_dose((*point)->dose);
            ppoint->set_x((*point)->x);
            ppoint->set_y((*point)->y);
            ppoint->set_z((*point)->z);
        }

        for (auto point = normCalcPointList.begin(); point != normCalcPointList.end(); ++point ){
            ppoint = importFileInfo->add_normcalpointlist();
            ppoint->set_dose((*point)->dose);
            ppoint->set_x((*point)->x);
            ppoint->set_y((*point)->y);
            ppoint->set_z((*point)->z);
        }

        ms::proto::RT_MS_ErrorResult* pErrorResult=nullptr;
        for (auto error = errorResultList.begin(); error != errorResultList.end(); ++error ){
            pErrorResult = importFileInfo->add_errorresultlist();
            pErrorResult->set_errorname((*error)->errorName);
            pErrorResult->set_averageerror((*error)->averageerror);
            pErrorResult->set_maxerror((*error)->maxerror);
            pErrorResult->set_errorlimit((*error)->errorlimit);
            pErrorResult->set_failurepoits((*error)->failurepoits);
        }

    }

};

TPS_END_NAMESPACE


#endif
