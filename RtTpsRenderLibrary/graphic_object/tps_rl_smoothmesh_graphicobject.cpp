////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  RONG Chengcheng chengcheng.rong@united-imaging.com
/// 
///  \file tps_rl_smoothmesh_graphicobject.cpp
/// 
///  \brief class declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/05/13
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_smoothmesh_graphicobject.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
#include "Mcsf3DArithmetic/mcsf_3d_vector3f.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

PointAttachmentInfo::PointAttachmentInfo()
{
    //VertexAdjacencyList=NULL;
    //TriangleAdjacencyList=NULL;
}
PointAttachmentInfo::~PointAttachmentInfo()
{
    //ClearVertexAdj();
    //ClearTriangleAdj();
}
void PointAttachmentInfo::ClearVertexAdj()
{
    //if(VertexAdjacencyList!=NULL)
    //{
    //    delete VertexAdjacencyList;
    //    VertexAdjacencyList=NULL;
    //}
}
void PointAttachmentInfo::ClearTriangleAdj()
{
    //if(TriangleAdjacencyList!=NULL)
    //{
    //    delete TriangleAdjacencyList;
    //    //VertexAdjacencyList=NULL;
    //}
}

RenderMesh::RenderMesh()
{
    IsPerVertexTriangleInfoEnabled=false;
    IsPerVertexVertexInfoEnabled=false;
}
RenderMesh::~RenderMesh()
{
    if(IsPerVertexTriangleInfoEnabled)
        ClearPerVertexTriangleAdj();
    if(IsPerVertexVertexInfoEnabled)
        ClearPerVertexVertexAdj();
}
int RenderMesh::AddVertex(Mcsf::Point3f& toAdd)
{
    int index = (int)Vertices.size();
    Vertices.push_back(toAdd);
    return index;
}
int  RenderMesh::AddFace(Triangle& tri)
{
    int  index = (int)Faces.size();
    Faces.push_back(tri);
    return index;
}

void RenderMesh::InitPerVertexVertexAdj()
{
    if(IsPerVertexVertexInfoEnabled)
        ClearPerVertexVertexAdj();
    IsPerVertexVertexInfoEnabled = true;
    if(AdjInfos.size()!=Vertices.size())
        AdjInfos.resize(Vertices.size());
    size_t vcount=Vertices.size();
    size_t fcount= Faces.size();
    for (size_t i = 0; i < vcount; i++)
    {
        std::vector<int> vertexAdjacencyList;
        vertexAdjacencyList.reserve(6);
        AdjInfos[i].VertexAdjacencyList = vertexAdjacencyList;
    }
    for (size_t i = 0; i < fcount; i++)
    {
        Triangle &t = Faces[i];
        std::vector<int>& p0list= AdjInfos[t.P0Index].VertexAdjacencyList;
        std::vector<int>& p1list= AdjInfos[t.P1Index].VertexAdjacencyList;
        std::vector<int>& p2list= AdjInfos[t.P2Index].VertexAdjacencyList;

        if (std::find(p0list.begin(), p0list.end(), t.P1Index)==p0list.end())
            p0list.push_back(t.P1Index);
        if (std::find(p0list.begin(), p0list.end(), t.P2Index)==p0list.end())
            p0list.push_back(t.P2Index);

        if (std::find(p1list.begin(), p1list.end(), t.P0Index)==p1list.end())
            p1list.push_back(t.P0Index);
        if (std::find(p1list.begin(), p1list.end(), t.P2Index)==p1list.end())
            p1list.push_back(t.P2Index);

        if (std::find(p2list.begin(), p2list.end(), t.P0Index)==p2list.end())
            p2list.push_back(t.P0Index);
        if (std::find(p2list.begin(), p2list.end(), t.P1Index)==p2list.end())
            p2list.push_back(t.P1Index);
    }
}
void RenderMesh::InitPerVertexTriangleAdj()
{
    if(IsPerVertexTriangleInfoEnabled)
        ClearPerVertexTriangleAdj();
    IsPerVertexTriangleInfoEnabled = true;
    if(AdjInfos.size()!=Vertices.size())
        AdjInfos.resize(Vertices.size());
    for (size_t i = 0; i < Vertices.size(); i++)
    {
        std::vector<int>* triangleAdjacencyList = new (std::nothrow)std::vector<int>();
        if(triangleAdjacencyList==NULL){return;}
        triangleAdjacencyList->reserve(6);
        AdjInfos[i].TriangleAdjacencyList= *triangleAdjacencyList;
    }
    for (size_t i = 0; i < Faces.size(); i++)
    {
        Triangle& t = Faces[i];
        std::vector<int>& t0list= AdjInfos[t.P0Index].TriangleAdjacencyList;
        std::vector<int>& t1list= AdjInfos[t.P1Index].TriangleAdjacencyList;
        std::vector<int>& t2list= AdjInfos[t.P2Index].TriangleAdjacencyList;
        t0list.push_back(i);
        t1list.push_back(i);
        t2list.push_back(i);
    }
}
void RenderMesh::ClearPerVertexVertexAdj()
{
    if(!IsPerVertexVertexInfoEnabled)
        return;
    for (size_t i = 0; i < Vertices.size(); i++)
    {
        AdjInfos[i].VertexAdjacencyList.clear();
    }
    IsPerVertexVertexInfoEnabled = false;
}
void RenderMesh::ClearPerVertexTriangleAdj()
{
    if(!IsPerVertexTriangleInfoEnabled)
        return;
    for (size_t i = 0; i < Vertices.size(); i++)
    {
        AdjInfos[i].TriangleAdjacencyList.clear();
    }
    IsPerVertexTriangleInfoEnabled = false;
}
bool RenderMesh::GetIsPerVertexVertexInfoEnabled()
{
    return IsPerVertexVertexInfoEnabled;
}
bool RenderMesh::GetIsPerVertexTriangleInfoEnabled()
{
    return IsPerVertexTriangleInfoEnabled;
}

void Smoothing::Laplacian(bool x, bool y, bool z)
{
    Mcsf::Point3f* tempList=new Mcsf::Point3f[mesh->Vertices.size()];
    for(size_t i=0;i<mesh->Vertices.size();i++)
    {
        tempList[i]=GetSmoothedVertex_Laplacian(i, x, y, z);
    }
    for(size_t i=0;i<mesh->Vertices.size();i++)
    {
        mesh->Vertices[i]=tempList[i];
    }
    delete[] tempList;
}

Mcsf::Point3f Smoothing::GetSmoothedVertex_Laplacian(size_t index, bool x, bool y, bool z)
{
    float nx=0,ny=0,nz=0;
    std::vector<int>& adjVertices = (this->mesh->AdjInfos[index].VertexAdjacencyList);
    if(adjVertices.size()==0)
        return mesh->Vertices[index];
    //Mcsf::Point3f& P=mesh->Vertices[index];
    for(size_t i=0;i<adjVertices.size();i++)
    {
        nx+=mesh->Vertices[adjVertices[i]].GetX();
        ny+=mesh->Vertices[adjVertices[i]].GetY();
        nz+=mesh->Vertices[adjVertices[i]].GetZ();
    }
    if(x) {
        nx/=adjVertices.size();
    }
    else {
        nx = mesh->Vertices[index].GetX();
    }
    if(y) {
        ny/=adjVertices.size();
    }
    else {
        ny = mesh->Vertices[index].GetY();
    }
    if(z) {
        nz/=adjVertices.size();
    }
    else {
        nz = mesh->Vertices[index].GetZ ();
    }
    return Mcsf::Point3f(nx,ny,nz);
}

void Smoothing::CurvatureFlow(int iterationTimes) {
    if(!mesh->GetIsPerVertexTriangleInfoEnabled())
        mesh->InitPerVertexTriangleAdj();
    Mcsf::Point3f* tempList=new Mcsf::Point3f[mesh->Vertices.size()];
    for(int c=0;c<iterationTimes;c++)
    {
        for(size_t i=0;i<mesh->Vertices.size();i++)
        {
            tempList[i]=GetSmoothedVertex_CurvatureFlow(i);
        }
        for(size_t i=0;i<mesh->Vertices.size();i++)
        {
            mesh->Vertices[i]=tempList[i];
        }
    }
    delete[] tempList;
}

Mcsf::Point3f Smoothing::GetSmoothedVertex_CurvatureFlow(size_t index) {
    float dx=0,dy=0,dz=0;
    std::vector<int>& adjVertices=this->mesh->AdjInfos[index].VertexAdjacencyList;
    //std::vector<int>& adjFaces=this->mesh->AdjInfos[index].TriangleAdjacencyList;
    Mcsf::Point3f& p=mesh->Vertices[index];
    if(adjVertices.size()==0
        //||adjVertices.size()!=adjFaces.size()
        )
        return mesh->Vertices[index];
    float sumweight=0;
    for(size_t i=0;i<adjVertices.size();i++)
    {
        Mcsf::Point3f& t=mesh->Vertices[adjVertices[i]];
        //float cotWeight=GetWeight(index,adjVertices[i],adjFaces);
        float cotWeight = 1.0f / adjVertices.size();
        dx+=cotWeight*(t.GetX()-p.GetX());
        dy+=cotWeight*(t.GetY()-p.GetY());
        //dz+=cotWeight*(t.GetZ()-p.GetZ());
        sumweight+=cotWeight;
    }
    dx/=sumweight;
    dy/=sumweight;
    //dz/=sumweight;
    float newx=dx+p.GetX();
    float newy=dy+p.GetY();
    float newz=dz+p.GetZ();
    return Mcsf::Point3f(newx,newy,newz);
}

float Smoothing::GetWeight(size_t index,int adjindex, std::vector<int>& adjFaces)
{
    float w=0;
    int count=0;
    for(size_t i=0;i<adjFaces.size();i++)
    {
        Triangle& t=mesh->Faces[adjFaces[i]];
        if(t.HasVertex(adjindex))
        {
            int otherIndex=t.GetOtherIndex(index,adjindex);
            float cot=GetCot(t,otherIndex);
            w+=cot;
            count++;
        }
    }
    if(count==0)
        return 0;
    w=w/count;
    return w;
}
float Smoothing::GetCot(Triangle& t,int index)
{
    std::vector<Mcsf::Point3f>& v=mesh->Vertices;
    float cos = 0.0f;
    if(t.P0Index==index)
    {
        cos=GetCos(v[t.P0Index],v[t.P1Index],v[t.P2Index]);
    }else
        if(t.P1Index==index)
        {
            cos=GetCos(v[t.P1Index],v[t.P0Index],v[t.P2Index]);
        }else
            if(t.P2Index==index)
            {
                cos=GetCos(v[t.P2Index],v[t.P1Index],v[t.P0Index]);
            }
            return cos/sqrt(1-cos*cos);
}
float Smoothing::GetCos(Mcsf::Point3f& ps,Mcsf::Point3f& pe1,Mcsf::Point3f& pe2)
{
    Mcsf::Vector3f pse1(pe1.GetX()-ps.GetX(),pe1.GetY()-ps.GetY(),pe1.GetZ()-ps.GetZ());
    Mcsf::Vector3f pse2(pe2.GetX()-ps.GetX(),pe2.GetY()-ps.GetY(),pe2.GetZ()-ps.GetZ());
    float mo1=sqrt(pse1.GetX()*pse1.GetX()+pse1.GetY()*pse1.GetY()+pse1.GetZ()*pse1.GetZ());
    float mo2=sqrt(pse2.GetX()*pse2.GetX()+pse2.GetY()*pse2.GetY()+pse2.GetZ()*pse2.GetZ());
    float mul=pse1.GetX()*pse2.GetX()+pse1.GetY()*pse2.GetY()+pse1.GetZ()*pse2.GetZ();
    return mul/(mo1*mo2);
}

void Smoothing::Taubin(int iterationTime,float lambda,float mu)
{
    Mcsf::Point3f* tempList=new Mcsf::Point3f[mesh->Vertices.size()];
    for(int c=0;c<iterationTime;c++)
    {
        for(size_t i=0;i<mesh->Vertices.size();i++)
        {
            tempList[i]=GetSmoothedVertex_Taubin_Step(i,lambda);
        }
        for(size_t i=0;i<mesh->Vertices.size();i++)
        {
            mesh->Vertices[i]=tempList[i];
        }
        for(size_t i=0;i<mesh->Vertices.size();i++)
        {
            tempList[i]=GetSmoothedVertex_Taubin_Step(i,mu);
        }
        for(size_t i=0;i<mesh->Vertices.size();i++)
        {
            mesh->Vertices[i]=tempList[i];
        }
    }
    delete[] tempList;
}

Mcsf::Point3f Smoothing::GetSmoothedVertex_Taubin_Step(size_t index,float lambda)
{
    float dx=0,dy=0,dz=0;
    std::vector<int>& adjVertices = this->mesh->AdjInfos[index].VertexAdjacencyList;
    Mcsf::Point3f& p = mesh->Vertices[index];
    if(adjVertices.size()==0)
        return mesh->Vertices[index];
    for(size_t i=0;i<adjVertices.size();i++)
    {
        Mcsf::Point3f& t=mesh->Vertices[adjVertices[i]];
        dx+=(t.GetX()-p.GetX());
        dy+=(t.GetY()-p.GetY());
        //dz+=(t.GetZ()-p.GetZ());
    }
    dx/=adjVertices.size();
    dy/=adjVertices.size();
    dz/=adjVertices.size();
    float newx=lambda*dx+p.GetX();
    float newy=lambda*dy+p.GetY();
    float newz=lambda*dz+p.GetZ();
    return Mcsf::Point3f(newx,newy,newz);
}

TPS_END_NAMESPACE  // end namespace tps