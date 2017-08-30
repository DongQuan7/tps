//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    ms_fw_watecarriageparsefile.h
///  \brief   
///
///  \version 1.0
///  \date    01.09.2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef MS_WATECARRIAGEFILE_BASE_H
#define MS_WATECARRIAGEFILE_BASE_H

#include "tps_da_defs.h"

using namespace std;

TPS_BEGIN_NAMESPACE

struct TPS_DA_DECLSPEC CoordinateData
{
    float x;
    float y;
    float z;
    float dose;
};

struct TPS_DA_DECLSPEC CurveProfile
{
    bool isDisplay;
    float mFszx;
    float mFszy;
    std::string mPho;
    float mEng;
    float mSsd;
    float mWedge;
    float mX;
    float mY;
    float mZ;
    float mDose;
    std::vector<CoordinateData> vWedgeInfoList;

    CurveProfile()
    {
        isDisplay = true;
        mFszx = 0;
        mFszy = 0;
        mPho = "";
        mEng = 0;
        mSsd = 0;
        mWedge = -1;
        mX = 0;
        mY = 0;
        mZ = 0;
        mDose = 0;
    }
};

class TPS_DA_DECLSPEC AcsFileRead
{
public:

    AcsFileRead();
    //AcsFileRead(std::string fileName);
    AcsFileRead(std::vector<string>& pathfileName, std::vector<CurveProfile>*& mCurveProfile);
    bool GetCurveProfile(std::vector<CurveProfile>*& mCurveProfile);
    bool FromStringExtractData(string& a,int& b);
    ~AcsFileRead(){};

private:
    bool StringParse(char s[8],int i,char x[8]);
    bool StringParse(char s[8],int i,char x[8],char y[8]);
    bool StringParse(char s[8],int i, char x[8],char y[8], char z[8]);
    bool StringParse(char s[8],int i, char x[8],char y[8], char z[8], char dose[8]);
    bool IsspaceOrTab(char s[8],int i);
    void GetFiles( std::string path, std::vector<string>& files );
    bool FileParse(string fileName);


private:
    vector<CoordinateData> mDoseList;
    int groupCount;
    int aGroupCount;
    char fszX[4],fszY[4], pho[4], eng[4],ssd[5],wedge[6],X1[8],Y1[8],Z1[8],X[8],Y[8],Z[8],dose[4],start[4];
    char s[4096]; //记录字符
    char v[2048]; //记录标识

    vector<CurveProfile> mCurveProfile;
    vector<int> vInt;
    int i,j;      //计数器
    int line; //行数
    vector<std::string> files;
    int curveCount ;

};

TPS_END_NAMESPACE

#endif 
