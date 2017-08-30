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

#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_watercarriageparsefile.h"

#include <io.h>

TPS_BEGIN_NAMESPACE

    AcsFileRead::AcsFileRead(){
        wedge[0] = '-';
        wedge[1] = '1';
}

AcsFileRead::AcsFileRead(std::vector<string>& pathfileName, std::vector<CurveProfile>*& curveProfile):curveCount(0),groupCount(0){
    //for (auto it = pathfileName.begin(); it != pathfileName.end(); it++){
    //     GetFiles(*it,files);
    //}

    for (auto file = pathfileName.begin(); file != pathfileName.end(); ++file){
        FileParse(*file);
    }

    curveProfile = &mCurveProfile;
}

bool AcsFileRead::FileParse(string fileName){
    std::cout<<"start parse file!!!\n"; 
    ifstream file(fileName.c_str());
    if (file.fail()){
        //throw                //don't open file fail.
        std::cout<<"fail parse file!!!\n"; 
    }

    while ( ! file.eof()){
        //line++;
        file.getline(s,4096);
        j = 0, i = 0;
        while (s[i] =='\t'){
            i++;                     //忽略前导空格。
        }
        v[0] = '\0';

        if (s[i] == '%'){
            int k;
            for (k =0;k< 3;k++){
                v[j++] = s[++i];
            }
            v[j++] = '\0';

            //int p =0;             //计数空格的次数。
            string fsz(v);
            if (fsz == "FSZ"){
                StringParse(s,i,fszX,fszY);
            }

            if (fsz == "BMT"){
                StringParse(s,i,pho,eng);
            }
            if (fsz == "SSD"){
                StringParse(s,i,ssd);
            }
            if (fsz == "WEG"){
                StringParse(s,i,wedge);
            }
            if (fsz == "STS"){
                StringParse(s,i,X,Y,Z);
            }
            line++;
        }
        if (s[i] == '='){
            aGroupCount++;
            StringParse(s,i,X1,Y1,Z1,dose);
            line++;
            CoordinateData doseData;
            doseData.x = atof(X1);
            doseData.y = atof(Y1);
            doseData.z = atof(Z1);
            doseData.dose = atof(dose);
            mDoseList.push_back(doseData);

        }
        if (s[i] == '#'){
            line++;
        }

        if (s[i] == ':'){
            int r;
            for (r = 0; r < 3 ; r++){
                start[r] = s[++i];
            }
            start[r++] = '\0';
            string sta(start);
            if (sta == "MSR"){
                cout<<"Start parse file data";
            }
            if (sta == "EOF"){
                cout<<"End parse file data";
            }
            if (sta == "EOM"){

                CurveProfile curve;
                curve.mFszx = atof(fszX);
                curve.mFszy = atof(fszY);
                string ph(pho);
                curve.mPho = ph;
                curve.mEng = atof(eng);
                curve.mSsd = atof(ssd);
                curve.mWedge = atof(wedge);
                curve.mX = atof(X);                  //把mm转化为cm
                curve.mY = atof(Y);
                curve.mZ = atof(Z);

                mCurveProfile.push_back(curve);
                mCurveProfile[curveCount].vWedgeInfoList =mDoseList;

                curveCount++;
                groupCount++;
                vInt.push_back(aGroupCount);
                mDoseList.clear();
            }
            line++;
        }
    }
    file.close();
    std::cout<<"end parse file!!!\n"; 

    return true;
}

bool AcsFileRead::GetCurveProfile(std::vector<CurveProfile>*& vCurveProfile){
    vCurveProfile = &mCurveProfile;

    return true;
}

bool AcsFileRead::StringParse(char s[8], int i,char x[8]){
    i++;
    while (s[i] == '\t'|| isspace(s[i])){
        i++;
    }
    //int b = 0;
    for (int b = 0; s[i] !='\t'; b++){
        x[b] = s[i];
        i++;
        if (s[i] == NULL || s[i]==0){
            b++;
            x[b++] = '\0';
            break;
        }
    }
    return true;
}

bool AcsFileRead::StringParse(char s[8],int i,char x[8],char y[8]){
    i++;
    while (s[i] == '\t'|| isspace(s[i])){
        i++;
    }
    int m;
    for ( m = 0; s[i] !='\t'; m++){
        x[m] = s[i];
        i++;
    }
    x[m] = '\0';
    while (s[i] == '\t'|| isspace(s[i])){
        i++;
    }
    //int mm = 0;
    for (int mm = 0; s[i] !='\t'; mm++){
        y[mm] = s[i];
        i++;
        if (s[i] == NULL || s[i]==0){
            mm++;
            y[mm++] = '\0';
            break;
        }
    }
    return true;
}

bool AcsFileRead::StringParse(char s[8],int i,char x[8],char y[8], char z[8]){
    i++;
    while (s[i] == '\t'|| isspace(s[i])){
        i++;
    }
    int m=0;
    for (m = 0; s[i] !='\t'; m++){
        x[m] = s[i];
        i++;
    }
    x[m] = '\0';
    while (s[i] == '\t'|| isspace(s[i])){
        i++;
    }
    int mm = 0;
    while(s[i] !='\t'||!isspace(s[i]) ){
        y[mm++] = s[i];
        i++;
    }
    y[mm++] = '\0';

    while (s[i] == '\t'|| isspace(s[i])){
        i++;
    }

    for ( int mmm = 0; (s[i] !='\t'); mmm++){
        z[mmm] = s[i];

        if (s[i] == NULL || s[i]==0|| s[i]=='#'||isspace(s[i])){
            z[mmm++]= '\0';
            break;
        }
        i++;
    }
    return true;

}

bool AcsFileRead::StringParse(char s[8],int i,char x[8],char y[8], char z[8], char dose[8]){
    i++;
    while (s[i] == '\t'|| isspace(s[i])){
        i++;
    }

    int m = 0;
    for (m = 0; s[i] !='\t'; m++){
        x[m] = s[i];
        i++;
    }
    x[m] = '\0';

    while (s[i] == '\t'|| isspace(s[i])){
        i++;
    }

    int mm = 0;
    while(s[i] !='\t'||!isspace(s[i]) ){
        y[mm++] = s[i];
        i++;
    }
    y[mm++] = '\0';

    while (s[i] == '\t'|| isspace(s[i])){
        i++;
    }
    int mmm = 0;
    while(s[i] !='\t'||!isspace(s[i]) ){
        z[mmm++] = s[i];
        i++;
    }
    z[mmm++] = '\0';
    while (s[i] == '\t'|| isspace(s[i])){
        i++;
    }

    int mmmm= 0;
    for ( mmmm = 0; (s[i] !='\t'); mmmm++){
        dose[mmmm] = s[i];

        if (s[i] == NULL || s[i]==0|| s[i]=='#'||isspace(s[i]))
        {
            dose[mmmm++]= '\0';
            break;
        }
        i++;
    }

    return true;
}
bool AcsFileRead::IsspaceOrTab(char s[8],int i){

    while (s[i] == '\t'|| isspace(s[i])){
        i++;
    }

    return true;
}

bool AcsFileRead::FromStringExtractData(string& a,int& b){
    int len = a.length();
    int i = 0 ;
    char buf[10];
    while (a[len] != '\0'){
        if ( isdigit(a[i])){
            buf[i] = a[i];
        }
        ++i;
    }

    buf[i] = '\0';

    //string temp(buf);
    b = atoi(buf);

    return true;
}

void AcsFileRead::GetFiles(std::string path, vector<string>& files ){

    //文件句柄
    long   hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    string p;

    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1){
        do{
            //如果是目录,迭代之
            //如果不是,加入列表
            if((fileinfo.attrib &  _A_SUBDIR)){
                if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
                    GetFiles( p.assign(path).append("\\").append(fileinfo.name), files );
            }
            else{
                files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
            }
        }while(_findnext(hFile, &fileinfo)  == 0);
        _findclose(hFile);
    }
}

TPS_END_NAMESPACE
