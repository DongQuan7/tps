////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_rc_autocontouring_api.h
/// 
///  \brief class Auto Contouring declaration 
/// 
///  \version 1.0
/// 
///  \date    Dec.10,2013
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include <math.h>
#include <omp.h>
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

//ZHENGHE
#include "McsfAlgorithms/McsfAppLib/McsfAlgoAutoContour.h"
//#include "McsfAlgorithms/McsfAppLib/McsfAlgoBedBoardRemove.h"
#include "McsfGeometry/mcsf_geometry_plane.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfMedViewer3DArithmetic/point2d.h"
#include "McsfAlgorithms/McsfAppLib/McsfAlgoShapeInterpolation.h"



//TPS
#include "tps_logger.h"
#include "tps_defs.h"
#include "RtTpsFramework/tps_fw_mask.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"
#include "RtTpsAlgorithmProxy/tps_pl_utility.h"
#include "RtTpsAlgorithmProxy/tps_pl_ct2density_helper.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_contour_set.h"
#include "RtTpsFramework/tps_fw_contour.h"
#include "RtTpsFramework/tps_fw_arithmetic.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"
#include "RtTpsROIUtility/tps_ru_polygon_utility.h"
#include "RtTpsROIUtility/tps_ru_roi_shape_converter.h"

// std
#include <stack>

//#include <fstream>

TPS_BEGIN_NAMESPACE

////////////////////////////////////////////////////////////////////////// static local and global function begin

void * Realloc(void *ptr, int size){
    if (size == 0) size = sizeof(int);
    if (ptr == NULL) {
        ptr = calloc(size, 1);
    }
    else {
        ptr = realloc(ptr, size);
    }
    return(ptr);
}

static int
FindNtrscRecurse(
PlaneContour *Contour,
float Point1[2],
float Point2[2],
float Intersects[2][FI_LIMIT],
float Distances[FI_LIMIT],
int recurse);

int
FindNtrsc(
PlaneContour *Contour,
float Point1[2],
float Point2[2],
float Intersects[2][FI_LIMIT],
float Distances[FI_LIMIT])
{
    return(FindNtrscRecurse(Contour, Point1, Point2, Intersects, Distances, 0));
}

static int
FindNtrscRecurse(
PlaneContour *Contour,
float Point1[2],
float Point2[2],
float Intersects[2][FI_LIMIT],
float Distances[FI_LIMIT],
int recurse)
{
    int vertex_count = 0;
    int index = 0, loop = 0, InnerLoop = 0, OuterLoop = 0;
    float *xTrans;
    float *yTrans;
    float theta = 0.0, sinTheta = 0.0, cosTheta = 0.0;
    float x1 = 0.0, x2 = 0.0, x3 = 0.0, x1r = 0.0, x2r = 0.0, x3r = 0.0;
    float y1 = 0.0, y2 = 0.0, y3 = 0.0, y1r = 0.0, y2r = 0.0, y3r = 0.0;
    //float alpha1 = 0.0, alpha2 = 0.0;
    float temp = 0.0;
    float fx = 0.0;
    float point3[2], point4[2];
    int IntersectCount = 0;
    float P2x = 0.0, P2y = 0.0;

    vertex_count = Contour->count;
    if (vertex_count == 0) return(0);
    xTrans = (float *)malloc(2 * vertex_count*sizeof(float));
    yTrans = xTrans + vertex_count;
    for (loop = 0; loop < vertex_count; loop++) {
        xTrans[loop] = Contour->x[loop] - Point1[0];
        yTrans[loop] = Contour->y[loop] - Point1[1];
    }

    P2x = Point2[0] - Point1[0];
    P2y = Point2[1] - Point1[1];

    if (fabs((double)P2x) < 0.0001) {
        if (P2y > 0.0) {
            theta = (float)PI / 2.0;
            sinTheta = 1.0;
            cosTheta = 0.0;
        }
        else {
            theta = (float)-PI / 2.0;
            sinTheta = -1.0;
            cosTheta = 0.0;
        }
    }
    else if (fabs((double)P2y) < 0.0001) {
        if (P2x > 0.0) {
            theta = 0.0;
            sinTheta = 0.0;
            cosTheta = 1.0;
        }
        else {
            theta = (float)PI;
            sinTheta = 0.0;
            cosTheta = -1.0;
        }
    }
    else {

        theta = (float)atan2((double)P2y, (double)P2x);
        sinTheta = (float)sin((double)theta);
        cosTheta = (float)cos((double)theta);
    }

    x1 = xTrans[vertex_count - 1];
    x2 = xTrans[0];
    x3 = xTrans[1];
    y1 = yTrans[vertex_count - 1];
    y2 = yTrans[0];
    y3 = yTrans[1];

    x1r = x1 * cosTheta + y1 * sinTheta;
    x2r = x2 * cosTheta + y2 * sinTheta;
    x3r = x3 * cosTheta + y3 * sinTheta;
    y1r = -(x1 * sinTheta) + y1 * cosTheta;
    y2r = -(x2 * sinTheta) + y2 * cosTheta;
    y3r = -(x3 * sinTheta) + y3 * cosTheta;

    P2x = (float)hypot((double)P2x, (double)P2y);
    P2y = 0.0;

    IntersectCount = 0;
    for (loop = 0; loop < vertex_count; loop++) {
        if (y2r == 0.0 && x2r >= 0.0) {
            if (y1r == 0.0) {
#define TAD1 (0.0005)
#define TAD2 (0.0004)
                point3[0] = Point1[0] + ((Point1[0] < 0.0) ? TAD1 : -TAD1);
                point3[1] = Point1[1] + ((Point1[1] < 0.0) ? TAD2 : -TAD2);
                point4[0] = Point2[0] + ((Point2[0] < 0.0) ? TAD2 : -TAD2);
                point4[1] = Point2[1] + ((Point2[1] < 0.0) ? TAD1 : -TAD1);
                recurse++;
                free(xTrans);
                if (recurse > 10) {
                    fprintf(stderr, "FindNtrsc: too many recursions\n");
                    return(-1);
                }
                return(FindNtrscRecurse(Contour, point3, point4,
                    Intersects, Distances, recurse));
            }
            else {
                if ((y1r * y3r < 0.0) && (x2r <= P2x)) {

                    if (IntersectCount < FI_LIMIT) {
                        Distances[IntersectCount] = x2r;
                        Intersects[0][IntersectCount] = x2r*cosTheta + Point1[0];
                        Intersects[1][IntersectCount] = x2r*sinTheta + Point1[1];
                    }
                    IntersectCount++;
                }
            }
        }
        else {
            if (y1r * y2r < 0.0) {

                temp = plunc::interp(0, y1r, y2r, 0.0, x1r, x2r, &fx);
                if (temp >= 0.0 && temp <= P2x) {
                    if (IntersectCount < FI_LIMIT) {
                        Distances[IntersectCount] = temp;
                        Intersects[0][IntersectCount] = temp*cosTheta + Point1[0];
                        Intersects[1][IntersectCount] = temp*sinTheta + Point1[1];
                    }
                    IntersectCount++;
                }
            }
        }

        x1 = x2;
        y1 = y2;
        x2 = x3;
        y2 = y3;
        x1r = x2r;
        y1r = y2r;
        x2r = x3r;
        y2r = y3r;
        switch (vertex_count - loop) {
        case 2:
            x3 = xTrans[0];
            y3 = yTrans[0];
            break;

        case 1:
            break;

        default:
            x3 = xTrans[loop + 2];
            y3 = yTrans[loop + 2];
            break;
        }
        x3r = x3 * cosTheta + y3 * sinTheta;
        y3r = -(x3 * sinTheta) + y3 * cosTheta;
    }

    /*
      Sort them into ascending order by distance.  Excuse the bubble sort.
      */

    index = (IntersectCount > FI_LIMIT) ? FI_LIMIT : IntersectCount;
    for (OuterLoop = 0; OuterLoop < index - 1; OuterLoop++)
        if (Distances[OuterLoop] < 0.0) {
        fprintf(stderr, "I thought I fixed this.\n");
        fprintf(stderr, "  negative distance in FindNtrsc\n");
        fprintf(stderr, "  distance = %f\n", Distances[OuterLoop]);
        fprintf(stderr, "  pt1 = (%f, %f)\n", Point1[0], Point1[1]);
        fprintf(stderr, "  pt2 = (%f, %f)\n", Point2[0], Point2[1]);
        fprintf(stderr, "  intersect = (%f, %f)\n",
            Intersects[0][OuterLoop], Intersects[1][OuterLoop]);
        }

    for (OuterLoop = 0; OuterLoop < index - 1; OuterLoop++) {
        for (InnerLoop = OuterLoop + 1; InnerLoop < index; InnerLoop++) {
            if (Distances[InnerLoop] < Distances[OuterLoop]) {
                temp = Distances[InnerLoop];
                Distances[InnerLoop] = Distances[OuterLoop];
                Distances[OuterLoop] = temp;

                temp = Intersects[0][InnerLoop];
                Intersects[0][InnerLoop] = Intersects[0][OuterLoop];
                Intersects[0][OuterLoop] = temp;

                temp = Intersects[1][InnerLoop];
                Intersects[1][InnerLoop] = Intersects[1][OuterLoop];
                Intersects[1][OuterLoop] = temp;
            }
        }
    }
    recurse = 0;
    free(xTrans);
    return(IntersectCount);
}

////////////////////////////////////////////////////////////////////////// static local and global function end

int SubVolumeHelperAPI::SliceSubVolume(SubVolume& subVolume,
    const Mcsf::Point3f* pVertices,
    unsigned int uiWidth,
    unsigned int uiHeight,
    TPSMask* pTpsMask){

    if (nullptr == pTpsMask){
        TPS_LOG_DEV_ERROR << "pTpsMask is null";
        return TPS_ER_FAILURE;
    }

    return SliceSubVolume(subVolume, pVertices, uiWidth, uiHeight, pTpsMask->GetMaskBuffer());
}

int SubVolumeHelperAPI::SliceSubVolume(SubVolume& subVolume,
    const Mcsf::Point3f* pVertices,
    unsigned int uiWidth,
    unsigned int uiHeight,
    unsigned char* buffer){

    if (nullptr == buffer){
        TPS_LOG_DEV_ERROR << "the buffer is null";
        return TPS_ER_FAILURE;
    }
    if (nullptr == pVertices){
        TPS_LOG_DEV_ERROR << "pVertices is null";
        return TPS_ER_FAILURE;
    }

    int uiDimension[3];
    subVolume.GetSize(uiDimension[0], uiDimension[1], uiDimension[2]);
    if (uiDimension[0] < 0 || uiDimension[1] < 0 || uiDimension[2] < 0){
        TPS_LOG_DEV_ERROR << "wrong uiDimension in SliceSubVolume";
#ifdef _DEBUG
        printf("\nwrong uiDimension in SliceSubVolume");
#endif
        return TPS_ER_FAILURE;
    }
    unsigned char* subVolData = (unsigned char*)subVolume.GetData();
    if (uiDimension[0] == 0 || uiDimension[1] == 0 ||
        uiDimension[2] == 0 || subVolData == nullptr)
    {
        memset(buffer, 0, uiWidth * uiHeight * sizeof(unsigned char));
        return TPS_ER_SUCCESS;
    }

    int offsetX = 0;
    int offsetY = 0;
    int offsetZ = 0;
    subVolume.GetOffSet(offsetX, offsetY, offsetZ);

    std::vector<Mcsf::Point3f> vVertex;
    for (int i = 0; i < 4; ++i){
        vVertex.push_back(Mcsf::Point3f(pVertices[i].GetX() - offsetX, pVertices[i].GetY() - offsetY,
            pVertices[i].GetZ() - offsetZ));
    }

    //#if _DEBUG
    //    printf("\n====Begin GenerateSectionFromVertex\n");
    //#endif

    if (!Mcsf::McsfAlgoAutoContour::GenerateSectionFromVertex(
        (unsigned char*)subVolume.GetData(),
        (unsigned int*)uiDimension,
        vVertex,
        uiWidth,
        uiHeight,
        buffer)){
        TPS_LOG_DEV_ERROR << "[McsfAlgoAutoContour::GenerateSectionFromVertex] failed";
        return TPS_ER_FAILURE;
    }

    //#if _DEBUG
    //    printf("\n====End GenerateSectionFromVertex\n");
    //#endif
    return TPS_ER_SUCCESS;
}

int SubVolumeHelperAPI::SliceSubVolume(unsigned char* subVolume,
    unsigned int* aDim,
    const Mcsf::Point3f* pVertices,
    unsigned int uiWidth,
    unsigned int uiHeight,
    unsigned char* buffer)
{
    std::vector<Mcsf::Point3f> vVertex;
    for (int i = 0; i < 4; ++i){
        vVertex.push_back(pVertices[i]);
    }

    if (!Mcsf::McsfAlgoAutoContour::GenerateSectionFromVertex(
        subVolume, aDim, vVertex, uiWidth, uiHeight, buffer)){
        TPS_LOG_DEV_ERROR << "[McsfAlgoAutoContour::GenerateSectionFromVertex] failed!";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

int SubVolumeHelperAPI::SliceSubVolume(short* subVolume,
    unsigned int* aDim,
    const Mcsf::Point3f* pVertices,
    unsigned int uiWidth,
    unsigned int uiHeight,
    short* buffer)
{
    std::vector<Mcsf::Point3f> vVertex;
    for (int i = 0; i < 4; ++i){
        vVertex.push_back(pVertices[i]);
    }

    if (!Mcsf::McsfAlgoAutoContour::GenerateSectionFromVertex(
        subVolume, aDim, vVertex, uiWidth, uiHeight, buffer, 0)){
        TPS_LOG_DEV_ERROR << "[McsfAlgoAutoContour::GenerateSectionFromVertex] failed!";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

int SubVolumeHelperAPI::SliceSubVolume(int* subVolume,
    unsigned int* aDim,
    const Mcsf::Point3f* pVertices,
    unsigned int uiWidth,
    unsigned int uiHeight,
    int* buffer)
{
    std::vector<Mcsf::Point3f> vVertex;
    for (int i = 0; i < 4; ++i){
        vVertex.push_back(pVertices[i]);
    }

    if (!Mcsf::McsfAlgoAutoContour::GenerateSectionFromVertex(
        subVolume, aDim, vVertex, uiWidth, uiHeight, buffer, 0)){
        TPS_LOG_DEV_ERROR << "[McsfAlgoAutoContour::GenerateSectionFromVertex] failed!";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

int SubVolumeHelperAPI::GetSectionOfVolume(
    float* pVolume, unsigned int* aDim,
    const Mcsf::Point3f* pVertices, unsigned int uiWidth,
    unsigned int uiHeight, float* pSection){

    std::vector<Mcsf::Point3f> vVertex;
    for (int i = 0; i < 4; ++i){
        vVertex.push_back(pVertices[i]);
    }

    if (!Mcsf::McsfAlgoAutoContour::GenerateSectionFromVertex(
        pVolume, aDim, vVertex, uiWidth, uiHeight, pSection)){
        TPS_LOG_DEV_ERROR << "[McsfAlgoAutoContour::GenerateSectionFromVertex] failed!";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

int SubVolumeHelperAPI::GetMaskFromContours(
    const std::vector<std::vector<Mcsf::Point2f>>& vvContour,
    unsigned int uiWidth,
    unsigned int uiHeight,
    TPSMask* pTpsMask){

    unsigned char* buffer = pTpsMask->GetMaskBuffer();
    return GetMaskFromContours(vvContour, uiWidth, uiHeight, buffer);
}

int SubVolumeHelperAPI::GetMaskFromContours(
    const std::vector<std::vector<Mcsf::Point2f>>& vvContour,
    unsigned int uiWidth,
    unsigned int uiHeight,
    unsigned char* buffer, bool isSpline){

    if (uiWidth <= 0 || uiHeight <= 0 || buffer == nullptr){
        TPS_LOG_DEV_ERROR << "Invalid input parameters.";
        return TPS_ER_FAILURE;
    }

    std::vector<Mcsf::Vector2f> vContour;
    std::vector<std::vector<Mcsf::Vector2f>> vvContourVec(vvContour.size(), vContour);
    for (int i = 0; i < vvContour.size(); ++i){
        vvContourVec[i].reserve(vvContour[i].size());
        for (int j = 0; j < vvContour[i].size(); ++j){
            vvContourVec[i].push_back(vvContour[i][j]);
        }
    }
    //FILE *fp(NULL);
    //std::string newfilePath = "D:/maskDataOfEraserPolygon.txt";
    //int const iErrNo4 = fopen_s(&fp, newfilePath.c_str(), "wb"); 
    //if (0 == iErrNo4)
    //{
    //    fwrite(buffer, sizeof(unsigned char), uiWidth * uiHeight, fp);
    //    fclose(fp);
    //    printf("Open file: %s\n",newfilePath.c_str());
    //}
    std::vector<Mcsf::Contour> vvMcsfContour;
    Mcsf::Contour contour;
    for (int i = 0; i < vvContourVec.size(); ++i)
    {
        contour.outContour = vvContourVec[i];
        vvMcsfContour.push_back(contour);
    }

    //     char file_name[100];
    //     for (int i = 0; i < vvMcsfContour.size(); ++i)
    //     {
    //         sprintf_s(file_name, "d:\\temp1\\contour %d.raw", i);
    //         fstream fs;
    //         fs.open(file_name, ios::out | ios::binary | ios::trunc);
    //         for (int j = 0; j < vvMcsfContour[i].outContour.size(); ++j)
    //         {
    //             fs<<vvMcsfContour[i].outContour[j].GetX()<<'|'
    //               <<vvMcsfContour[i].outContour[j].GetY()<<'|';
    //         }
    //         fs.close();
    //     }
    // 
    //     fstream fs;
    //     fs.open("d:\\temp1\\contour 0.raw", ios::in | ios::binary);
    //     float x, y;
    //     char ch;
    //     fs >> x >> ch >> y >> ch;
    //     fs >> x;
    //     fs >> ch;
    //     fs >> y;
    //     fs.close();

    bool bRet = Mcsf::McsfAlgoAutoContour::GetMaskFromContours(vvMcsfContour,
        uiWidth, uiHeight, 1, buffer, isSpline);
    if (!bRet){
        TPS_LOG_DEV_ERROR << "GetMaskFromContours error.";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

int SubVolumeHelperAPI::GetMaskFromCompletedContours(
    const std::vector<std::vector<::Mcsf::Point2f>>& vvContour,
    unsigned int uiWidth,
    unsigned int uiHeight,
    TPSMask* pTpsMask)
{
    unsigned char* buffer = pTpsMask->GetMaskBuffer();
    return GetMaskFromCompletedContours(vvContour, uiWidth, uiHeight, buffer);
}

int SubVolumeHelperAPI::GetMaskFromCompletedContours(
    const std::vector<std::vector<::Mcsf::Point2f>>& vvContour,
    unsigned int uiWidth,
    unsigned int uiHeight,
    unsigned char* buffer, bool isSpline/* = false*/)
{
    if (uiWidth <= 0 || uiHeight <= 0 || buffer == nullptr){
        TPS_LOG_DEV_ERROR << "Invalid input parameters.";
        return TPS_ER_FAILURE;
    }
    memset(buffer, 0, uiWidth * uiHeight * sizeof(unsigned char));

    std::vector<Mcsf::Contour> vvMcsfContour;
    vvMcsfContour.push_back(Mcsf::Contour());
    unsigned char* tmpBuffer = new unsigned char[uiWidth * uiHeight];
    for (int i = 0; i < vvContour.size(); ++i)
    {
        vvMcsfContour[0].outContour.resize(vvContour[i].size());
        std::transform(vvContour[i].begin(), vvContour[i].end(),
            vvMcsfContour[0].outContour.begin(), [](Mcsf::Point2f pt)->Mcsf::Vector2f
        {
            return pt;
        });

        if (!Mcsf::McsfAlgoAutoContour::GetMaskFromContours(vvMcsfContour,
            uiWidth, uiHeight, 1, tmpBuffer, isSpline))
        {
            TPS_DEL_ARRAY(tmpBuffer);
            TPS_LOG_DEV_ERROR << "Mcsf::McsfAlgoAutoContour::GetMaskFromContours error.";
            return TPS_ER_FAILURE;
        }

        for (int j = 0; j < uiWidth * uiHeight; ++j)
        {
            buffer[j] ^= tmpBuffer[j];
        }
    }
    TPS_DEL_ARRAY(tmpBuffer);
    return TPS_ER_SUCCESS;
}

/*
 int SubVolumeHelperAPI::ConvertMaskFromContour(std::vector<std::vector<Mcsf::Point2f>>& vvContour,
 unsigned int uiWidth, unsigned int uiHeight, unsigned char* buffer){
 if (uiWidth <= 0 || uiHeight <= 0 || buffer == nullptr){
 TPS_LOG_DEV_ERROR<<"Invalid input parameters.";
 return TPS_ER_FAILURE;
 }
 const int size = uiWidth * uiHeight;
 memset(buffer, 0, sizeof(unsigned char) * size);

 std::vector<Mcsf::Point2f> oneContour;
 int x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
 int i = 0, j =0;
 float closeEnough = 0.01f;  // because we need more points
 float v1(0.),v2(0.),v3(0.);

 for (i=0; i<vvContour.size(); ++i){
 oneContour = vvContour[i];
 for (j=1; j<oneContour.size(); ++j){
 x2 = oneContour[j].GetX();
 y2 = oneContour[j].GetY();

 x1 = oneContour[j-1].GetX();
 y1 = oneContour[j-1].GetY();
 buffer[y2 * uiWidth + x2] = 1;

 //buffer[y2 * uiWidth + x2 + 1] = 1;
 //buffer[y2 * uiWidth + x2 - 1] = 1;
 //buffer[(y2 + 1) * uiWidth + x2] = 1;
 //buffer[(y2 - 1) * uiWidth + x2] = 1;
 closeEnough = max(abs(y1- y2), abs(x1 -x2))/2 + 1;

 for(y3 = min(y1,y2); y3 <= max(y1,y2); ++y3){
 for(x3 = min(x1,x2); x3 <= max(x1,x2); ++x3){
 v1 = x1 * (y2 - y3);
 v2 = x2 * (y3 - y1);
 v3 = x3 * (y1 - y2);
 if(abs(v1 + v2 + v3) < closeEnough){
 buffer[y3 * uiWidth + x3] = 1;

 //buffer[ y3 * uiWidth + x3 + 1] = 1;
 //buffer[ y3 * uiWidth + x3 - 1] = 1;
 //buffer[(y3 + 1) * uiWidth + x3] = 1;
 //buffer[(y3 - 1) * uiWidth + x3] = 1;
 }
 }
 }
 }
 }

 unsigned char* temp = new unsigned char[size];
 memset(temp, 0, sizeof(unsigned char) * size);
 const int count = plunc::SeedFill((const char*)buffer,uiWidth, uiHeight, (char*)temp);
 memcpy(buffer, temp, sizeof(unsigned char) * size);
 delete[] temp; temp = nullptr;
 return 1 == count ? TPS_ER_SUCCESS : TPS_ER_FAILURE;
 }
 */

int SubVolumeHelperAPI::GetContoursFromMask(const TPSMask& maskData,
    std::vector<std::vector<Mcsf::Point2f>>& vvContour){

    unsigned char* buffer = maskData.GetMaskBuffer();
    size_t width, height;
    maskData.GetMaskSize(width, height);

    return GetContoursFromMask(buffer, width, height, vvContour);
}

int SubVolumeHelperAPI::GetContoursFromMask(unsigned char* buffer,
    unsigned int width, unsigned int height,
    std::vector<std::vector<Mcsf::Point2f>>& vvContour)
{
    vvContour.clear();

    if (buffer == nullptr || width <= 0 || height <= 0){
        TPS_LOG_DEV_ERROR << "Invalid parameters.";
        return TPS_ER_FAILURE;
    }

    //unsigned char* tempBuffer = new unsigned char[height * width * 4];
    //for (int i=0; i<width*height; ++i){
    //    tempBuffer[4*i] = buffer[i] * 255;
    //    tempBuffer[4*i + 1] = buffer[i] * 255;
    //    tempBuffer[4*i + 2] = buffer[i] * 255;
    //    tempBuffer[4*i + 3] = 255;
    //}
    //char fileName[256];
    //sprintf_s(fileName, "d:\\temp\\sliceBuffer.bmp");
    //    
    //SaveToTrueColorBitmap(fileName, tempBuffer, height, width);
    //delete[] tempBuffer;
    //    std::vector<std::vector<Mcsf::Vector2f>> vvTemp;
    std::vector<Mcsf::Contour> vvTemp;
    bool bRet = Mcsf::McsfAlgoAutoContour::GetContoursFromMask(buffer,
        width, height, 1, vvTemp, false);
    if (!bRet){
        TPS_LOG_DEV_ERROR << "GetContoursFromMask error";
        return TPS_ER_FAILURE;
    }
    for (auto itr = vvTemp.begin(); itr != vvTemp.end(); ++itr){
        std::vector<Mcsf::Point2f> vContour;
        for (auto itrPt = (*itr).outContour.cbegin(); itrPt != (*itr).outContour.cend(); ++itrPt){
            vContour.push_back(Mcsf::Point2f((*itrPt).GetX(), (*itrPt).GetY()));
        }
        if (!vContour.empty()){
            vvContour.push_back(vContour);
        }

        for (auto itrIn = (*itr).innerContours.cbegin(); itrIn != (*itr).innerContours.cend(); ++itrIn){
            std::vector<Mcsf::Point2f> vContourInner;
            for (auto itrPt = (*itrIn).cbegin(); itrPt != (*itrIn).cend(); ++itrPt){
                vContourInner.push_back(Mcsf::Point2f((*itrPt).GetX(), (*itrPt).GetY()));
            }
            if (!vContourInner.empty()){
                vvContour.push_back(vContourInner);
            }
        }
    }

    return TPS_ER_SUCCESS;
}

int SubVolumeHelperAPI::GetOutlineFromMask(const unsigned char* pImageInput,
    unsigned int uiWidth, unsigned int uiHeight,
    unsigned char* pImageOutput){

    if (nullptr == pImageInput || nullptr == pImageOutput){
        TPS_LOG_DEV_ERROR << "pImageInput or pImageOutput is null.";
        return TPS_ER_FAILURE;
    }
    unsigned int iSize = uiWidth * uiHeight;
    if (iSize < 1){
        TPS_LOG_DEV_ERROR << "image size error.";
        return TPS_ER_FAILURE;
    }

    memset(pImageOutput, 0, iSize);


    //Roberts Cross operator fast edge detecting
    //G(x,y)=abs(f(x,y)-f(x+1,y+1))+abs(f(x,y+1)-f(x+1,y))
    unsigned char xy = 0, x1y1 = 0, xy1 = 0, x1y = 0, gxy = 0;
    for (unsigned int j = 0; j < uiHeight - 1; ++j){
        for (unsigned int i = 0; i < uiWidth - 1; ++i){

            xy = *(pImageInput + uiWidth * j + i);
            x1y1 = *(pImageInput + uiWidth * (j + 1) + i + 1);
            xy1 = *(pImageInput + uiWidth * (j + 1) + i);
            x1y = *(pImageInput + uiWidth * j + i + 1);

            gxy = abs(xy - x1y1) + abs(xy1 - x1y);
            //set outline
            *(pImageOutput + uiWidth*j + i) = gxy > 0 ? 1 : 0;
        }
    }


    //another method
    //    unsigned int iSizeEdge = (uiWidth + 2) * (uiHeight + 2);
    //    unsigned char *data = new unsigned char[iSizeEdge];
    //    memset(data, 0, iSizeEdge);
    //    for(unsigned int j = 0; j < uiHeight; ++j){
    //        memcpy(data + (j + 1 ) * (uiWidth + 2) + 1, pImageInput + j * uiWidth, uiWidth);
    //    }
    //
    //#define test_four_point(j)         \
    //    (data[j - 1] == 0 ||           \
    //    data[j + 1] == 0 ||            \
    //    data[j - uiWidth - 2] == 0 ||  \
    //    data[j + uiWidth + 2] == 0)
    //
    //    //edge should not included in the loop
    //    for (unsigned int i = uiWidth + 3; i < iSizeEdge - uiWidth -3; ++i) {
    //        if (data[i] == 1) {
    //            if (test_four_point(i)) {
    //                pImageOutput[i] = 1;
    //            }
    //        }
    //    }
    //    delete[] data; data = nullptr;
    return TPS_ER_SUCCESS;
}


int SubVolumeHelperAPI::UpdateSubVolume(const Mcsf::Point3f *pVertices,
    const TPSMask& maskData,
    SubVolume* pSubVolume, bool isAdd){

    unsigned char* buffer = maskData.GetMaskBuffer();
    size_t width, height;
    maskData.GetMaskSize(width, height);

    return UpdateSubVolume(pVertices, buffer, width, height, pSubVolume, isAdd);
}

float RefineFloatValue(float v)
{
    if (ceil(v) - v <= 1e-3) return ceil(v);
    if (v - floor(v) <= 1e-3) return floor(v);
    return v;
}

int SubVolumeHelperAPI::UpdateSubVolume(const Mcsf::Point3f *pVertices,
    unsigned char* buffer, unsigned int width, unsigned int height,
    SubVolume* pSubVolume, bool isAdd){

    if (nullptr == buffer){
        TPS_LOG_DEV_ERROR << "maskData GetMaskBuffer is null.";
        return TPS_ER_FAILURE;
    }

    if (nullptr == pSubVolume){
        TPS_LOG_DEV_ERROR << "pSubVolume is null.";
        return TPS_ER_FAILURE;
    }

    int offsetX = 0;
    int offsetY = 0;
    int offsetZ = 0;
    pSubVolume->GetOffSet(offsetX, offsetY, offsetZ);

    std::vector<Mcsf::Point3f> vVertex;
    unsigned int i = 0;
    float x, y, z;
    for (i = 0; i < 4; ++i){
        //临时代码，去除因浮点数导致的缺层问题，待PI组修复后，删除该代码
        x = RefineFloatValue(pVertices[i].GetX());
        y = RefineFloatValue(pVertices[i].GetY());
        z = RefineFloatValue(pVertices[i].GetZ());

        vVertex.push_back(Mcsf::Point3f(x - offsetX, y - offsetY, z - offsetZ));
    }

    // too slow
    bool bRet;
    if (isAdd) {
        bRet = Mcsf::McsfAlgoAutoContour::AddMaskToSubVolume(vVertex, width, height,
            buffer, 1, *pSubVolume);
    }
    else {
        bRet = Mcsf::McsfAlgoAutoContour::UpdateSubVolumeFromMask(vVertex, width, height,
            buffer, 1, *pSubVolume);
    }
    //#ifdef _DEBUG
    //    unsigned char* bmpBuffer = new unsigned char[width * height * 4];
    //    for (int j=0; j < width * height; ++j){
    //        bmpBuffer[4 * j] = buffer[j] * 255;
    //        bmpBuffer[4 * j + 1] = buffer[j] * 255;
    //        bmpBuffer[4 * j + 2] = buffer[j] * 255;
    //        bmpBuffer[4 * j + 3] = 255;
    //    }
    //    char fileName[256];
    //    sprintf_s(fileName, "d:\\temp\\updatebuffer_result.bmp");
    //    SubVolumeHelperAPI::SaveToTrueColorBitmap(fileName, bmpBuffer, height, width);
    //    delete []bmpBuffer;
    //#endif
    if (!bRet){
        TPS_LOG_DEV_ERROR << "UpdateSubVolumeFromMask error";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

int SubVolumeHelperAPI::UpdateSubVolumeFromContour(std::vector<std::vector<Mcsf::Point2f>>& vvContour,
    unsigned int uiWidth,
    unsigned int uiHeight,
    const Mcsf::Point3f *pVertices,
    SubVolume* pSubVolume)
{
    if (nullptr == pSubVolume)
    {
        TPS_LOG_DEV_ERROR << "pSubVolume is null.";
        return TPS_ER_FAILURE;
    }

    const unsigned int size = uiWidth * uiHeight;
    unsigned char* buffer = new unsigned char[size];
    memset(buffer, 0, size);

    if (TPS_ER_SUCCESS != GetMaskFromCompletedContours(vvContour, uiWidth, uiHeight, buffer))
    {
        TPS_LOG_DEV_ERROR << "GetMaskFromContours failed!";
        delete[] buffer;
        buffer = nullptr;
        return TPS_ER_FAILURE;
    }

    int offsetX = 0;
    int offsetY = 0;
    int offsetZ = 0;
    pSubVolume->GetOffSet(offsetX, offsetY, offsetZ);
    unsigned char * pData = (unsigned char*)pSubVolume->GetData();
    const unsigned int z = (unsigned int)(pVertices[0].GetZ() - offsetZ + 0.5);
    int iX = 0, iY = 0, iZ = 0;
    pSubVolume->GetSize(iX, iY, iZ);
    if (z < iZ)
    {
        for (unsigned int i = 0; i < size; ++i)
        {
            pData[size * z + i] = (0 == pData[size * z + i]) ? buffer[i] : 1;// 0 means backgroud
        }
    }

    delete[] buffer;
    buffer = nullptr;
    return TPS_ER_SUCCESS;
}

//TODO


//int SubVolumeHelperAPI::BuildSubVolumeFromContour(const TpsImage3DEntity& pImage3D, VOIEntity* pVoiEntity){
//        
//    auto pTmsVoi = pVoiEntity->GetTMSVoiObject();
//    const std::vector<RtContour*>& vecContour = pTmsVoi->get_contours();
//    if (vecContour.size() < 1){
//        TPS_LOG_DEV_ERROR<<"no any contour.";
//        return TPS_ER_FAILURE;
//    }
//    
//    plunc::ANASTRUCT* ana = new plunc::ANASTRUCT();
//    plunc::contours_to_anastruct(vecContour, ana);
//    plunc::anastruct_to_voi(ana, pImage3D, pVoiEntity);
//
//    //release
//    for(int i=0; i<ana->contour_count; ++i){
//        delete[] ana->contours[i].x; ana->contours[i].x = nullptr;
//        delete[] ana->contours[i].y; ana->contours[i].y = nullptr;
//    }
//    delete[] ana->contours; ana->contours = nullptr;
//    delete ana; ana = nullptr;
//
//    return TPS_ER_SUCCESS;
//}


//void SubVolumeHelperAPI::ReduceVOI(const VOIEntity& voiEntity, const TpsImage3DEntity& image3D, 
//    std::vector<RtContour*>& vtContour){
//
//    plunc::ANASTRUCT* ana = (plunc::ANASTRUCT*)malloc(sizeof(plunc::ANASTRUCT));
//    voi_to_anastruct_ext(&voiEntity, &image3D, ana);//include point reduce
//    
//    anastruct_to_contours_ext(*ana, image3D, voiEntity, vtContour);
//    
//    //release
//    for(int i=0; i<ana->contour_count; ++i){
//        free(ana->contours[i].x); ana->contours[i].x = nullptr;
//        free(ana->contours[i].y); ana->contours[i].y = nullptr;
//    }
//    free(ana->contours); ana->contours = nullptr;
//    free(ana); ana = nullptr;
//}

//void SubVolumeHelperAPI::ReduceVOI2Contours(const VOIEntity& voiEntity, 
//    const std::string& sImage3DUID, Mcsf::Matrix4f& matPat2World){
//
//    TpsImage3DEntity* image3D = nullptr;
//    TpsImageDataManager::GetInstance()->GetImage3DData(sImage3DUID, &image3D);
//    if (nullptr == image3D) {
//        return;
//    }
//
//    plunc::ANASTRUCT* ana = (plunc::ANASTRUCT*)malloc(sizeof(plunc::ANASTRUCT));
//    voi_to_anastruct_ext(&voiEntity, image3D, ana, 2.0f);//include point reduce
//
//    anastruct_to_contours_list(*ana, voiEntity, matPat2World);
//
//    //release
//    for(int i=0; i<ana->contour_count; ++i){
//        free(ana->contours[i].x); ana->contours[i].x = nullptr;
//        free(ana->contours[i].y); ana->contours[i].y = nullptr;
//    }
//    free(ana->contours); ana->contours = nullptr;
//    free(ana); ana = nullptr;
//}

// int SubVolumeHelperAPI::GetBoundingBox(const VOIEntity& voiEntity,
//     const TpsImage3DEntity& image3D, float* boundaryBox)
// {
//     if (nullptr == boundaryBox){
//         TPS_LOG_DEV_ERROR << "boundaryBox is null.";
//         return TPS_ER_FAILURE;
//     }
// 
//     RtImage3DHeader* header;
//     if (!image3D.GetImage3DHeader(header)){
//         TPS_LOG_DEV_ERROR << "Failed to get current image 3d header.";
//         return TPS_ER_FAILURE;
//     }
// 
//     const std::vector<RtContour*>& vecContour = voiEntity.GetContours();
//     if (vecContour.empty()){
//         boundaryBox[0] = 0.f;
//         boundaryBox[1] = 0.f;
//         boundaryBox[2] = 0.f;
//         boundaryBox[3] = 0.f;
//         boundaryBox[4] = 0.f;
//         boundaryBox[5] = 0.f;
//         return TPS_ER_SUCCESS;
//     }
// 
//     int iDimX = header->m_iXDim;
//     int iDimY = header->m_iYDim;
//     boundaryBox[0] = iDimX - 1.f;
//     boundaryBox[1] = 0.f;
//     boundaryBox[2] = iDimY - 1.f;
//     boundaryBox[3] = 0.f;
//     boundaryBox[4] = header->m_iSliceCount - 1.f;
//     boundaryBox[5] = 0.f;
// 
//     Mcsf::Matrix4f matPatientToVolume;
//     if (!image3D.GetVolumeToPatientMatrix(matPatientToVolume)){
//         TPS_LOG_DEV_ERROR << "Failed to get volume to patient matrix.";
//         return TPS_ER_FAILURE;
//     }
//     matPatientToVolume = matPatientToVolume.Inverse();
// 
//     Mcsf::Point3f point;
//     TPS_ANASTRUCT* ana = voiEntity.GetAnastruct();
//     TPS_CONTOUR_STC** allContours = ana->contours;
//     for (int iSlice = 0; iSlice < ana->slice_count; iSlice++) {
//         // 该层面上有多少条contour
//         int contoursOnSlice = ana->contour_count[iSlice];
//         // 遍历该层面上所有contour
//         for (int iContour = 0; iContour < contoursOnSlice; iContour++) {
//             TPS_CONTOUR_STC* contour = &allContours[iSlice][iContour];
//             // 遍历这条contour上的所有点
//             for (int iPt = 0; iPt < contour->vertex_count; iPt++) {
//                 Mcsf::Point3f& contourPt = contour->pt[iPt];
//                 point = matPatientToVolume.Transform(contourPt);
//                 boundaryBox[0] = TPS_MIN(point.GetX(), boundaryBox[0]);
//                 boundaryBox[1] = TPS_MAX(point.GetX(), boundaryBox[1]);
//                 boundaryBox[2] = TPS_MIN(point.GetY(), boundaryBox[2]);
//                 boundaryBox[3] = TPS_MAX(point.GetY(), boundaryBox[3]);
//                 boundaryBox[4] = TPS_MIN(point.GetZ(), boundaryBox[4]);
//                 boundaryBox[5] = TPS_MAX(point.GetZ(), boundaryBox[5]);
//             }
//         }
//     }
// 
//     return TPS_ER_SUCCESS;
// }

int SubVolumeHelperAPI::GetContoursFromSubvolume(SubVolume& subVolume,
    const Mcsf::Point3f* pVertices,
    unsigned int uiWidth,
    unsigned int uiHeight,
    std::vector<std::vector<Mcsf::Vector2f>>* vvContour){
    //TODO
    subVolume;
    pVertices;
    uiWidth;
    uiHeight;
    vvContour;
    return 0;
}

int SubVolumeHelperAPI::GetMinimumBoundingRectangle(unsigned int* dim,
    const Mcsf::Point3f* pVertices,
    Mcsf::Point3f* pMiniVertices) {

    //todo: float to int , then calculate, then int to float
    float x0 = pVertices[0].GetX();
    float y0 = pVertices[0].GetY();
    float z0 = pVertices[0].GetZ();
    float x1 = pVertices[1].GetX();
    float y1 = pVertices[1].GetY();
    float z1 = pVertices[1].GetZ();
    float x2 = pVertices[2].GetX();
    float y2 = pVertices[2].GetY();
    float z2 = pVertices[2].GetZ();
    float x3 = pVertices[3].GetX();
    float y3 = pVertices[3].GetY();
    float z3 = pVertices[3].GetZ();
    float xPara = (y2 - y1)*(z3 - z1) - (z2 - z1)*(y3 - y1);
    float yPara = (x2 - x1)*(z3 - z1) - (x3 - x1)*(z2 - z1);
    float zPara = (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1);
    float planeFunc = (x0 - x1)*xPara - (y0 - y1)*yPara + (z0 - z1)*zPara;

    if ((planeFunc > 1e-6) || (planeFunc < -(1e-6))) return TPS_ER_FAILURE;

    // caculate the intersection of 12 edges of the volume with the cut plane

    std::vector<Mcsf::Point3f> intersections;

    Mcsf::Point3f intersection;

    float x = 0;
    float y = 0;
    float z = 0;
    bool flag = false;

    if ((xPara > 1e-6) || (xPara < -1e-6)) {

        flag = true;

        x = ((0 - y1) * yPara - (dim[2] - z1) * zPara) / xPara + x1;

        if (x >= 0 && x <= dim[0]) {
            intersection.SetX(x);
            intersection.SetY(0);
            intersection.SetZ(dim[2]);
            intersections.push_back(intersection);
        }

        x = ((dim[1] - y1) * yPara - (dim[2] - z1) * zPara) / xPara + x1;

        if (x >= 0 && x <= dim[0]) {
            intersection.SetX(x);
            intersection.SetY(dim[1]);
            intersection.SetZ(dim[2]);
            intersections.push_back(intersection);
        }

        x = ((0 - y1) * yPara - (0 - z1) * zPara) / xPara + x1;

        if (x >= 0 && x <= dim[0]) {
            intersection.SetX(x);
            intersection.SetY(0);
            intersection.SetZ(0);
            intersections.push_back(intersection);
        }

        x = ((dim[1] - y1) * yPara - (0 - z1) * zPara) / xPara + x1;

        if (x >= 0 && x <= dim[0]) {
            intersection.SetX(x);
            intersection.SetY(dim[1]);
            intersection.SetZ(0);
            intersections.push_back(intersection);
        }
    }
    if ((yPara > 1e-6) || (yPara < -1e-6)) {

        y = ((0 - x1) * xPara + (0 - z1) * zPara) / yPara + y1;

        if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
            intersection.SetX(0);
            intersection.SetY(y);
            intersection.SetZ(0);
            intersections.push_back(intersection);
        }

        y = ((dim[0] - x1) * xPara + (dim[2] - z1) * zPara) / yPara + y1;

        if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
            intersection.SetX(dim[0]);
            intersection.SetY(y);
            intersection.SetZ(dim[2]);
            intersections.push_back(intersection);
        }

        y = ((0 - x1) * xPara + (dim[2] - z1) * zPara) / yPara + y1;

        if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
            intersection.SetX(0);
            intersection.SetY(y);
            intersection.SetZ(dim[2]);
            intersections.push_back(intersection);
        }

        y = ((dim[0] - x1) * xPara + (0 - z1) * zPara) / yPara + y1;

        if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
            intersection.SetX(dim[0]);
            intersection.SetY(y);
            intersection.SetZ(0);
            intersections.push_back(intersection);
        }

        flag = true;
    }

    if ((zPara > 1e-6) || (zPara < -1e-6)){

        z = ((0 - y1) * yPara - (0 - x1) * xPara) / zPara + z1;

        if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
            intersection.SetX(0);
            intersection.SetY(0);
            intersection.SetZ(z);
            intersections.push_back(intersection);
        }

        z = ((0 - y1) * yPara - (dim[0] - x1) * xPara) / zPara + z1;

        if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
            intersection.SetX(dim[0]);
            intersection.SetY(0);
            intersection.SetZ(z);
            intersections.push_back(intersection);
        }

        z = ((dim[1] - y1) * yPara - (dim[0] - x1) * xPara) / zPara + z1;

        if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
            intersection.SetX(dim[0]);
            intersection.SetY(dim[1]);
            intersection.SetZ(z);
            intersections.push_back(intersection);
        }

        z = ((dim[1] - y1) * yPara - (0 - x1) * xPara) / zPara + z1;

        if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
            intersection.SetX(0);
            intersection.SetY(dim[1]);
            intersection.SetZ(z);
            intersections.push_back(intersection);
        }
    }

    if (intersections.size() < 3) {
        return TPS_ER_FAILURE;
    }

    //if the intersections have four points and they are a rectangle, return TPS_ER_SUCCESS
    //todo: now I don't know how to get a rectangle which start from left buttom to right buttom to right top to left top with a easy method!
    //if (intersections.size() == 4) {
    //    float dot01 = (intersections[2].GetX() - intersections[1].GetX()) * (intersections[1].GetX() - intersections[0].GetX()) + 
    //                  (intersections[2].GetY() - intersections[1].GetY()) * (intersections[1].GetY() - intersections[0].GetY()) + 
    //                  (intersections[2].GetZ() - intersections[1].GetZ()) * (intersections[1].GetZ() - intersections[0].GetZ());
    //    float dot12 = (intersections[3].GetX() - intersections[2].GetX()) * (intersections[2].GetX() - intersections[1].GetX()) + 
    //                  (intersections[3].GetY() - intersections[2].GetY()) * (intersections[2].GetY() - intersections[1].GetY()) + 
    //                  (intersections[3].GetZ() - intersections[2].GetZ()) * (intersections[2].GetZ() - intersections[1].GetZ());
    //    float dot23 = (intersections[0].GetX() - intersections[3].GetX()) * (intersections[3].GetX() - intersections[2].GetX()) + 
    //                  (intersections[0].GetY() - intersections[3].GetY()) * (intersections[3].GetY() - intersections[2].GetY()) + 
    //                  (intersections[0].GetZ() - intersections[3].GetZ()) * (intersections[3].GetZ() - intersections[2].GetZ());
    //    float dot30 = (intersections[1].GetX() - intersections[0].GetX()) * (intersections[0].GetX() - intersections[3].GetX()) + 
    //                  (intersections[1].GetY() - intersections[0].GetY()) * (intersections[0].GetY() - intersections[3].GetY()) + 
    //                  (intersections[1].GetZ() - intersections[0].GetZ()) * (intersections[0].GetZ() - intersections[3].GetZ());
    //    if ((dot01 < 1e-6) && (dot01 > -1e-6) && (dot12 < 1e-6) && (dot12 > -1e-6) && 
    //        (dot23 < 1e-6) && (dot23 > -1e-6) && (dot30 < 1e-6) && (dot30 > -1e-6))
    //    {
    //        pMiniVertices[0] = intersections[0];
    //        pMiniVertices[1] = intersections[1];
    //        pMiniVertices[2] = intersections[2];
    //        pMiniVertices[3] = intersections[3];
    //        return TPS_ER_SUCCESS;
    //    }
    //}

    //tranform 3D point to 2D point, build a coordinate system which original point is pVertices[0], 
    //x direction is pVertices[1] - pVertices[0], y direction is pVertices[3] - pVertices[0]
    Mcsf::Vector3f vecX = pVertices[1] - pVertices[0];
    vecX.Normalize();
    Mcsf::Vector3f vecY = pVertices[3] - pVertices[0];
    vecY.Normalize();
    Mcsf::Vector3f vecZ = vecY.CrossProduct(vecX);
    Mcsf::Vector3f temp1(pVertices[0].GetX(), pVertices[0].GetY(), pVertices[0].GetZ());
    Mcsf::Matrix4f volumeToView(Mcsf::Vector4f(vecX.GetX(), vecY.GetX(), vecZ.GetX(), 0.0),
        Mcsf::Vector4f(vecX.GetY(), vecY.GetY(), vecZ.GetY(), 0.0),
        Mcsf::Vector4f(vecX.GetZ(), vecY.GetZ(), vecZ.GetZ(), 0.0),
        Mcsf::Vector4f(-1 * temp1.DotProduct(vecX), -1 * temp1.DotProduct(vecY),
        -1 * temp1.DotProduct(vecZ), 1.0));

    std::vector<Mcsf::Point3f> intersections2D;
    typedef std::vector<Mcsf::Point3f>::iterator iter;
    for (iter it = intersections.begin(); it != intersections.end(); ++it) {
        intersections2D.push_back(volumeToView.Transform(*it));
    }

    // calculate the centroid point(mean value)
    float meanX = 0, meanY = 0, meanZ = 0;
    for (iter it = intersections2D.begin(); it != intersections2D.end(); ++it) {
        meanX += it->GetX();
        meanY += it->GetY();
        meanZ += it->GetZ();
    }
    meanX = meanX / intersections2D.size();
    meanY = meanY / intersections2D.size();
    meanZ = meanZ / intersections2D.size();

    //Determination of principal axes by minimum the sum of the square of the perpendicular distances
    float tan2angel = 0.0;
    float numerator = 0.0;
    float denominator = 0.0;
    for (iter it = intersections2D.begin(); it != intersections2D.end(); ++it) {
        numerator += 2 * (it->GetX() - meanX) * (it->GetY() - meanY);
        denominator += ((it->GetX() - meanX) * (it->GetX() - meanX) - (it->GetY() - meanY) * (it->GetY() - meanY));
    }

    //if the principal axes are not parallel to the real axes of the 2D plane,we get the rectangle with two steps:
    //step1、computing the upper and lower furthest edge points
    if (((denominator > 1e-6) || (denominator < -1e-6)) && ((numerator > 1e-6) || (numerator < -1e-6)) && (intersections2D.size() >= 4)) {

        tan2angel = numerator / denominator;

        float angel = atan(tan2angel) / 2;
        float sinAngel = sin(angel);
        float cosAngel = cos(angel);
        float tanAngel = tan(angel);
        float cotAngel = 1.0 / tanAngel;
        float upper = 0.0, buttom = 0.0, left = 0.0, right = 0.0;
        Mcsf::Point3f upperPoint, buttomPoint, leftPoint, rightPoint;

        float temp = 0.0;
        float minorTemp = 0.0;
        for (iter it = intersections2D.begin(); it != intersections2D.end(); ++it) {
            temp = ((it->GetX() - meanX) * sinAngel - (it->GetY() - meanY) * cosAngel);
            temp = temp * temp;
            minorTemp = ((it->GetX() - meanX) * cosAngel + (it->GetY() - meanY) * sinAngel);
            minorTemp = minorTemp * minorTemp;
            if ((it->GetY() - meanY - tanAngel * (it->GetX() - meanX)) > 0) {
                if (upper < temp) {
                    upper = temp;
                    upperPoint = *it;
                }
            }
            else if ((it->GetY() - meanY - tanAngel * (it->GetX() - meanX)) < 0) {
                if (buttom < temp) {
                    buttom = temp;
                    buttomPoint = *it;
                }
            }
            if ((it->GetY() - meanY + cotAngel * (it->GetX() - meanX)) > 0) {
                if (right < minorTemp) {
                    right = minorTemp;
                    rightPoint = *it;
                }
            }
            else if ((it->GetY() - meanY + cotAngel * (it->GetX() - meanX)) < 0) {
                if (left < minorTemp) {
                    left = minorTemp;
                    leftPoint = *it;
                }
            }
        }
        //step2: computing the vertices for the bounding rectangle
        pMiniVertices[0].SetX((buttomPoint.GetX() * tanAngel + leftPoint.GetX() * cotAngel + leftPoint.GetY() - buttomPoint.GetY()) / (tanAngel + cotAngel));
        pMiniVertices[0].SetY((buttomPoint.GetY() * tanAngel + leftPoint.GetY() * cotAngel + leftPoint.GetX() - buttomPoint.GetX()) / (tanAngel + cotAngel));
        pMiniVertices[0].SetZ(0.0);
        pMiniVertices[1].SetX((buttomPoint.GetX() * tanAngel + rightPoint.GetX() * cotAngel + rightPoint.GetY() - buttomPoint.GetY()) / (tanAngel + cotAngel));
        pMiniVertices[1].SetY((buttomPoint.GetY() * tanAngel + rightPoint.GetY() * cotAngel + rightPoint.GetX() - buttomPoint.GetX()) / (tanAngel + cotAngel));
        pMiniVertices[1].SetZ(0.0);
        pMiniVertices[2].SetX((upperPoint.GetX() * tanAngel + rightPoint.GetX() * cotAngel + rightPoint.GetY() - upperPoint.GetY()) / (tanAngel + cotAngel));
        pMiniVertices[2].SetY((upperPoint.GetY() * tanAngel + rightPoint.GetY() * cotAngel + rightPoint.GetX() - upperPoint.GetX()) / (tanAngel + cotAngel));
        pMiniVertices[2].SetZ(0.0);
        pMiniVertices[3].SetX((upperPoint.GetX() * tanAngel + leftPoint.GetX() * cotAngel + leftPoint.GetY() - upperPoint.GetY()) / (tanAngel + cotAngel));
        pMiniVertices[3].SetY((upperPoint.GetY() * tanAngel + leftPoint.GetY() * cotAngel + leftPoint.GetX() - upperPoint.GetX()) / (tanAngel + cotAngel));
        pMiniVertices[3].SetZ(0.0);
    }

    // if the principal axes are parallel to the real axes of the 2D plane, we get the rectangle by calculate the min and max (both X and Y)values of the point

    if (((((numerator < 1e-6) && (numerator > -1e-6)) || ((denominator < 1e-6) && (denominator > -1e-6))) && (intersections2D.size() >= 4)) || (intersections2D.size() == 3)) {
        float minX = intersections2D[0].GetX();
        float maxX = intersections2D[0].GetX();
        float minY = intersections2D[0].GetY();
        float maxY = intersections2D[0].GetY();
        for (int i = 1; i < intersections2D.size(); ++i) {
            if (minX > intersections2D[i].GetX()) {
                minX = intersections2D[i].GetX();
            }
            if (maxX < intersections2D[i].GetX()) {
                maxX = intersections2D[i].GetX();
            }
            if (minY > intersections2D[i].GetY()) {
                minY = intersections2D[i].GetY();
            }
            if (maxY < intersections2D[i].GetY()) {
                maxY = intersections2D[i].GetY();
            }
        }
        pMiniVertices[0].SetX(minX);
        pMiniVertices[0].SetY(minY);
        pMiniVertices[0].SetZ(0.0);
        pMiniVertices[1].SetX(maxX);
        pMiniVertices[1].SetY(minY);
        pMiniVertices[1].SetZ(0.0);
        pMiniVertices[2].SetX(maxX);
        pMiniVertices[2].SetY(maxY);
        pMiniVertices[2].SetZ(0.0);
        pMiniVertices[3].SetX(minX);
        pMiniVertices[3].SetY(maxY);
        pMiniVertices[3].SetZ(0.0);
    }
    //tranform 2D point to 3D point
    pMiniVertices[0] = volumeToView.Inverse().Transform(pMiniVertices[0]);
    pMiniVertices[1] = volumeToView.Inverse().Transform(pMiniVertices[1]);
    pMiniVertices[2] = volumeToView.Inverse().Transform(pMiniVertices[2]);
    pMiniVertices[3] = volumeToView.Inverse().Transform(pMiniVertices[3]);

    //Make sure pMiniVertices is a rectangle and the order of the points is the same as pVertices


    return TPS_ER_SUCCESS;
}

float SubVolumeHelperAPI::CalculateInGridPercentage(const TpsImage3DEntity& image3D, ROIEntity *voi, RtDosegrid* dosegrid)
{
    if (voi == nullptr || dosegrid == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The pointer of voi or dosegrid is nullptr.";
        return 0.0f;
    }

    auto sdf = voi->GetSDF();
    if (sdf == nullptr){
        TPS_LOG_DEV_ERROR << "The sdf of VOI is empty.";
        return 0.0f;
    }

    Mcsf::Matrix4f matV2P;
    if (!image3D.GetVolumeToPatientMatrix(matV2P)){
        TPS_LOG_DEV_ERROR << "Failed to get volume to patient matrix.";
        return 0.0f;
    }

    int svSize[3], offset[3];
    sdf->GetSize(svSize);
    sdf->GetStart(offset);
    float* buffer = sdf->GetData();
    if (svSize[0] == 0 || svSize[1] == 0 || svSize[2] == 0 || buffer == nullptr)
    {
        return 0.0f;
    }

    //遍历subvolume求得该VOI 的总体积（多少个像素）
    // 每遍历到一个像素属于该VOI，判断该像素是否在dosegrid范围内，并统计之
    int i, j, k, totalVolume = 0;
    int countInGrid = 0;
    int svPlaneCount = svSize[0] * svSize[1];

    Mcsf::Matrix4f matSDF2Grid = TpsArithmeticConverter::ConvertToMatrix4f(sdf->GetPatToSDF().Inverse());
    matSDF2Grid.Prepend(ConvertToMatrix4f(dosegrid->get_grid_to_pat_t()).Inverse());
    int gridSize[3] = {
        dosegrid->get_xcount(), dosegrid->get_ycount(), dosegrid->get_zcount() };

    //如果Volume与Dose Grid方向一致（UTPS做的Plan通常如此），那么更快的统计在dose grid内的VOI点(经测试，大约提升5倍)
    //这里使用openmp在我本机会崩溃，先取消对其的使用，查相关资料，可能是openmp当前bug，更新新的编译器或许可以解决-GML
    auto upper3x3 = matSDF2Grid.GetUpper3x3();
    bool isDiagonal =
        fabs(upper3x3.GetElem(1, 0)) <= 1e-6 &&
        fabs(upper3x3.GetElem(2, 0)) <= 1e-6 &&
        fabs(upper3x3.GetElem(2, 1)) <= 1e-6 &&
        fabs(upper3x3.GetElem(1, 2)) <= 1e-6 &&
        fabs(upper3x3.GetElem(0, 2)) <= 1e-6 &&
        fabs(upper3x3.GetElem(0, 1)) <= 1e-6;
    if (isDiagonal)
    {
        Mcsf::Matrix4f matG2V = matSDF2Grid.Inverse();
        Mcsf::Point3f ptFirst = matG2V.Transform(Mcsf::Point3f(-0.5f, -0.5f, -0.5f));
        Mcsf::Point3f ptLast = matG2V.Transform(Mcsf::Point3f(
            gridSize[0] - 0.5f, gridSize[1] - 0.5f, gridSize[2] - 0.5f));
        float gridBoundaryInVolume[6] = 
        {
            TPS_MIN(ptFirst.GetX(), ptLast.GetX()),
            TPS_MAX(ptFirst.GetX(), ptLast.GetX()),
            TPS_MIN(ptFirst.GetY(), ptLast.GetY()),
            TPS_MAX(ptFirst.GetY(), ptLast.GetY()),
            TPS_MIN(ptFirst.GetZ(), ptLast.GetZ()),
            TPS_MAX(ptFirst.GetZ(), ptLast.GetZ()),
        };
        int idx = 0;
        //#pragma omp parallel for reduction(+ : totalVolume, countInGrid) private(idx)
        for (k = 0; k < svSize[2]; ++k)
        {
            for (j = 0; j < svSize[1]; ++j)
            {
                for (i = 0; i < svSize[0]; ++i)
                {
                    idx = k * svPlaneCount + j * svSize[0] + i;
                    if (buffer[idx] < -1e-6) continue;
                    totalVolume++;

                    //这里判断是在dose grid内部时，将dose grid每个点当作一个体素看，判断
                    //是否在体素范围内，所以各扩0.5

                    if (i >= gridBoundaryInVolume[0] &&
                        i <= gridBoundaryInVolume[1] &&
                        j >= gridBoundaryInVolume[2] &&
                        j <= gridBoundaryInVolume[3] &&
                        k >= gridBoundaryInVolume[4] &&
                        k <= gridBoundaryInVolume[5])
                    {
                        countInGrid++;
                    }
                }
            }
        }
    }
    else
    {
        Mcsf::Point3f point;
        int idx = 0;
        //#pragma omp parallel for reduction(+ : totalVolume, countInGrid) private(point, idx)
        for (k = 0; k < svSize[2]; ++k)
        {
            for (j = 0; j < svSize[1]; ++j)
            {
                for (i = 0; i < svSize[0]; ++i)
                {
                    idx = k * svPlaneCount + j * svSize[0] + i;
                    if (buffer[idx] < -1e-6) continue;
                    // 1. 求得该VOI 的总体积
                    totalVolume++;

                    // 2. 判断该像素是否在dosegrid范围内
                    point.SetX(i);
                    point.SetY(j);
                    point.SetZ(k);
                    point = matSDF2Grid.Transform(point);
                    //这里判断是在dose grid内部时，将dose grid每个点当作一个体素看，判断
                    //是否在体素范围内，所以各扩0.5
                    if (point.GetX() >= -0.5 && point.GetX() <= gridSize[0] - 0.5 &&
                        point.GetY() >= -0.5 && point.GetY() <= gridSize[1] - 0.5 &&
                        point.GetZ() >= -0.5 && point.GetZ() <= gridSize[2] - 0.5)
                    {
                        countInGrid++;
                    }
                }
            }
        }
    }
    return (float)countInGrid / totalVolume;
}

// bool CalcRoiDensity_i(
//     const RtCt2density* pCt2Density, 
//     void* image3DBuffer, int vSize[3], int dataType, 
//     float slope, float intercept, 
//     SubVolume* subVolume, VOIEntity* roi)
// {
//     //density information
//     if (roi->GetIsOverrided())
//     {
//         float density = roi->GetOverrideMassDensity();
//         roi->SetAverageDensity(density);
//         roi->SetMaxDensity(density);
//         roi->SetMinDensity(density);
//         return true;
//     }
// 
//     if (pCt2Density == nullptr)
//     {
//         return true;
//     }
// 
//     int svSize[3], offset[3];
//     subVolume->GetSize(svSize[0], svSize[1], svSize[2]);
//     subVolume->GetOffSet(offset[0], offset[1], offset[2]);
//     char* buffer = subVolume->GetData();
// 
//     double maxSV = 0., minSV = 0.;
//     long long sumSV = 0;
//     int count = 0;
//     int svPlaneCount = svSize[0] * svSize[1];
//     int vPlaneCount = vSize[0] * vSize[1];
//     int idx1, idx2;
//     if (dataType == 0)
//     {
//         unsigned short storedValue;
//         unsigned short* volumeData = (unsigned short*)image3DBuffer;
//         maxSV = 0; minSV = 65535;
// #pragma omp parallel reduction(+ : sumSV, count) private(idx1, idx2)
//         {
//             unsigned short _max = 0, _min = 65535;
// #pragma omp for nowait
//             for (int k = 0; k < svSize[2]; ++k)
//             {
//                 for (int j = 0; j < svSize[1]; ++j)
//                 {
//                     idx1 = svPlaneCount * k + svSize[0] * j;
//                     idx2 = vPlaneCount * (k + offset[2]) + vSize[0] * (j + offset[1]);
//                     for (int i = 0; i < svSize[0]; ++i)
//                     {
//                         if (!buffer[idx1 + i]) continue;
//                         ++count;
//                         storedValue = volumeData[idx2 + i + offset[0]];
//                         sumSV += storedValue;
//                         _max = TPS_MAX(_max, storedValue);
//                         _min = TPS_MIN(_min, storedValue);
//                     }
//                 }
//             }
// #pragma omp critical
//             {
//                 maxSV = TPS_MAX(_max, maxSV);
//                 minSV = TPS_MIN(_min, minSV);
//             }
//         }
//     }
//     else
//     {
//         short storedValue;
//         short* volumeData = (short*)image3DBuffer;
//         maxSV = -32768; minSV = 32767;
// #pragma omp parallel reduction(+ : sumSV, count) private(idx1, idx2)
//         {
//             short _max = -32768, _min = 32767;
// #pragma omp for nowait
//             for (int k = 0; k < svSize[2]; ++k)
//             {
//                 for (int j = 0; j < svSize[1]; ++j)
//                 {
//                     idx1 = svPlaneCount * k + svSize[0] * j;
//                     idx2 = vPlaneCount * (k + offset[2]) + vSize[0] * (j + offset[1]);
//                     for (int i = 0; i < svSize[0]; ++i)
//                     {
//                         if (!buffer[idx1 + i]) continue;
//                         ++count;
//                         storedValue = volumeData[idx2 + i + offset[0]];
//                         sumSV += storedValue;
//                         _max = TPS_MAX(_max, storedValue);
//                         _min = TPS_MIN(_min, storedValue);
//                     }
//                 }
//             }
// #pragma omp critical
//             {
//                 maxSV = TPS_MAX(_max, maxSV);
//                 minSV = TPS_MIN(_min, minSV);
//             }
//         }
//     }
// 
//     //calculate the result information
//     double max_d = Ct2DensityHelper::GetDensityFromCTValue(maxSV * slope + intercept, *pCt2Density);
//     double min_d = Ct2DensityHelper::GetDensityFromCTValue(minSV * slope + intercept, *pCt2Density);
//     double avg_d = Ct2DensityHelper::GetDensityFromCTValue(double(sumSV) / count * slope + intercept, *pCt2Density);
//     roi->SetAverageDensity(avg_d);
//     roi->SetMaxDensity(max_d);
//     roi->SetMinDensity(min_d);
//     return true;
// }

bool CalcRoiVolume_i(ROIEntity* roi)
{
    auto cs = roi->GetContourSet();

    double spacing = cs->pat_to_vol.Inverse().Transform(VEC3(0, 0, 1)).Magnitude();
    double area = 0.f;
    int sign;
    PageContours* pc;
    for (auto it = cs->page_contours.begin(); it != cs->page_contours.end(); ++it)
    {
        pc = it->second;
        for (int i = 0; i < pc->contour_list.size(); ++i)
        {
            if (pc->contour_list[i].VertexCount() < 3) continue;
            //环的面积叠加问题，因为在ROI的同层Contour之间不会相交，
            //所以在这里判断一个contour在另一个contour内部时，只测试contour的一个点--GML
            int inner_c = 0;
            for (int j = 0; j < pc->contour_list.size(); ++j)
            {
                if(j == i) continue;
                if (PolygonUtility::IsIn(pc->contour_list[j].pt._Myfirst, 
                    pc->contour_list[j].VertexCount(), cs->normal, pc->contour_list[i].pt[0]))
                    inner_c++;
            }

            sign = (inner_c&0x1) ? -1 : 1;
            area += sign * PolygonUtility::Area(pc->contour_list[i].pt._Myfirst, 
                pc->contour_list[i].VertexCount(), cs->normal);
        }
    }

    roi->GetInnerRoi()->set_volume(area * spacing);
    return true;
}

bool CalcRoiCenter_i(ROIEntity* roi)
{
    auto sdf = roi->GetSDF();
    int size[3];
    sdf->GetSize(size);
    float* buffer = sdf->GetData();

    //calculate center
    long long sumX = 0, sumY = 0, sumZ = 0;
    int i, j, k, count = 0;
    int plane_size = size[0] * size[1];
#pragma omp parallel for reduction(+ : sumX, sumY, sumZ, count) private(i, j, k)
    for (k = 0; k < size[2]; ++k)
    {
        for (j = 0; j < size[1]; ++j)
        {
            for (i = 0; i < size[0]; ++i)
            {
                if (buffer[k * plane_size + j * size[0] + i] < -1e-6) continue;
                sumX += i; sumY += j; sumZ += k;
                count++;
            }
        }
    }
    PT3 center((double)sumX / count, (double)sumY / count, (double)sumZ / count);
    center = sdf->GetPatToSDF().Inverse().Transform(center);
    roi->GetInnerRoi()->set_centroidx(center.x);
    roi->GetInnerRoi()->set_centroidy(center.y);
    roi->GetInnerRoi()->set_centroidz(center.z);
    return true;
}

// bool CalcRoiDensity_i(
//     const RtCt2density* pCt2Density, 
//     void* image3DBuffer, int vSize[3], int dataType, 
//     float slope, float intercept, ROIEntity* roi)
// {
//     //density information
//     auto innerRoi = roi->GetInnerRoi();
//     if (innerRoi->get_isoverride())
//     {
//         float density = innerRoi->get_overridedenstiy();
//         innerRoi->set_averagedensity(density);
//         innerRoi->set_maxdensity(density);
//         innerRoi->set_mindensity(density);
//         return true;
//     }
// 
//     if (pCt2Density == nullptr)
//     {
//         return true;
//     }
// 
//     //TODO
//     return true;
// }

bool SubVolumeHelperAPI::UpdateVOIStatisticalInformation(
    const RtCt2density* /*pCt2Density*/, 
    const TpsImage3DEntity& /*image3D*/, 
    ROIEntity* roi)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    if (roi == nullptr) return false;
    RtVoi* innerRoi = roi->GetInnerRoi();

    //clear voi info
    innerRoi->set_volume(0.0f);
    innerRoi->disable_field(VOI_CENTROIDX);
    innerRoi->disable_field(VOI_CENTROIDY);
    innerRoi->disable_field(VOI_CENTROIDZ);
    innerRoi->disable_field(VOI_AVERAGEDENSITY);
    innerRoi->disable_field(VOI_MAXDENSITY);
    innerRoi->disable_field(VOI_MINDENSITY);

    if (roi->IsEmpty())
    {
        return true;
    }

    //prepare data
//     RtImage3DHeader* header = nullptr;
//     if (!image3D.GetImage3DHeader(header) || header == nullptr)
//     {
//         TPS_LOG_DEV_ERROR << "There is no header in image 3d.";
//         return false;
//     }
// 
//     float spacing[3] = { header->m_dXSize, header->m_dYSize, header->m_fSliceSpacing };
//     int vSize[3] = { header->m_iXDim, header->m_iYDim, header->m_iSliceCount };
//     int dataType = header->m_iDataType;
//     float slope = header->m_dScaleSlope;
//     float intercept = header->m_dScaleIntercept;
//     char* image3DBuffer = nullptr;
//     if (!image3D.GetImage3DBuffer(image3DBuffer) || image3DBuffer == nullptr)
//     {
//         TPS_LOG_DEV_ERROR << "Failed to get image 3d buffer.";
//         return false;
//     }
//     Mcsf::Matrix4f matV2P;
//     if (!image3D.GetVolumeToPatientMatrix(matV2P))
//     {
//         TPS_LOG_DEV_ERROR << "Failed to get volume to patient matrix.";
//         return false;
//     }
//     if (vSize[0] <= 0 || vSize[1] <= 0 || vSize[2] <= 0){
//         TPS_LOG_DEV_ERROR << "The image 3d is invalid.";
//         return false;
//     }

    if (!CalcRoiVolume_i(roi))
    {
        TPS_LOG_DEV_ERROR<<"Failed to calculate roi volume.";
        return false;
    }

    if (!CalcRoiCenter_i(roi))
    {
        TPS_LOG_DEV_ERROR<<"Failed to calculate roi center.";
        return false;
    }

    //if (!CalcRoiDensity_i(pCt2Density, image3DBuffer, vSize, dataType,
    //    slope, intercept, subVolume, voi))
    //{
    //    TPS_LOG_DEV_ERROR<<"Failed to calculate roi density.";
    //    return false;
    //}
    TEST_PERFORMANCE_END("UpdateVOIStatisticalInformation");
    return true;
}

int SubVolumeHelperAPI::GetSubVolumeCenter(SubVolume& subVolume, Mcsf::Point3f& center) {
    int xSize = 0, ySize = 0, zSize = 0;
    subVolume.GetSize(xSize, ySize, zSize);
    int offset[3];
    subVolume.GetOffSet(offset[0], offset[1], offset[2]);
    char *buffer = subVolume.GetData();
    //volume全空，无中心点
    if (xSize < 0 || ySize < 0 || zSize < 0 || buffer == nullptr){
        TPS_LOG_DEV_ERROR << "The sub-volume is invalid.";
        return TPS_ER_FAILURE;
    }

    float sumX = 0.0, sumY = 0.0, sumZ = 0.0;
    int i, j, k, size = 0, idx;
    int planeCount = xSize * ySize;
#pragma omp parallel for reduction(+ : sumX, sumY, sumZ, size) private(i, j, k, idx)
    for (k = 0; k < zSize; ++k)
    {
        for (j = 0; j < ySize; ++j)
        {
            for (i = 0; i < xSize; ++i)
            {
                idx = i + j*xSize + k*planeCount;
                if (!buffer[idx]) continue;
                sumX += i; sumY += j; sumZ += k;
                size++;
            }
        }
    }

    if (size == 0)
    {
        TPS_LOG_DEV_WARNING << "The sub-volume is empty.";
        return TPS_ER_FAILURE;
    }

    center.SetX(offset[0] + sumX / size);
    center.SetY(offset[1] + sumY / size);
    center.SetZ(offset[2] + sumZ / size);
    return TPS_ER_SUCCESS;
}

void SubVolumeHelperAPI::GetBoundingRectangle(size_t* dim,
    const Mcsf::Point3f* vertices, Mcsf::Point3f* resultVertices,
    Mcsf::Matrix4f& volumeToNormalPlaneTM){

    using namespace Mcsf;
    //get volume to plane view TM
    Vector3f vecX = vertices[1] - vertices[0];
    if (vecX.Magnitude() < 1e-10) {
        TPS_LOG_DEV_ERROR << "Magnitude of vecX is zero.";
        return;
    }
    vecX.Normalize();
    Vector3f vecY = vertices[3] - vertices[0];
    if (vecY.Magnitude() < 1e-10) {
        TPS_LOG_DEV_ERROR << "Magnitude of vecY is zero.";
        return;
    }
    vecY.Normalize();
    Vector3f vecZ = vecY.CrossProduct(vecX);
    Vector3f temp(vertices[0].GetX(), vertices[0].GetY(), vertices[0].GetZ());
    Matrix4f volumeToView(Vector4f(vecX.GetX(), vecY.GetX(), vecZ.GetX(), 0.0),
        Vector4f(vecX.GetY(), vecY.GetY(), vecZ.GetY(), 0.0),
        Vector4f(vecX.GetZ(), vecY.GetZ(), vecZ.GetZ(), 0.0),
        Vector4f(-1 * temp.DotProduct(vecX), -1 * temp.DotProduct(vecY),
        -1 * temp.DotProduct(vecZ), 1.0));

    //transform 8 vertices of volume cube to plane view & get the rect range in view c.s.
    Point3f verticesOfVolume[8] =
    {
        Point3f(-0.5, -0.5, -0.5),
        Point3f(-0.5, -0.5, dim[2] - 0.5),
        Point3f(-0.5, dim[1] - 0.5, -0.5),
        Point3f(-0.5, dim[1] - 0.5, dim[2] - 0.5),
        Point3f(dim[0] - 0.5, -0.5, -0.5),
        Point3f(dim[0] - 0.5, -0.5, dim[2] - 0.5),
        Point3f(dim[0] - 0.5, dim[1] - 0.5, -0.5),
        Point3f(dim[0] - 0.5, dim[1] - 0.5, dim[2] - 0.5)
    };
    Point3f tempInView;
    double minX = 10000000, maxX = -1, minY = 10000000, maxY = -1;
    for (int i = 0; i < 8; ++i){
        tempInView = volumeToView.Transform(verticesOfVolume[i]);
        minX = TPS_MIN(tempInView.GetX(), minX);
        maxX = TPS_MAX(tempInView.GetX(), maxX);
        minY = TPS_MIN(tempInView.GetY(), minY);
        maxY = TPS_MAX(tempInView.GetY(), maxY);
    }
    //transform back to volume coordinate system
    Matrix4f viewToVolume = volumeToView.Inverse();
    resultVertices[0] = viewToVolume.Transform(Point3f(minX, minY, 0));
    resultVertices[1] = viewToVolume.Transform(Point3f(maxX, minY, 0));
    resultVertices[2] = viewToVolume.Transform(Point3f(maxX, maxY, 0));
    resultVertices[3] = viewToVolume.Transform(Point3f(minX, maxY, 0));

    vecX = resultVertices[1] - resultVertices[0];
    vecY = resultVertices[3] - resultVertices[0];

    int width = static_cast<int>(TpsUtility::Ceil(vecX.Magnitude()));
    int height = static_cast<int>(TpsUtility::Ceil(vecY.Magnitude()));
    vecX.Normalize();
    vecY.Normalize();
    //顶点取内包围边界（比如，第一个点为（0，0），而非（-0.5，-0.5））
    resultVertices[0] += 0.5 * vecX + 0.5 * vecY;
    resultVertices[1] = resultVertices[0] + (width - 1) * vecX;
    resultVertices[2] = resultVertices[1] + (height - 1) * vecY;
    resultVertices[3] = resultVertices[0] + (height - 1) * vecY;
    double widthInView = volumeToView.Transform(width * vecX).Magnitude();
    double heightInView = volumeToView.Transform(height * vecY).Magnitude();

    //calculate the transform matrix
    volumeToNormalPlaneTM = volumeToView;
    volumeToNormalPlaneTM.Prepend(MakeTranslate(Vector3f(-minX, -minY, 0)));
    volumeToNormalPlaneTM.Prepend(MakeScale(
        Vector3f(1.0 / widthInView, 1.0 / heightInView, 1.0)));
}

bool SubVolumeHelperAPI::SaveToTrueColorBitmap(const std::string& path,
    unsigned char* buffer,
    const int& height,
    const int& width){

#ifdef WIN32
    //todo: check whether the file is existed

    // calculate the pad size of whole image
    int nStride = (width * 32 + 7) / 8;
    int paddedsize = nStride*height;

    // declare bmp structures 
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER info;

    // andinitialize them to zero
    memset(&bmfh, 0, sizeof(BITMAPFILEHEADER));
    memset(&info, 0, sizeof(BITMAPINFOHEADER));

    // fill the fileheader with data
    bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
    bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits

    // fill the infoheader

    info.biSize = sizeof(BITMAPINFOHEADER);
    info.biWidth = width;
    info.biHeight = height;
    info.biPlanes = 1;			// we only have one bitplane
    info.biBitCount = 32;		// RGB mode is 24 bits
    info.biCompression = BI_RGB;
    info.biSizeImage = 0;		// can be 0 for 24 bit images
    info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
    info.biYPelsPerMeter = 0x0ec4;
    info.biClrUsed = 0;			// we are in RGB mode and have no palette
    info.biClrImportant = 0;    // all colors are important

    // now we open the file to write to
    //#ifdef _DEBUG
    //    HANDLE file = CreateFileW(sPath.c_str() , GENERIC_WRITE, FILE_SHARE_READ,
    //        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    //#else
    std::wstring wsPath(path.begin(), path.end());
    HANDLE file = CreateFile(wsPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    //#endif

    if (file == NULL)
    {
        CloseHandle(file);
        return false;
    }

    // write file header
    unsigned long bwritten;
    if (WriteFile(file, &bmfh, sizeof(BITMAPFILEHEADER), &bwritten, NULL) == false)
    {
        CloseHandle(file);
        return false;
    }
    // write infoheader
    if (WriteFile(file, &info, sizeof(BITMAPINFOHEADER), &bwritten, NULL) == false)
    {
        CloseHandle(file);
        return false;
    }
    // write image data
    if (WriteFile(file, buffer, paddedsize, &bwritten, NULL) == false)
    {
        CloseHandle(file);
        return false;
    }

    // and close file
    CloseHandle(file);
#else
    path;
    buffer;
    width;
    height;
#endif // _WINDOWS

    return true;
}

bool SubVolumeHelperAPI::SaveToTrueColorBitmapFloat(const std::string& sFilepath, const float* buffer,
    const int& height, const int& width){
    if (nullptr == buffer){
        printf("buffer is null.\n");
        return false;
    }

    float fMax = -1, fMin = 10000000;
    int currentNot0 = 0;
    for (int k = 0; k<width * height; ++k) {
        if (buffer[k] > fMax) fMax = buffer[k];
        if (buffer[k] < fMin) fMin = buffer[k];
        if (abs(buffer[k]) > 1e-6) currentNot0++;
    }
    unsigned char* bmpBuffer = new unsigned char[width * height * 4];
    float temp;
    for (unsigned int j = 0; j < width * height; ++j){
        temp = (buffer[j] - fMin) / (fMax - fMin);
        bmpBuffer[4 * j] = temp * 255;
        bmpBuffer[4 * j + 1] = temp * 255;
        bmpBuffer[4 * j + 2] = temp * 255;
        bmpBuffer[4 * j + 3] = 255;
    }
    bool bRet = SaveToTrueColorBitmap(sFilepath, bmpBuffer, height, width);
    delete[]bmpBuffer;
    bmpBuffer = nullptr;

    return bRet;
}

void SubVolumeHelperAPI::SaveToGrayBitmap(const std::string& path,
    unsigned char* buffer,
    const int& height,
    const int& width, int color){

    const int size = width * height;
    color = color < 0 ? 0 : color;
    color = color > 255 ? 255 : color;
    unsigned char* bmpBuffer = new unsigned char[size * 4];
    memset(bmpBuffer, 0, sizeof(unsigned char) * size * 4);
    for (int j = 0; j < size; ++j){
        if (buffer[j]){
            bmpBuffer[4 * j] = color;
            bmpBuffer[4 * j + 1] = color;
            bmpBuffer[4 * j + 2] = color;
            bmpBuffer[4 * j + 3] = 255;
        }
    }

    SaveToTrueColorBitmap(path, bmpBuffer, height, width);

    delete[]bmpBuffer;

}

void SubVolumeHelperAPI::SubVolumeResample(
    const unsigned char *bufferSrc,
    int xSrc, int ySrc, int zSrc,
    float xScale, float yScale, float zScale,
    unsigned char* bufferDes){

    if (nullptr == bufferSrc || nullptr == bufferDes){
        TPS_LOG_DEV_ERROR << "bufferSrc or bufferDes is null";
        return;
    }

    if (fabs(xScale) < EPSINON || fabs(yScale) < EPSINON || fabs(zScale) < EPSINON){
        TPS_LOG_DEV_ERROR << "xScale or yScale or zScale is 0";
        return;
    }

    const int iDim[3] = { xSrc, ySrc, zSrc };
    const int sampleRate[3] = { 1 / xScale, 1 / yScale, 1 / zScale };
    int count(0);
    int k(0), j(0), i(0);

    // sample code ..
    //slice sample
    const int xySize = iDim[0] * iDim[1];
    for (k = 0; k < iDim[2]; k += sampleRate[2]){
        // weight sample
        for (j = 0; j < iDim[1]; j += sampleRate[1]){
            // height sample
            for (i = 0; i < iDim[0]; i += sampleRate[0]){
                if (1 == bufferSrc[i + j*iDim[0] + k*xySize])
                    bufferDes[count++] = 1;
                else
                    bufferDes[count++] = 0;
            }
        }
    }
}

//// RtVoi里面包含的RtContour对象只包含点集的数据文件，需要在这里完成对点集数据文件的读取
//bool SubVolumeHelperAPI::SetupAnastruct(const TpsImage3DEntity* pImage3D,
//    const std::vector<RtContour*>& vecContours, VOIEntity* pVoiEntity)
//{
//    if (pImage3D == nullptr || vecContours.empty() || pVoiEntity == nullptr){
//        TPS_LOG_DEV_ERROR << "Invalid parameters.";
//        return false;
//    }
//
//    unsigned int startImageNum = pImage3D->GetStartSliceNumber();
//
//    TpsSafeLock<VOIEntity> lockAna(pVoiEntity);
//    TPS_ANASTRUCT* ana = pVoiEntity->GetAnastruct();
//    if (ana == nullptr){
//        TPS_LOG_DEV_ERROR << "The ana-struct of voi entity is empty.";
//        return false;
//    }
//
//    //clear old data
//    memset(ana->is_dirty, 0, ana->slice_count* sizeof(char));
//    memset(ana->contour_count, 0, ana->slice_count * sizeof(int));
//    for (int i = 0; i < ana->slice_count; ++i){
//        if (ana->contours[i]) delete[] ana->contours[i];
//    }
//    memset(ana->contours, 0, ana->slice_count * sizeof(TPS_CONTOUR_STC*));
//
//    ana->is_visible = pVoiEntity->GetVoiVisibilty();
//    pVoiEntity->GetColor(ana->color);
//
//    int sliceIndex;
//    for (int i = 0; i < vecContours.size(); ++i){
//        sliceIndex = vecContours[i]->get_sliceindex() - startImageNum;
//        if (sliceIndex < 0 || sliceIndex > ana->slice_count - 1) continue;
//        ana->contour_count[sliceIndex]++;
//    }
//
//    for (int i = 0; i < ana->slice_count; ++i){
//        if (!ana->contour_count[i]) continue;
//        ana->contours[i] = new TPS_CONTOUR_STC[ana->contour_count[i]];
//    }
//
//    size_t vertexCount;
//    TPS_CONTOUR_STC* contour = nullptr;
//    int* currentConcourIndex = new int[ana->slice_count];
//    memset(currentConcourIndex, 0, ana->slice_count * sizeof(int));
//    std::vector<db_Point3f> vecPts;
//    for (int i = 0; i < vecContours.size(); ++i){
//        sliceIndex = vecContours[i]->get_sliceindex() - startImageNum;
//        if (sliceIndex < 0 || sliceIndex > ana->slice_count - 1) continue;
//        contour = ana->contours[sliceIndex] + (currentConcourIndex[sliceIndex]++);
//
//        //rcc: 这里茂亮建议改成直接从db_Point3f拷贝成Mcsf::Point3f
//        //vecPoints = std::move(vecContours[i]->GetPoints());
//
//        //std::string ptFilePath = "";//vecContours[i]->get_pointsfilepath();
//        //vecContours[i]->GetPoints();
//        //        int accuracy = vecContours[i]->get_accuracy();
//        //if (!ReadContourPointFromFile(ptFilePath, accuracy, vecPoints)) {
//        //    TPS_LOG_DEV_ERROR<<"Failed to read contour points from local file["<<ptFilePath<<"].";
//        //    continue;
//        //}
//        vecPts = std::move(vecContours[i]->get_contour_points());
//        vertexCount = vecPts.size();
//        if (vertexCount == 0){
//            TPS_LOG_DEV_WARNING << "Contour size is zero, ignore it.";
//            continue;
//        }
//        contour->pt = new Mcsf::Point3f[vertexCount];
//        for (int i = 0; i < vertexCount; i++){
//            contour->pt[i].SetX(vecPts[i].x);
//            contour->pt[i].SetY(vecPts[i].y);
//            contour->pt[i].SetZ(vecPts[i].z);
//        }
//        contour->vertex_count = vertexCount;
//        contour->slice_number = sliceIndex;
//    }
//    delete[] currentConcourIndex;
//    return true;
//}

bool SubVolumeHelperAPI::SetupContourSet(const TpsImage3DEntity* pImage3D,
    const std::vector<std::vector<PT3>>& vecContours, ROIEntity* pRoiEntity)
{
    if (pImage3D == nullptr || pRoiEntity == nullptr){
        TPS_LOG_DEV_ERROR << "Invalid parameters.";
        return false;
    }

    bool hasValidContour = false;
    PT3 p0, p1, p2;
    VEC3 v0, v1, n;
    for (int i = 0; i < vecContours.size(); ++i)
    {
        const std::vector<PT3>& pt_list = vecContours[i];
        if (pt_list.size() < 3) continue;

        p0 = pt_list[0];
        int j = 2;
        while(j < pt_list.size())
        {
            p1 = pt_list[j-1];
            p2 = pt_list[j];
            v0 = p1 - p0;
            v1 = p2 - p1;
            if (!v0.IsParallel2(v1))
            {
                n = v0.CrossProduct(v1);
                n.Normalize();
                hasValidContour = true;
                break;
            }
            ++j;
        }
        if(hasValidContour) break;
    }
    auto cs = pRoiEntity->GetContourSet();
    cs->Clear();
    if (!hasValidContour)
    {
        return true;
    }

    auto pat_to_vol = pImage3D->GetVolumeToPatient().Inverse();
    auto nv = pat_to_vol.Transform(n);
    nv.Normalize();
    int axis = -1;
    for (int i = 0; i < 3; ++i)
    {
        if (fabs(fabs(Get(nv, i)) - 1.0) <= 1e-6)
        {
            axis = i;
            break;
        }
    }
    if (axis == -1)
    {
        TPS_LOG_DEV_ERROR<<"The contours are oblique.";
        return false;
    }
    cs->axis = axis;

    cs->normal = n;
    cs->pat_to_vol = pat_to_vol;
    switch(axis)
    {
    case 0:
        {
            //以x为slice z，y->x, z->y
            QUAT myQuat(-PI / 2, VEC3(0, 1, 0));
            cs->pat_to_vol.Prepend(myQuat.ToMatrix());
            myQuat = QUAT(-PI / 2, VEC3(0, 0, 1));
            cs->pat_to_vol.Prepend(myQuat.ToMatrix());
        }
        break;
    case 1:
        {
            //以y为slice z，x->y, z->x
            QUAT myQuat(PI / 2, VEC3(1, 0, 0));
            cs->pat_to_vol.Prepend(myQuat.ToMatrix());
            myQuat = QUAT(PI / 2, VEC3(0, 0, 1));
            cs->pat_to_vol.Prepend(myQuat.ToMatrix());
        }
        break;
    }
    auto matCSVol2Pat = cs->pat_to_vol.Inverse();
    cs->spacing[0] = matCSVol2Pat.Transform(VEC3(1, 0, 0)).Magnitude();
    cs->spacing[1] = matCSVol2Pat.Transform(VEC3(0, 1, 0)).Magnitude();
    cs->spacing[2] = matCSVol2Pat.Transform(VEC3(0, 0, 1)).Magnitude();

    PageContours* pc = nullptr;
    for (int i = 0; i < vecContours.size(); ++i)
    {
        if (vecContours[i].size() < 3){
            TPS_LOG_DEV_WARNING << "Contour size is less than 3, ignore it.";
            continue;
        }

        int id = (int)TPS_ROUND(cs->pat_to_vol.Transform(vecContours[i][0]).z);
        auto it = cs->page_contours.find(id);
        if (it == cs->page_contours.end())
        {
            pc = new PageContours();
            pc->slice_number = id;
            cs->page_contours[id] = pc;
        }
        else
        {
            pc = it->second;
        }

        pc->is_dirty = false;
        Contour contour;
        contour.pt = vecContours[i];
        contour.slice_number = id;
        pc->contour_list.push_back(contour);
    }
    return true;
}

bool SubVolumeHelperAPI::UpdateRtContours(TpsImage3DEntity* pImage3D, ROIEntity& roi)
{
    if (nullptr == pImage3D)
    {
        TPS_LOG_DEV_ERROR << "pImage3D is null.";
        return false;
    }

    std::vector<RtContour*> contours;
    auto cs = roi.GetContourSet();
    if (cs->IsEmpty()) return true;
    contours.reserve(cs->GetTotalContoursCount());

    RtImage3DHeader* header;
    pImage3D->GetImage3DHeader(header);
    float thickness[3] = 
    {
        header->m_dXSize, header->m_dYSize, header->m_fSliceSpacing
    };

    MAT4 matCS2V = pImage3D->GetVolumeToPatient().Inverse();
    matCS2V.Append(cs->pat_to_vol.Inverse());
    VEC3 vz = matCS2V.Transform(VEC3(0, 0, 1));
    vz.Normalize();
    int axis = fabs(fabs(vz.x) - 1.0) <= 1e-6 ? 0 : 
              (fabs(fabs(vz.y) - 1.0) <= 1e-6 ? 1 : 
              (fabs(fabs(vz.z) - 1.0) <= 1e-6 ? 2 : -1));
    if (axis == -1)
    {
        TPS_LOG_DEV_ERROR<<"the contour set is oblique.";
        return false;
    }
    
    int start_slice_no = axis == 2 ? pImage3D->GetStartSliceNumber() : 1;

    std::string roiUID = roi.GetUid();
    PageContours* pc;
    std::vector<db_Point3f> vDBPoints;
    for (auto it = cs->page_contours.begin(); it != cs->page_contours.end(); ++it)
    {
        pc = it->second;
        if (pc == nullptr) continue;
        for (int i = 0; i < pc->contour_list.size(); ++i)
        {
            if (pc->contour_list[i].VertexCount() < 3)
                continue;

            RtContour* contour = new RtContour(true);
            contour->set_sliceindex(pc->slice_number + start_slice_no);
            //contour->set_imageuid(pImage3D->GetImageUidByIndex(pc->slice_number));
            contour->set_thickness(thickness[axis]);
            contour->set_voiuid(roiUID);
            contour->set_iscollimatedoutline(false);
            contour->set_accuracy(4);

            vDBPoints.clear();
            vDBPoints.reserve(pc->contour_list[i].VertexCount());
            for (int j = 0; j < pc->contour_list[i].VertexCount(); ++j)
            {
                vDBPoints.push_back(db_Point3f(
                    static_cast<float>(pc->contour_list[i].pt[j].x), 
                    static_cast<float>(pc->contour_list[i].pt[j].y), 
                    static_cast<float>(pc->contour_list[i].pt[j].z)));
            }
            contour->set_contour_points(vDBPoints);
            contours.push_back(contour);
        }
    }

    auto innerRoi = roi.GetInnerRoi();
    auto old_contours = innerRoi->get_contours();
    for (int i = 0; i < old_contours.size(); ++i)
    {
        TPS_DEL_PTR(old_contours[i]);
    }
    innerRoi->set_contours(contours);
    return true;
}

bool SubVolumeHelperAPI::IsInContour(PlaneContour* contour, Mcsf::MedViewer3D::Point2D pt)
{
    int int_count = 0;
    float p1[2], p2[2], intersects[2][FI_LIMIT], distances[FI_LIMIT];
    p1[0] = pt.x;
    p1[1] = pt.y;
    p2[0] = 1000.0;
    p2[1] = pt.y;

    int_count = 0;
    int_count += FindNtrsc(contour, p1, p2, intersects, distances);
    if (int_count & 1) return(true);
    return(false);
}

//void SubVolumeHelperAPI::ExpandSubVolumeToOriginSize(
//    size_t* size, VOIEntity *voi){
//
//    if (size == nullptr || voi == nullptr) {
//        TPS_LOG_DEV_ERROR << "The parameters are invalid.";
//        return;
//    }
//    auto subVolume = voi->GetSubVolume();
//    if (subVolume == nullptr){
//        TPS_LOG_DEV_ERROR << "The sub-volume is empty.";
//    }
//
//    char *buffers = nullptr;
//    char* tempBuffers = nullptr;
//    int sizex = 0, sizey = 0, sizez = 0, newSizex = 0, newSizey = 0, newSizez = 0;
//    int offSetX = 0, offSetY = 0, offSetZ = 0;
//
//    buffers = subVolume->GetData();
//
//    subVolume->GetSize(sizex, sizey, sizez);
//    subVolume->GetOffSet(offSetX, offSetY, offSetZ);
//    if (sizez + offSetZ > size[2]) {
//        sizez = size[2] - offSetZ;
//    }
//    if (sizey + offSetY > size[1]) {
//        sizey = size[1] - offSetY;
//    }
//    if (sizex + offSetX > size[0]) {
//        sizex = size[0] - offSetX;
//    }
//    newSizex = size[0];
//    newSizey = size[1];
//    newSizez = size[2];
//    int planeCount = sizex * sizey;
//    int totalCount = sizex*sizey*sizez;
//    int totalSize = totalCount * sizeof(char);
//
//    if (newSizex != sizex || newSizey != sizey || newSizez != sizez)
//    {
//        tempBuffers = new char[totalCount];
//        memcpy(tempBuffers, buffers, totalSize);
//        subVolume->SetOffSet(0, 0, 0);
//        subVolume->SetSize(newSizex, newSizey, newSizez);
//        int newPlaneCount = newSizex * newSizey;
//        char *newBuffers = subVolume->GetData();
//        memset(newBuffers, 0, newPlaneCount * newSizez * sizeof(char));
//#pragma omp parallel for
//        for (int i = 0; i < sizez; ++i)
//        {
//            for (int j = 0; j < sizey; ++j)
//            {
//                memcpy(newBuffers + newPlaneCount * (i + offSetZ) +
//                    newSizex * (j + offSetY) + offSetX,
//                    tempBuffers + planeCount * i + j * sizex, sizex * sizeof(char));
//            }
//        }
//
//        delete[]tempBuffers; tempBuffers = nullptr;
//        voi->SetIsSubVolumeMinimumBounded(false);
//    }
//}

//void SubVolumeHelperAPI::RefineSubvolume(size_t* dim, VOIEntity *voi) {
//    if (dim == nullptr || voi == nullptr) {
//        TPS_LOG_DEV_ERROR << "The parameters are invalid.";
//        return;
//    }
//    auto subVolume = voi->GetSubVolume();
//    if (subVolume == nullptr){
//        TPS_LOG_DEV_ERROR << "The sub-volume is empty.";
//    }
//
//    if (voi->IsSubVolumeMinimumBounded()){
//        TPS_LOG_DEV_INFO << "The sub-volume is already minimum bounded.";
//        return;
//    }
//
//    char* buffer;
//    int size[3], offset[3], boundary[6]/*x1, x2, y1, y2, z1, z2*/;
//    buffer = subVolume->GetData();
//    subVolume->GetSize(size[0], size[1], size[2]);
//    subVolume->GetOffSet(offset[0], offset[1], offset[2]);
//
//    int stride = size[0] * size[1];
//    char* tmpBuffer = new char[stride];
//    memset(tmpBuffer, 0, stride * sizeof(char));
//    int i = 0;
//    for (; i < size[2] && !memcmp(buffer + i * stride,
//        tmpBuffer, stride * sizeof(char)); ++i);
//        if (i == size[2]){
//        TPS_LOG_DEV_WARNING << "The sub-volume is empty.";
//        subVolume->SetSize(0, 0, 0);
//        subVolume->SetOffSet(0, 0, 0);
//        delete[] tmpBuffer;
//        return;
//        }
//    boundary[4] = i;
//    i = size[2] - 1;
//    for (; i > boundary[4] && !memcmp(buffer + i * stride,
//        tmpBuffer, stride * sizeof(char)); --i);
//        boundary[5] = i;
//
//    boundary[0] = size[0] - 1; boundary[1] = 0;
//    boundary[2] = size[1] - 1; boundary[3] = 0;
//    for (i = boundary[4]; i <= boundary[5]; ++i){
//#pragma omp parallel
//        {
//            int minX = size[0] - 1, maxX = 0;
//            int minY = size[1] - 1, maxY = 0;
//#pragma omp for nowait
//            for (int j = 0; j < size[1]; ++j){
//                if (memcmp(buffer + i * stride + j * size[0],
//                    tmpBuffer, size[0] * sizeof(char))){
//                    minY = TPS_MIN(minY, j);
//                    maxY = TPS_MAX(maxY, j);
//                }
//                for (int k = 0; k < size[0]; ++k){
//                    if (!buffer[i * stride + j * size[0] + k]) continue;
//                    minX = TPS_MIN(minX, k);
//                    maxX = TPS_MAX(maxX, k);
//                }
//            }
//#pragma omp critical
//            {
//                boundary[0] = TPS_MIN(boundary[0], minX);
//                boundary[1] = TPS_MAX(boundary[1], maxX);
//                boundary[2] = TPS_MIN(boundary[2], minY);
//                boundary[3] = TPS_MAX(boundary[3], maxY);
//            }
//        }
//    }
//    delete[] tmpBuffer; tmpBuffer = nullptr;
//
//    if (boundary[0] == 0 && boundary[1] == size[0] - 1 &&
//        boundary[2] == 0 && boundary[3] == size[1] - 1 &&
//        boundary[4] == 0 && boundary[5] == size[2] - 1) return;
//    //for sub-volume definition, we need leave one slice of "0" around the sub-volume 
//    //at each direction if not out of original range
//    int newBoundary[6] = {
//        TPS_CLAMP(boundary[0] - 1, 0, dim[0] - 1),
//        TPS_CLAMP(boundary[1] + 1, 0, dim[0] - 1),
//        TPS_CLAMP(boundary[2] - 1, 0, dim[1] - 1),
//        TPS_CLAMP(boundary[3] + 1, 0, dim[1] - 1),
//        TPS_CLAMP(boundary[4] - 1, 0, dim[2] - 1),
//        TPS_CLAMP(boundary[5] + 1, 0, dim[2] - 1) };
//    int bdryOffset[3] = {
//        boundary[0] - newBoundary[0],
//        boundary[2] - newBoundary[2],
//        boundary[4] - newBoundary[4] };
//
//    tmpBuffer = new char[size[0] * size[1] * size[2]];
//    memcpy(tmpBuffer, buffer, size[0] * size[1] * size[2] * sizeof(char));
//    int newOffset[3] = {
//        offset[0] + newBoundary[0],
//        offset[1] + newBoundary[2],
//        offset[2] + newBoundary[4] };
//    int newSize[3] = {
//        newBoundary[1] - newBoundary[0] + 1,
//        newBoundary[3] - newBoundary[2] + 1,
//        newBoundary[5] - newBoundary[4] + 1 };
//    int newStride = newSize[0] * newSize[1];
//    subVolume->SetOffSet(newOffset[0], newOffset[1], newOffset[2]);
//    subVolume->SetSize(newSize[0], newSize[1], newSize[2]);
//    buffer = subVolume->GetData();
//    memset(buffer, 0, newSize[0] * newSize[1] * newSize[2] * sizeof(char));
//    int copyCount = TPS_MIN(newSize[0], size[0]);
//#pragma omp parallel for
//    for (int j = boundary[4]; j <= boundary[5]; ++j)
//    {
//        for (int k = boundary[2]; k <= boundary[3]; ++k)
//        {
//            memcpy(buffer + newStride * (j - boundary[4] + bdryOffset[2]) +
//                newSize[0] * (k - boundary[2] + bdryOffset[1]) + bdryOffset[0],
//                tmpBuffer + stride * j + size[0] * k + boundary[0], copyCount * sizeof(char));
//        }
//    }
//    delete[] tmpBuffer; tmpBuffer = nullptr;
//    voi->SetIsSubVolumeMinimumBounded(true);
//}

//bool SubVolumeHelperAPI::RefineSubvolumeViaPlane(
//    const size_t* dim, const Mcsf::Point3f* vertices,
//    const unsigned char* buffer, int width, int height, VOIEntity *voi){
//
//    if (dim == nullptr || vertices == nullptr ||
//        buffer == nullptr || width <= 0 || height <= 0 || voi == nullptr) {
//        TPS_LOG_DEV_ERROR << "The parameters are invalid.";
//        return false;
//    }
//    unsigned char* stencil = new unsigned char[width * height];
//    memset(stencil, 0, width * height * sizeof(unsigned char));
//    int cmpRes = memcmp(buffer, stencil, width * height * sizeof(unsigned char));
//    TPS_DEL_ARRAY(stencil);
//    //the mask is all black
//    if (cmpRes == 0) return true;
//
//    auto subVolume = voi->GetSubVolume();
//    if (subVolume == nullptr){
//        TPS_LOG_DEV_ERROR << "The sub-volume is empty.";
//        return false;
//    }
//
//    int size[3];
//    int offset[3];
//    subVolume->GetOffSet(offset[0], offset[1], offset[2]);
//    subVolume->GetSize(size[0], size[1], size[2]);
//    bool isEmptySubVolume = size[0] == 0 || size[1] == 0 || size[2] == 0;
//
//    Mcsf::Vector3f vecX = (vertices[1] - vertices[0]) / (width - 1);
//    Mcsf::Vector3f vecY = (vertices[3] - vertices[0]) / (height - 1);
//    float boundary[6] = { dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1, 0 };
//    Mcsf::Point3f point;
//    for (int j = 0; j < height; ++j){
//        for (int i = 0; i < width; ++i){
//            if (!buffer[j * width + i]) continue;
//            point = vertices[0] + i * vecX + j * vecY;
//            boundary[0] = TPS_MIN(boundary[0], point.GetX());
//            boundary[1] = TPS_MAX(boundary[1], point.GetX());
//            boundary[2] = TPS_MIN(boundary[2], point.GetY());
//            boundary[3] = TPS_MAX(boundary[3], point.GetY());
//            boundary[4] = TPS_MIN(boundary[4], point.GetZ());
//            boundary[5] = TPS_MAX(boundary[5], point.GetZ());
//        }
//    }
//    for (int i = 0; i < 6; ++i)
//    {
//        boundary[i] = TPS_CLAMP(boundary[i], 0, dim[i / 2] - 1);
//    }
//
//    if (!isEmptySubVolume)
//    {
//        float oldBoundary[6] = {
//            offset[0], offset[0] + size[0] - 1,
//            offset[1], offset[1] + size[1] - 1,
//            offset[2], offset[2] + size[2] - 1 };
//        boundary[0] = TPS_MIN(boundary[0], oldBoundary[0]);
//        boundary[1] = TPS_MAX(boundary[1], oldBoundary[1]);
//        boundary[2] = TPS_MIN(boundary[2], oldBoundary[2]);
//        boundary[3] = TPS_MAX(boundary[3], oldBoundary[3]);
//        boundary[4] = TPS_MIN(boundary[4], oldBoundary[4]);
//        boundary[5] = TPS_MAX(boundary[5], oldBoundary[5]);
//
//        bool needRefine = false;
//        for (int i = 0; i < 6; ++i){
//            if (fabs(boundary[i] - oldBoundary[i]) <= 1e-6) continue;
//            needRefine = true;
//            break;
//        }
//        if (!needRefine) return true;
//    }
//
//    int currentBounary[6] = {
//        floorf(boundary[0]), ceilf(boundary[1]),
//        floorf(boundary[2]), ceilf(boundary[3]),
//        floorf(boundary[4]), ceilf(boundary[5]) };
//    int newSize[3] = {
//        currentBounary[1] - currentBounary[0] + 1,
//        currentBounary[3] - currentBounary[2] + 1,
//        currentBounary[5] - currentBounary[4] + 1 };
//    if (isEmptySubVolume)
//    {
//        subVolume->SetOffSet(currentBounary[0], currentBounary[2], currentBounary[4]);
//        subVolume->SetSize(newSize[0], newSize[1], newSize[2]);
//        auto svBuffer = subVolume->GetData();
//        memset(svBuffer, 0, newSize[0] * newSize[1] * newSize[2] * sizeof(char));
//    }
//    else
//    {
//        int planeCount = size[0] * size[1];
//        char* svBuffer = subVolume->GetData();
//        char* tempBuffer = new char[planeCount * size[2]];
//        memcpy(tempBuffer, svBuffer, planeCount * size[2] * sizeof(char));
//        subVolume->SetOffSet(currentBounary[0], currentBounary[2], currentBounary[4]);
//        offset[0] -= currentBounary[0];
//        offset[1] -= currentBounary[2];
//        offset[2] -= currentBounary[4];
//        subVolume->SetSize(newSize[0], newSize[1], newSize[2]);
//        int newPlaneCount = newSize[0] * newSize[1];
//        svBuffer = subVolume->GetData();
//        memset(svBuffer, 0, newPlaneCount * newSize[2] * sizeof(char));
//#pragma omp parallel for
//        for (int i = 0; i < size[2]; ++i)
//        {
//            for (int j = 0; j < size[1]; ++j)
//            {
//                memcpy(svBuffer + newPlaneCount * (i + offset[2]) +
//                    newSize[0] * (j + offset[1]) + offset[0],
//                    tempBuffer + planeCount * i + j * size[0], size[0] * sizeof(char));
//            }
//        }
//        delete[]tempBuffer; tempBuffer = nullptr;
//    }
//    voi->SetIsSubVolumeMinimumBounded(false);
//    return true;
//}

bool SubVolumeHelperAPI::IsMaskIntersect(unsigned char* oldMask, unsigned char* newMask, int wh){
    for (int i = 0; i < wh; ++i){
        if (oldMask[i] & newMask[i]){
            return true;
        }
    }
    return false;
}

bool SubVolumeHelperAPI::SmoothVOI(ROIEntity *voi, const TpsImage3DEntity& pImage3D)
{
    //////////////////////////////////////////////////////////////////////////
    //算法修改尝试：尝试使用3×3×3的高斯滤波(0.5为阈值，进化缓慢，平滑效果不明显)，
    //以及形态学先开后闭的平滑方法（第一下变化巨大，再次使用无变化），效果均不达预期，在此做记录--GML
    //
    plunc::ANASTRUCT ana;
    SubVolumeHelperAPI::VOIToPluncAnastruct(voi, &ana);

    RtImage3DHeader *header = nullptr;
    if (!pImage3D.GetImage3DHeader(header)) {
        TPS_LOG_DEV_ERROR << "Failed to get image header.";
        return false;
    }

    float xSize = header->m_dXSize;
    float ySize = header->m_dYSize;
    float zSize = header->m_fSliceSpacing;
    plunc::smooth_ana_cb(&ana, xSize, ySize, zSize);
    SubVolumeHelperAPI::PluncAnastructToVOI(&ana, pImage3D, voi);

    //delete ana
    for (int i = 0; i < ana.contour_count; ++i){
        free(ana.contours[i].x);
        free(ana.contours[i].y);
    }
    free(ana.contours);
    return true;
}

bool SubVolumeHelperAPI::InterpolateVOI(ROIEntity* /*roi*/)
{
//     if (roi == nullptr)
//     {
//         TPS_LOG_DEV_ERROR << "The voi is null.";
//         return false;
//     }
// 
//     auto sdf = roi->GetSDF();
//     auto pat2sdf = sdf->GetPatToSDF();
// 
// 
// 
//     std::vector<bool> interFlags = roi->GetInnerRoi()->get_interpolate();
// 
//     auto subVolume = voiEntity->GetSubVolume();
//     if (subVolume == nullptr)
//     {
//         TPS_LOG_DEV_ERROR << "The sub-volume of VOI: " << voiEntity->GetVoiUid() << " does not exist.";
//         return false;
//     }
// 
//     int ox = 0, oy = 0, oz = 0;
//     subVolume->GetOffSet(ox, oy, oz);
//     int sx = 0, sy = 0, sz = 0;
//     subVolume->GetSize(sx, sy, sz);
//     if ((sx == 0 || sy == 0 || sz == 0) && subVolume->GetData() == nullptr)
//     {
//         return true;
//     }
//     if (sx < 0 || sy < 0 || sz < 0 || subVolume->GetData() == nullptr)
//     {
//         return false;
//     }
// 
//     int svRange[2] = {oz, oz + sz - 1};
//     int startSlice;
//     int endSlice = svRange[0];
//     std::vector<bool> isInterpolate = voiEntity->GetInterpolationFlag();
// 
//     while(endSlice <= svRange[1])
//     {
//         startSlice = endSlice;
// 
//         endSlice = startSlice + 1;
//         while (endSlice <= svRange[1] && isInterpolate[endSlice])
//         {
//             endSlice++;
//         }
//         if (endSlice > svRange[1])
//         {
//             return true;
//         }
//         
//         voiEntity->SetSlicesDirtyForAna(startSlice, endSlice);
//         Mcsf::AutoInterpolateBetweenTwoSlicesForIA(startSlice - oz, endSlice - oz, subVolume->GetData(), sx, sy);
//     }

    return true;
}

Mcsf::Matrix4f SubVolumeHelperAPI::ConvertToMatrix4f(
    const float arrayMatrix[16]){
    Mcsf::Matrix4f matrix;
    for (int n = 0; n < 4; n++) {
        for (int m = 0; m < 4; m++) {
            matrix.SetElem(n, m, arrayMatrix[n * 4 + m]);
        }
    }
    return matrix;
}

void SubVolumeHelperAPI::VOIToPluncAnastruct(const ROIEntity* voi, plunc::ANASTRUCT* ana)
{
    if (nullptr == voi)
    {
        TPS_LOG_DEV_ERROR << "Invalid parameters.";
        return;
    }
    auto cs = voi->GetContourSet();
    if (cs == nullptr)
    {
        TPS_LOG_DEV_ERROR << "There is no ana-struct in voi entity.";
        return;
    }

    Mcsf::Matrix4f matVol2Patient = TpsArithmeticConverter::ConvertToMatrix4f(cs->pat_to_vol);
    matVol2Patient = matVol2Patient.Inverse();
    float thickness = matVol2Patient.Transform(Mcsf::Vector3f(0.f, 0.f, 1.f)).Magnitude();

    ana->contour_count = 0;
    ana->min.x = std::numeric_limits<float>::max();
    ana->min.y = std::numeric_limits<float>::max();
    ana->min.z = std::numeric_limits<float>::max();
    ana->max.x = std::numeric_limits<float>::lowest();
    ana->max.y = std::numeric_limits<float>::lowest();
    ana->max.z = std::numeric_limits<float>::lowest();

    for (auto it = cs->page_contours.begin(); it != cs->page_contours.end(); ++it)
    {
        ana->contour_count += it->second->contour_list.size();
    }

    ana->contours = new plunc::CONTOUR_STC[ana->contour_count];
    int counter = 0;
    for (auto it = cs->page_contours.begin(); it != cs->page_contours.end(); ++it)
    {
        for (int i = 0; i < it->second->contour_list.size(); ++i)
        {
            plunc::CONTOUR_STC& contour = ana->contours[counter++];
            Contour& tps_contour = it->second->contour_list[i];
            if (tps_contour.pt.size() == 0) continue;

            contour.slice_number = it->second->slice_number;
            contour.vertex_count = tps_contour.pt.size();
            contour.x = new float[contour.vertex_count];
            contour.y = new float[contour.vertex_count];
            contour.min.x = std::numeric_limits<float>::max();
            contour.min.y = std::numeric_limits<float>::max();
            contour.min.z = std::numeric_limits<float>::max();
            contour.max.x = std::numeric_limits<float>::lowest();
            contour.max.y = std::numeric_limits<float>::lowest();
            contour.max.z = std::numeric_limits<float>::lowest();
            for (int j = 0; j < contour.vertex_count; ++j)
            {
                contour.x[j] = tps_contour.pt[j].x;
                contour.y[j] = tps_contour.pt[j].y;
                contour.z = tps_contour.pt[j].z;

                contour.min.x = TPS_MIN(contour.min.x, contour.x[j]);
                contour.min.y = TPS_MIN(contour.min.y, contour.y[j]);
                contour.min.z = TPS_MIN(contour.min.z, contour.z - thickness * 0.5);
                contour.max.x = TPS_MAX(contour.max.x, contour.x[j]);
                contour.max.y = TPS_MAX(contour.max.y, contour.y[j]);
                contour.max.z = TPS_MAX(contour.max.z, contour.z + thickness * 0.5);
            }

            ana->min.x = TPS_MIN(ana->min.x, contour.min.x);
            ana->min.y = TPS_MIN(ana->min.y, contour.min.y);
            ana->min.z = TPS_MIN(ana->min.z, contour.min.z);
            ana->max.x = TPS_MAX(ana->max.x, contour.max.x);
            ana->max.y = TPS_MAX(ana->max.y, contour.max.y);
            ana->max.z = TPS_MAX(ana->max.z, contour.max.z);
        }
    }
}

void SubVolumeHelperAPI::PluncAnastructToVOI(plunc::ANASTRUCT* ana, const TpsImage3DEntity& pImage3D, ROIEntity* voi)
{
    std::vector<std::vector<PT3>> contours;
    contours.reserve(ana->contour_count);
    for (int i = 0; i < ana->contour_count; ++i)
    {
        std::vector<PT3> ctr;
        ctr.reserve(ana->contours[i].vertex_count);
        for (int j = 0; j < ana->contours[i].vertex_count; ++j)
        {
            ctr.push_back(PT3(
                (double)ana->contours[i].x[j], 
                (double)ana->contours[i].y[j], 
                (double)ana->contours[i].z));
        }
        contours.push_back(ctr);
    }

    SubVolumeHelperAPI::SetupContourSet(&pImage3D, contours, voi);
    ROIShapeConverter::ContourSetToSDF(*voi->GetContourSet(), *voi->GetSDF(), false);
}
TPS_END_NAMESPACE

