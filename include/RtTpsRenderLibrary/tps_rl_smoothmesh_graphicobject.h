////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  RONG Chengcheng chengcheng.rong@united-imaging.com
/// 
///  \file tps_rl_smoothmesh_graphicobject.h
/// 
///  \brief class declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/05/13
/////////////////////////////////////////////////////////////////
#ifndef TPS_RL_SMOOTHMESH_GRAPHICOBJECT_H_
#define TPS_RL_SMOOTHMESH_GRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include <map>
#include <vector>

namespace Mcsf{
    class Point3f;
}

TPS_BEGIN_NAMESPACE

struct Triangle
{
public :
    int P0Index;
    int P1Index;
    int P2Index;
    Triangle(int p0index, int p1index, int p2index):P0Index(p0index),P1Index(p1index),P2Index(p2index){}
    Triangle():P0Index(-1),P1Index(-1),P2Index(-1){}
public:
    bool HasVertex(int index)
    {
        return P0Index==index||P1Index==index||P2Index==index;
    }
    int GetOtherIndex(int i1,int i2)
    {
        return P0Index+P1Index+P2Index-i1-i2;
    }
};

//点邻接信息结构，每一个顶点对应一个这个结构
struct PointAttachmentInfo 
{
public:
    PointAttachmentInfo();
    ~PointAttachmentInfo();
    void ClearVertexAdj();
    void ClearTriangleAdj();
    std::vector<int>  VertexAdjacencyList;//该顶点的相邻点索引
    std::vector<int>  TriangleAdjacencyList;//该顶点的相邻面索引
};

//带各个顶点邻接信息的Mesh结构
class RenderMesh
{
public:
    std::vector<Mcsf::Point3f> Vertices;//顶点列表
    std::vector<Triangle> Faces;//面列表
    std::vector<PointAttachmentInfo> AdjInfos;//顶点邻接信息表，长度等于顶点列表
    std::map<int, int> PointIndex;
    bool GetIsPerVertexVertexInfoEnabled();
    bool GetIsPerVertexTriangleInfoEnabled();
    RenderMesh();
    ~RenderMesh();
    int AddVertex(Mcsf::Point3f& toAdd);
    int AddFace(Triangle& tri);
    void InitPerVertexVertexAdj();//初始化点点邻接关系
    void InitPerVertexTriangleAdj();//初始化点面邻接关系
    void ClearPerVertexVertexAdj();
    void ClearPerVertexTriangleAdj();
private:
    bool IsPerVertexVertexInfoEnabled;
    bool IsPerVertexTriangleInfoEnabled;
};

class Smoothing
{
private:
    RenderMesh* mesh;
public:
    Smoothing(RenderMesh* m)
    {
        this->mesh=m;
        //m->InitPerVertexVertexAdj();
    }
    ~Smoothing()
    {
        this->mesh=NULL;
    }
    void Laplacian(bool x, bool y, bool z);
    void CurvatureFlow(int iterationTimes);
    Mcsf::Point3f GetSmoothedVertex_CurvatureFlow(size_t index);
    float GetCos(Mcsf::Point3f& ps,Mcsf::Point3f& pe1,Mcsf::Point3f& pe2);
    Mcsf::Point3f GetSmoothedVertex_Laplacian(size_t index, bool x, bool y, bool z);
    float GetWeight(size_t index,int adjindex, std::vector<int>& adjFaces);
    float GetCot(Triangle& t,int index);
    void Taubin(int iterationTime,float lambda,float mu);
    Mcsf::Point3f GetSmoothedVertex_Taubin_Step(size_t index,float lambda);
};

TPS_END_NAMESPACE

#endif