////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_pl_autoshape_func.cpp
/// 
///  \brief implementations for beam outline collimator, mostly  
///         borrowed from PLUNC 
/// 
///  \version 1.0
/// 
///  \date    2014/05/12
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_pl_collimator.h"
#include "RtTpsAlgorithmProxy/tps_pl_utility.h"
#include "tps_logger.h"

TPS_PLUNC_BEGIN_NAMESPACE

    static int
    FindNtrscRecurse(
    CONTOUR_STC *Contour,
    float Point1[2],
    float Point2[2],
    float Intersects[2][TPS_FI_LIMIT],
    float Distances[TPS_FI_LIMIT],
    int recurse);

/*
---------------------------------------------------------------------------
NAME
FindNtrsc - find points of intersection of line segment with
a polygon

SYNOPSIS
int FindNtrsc(Contour, Point1, Point2, Intersects, Distances)
CONTOUR *Contour;
float Point1[2], Point2[2], Intersects[2][FI_LIMIT], Distances[FI_LIMIT];

DESCRIPTION
This routine accepts as input a polygon specified by a set
of vertex coordinates, and the coordinates of the endpoints
of a line segment.  The points of intersection of the line
segment and the polygon are returned in Intersects and
the number of intersections is returned as the function
value.  The distances from Point1 to the intersections are
returned in Distances.  Both Intersects and Distances are
limited to the first FI_LIMIT points.  The function return
will be the total intersect count.

DIAGNOSTICS
The calling routine should check to see that the return
value is <= FI_LIMIT.  If not, intersects were found which
could not be returned.
---------------------------------------------------------------------------
*/
int FindNtrsc(CONTOUR_STC *Contour,
    float Point1[2],
    float Point2[2],
    float Intersects[2][TPS_FI_LIMIT],
    float Distances[TPS_FI_LIMIT])
{
    return(FindNtrscRecurse(Contour, Point1, Point2, Intersects, Distances, 0));
}

static int FindNtrscRecurse(
    CONTOUR_STC *Contour,
    float Point1[2],
    float Point2[2],
    float Intersects[2][TPS_FI_LIMIT],
    float Distances[TPS_FI_LIMIT],
    int recurse)
{
    int vertex_count;
    int index, loop, InnerLoop, OuterLoop;
    float *xTrans;
    float *yTrans;
    float theta, sinTheta, cosTheta;
    float x1, x2, x3, x1r, x2r, x3r;
    float y1, y2, y3, y1r, y2r, y3r;
    //float alpha1, alpha2;
    float temp;
    float fx;
    float point3[2], point4[2];
    int IntersectCount;
    float P2x, P2y;

    memset(Intersects, 0, sizeof(float) * 2 * TPS_FI_LIMIT);
    memset(Distances, 0, sizeof(float) * TPS_FI_LIMIT);

    // Translate Contour so that Point1 is the origin
    vertex_count = Contour->vertex_count;
    if (vertex_count == 0) return(0);
    xTrans = (float *)Malloc(2 * vertex_count * sizeof(float));
    yTrans = xTrans + vertex_count;

    for (loop = 0; loop < vertex_count; loop++)
    {
        xTrans[loop] = Contour->x[loop] - Point1[0];
        yTrans[loop] = Contour->y[loop] - Point1[1];
    }

    /*
    Next, deduce the clockwise angle through which the whole system
    needs to rotate in order for the line segment to lie along the
    positive x-axis.
    */
    P2x = Point2[0] - Point1[0];
    P2y = Point2[1] - Point1[1];

    if (fabs((double) P2x) < 0.0001)
    {
        if (P2y > 0.0)
        {
            theta = (float)(PI / 2.0);
            sinTheta = 1.0;
            cosTheta = 0.0;
        }
        else
        {
            theta = (float)(-PI / 2.0);
            sinTheta = -1.0;
            cosTheta = 0.0;
        }
    }
    else if (fabs((double) P2y) < 0.0001)
    {
        if (P2x > 0.0)
        {
            theta = 0.0;
            sinTheta = 0.0;
            cosTheta = 1.0;
        }
        else
        {
            theta = (float)PI;
            sinTheta = 0.0;
            cosTheta = -1.0;
        }
    }
    else
    {
        /*
        Beware:  The man pages for Ultrix 2.*, BSD4.3, and Stellix1.5 differ
        as to the argument list of atan2.  Ultrix says (x,y), BSD says
        (y,x), Stellix doesn't say.  I believe (y,x) is what we want and I
        have demonstrated that all three give the (same) desired result for
        the following line.  Don't say I didn't warn you.
        */
        theta = (float) atan2((double) P2y, (double) P2x);
        sinTheta = (float) sin((double) theta);
        cosTheta = (float) cos((double) theta);
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

    // 计算直角三角形的斜边长
    P2x = (float) hypot((double) P2x, (double) P2y);
    P2y = 0.0;

    IntersectCount = 0;
    for (loop = 0; loop < vertex_count; loop++)
    {
        if (y2r == 0.0 && x2r >= 0.0)
        {
            /*
            Vertex of polygon lies along ray defined by line segment
            */
            if (y1r == 0.0)
            {
                /*
                Facet lies along line segment - boy is this a can of
                worms.

                I need to preserve parity of intersections - that is
                if both segment endpoints are inside or outside the outline,
                the intersection count is even.  Similarly if one
                point is inside and the other outside, the count is
                odd.  Thus, the count can be used to assess
                insideness of point1.

                This is a very complex problem to handle for the
                general concave outline.  I have wimped here by just
                nudging the ray over a bit (toward the origin) and
                recurring.
                */
#define TAD1 (0.0005)
#define TAD2 (0.0004)
                point3[0] = Point1[0] + ((Point1[0] < 0.0) ? TAD1 : -TAD1);
                point3[1] = Point1[1] + ((Point1[1] < 0.0) ? TAD2 : -TAD2);
                point4[0] = Point2[0] + ((Point2[0] < 0.0) ? TAD2 : -TAD2);
                point4[1] = Point2[1] + ((Point2[1] < 0.0) ? TAD1 : -TAD1);
                recurse++;
                Free(xTrans);
                if (recurse > 10)
                {
                    fprintf(stderr, "FindNtrsc: too many recursions\n");
                    return(-1);
                }
                return(FindNtrscRecurse(Contour, point3, point4,
                    Intersects, Distances, recurse));
            }
            else
            {
                if ((y1r * y3r < 0.0) && (x2r <= P2x))
                {
                    /*
                    Make sure we don't include a singularity - vertices adjacent to
                    current vertex (#2) must be on opposite sides of x-axis for point
                    to be included.
                    */
                    if (IntersectCount < TPS_FI_LIMIT)
                    {
                        Distances[IntersectCount] = x2r;
                        Intersects[0][IntersectCount] = x2r*cosTheta+Point1[0];
                        Intersects[1][IntersectCount] = x2r*sinTheta+Point1[1];
                    }
                    IntersectCount++;
                }
            }
        }
        else
        {
            if (y1r * y2r < 0.0)
            {
                /*
                Special cases are taken care of  -  check for plain ole intersect
                */
                temp = interp(0, y1r, y2r, 0.0, x1r, x2r, &fx);
                if (temp >= 0.0 && temp <= P2x)
                {
                    if (IntersectCount < TPS_FI_LIMIT)
                    {
                        Distances[IntersectCount] = temp;
                        Intersects[0][IntersectCount] = temp*cosTheta+Point1[0];
                        Intersects[1][IntersectCount] = temp*sinTheta+Point1[1];
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
        switch (vertex_count - loop)
        {
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
    index = (IntersectCount > TPS_FI_LIMIT) ? TPS_FI_LIMIT: IntersectCount;
    for (OuterLoop = 0; OuterLoop < index - 1; OuterLoop++)
    {
        if (Distances[OuterLoop] < 0.0)
        {
            fprintf (stderr, "I thought I fixed this.\n");
            fprintf (stderr, "  negative distance in FindNtrsc\n");
            fprintf (stderr, "  distance = %f\n", Distances[OuterLoop]);
            fprintf (stderr, "  pt1 = (%f, %f)\n", Point1[0], Point1[1]);
            fprintf (stderr, "  pt2 = (%f, %f)\n", Point2[0], Point2[1]);
            fprintf (stderr, "  intersect = (%f, %f)\n",
                Intersects[0][OuterLoop], Intersects[1][OuterLoop]);
        }
    }

    for (OuterLoop = 0; OuterLoop < index - 1; OuterLoop++)
    {
        for (InnerLoop = OuterLoop + 1; InnerLoop < index; InnerLoop++)
        {
            if (Distances[InnerLoop] < Distances[OuterLoop])
            {
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
    Free(xTrans);
    return(IntersectCount);
}

int CollimatorEnclosure(int nVertex, float* pVertexX, float* pVertexY, int nLeafNum,
    LEAF_POSITION_STC* pLeaf, double minBoundary, double maxBoundary, int nMlcEditMode)
{
    //int mlcEditMode = 0;//default is Center
    float x1, x2, y1, y2;
    int count = 0;
    int loop = 0;

    if (nVertex == 0) return -1;

    CONTOUR_STC bo;
    bo.vertex_count = nVertex;
    bo.slice_number = 1;
    bo.z = -1000;
    bo.x = new float[nVertex];
    bo.y = new float[nVertex];

    /* Set collimator to just enclose the beam outline */
    x1 = pVertexX[0];       // minimum x
    x2 = pVertexX[0];       // maximum x
    bo.x[0] = pVertexX[0];
    bo.y[0] = pVertexY[0];

    // After the below loop, x1 is min, x2 is max
    for (int i = 1; i < nVertex; i++)
    {
        if (x1 > pVertexX[i])
        {
            x1 = pVertexX[i];
        }
        if (x2 < pVertexX[i])
        {
            x2 = pVertexX[i];
        }
        bo.x[i] = pVertexX[i];
        bo.y[i] = pVertexY[i];
    }

    y1 = pVertexY[0];
    y2 = pVertexY[0];

    // After the below loop, y1 is min, y2 is max
    for (int i = 1; i < nVertex; i++)
    {
        if (y1 > pVertexY[i])
        {
            y1 = pVertexY[i];
        }
        if (y2 < pVertexY[i])
        {
            y2 = pVertexY[i];
        }
    }

    for (int j = 0; j < nLeafNum; j++)
    {
        // Symmetric leaf
        if (pLeaf[j].independent == 0)
        {
            double xpos = MAX(abs(x1), abs(x2));
            pLeaf[j].pos_1 = -xpos;
            pLeaf[j].pos_2 = xpos;
        }
        // Asymmetric leaf
        else if (pLeaf[j].independent == 1)
        {
            pLeaf[j].pos_1 = x1;
            pLeaf[j].pos_2 = x2;
        }
        // Multileaf Collimator, fit each leaf to beam
        else
        {
            float p1[2], p2[2], inter[2][50], dist[50];
            p1[0] = -1000;
            p2[0] =  1000;
            float fLastMlcHeight;
            switch (nMlcEditMode)
            {
            case 1://Top
                {
                    p1[1] = p2[1] = pLeaf[j].max_w;
                    fLastMlcHeight = (j == 0) ? p1[1]: pLeaf[j - 1].max_w;
                }
                break;
            case 2://Bottom
                {
                    p1[1] = p2[1] = pLeaf[j].min_w;
                    fLastMlcHeight = (j == 0) ? p1[1]: pLeaf[j - 1].min_w;
                }
                break;
            case 0://Center
            default:
                p1[1] = p2[1] = 0.5 * (pLeaf[j].min_w + pLeaf[j].max_w);
                fLastMlcHeight = (j == 0) ? p1[1]: 0.5 * (pLeaf[j - 1].min_w + pLeaf[j - 1].max_w);
                break;
            }

            // 所画轮廓不足一个mlc叶片厚度 fix dim 53994
            if (fLastMlcHeight <= y1 && y2 <= p1[1])
            {
                pLeaf[j].pos_1 = x1;
                pLeaf[j].pos_2 = x2;
            }
            else
            {
                count = FindNtrsc(&bo, p1, p2, inter, dist);
                if (count < 2)
                {
                    pLeaf[j].pos_1 = -1000.0;
                    pLeaf[j].pos_2 = -1000.0;
                }
                else
                {
                    float min(dist[0]), max(dist[0]);
                    for (loop = 1; loop < count; loop++)
                    {
                        min = TPS_MIN(min, dist[loop]);
                        max = TPS_MAX(max, dist[loop]);
                    }
                    pLeaf[j].pos_1 = min - 1000.0;
                    pLeaf[j].pos_2 = max - 1000.0;
                }
            }
            if (fabs(pLeaf[j].pos_1 - pLeaf[j].pos_2) < 1.0)
            {
                pLeaf[j].pos_1 = pLeaf[j].pos_2 = 0.0;
            }
        }
    }

    // leaf range: -200mm ~ 200mm
    for (int j = 0; j < nLeafNum; j++)
    {
        pLeaf[j].pos_1 = TPS_CLAMP(pLeaf[j].pos_1, minBoundary, maxBoundary);
        pLeaf[j].pos_2 = TPS_CLAMP(pLeaf[j].pos_2, minBoundary, maxBoundary);
    }
    delete[] bo.x;
    delete[] bo.y;
    return 0;
}

// 此接口只限内部调用外部慎用
void ScanLeafPositionFromMap(SCAN3D_MAP* pMap, int y, float& x1, float& x2)
{
    x1 = 0.0f;
    x2 = 0.0f;
    int uiWidth = pMap->x_dim;
    //int uiHeight = pMap->y_dim;
    int index = 0;
    int i;

    // 获取左边叶片位置
    for (i = 1; i < uiWidth - 1; ++i)
    {
        index = y * uiWidth + i;
        if (pMap->data[index] == 1)
        {
            x1 = pMap->x_start + pMap->x_scale * i;
            break;
        }
    }
    // 获取右边叶片位置
    for (i = uiWidth - 2; i > 0; --i)
    {
        index = y * uiWidth + i;
        if (pMap->data[index] == 1)
        {
            x2 = pMap->x_start + pMap->x_scale * i;
            break;
        }
    }
}

// 此接口只限内部调用外部慎用
void GetLeafPosition(SCAN3D_MAP* pMap, float y, int mode, float& x1, float& x2)
{
    x1 = 0.0f;
    x2 = 0.0f;
    int m = floor(y);
    int n = floor(y + 1.0f);

    if (y < 1 || y > pMap->y_dim - 1)
    {
        return;
    }

    float x1_1, x1_2, x2_1, x2_2;

    // 临近插值
    if (mode == 0)
    {
       ScanLeafPositionFromMap(pMap, m, x1, x2);
    }
    else if (mode == 1) // 线性插值
    {
       ScanLeafPositionFromMap(pMap, m, x1_1, x2_1);
       ScanLeafPositionFromMap(pMap, n, x1_2, x2_2);

       if (fabs (x1_2 - x1_1) < 1e-3)
       {
           x1 = x1_1;
       }
       else
       {
           x1 = x1_1 + (y - m) / (x1_2 - x1_1);
       }

       if (fabs(x2_2 - x2_1) < 1e-3)
       {
            x2 = x2_1;
       }
       else
       {
           x2 = x2_1 + (y - m) / (x2_2 - x2_1);
       }
    }
}

int LeafAdaptForProjectionMap(SCAN3D_MAP* pMap, int nLeafNum, LEAF_POSITION_STC* pLeaf,
    double minBoundary, double maxBoundary, int nMlcEditMode /* = 0*/)
{
    if (nullptr == pMap || nullptr == pMap->data
        || pMap->x_dim <= 0 || pMap->y_dim <= 0 || nLeafNum <= 0)
    {
        TPS_LOG_DEV_ERROR << "LeafAdaptForProjectionMap input paramater error";
        return TPS_ER_FAILURE;
    }

    int i, j;

    int uiWidth = pMap->x_dim;
    int uiHeight = pMap->y_dim;
    const int iSize = sizeof(char) * uiWidth * uiHeight;
    char *data = new char[iSize];
    memset(data, 0, iSize);

    // 输入Mask四条边置0
    char* pInputMask = pMap->data;
    memset(pInputMask, 0, uiWidth);//top
    memset(pInputMask + (uiHeight - 1) * uiWidth, 0, uiWidth);//bottom
    for (i = 0; i < iSize; ++i)
    {
        if ((0 == i % uiWidth)//left
            || ((uiWidth - 1) == i % uiWidth))//right
        {
            pInputMask[i] = 0;
        }
    }

    // 边缘检测加膨胀
    for (i = uiWidth + 1; i < iSize - uiWidth - 1; ++i)
    {
        if (pInputMask[i] == 1)
        {
            if (pInputMask[i - 1] != 1)          data[i - 1] = 1;
            if (pInputMask[i + 1] != 1)          data[i + 1] = 1;
            if (pInputMask[i - uiWidth] != 1)    data[i - uiWidth] = 1;
            if (pInputMask[i + uiWidth] != 1)    data[i + uiWidth] = 1;
        }
    }

    // 寻找Map数据中上下左右边缘坐标
    float x1, x2, y1, y2;
    int index = 0;
    int nLeftEdgeIndex = uiWidth -1;
    int nRightEdgeIndex = 1;
    int nTopEdgeIndex = uiHeight -1;
    int nBottomEdgeIndex = 1;
    for (j = 1; j < uiHeight -1; ++j)
    {
        for (i = 1; i < uiWidth -1; ++i)
        {
            index = j * uiWidth + i;
            if (data[index] == 1)
            {
                if (i < nLeftEdgeIndex)
                {
                    nLeftEdgeIndex = i;
                }
                if (i > nRightEdgeIndex)
                {
                    nRightEdgeIndex = i;
                }
                if (j < nTopEdgeIndex)
                {
                    nTopEdgeIndex = j;
                }
                if (j > nBottomEdgeIndex)
                {
                    nBottomEdgeIndex = j;
                }
            }
        }
    }
    x1 = pMap->x_start + pMap->x_scale * nLeftEdgeIndex;
    x2 = pMap->x_start + pMap->x_scale * nRightEdgeIndex;
    y1 = pMap->y_start + pMap->y_scale * nTopEdgeIndex;
    y2 = pMap->y_start + pMap->y_scale * nBottomEdgeIndex;

    // 叶片适形
    for (j = 0; j < nLeafNum; j++)
    {
        // Symmetric leaf
        if (pLeaf[j].independent == 0)
        {
            double xpos = MAX(abs(x1), abs(x2));
            pLeaf[j].pos_1 = -xpos;
            pLeaf[j].pos_2 = xpos;
        }
        // Asymmetric leaf
        else if (pLeaf[j].independent == 1)
        {
            pLeaf[j].pos_1 = x1;
            pLeaf[j].pos_2 = x2;
        }
        // Multileaf Collimator, fit each leaf to beam
        else
        {
            float fLastMlcHeight, fCurrentMlcHeight;
            switch (nMlcEditMode)
            {
                case 1://Top
                    {
                        fCurrentMlcHeight = pLeaf[j].max_w;
                        fLastMlcHeight = (j == 0) ? fCurrentMlcHeight: pLeaf[j - 1].max_w;
                    }
                    break;
                case 2://Bottom
                    {
                        fCurrentMlcHeight = pLeaf[j].min_w;
                        fLastMlcHeight = (j == 0) ? fCurrentMlcHeight: pLeaf[j - 1].min_w;
                    }
                    break;
                case 0://Center
                default:
                    fCurrentMlcHeight = 0.5 * (pLeaf[j].min_w + pLeaf[j].max_w);
                    fLastMlcHeight = (j == 0) ? fCurrentMlcHeight: 0.5 * (pLeaf[j - 1].min_w + pLeaf[j - 1].max_w);
                    break;
            }

            // 所画轮廓不足一个mlc叶片厚度 fix dim 53994
            //if (fLastMlcHeight <= y1 && y2 <= fCurrentMlcHeight)
            if (fabs(y2 - y1) < fabs(fCurrentMlcHeight - fLastMlcHeight) && y2 <= fCurrentMlcHeight && y2 >= fLastMlcHeight)
            {
                pLeaf[j].pos_1 = x1;
                pLeaf[j].pos_2 = x2;
            }
            else
            {
                float y = (fCurrentMlcHeight - pMap->y_start) / pMap->y_scale;
                pLeaf[j].pos_1 = 0.0f;
                pLeaf[j].pos_2 = 0.0f;
                if (y < 1 || y > uiHeight - 1)
                {
                    continue;
                }
                GetLeafPosition(pMap, y, 1, pLeaf[j].pos_1, pLeaf[j].pos_2);
            }
        }
    }

    // leaf range: -200mm ~ 200mm
    for (int j = 0; j < nLeafNum; j++)
    { 
        pLeaf[j].pos_1 = TPS_CLAMP(pLeaf[j].pos_1, minBoundary, maxBoundary);
        pLeaf[j].pos_2 = TPS_CLAMP(pLeaf[j].pos_2, minBoundary, maxBoundary);
    }

    TPS_DEL_ARRAY(data);

    return TPS_ER_SUCCESS;
}



    TPS_PLUNC_END_NAMESPACE
