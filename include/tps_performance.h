////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_performance.h
/// 
///  \brief tps performance common macro define
/// 
///  \version 1.0
/// 
///  \date    2013/11/19
////////////////////////////////////////////////////////////////

#ifndef TPS_PERFORMANCE_H_
#define TPS_PERFORMANCE_H_

#ifdef TEST_PERFORMANCE_ON
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h> 

#define TEST_PERFORMANCE_INIT \
    LARGE_INTEGER BeginTime; \
    LARGE_INTEGER EndTime; \
    LARGE_INTEGER Frequency; \
    double dTime(0); \
    std::string sDes("");\
    QueryPerformanceFrequency(&Frequency);

#define TEST_PERFORMANCE_BEGIN \
    QueryPerformanceCounter(&BeginTime);
    
#define TEST_PERFORMANCE_END(descript) \
    QueryPerformanceCounter(&EndTime); \
    dTime = (double)(EndTime.QuadPart - BeginTime.QuadPart)/Frequency.QuadPart; \
    sDes = (std::string)descript;\
    printf("\n[%.2f] ms \tFOR [%s]", dTime*1000,sDes.c_str());
#else
    #define TEST_PERFORMANCE_INIT
    #define TEST_PERFORMANCE_BEGIN
    #define TEST_PERFORMANCE_END(descript)
#endif

#endif
