////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_mascot_graphicobject.cpp
/// 
///  \brief class MascotGraphicObejct implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/09/05
////////////////////////////////////////////////////////////////

//#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mascot_graphicobject.h"
#include "gl\glew.h"            // for GL_MULTISAMPLE
#include <gl\gl.h>

#include "tps_logger.h"
#include "Common\McsfSystemEnvironmentConfig\mcsf_systemenvironment_factory.h"

TPS_BEGIN_NAMESPACE

//std::shared_ptr<MascotGraphicObject::MascotData>  MascotGraphicObject::m_spData = nullptr;
MascotGraphicObject::MascotData*  MascotGraphicObject::m_spData = nullptr;

MascotGraphicObject::MascotGraphicObject():
    m_bVisible(true) {

}

MascotGraphicObject::~MascotGraphicObject() {

}

bool MascotGraphicObject::Initialize() {
    if (m_spData == nullptr)
    {
        Mcsf::ISystemEnvironmentConfig* systemEnviromentConfig = 
            Mcsf::ConfigSystemEnvironmentFactory::Instance()->GetSystemEnvironment();
        if (systemEnviromentConfig == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsDisplayDelegate][Initialize]: "
                <<"Failed to get system environment.";
            return false;
        }

        std::string strTpsConfigPath = "RtTpsConfigPath";
        std::string cfgPath = 
            systemEnviromentConfig->GetApplicationPath(strTpsConfigPath);
        
        std::string strObjFile = cfgPath + "..\\mascot.obj";
        std::string strMtlFile = cfgPath + "..\\mascot.mtl";

        //m_spData.reset(new MascotData());
        m_spData = new MascotData();
        m_spData->m_nDisplayList = -1;
        if (!LoadOBJ(strObjFile.c_str(), //"..\\appdata\\rt\\tps\\mascot.obj", 
                     m_spData->m_vecVertice, m_spData->m_vecVertexNormals, m_spData->m_vecGroups, 
                     m_spData->m_ptLeftBottom, m_spData->m_ptTopRight, 
                     m_spData->m_matModel2World))
        {
            TPS_LOG_DEV_ERROR << "MascotGraphicObject: Initialize() Failed to parse mascotobj:" << strObjFile;
            TPS_PRINTF_DEBUG("Failed to parse OBJ file.\n");
            //this->m_bVisible = false;
            delete m_spData;
            m_spData = nullptr;
            return false;
        }
        
        if (!LoadMaterial(strMtlFile.c_str(), //"..\\appdata\\rt\\tps\\mascot.mtl", 
                          m_spData->m_vecMaterials))
        {
            TPS_PRINTF_DEBUG("Failed to parse MTL file.\n");
            TPS_LOG_DEV_ERROR << "MascotGraphicObject: Initialize() Failed to parse mascot.mtl:" << strMtlFile;
            delete m_spData;
            m_spData = nullptr;
            return false;
        }
    }
    return true;
}

bool MascotGraphicObject::Finalize() {
    return true;
}


bool MascotGraphicObject::LoadMaterial(const char *path, 
                                       std::vector<MaterialStc>& outMaterials)
{
    bool bResult = false;

    FILE * file = nullptr;
    fopen_s(&file, path, "r"); 
    if( file == NULL )
    { 
        printf("Failed to open the file %s ! Please check the path\n", path); 
        return false; 
    }

    MaterialStc material;
    bool bInsertMaterial = false;

    ColorStc color;
    //float fValue;
    char lineHeader[512]; 
    char stupidBuffer[1024]; 
    char name[128];

    while( 1 )
    {
        // read the first word of the line 
        int res = fscanf_s(file, "%s", lineHeader, _countof(lineHeader)); 
        // EOF = End Of File. Quit the loop.
        if (res == EOF) 
        {
            bResult = true;
            break; 
        }
        // else : parse lineHeader
        if (strcmp(lineHeader, "newmtl" ) == 0 )
        { 
            if (bInsertMaterial)
            {
                outMaterials.push_back(material);
            }

            if (fscanf_s(file, "%s\n", &name, _countof(name)) == 1)
            {
                material.strMaterialName = name;
                bInsertMaterial = true;
            }
            else
            {
                printf("Failed to parse new material! material index: %d\n", outMaterials.size());
                break;
            }
        }
        else if (strcmp(lineHeader, "Ka" ) == 0 )
        { 
            if(fscanf_s(file, "%f %f %f\n", &color.r, &color.g, &color.b) == 3)
            {
                material.Ka = color;
            }
            else
            {
                printf("Failed to parse Ka! material index: %d\n", outMaterials.size());
                break;
            }
        }
        else if (strcmp(lineHeader, "Kd" ) == 0 )
        { 
            if(fscanf_s(file, "%f %f %f\n", &color.r, &color.g, &color.b) == 3)
            {
                material.Kd = color;
            }
            else
            {
                printf("Failed to parse Kd! material index: %d\n", outMaterials.size());
                break;
            }
        }
        else if (strcmp(lineHeader, "Ks" ) == 0 )
        { 
            if(fscanf_s(file, "%f %f %f\n", &color.r, &color.g, &color.b) == 3)
            {
                material.Ks = color;
            }
            else
            {
                printf("Failed to parse Ks! material index: %d\n", outMaterials.size());
                break;
            }
        }
        else if (strcmp(lineHeader, "Ke" ) == 0 )
        { 
            if(fscanf_s(file, "%f %f %f\n", &color.r, &color.g, &color.b) == 3)
            {
                material.Ke = color;
            }
            else
            {
                printf("Failed to parse Ke! material index: %d\n", outMaterials.size());
                break;
            }
        }
        else
        { 
            // Probably a comment, eat up the rest of the line   
            fgets(stupidBuffer, 1024, file); 
        }
    }
    fclose(file);
    if (bInsertMaterial)
    {
        outMaterials.push_back(material);
    }
    return bResult;
}
bool MascotGraphicObject::LoadOBJ(const char *path, 
                 std::vector<PointStc>& outVertice, 
                 std::vector<NormalStc>& outNormals, 
                 std::vector<GroupStc>& outGroups,
                 PointStc& minPoint,
                 PointStc& maxPoint,
                 float matModel2World[4][4])
{

    bool bResult = false;
 //   printf("Loading OBJ file %s...\n", path);

    FILE * file = nullptr;
    fopen_s(&file,path, "r"); 
    if( file == NULL )
    { 
        printf("Failed to open the file %s ! Please check the path\n", path); 
        return false; 
    }
    
    PointStc vertex;
    NormalStc normal;
    GroupStc group;
    bool bInsertGroup = false;
    int nFacesInGroup = 0;
    FaceVertexStc vertexIndex;
    FaceTextureStc vertexTextureIndex;
    FaceNormalStc normalIndex;
    minPoint.x = 10000.0;
    minPoint.y = 10000.0;
    minPoint.z = 10000.0;

    maxPoint.x = -10000.0;
    maxPoint.y = -10000.0;
    maxPoint.z = -10000.0;

    char lineHeader[512]; 
    char stupidBuffer[1024]; 
    char name[128];
    while( 1 )
    {
        // read the first word of the line 
        int res = fscanf_s(file, "%s", lineHeader, _countof(lineHeader)); 
        // EOF = End Of File. Quit the loop.
        if (res == EOF) 
        {
            bResult = true;
            break; 
        }
        // else : parse lineHeader
        if (strcmp(lineHeader, "v" ) == 0 )
        { 
            if (fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z) == 3)
            {
                if (vertex.x < minPoint.x) minPoint.x = vertex.x;
                if (vertex.y < minPoint.y) minPoint.y = vertex.y;
                if (vertex.z < minPoint.z) minPoint.z = vertex.z;

                if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
                if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
                if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;

                outVertice.push_back(vertex);
            }
            else
            {
                printf("Failed to parse vertex! index: %d\n", outVertice.size());
                break;
            }
        }
        else if (strcmp(lineHeader, "vn" ) == 0 )
        { 
            if(fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z) == 3)
            {
                outNormals.push_back(normal);
            }
            else
            {
                printf("Failed to parse normal! index: %d\n", outNormals.size());
                break;
            }
        }
        else if ( strcmp( lineHeader, "f" ) == 0 )
        { 
            fgets(stupidBuffer, 1024, file);
            if (6 == sscanf_s(stupidBuffer, "%d//%d %d//%d %d//%d\n", &vertexIndex.p1, &normalIndex.p1, &vertexIndex.p2, &normalIndex.p2, &vertexIndex.p3, &normalIndex.p3))
            { 
                group.vecFaceVertex.push_back(vertexIndex);
                group.vecFaceNormal.push_back(normalIndex);
            }
            else if( 9 == sscanf_s(stupidBuffer, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex.p1, &vertexTextureIndex.p1, &normalIndex.p1, 
                                                                               &vertexIndex.p2, &vertexTextureIndex.p2, &normalIndex.p2, 
                                                                               &vertexIndex.p3, &vertexTextureIndex.p3, &normalIndex.p3))
            { 
                group.vecFaceVertex.push_back(vertexIndex);
                group.vecFaceNormal.push_back(normalIndex);
            }
            else if( 3 == sscanf_s(stupidBuffer, "%d %d %d\n", &vertexIndex.p1, &vertexIndex.p2, &vertexIndex.p3))
            { 
                group.vecFaceVertex.push_back(vertexIndex);
                normalIndex.p1 = normalIndex.p2 = normalIndex.p3 = 0;
                group.vecFaceNormal.push_back(normalIndex);
            }
            else if( 6 == sscanf_s(stupidBuffer, "%d/%d %d/%d %d/%d\n", &vertexIndex.p1, &vertexTextureIndex.p1,
                                                                               &vertexIndex.p2, &vertexTextureIndex.p2,
                                                                               &vertexIndex.p3, &vertexTextureIndex.p3))
            { 
                group.vecFaceVertex.push_back(vertexIndex);
                normalIndex.p1 = normalIndex.p2 = normalIndex.p3 = 0;
                group.vecFaceNormal.push_back(normalIndex);
            }
            else
            {
                printf("Failed to parse face! group: %s face index:%d\n", group.strGroupName.c_str(), group.vecFaceVertex.size()); 
                break;
            }
            nFacesInGroup++;
        }
        else if (strcmp(lineHeader, "g" ) == 0 )
        { 
            fgets(stupidBuffer, 1024, file); 
            if(sscanf_s(stupidBuffer, "%s", name,_countof(name)) == 1)
            {
            }
            else
            {
                sprintf_s(name,"%d", outGroups.size());
               // printf("Failed to parse group name.use default name: %s\n", name);
            }
            if (bInsertGroup)
            {
                group.nTotalFacesNumber = nFacesInGroup;
                group.vecFaceCountForMaterial.push_back(nFacesInGroup);
                outGroups.push_back(group);
            }
            group.vecFaceVertex.clear();
            group.vecFaceNormal.clear();
            group.vecFaceTexture.clear();
            group.vecMaterialName.clear();
            group.vecFaceCountForMaterial.clear();
            group.nTotalFacesNumber = 0;
            group.strGroupName = name;
            group.hasMaterial = false;
            bInsertGroup = true;
            nFacesInGroup = 0;
            //else
            //{
            //    printf("Failed to parse group! index: %d\n", outGroups.size());
            //}
        }
        else if (strcmp(lineHeader, "usemtl" ) == 0 )
        { 
            if(fscanf_s(file, "%s", name,_countof(name)) == 1)
            {
                group.vecMaterialName.push_back(name);
                group.vecFaceCountForMaterial.push_back(nFacesInGroup);
            }
            else
            {
                printf("Failed to parse material! group name: %s\n", group.strGroupName.c_str());
            }
        }
        else
        { 
            // Probably a comment, eat up the rest of the line   
            fgets(stupidBuffer, 1024, file); 
        }
    }
    
    fclose(file);

    if (bInsertGroup)
    {
        group.nTotalFacesNumber = nFacesInGroup;
        group.vecFaceCountForMaterial.push_back(nFacesInGroup);
        outGroups.push_back(group);
    }

    if (bResult)
    {
        float xDim = maxPoint.x - minPoint.x;
        float yDim = maxPoint.y - minPoint.y;
        float zDim = maxPoint.z - minPoint.z;
        float cubeSize = xDim;
        if(cubeSize < yDim) cubeSize = yDim;
        if(cubeSize < zDim) cubeSize = zDim;

        float scale = 2.0 / cubeSize;
        memset(matModel2World, 0, sizeof(float) * 16);
        matModel2World[0][0] = scale;
        matModel2World[1][1] = scale;
        matModel2World[2][2] = scale;
        matModel2World[3][3] = 1.0f;

        matModel2World[3][0] = -0.5f * (minPoint.x + maxPoint.x) * scale;
        matModel2World[3][1] = -0.5f * (minPoint.y + maxPoint.y) * scale;
        matModel2World[3][2] = -0.5f * (minPoint.z + maxPoint.z) * scale;
    }
    //printf("\nmascot obj file parsed ok.\n");
    return bResult; 
}

void MascotGraphicObject::Draw(float lightPosition[4])
{    
    if(m_spData == nullptr)
    {
        return;
    }
    //float point[4]={0.0f, 0.0f, 2.0f,1.0f};
    lightPosition;
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    if(m_spData->m_nDisplayList == -1)
    {
        m_spData->m_nDisplayList  = glGenLists(1);
        glNewList(m_spData->m_nDisplayList , GL_COMPILE);
        Draw_i(m_spData->m_vecVertice, m_spData->m_vecVertexNormals, m_spData->m_vecGroups, m_spData->m_matModel2World, m_spData->m_vecMaterials);
        glEndList();
    }
    if (m_spData->m_nDisplayList  == -1)
    {
        Draw_i(m_spData->m_vecVertice, m_spData->m_vecVertexNormals, m_spData->m_vecGroups, m_spData->m_matModel2World, m_spData->m_vecMaterials);
    }
    else
    {
        glCallList(m_spData->m_nDisplayList );
    }
}

static void transform_point(MascotGraphicObject::PointStc pt, float T[4][4], MascotGraphicObject::PointStc *tpt) 
{
    tpt->x = pt.x * T[0][0] + pt.y * T[1][0] + pt.z * T[2][0] + T[3][0];
    tpt->y = pt.x * T[0][1] + pt.y * T[1][1] + pt.z * T[2][1] + T[3][1];
    tpt->z = pt.x * T[0][2] + pt.y * T[1][2] + pt.z * T[2][2] + T[3][2];
    float w = pt.x * T[0][3] + pt.y * T[1][3] + pt.z * T[2][3] + T[3][3];
    if (w != 0.0)
    {
        tpt->x /= w;
        tpt->y /= w;
        tpt->z /= w;
    }
}

void MascotGraphicObject::Draw_i( std::vector<PointStc>& vecVertice, 
                       std::vector<NormalStc>& vecNormals,
                       std::vector<GroupStc>& vecGroups,
                       float matModel2World[4][4],
                       std::vector<MaterialStc>& vecMaterials)
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_LIGHTING);
    //float global_ambient[4] = {0.5, 0.5, 0.5, 1.0};
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, TRUE);

    //glDisable(GL_LIGHTING);

    glEnable(GL_LIGHT0);

    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    //float color[4] = {181.0f / 255.0f, 131.0f / 255.0f, 68.0f / 255.0, 1.0f}; // black and yellow
    //float color[4] = {208.5f / 255.0f, 161.5f / 255.0f, 150.6f / 255.0, 1.0f};
    //float color[4] = {210.0f / 255.0f, 84.0f / 255.0f, 46.0f / 255.0, 1.0f};
    float amb[4] = {color[0]*0.96f, color[1]*0.96f, color[2]*0.96f, 1.0f};
    float dif[4] = {color[0], color[1], color[2], 1.0f};
    float spec[4] ={0.3f, 0.3f, 0.3f, 1.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 2.0f);

    //GLint buf = 0, sbuf = 0;
    //glGetIntegerv(GL_SAMPLE_BUFFERS, &buf);
    //glGetIntegerv(GL_SAMPLES, &sbuf);
    //printf(" sample buffer number: %d sample number:%d \n", buf, sbuf);
    //if (buf >= 1 && sbuf > 1)
    //{
    //    glEnable(GL_MULTISAMPLE);
    //    printf("multisample enabled!\n");
    //}

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glBlendEquation(GL_FUNC_ADD);

    glShadeModel(GL_SMOOTH);
    //glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    float matObj2Pat[4][4];
    memset(matObj2Pat, 0, sizeof(float) * 16);
    matObj2Pat[0][0] = 1.0f;
    matObj2Pat[2][1] = -1.0f;
    matObj2Pat[1][2] = 1.0f;
    matObj2Pat[3][3] = 1.0f;

    int nMaterialCount = 0;
    glBegin(GL_TRIANGLES);
    glEdgeFlag(GL_FALSE);
    for (int i = 0; i < vecGroups.size(); i++)
    {
        nMaterialCount = 0;
        for (int f = 0; f < vecGroups[i].vecFaceVertex.size(); f++)
        {
            if(vecGroups[i].vecMaterialName.size() > 0)
            {
                if (f == vecGroups[i].vecFaceCountForMaterial[nMaterialCount])
                {
                    std::string strMaterialName = vecGroups[i].vecMaterialName[nMaterialCount];
                    for(int k = 0; k < vecMaterials.size(); k++)
                    {
                        if(strMaterialName == vecMaterials[k].strMaterialName)
                        {
                            //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (float*)&vecMaterials[k].Ka.r);
                            //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (float*)&vecMaterials[k].Kd.r);
                            //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (float*)&vecMaterials[k].Ks.r);
                            //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (float*)&vecMaterials[k].Ke.r);
                            float diffuse[4];
                            diffuse[0] = vecMaterials[k].Kd.r;
                            diffuse[1] = vecMaterials[k].Kd.g;
                            diffuse[2] = vecMaterials[k].Kd.b;
                            diffuse[3] = 1.0;
                            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

                            diffuse[0] = vecMaterials[k].Ka.r;
                            diffuse[1] = vecMaterials[k].Ka.g;
                            diffuse[2] = vecMaterials[k].Ka.b;
                            diffuse[3] = 1.0;
                            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);


                            diffuse[0] = vecMaterials[k].Ks.r;
                            diffuse[1] = vecMaterials[k].Ks.g;
                            diffuse[2] = vecMaterials[k].Ks.b;
                            diffuse[3] = 1.0;
                            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, diffuse);

                            diffuse[0] = vecMaterials[k].Ke.r;
                            diffuse[1] = vecMaterials[k].Ke.g;
                            diffuse[2] = vecMaterials[k].Ke.b;
                            diffuse[3] = 1.0;
                            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, diffuse);
                        }
                    }
                    nMaterialCount++;
                }
            }
            else
            {
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
                glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 2.0f);
            }
            // Specify the vertex normal
            int vertexIndex, normalIndex;
            PointStc pointObj;
            PointStc pointWorld;
            PointStc pointPat;

            // Point 1
            normalIndex = vecGroups[i].vecFaceNormal[f].p1 - 1;
            if (normalIndex != -1)
            {
                pointObj = vecNormals[normalIndex];
                transform_point(pointObj, matObj2Pat, &pointPat);
                glNormal3f(pointPat.x, pointPat.y, pointPat.z);
                //glNormal3f(pointObj.x, pointObj.y, pointObj.z);
            }

            vertexIndex = vecGroups[i].vecFaceVertex[f].p1 - 1;
            pointObj = vecVertice[vertexIndex];
            transform_point(pointObj, matModel2World, &pointWorld);
            transform_point(pointWorld, matObj2Pat, &pointPat);
            glVertex3f(pointPat.x, pointPat.y, pointPat.z);
            //glVertex3f(pointWorld.x, pointWorld.y, pointWorld.z);

            // Point 2
            normalIndex = vecGroups[i].vecFaceNormal[f].p2 - 1;
            if (normalIndex != -1)
            {
                pointObj = vecNormals[normalIndex];
                transform_point(pointObj, matObj2Pat, &pointPat);
                glNormal3f(pointPat.x, pointPat.y, pointPat.z);
                //glNormal3f(pointObj.x, pointObj.y, pointObj.z);
            }

            vertexIndex = vecGroups[i].vecFaceVertex[f].p2 - 1;
            pointObj = vecVertice[vertexIndex];
            transform_point(pointObj, matModel2World, &pointWorld);
            //glVertex3f(pointWorld.x, pointWorld.y, pointWorld.z);
            transform_point(pointWorld, matObj2Pat, &pointPat);
            glVertex3f(pointPat.x, pointPat.y, pointPat.z);

            // Point 3
            normalIndex = vecGroups[i].vecFaceNormal[f].p3 - 1;
            if (normalIndex != -1)
            {
                pointObj = vecNormals[normalIndex];
                transform_point(pointObj, matObj2Pat, &pointPat);
                glNormal3f(pointPat.x, pointPat.y, pointPat.z);
                //glNormal3f(pointObj.x, pointObj.y, pointObj.z);
            }

            vertexIndex = vecGroups[i].vecFaceVertex[f].p3 - 1;
            pointObj = vecVertice[vertexIndex];
            transform_point(pointObj, matModel2World, &pointWorld);
            //glVertex3f(pointWorld.x, pointWorld.y, pointWorld.z);
            transform_point(pointWorld, matObj2Pat, &pointPat);
            glVertex3f(pointPat.x, pointPat.y, pointPat.z);
        }
    }
    glEnd();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE);

    //disable line smooth to fix line width bug
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}

TPS_END_NAMESPACE
