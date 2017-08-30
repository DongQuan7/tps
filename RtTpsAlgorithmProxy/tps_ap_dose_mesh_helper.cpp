////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song   song.li@united-imaging.com
/// 
///  \file tps_rc_dose_mesh_helper.cpp
/// 
///  \brief class TpsDoseMeshHelper declaration 
/// 
///  \version 1.0
/// 
///  \date    Dec.10,2013
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_ap_dose_mesh_helper.h"
#include <string.h>                                     // for memset, memcpy etc
#include <stdio.h>                                      // for printf etc

//ZHENGHE
#include <math.h>
#include "McsfGeometry/mcsf_geometry_plane.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

//TPS
#include "tps_logger.h"
#include "tps_defs.h"
#include "RtTpsFramework/tps_fw_mask.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"

#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsFramework/tps_fw_safelock.h"             // for TpsSafeLock

#include <omp.h>

TPS_BEGIN_NAMESPACE


static int PointVector2Contour(std::vector<plunc::PNT3D>& vecPoints, int slice_num, float slice_thickness, plunc::CONTOUR_STC* pContour)
{
    if (pContour == nullptr)
        return -1;

    int nCount = vecPoints.size();
    pContour->vertex_count = nCount;
    if(nCount == 0)
    {
        return -1;
    }

    pContour->x = (float*)malloc(nCount * sizeof(float));
    pContour->y = (float*)malloc(nCount * sizeof(float));
    memset(pContour->x, 0, nCount * sizeof(float));
    memset(pContour->y, 0, nCount * sizeof(float));

    pContour->z = vecPoints[0].z;                       /* nominal slice position  */
    for (int i = 0; i < nCount; i++)
    {
        pContour->x[i] = vecPoints[i].x;
        pContour->y[i] = vecPoints[i].y;
    }
    pContour->slice_number = slice_num;   /* number of associated CT slice */
    //PNT3D max;          /* extent of prism represented by this contour */
    //PNT3D min;
    pContour->min.z = pContour->z - 0.5 * slice_thickness;
    pContour->max.z = pContour->z + 0.5 * slice_thickness;
    return 0;
}

static int DiscretePoint2Contour(std::vector<float>& x1, std::vector<float>& y1, std::vector<float>& z1, 
                                 std::vector<float>& x2, std::vector<float>& y2, std::vector<float>& z2, 
                                 int slice_num, float slice_thickness, 
                                 std::vector<plunc::CONTOUR_STC>* pVecContour)
{
    if (pVecContour == nullptr)
    {
        return -1;
    }
    pVecContour->clear();

    // Convert to lines list - for dynamic remove
    STC_LINE_SEGMENT line;
    std::list<STC_LINE_SEGMENT> listLines;
    for (int i = 0; i < x1.size(); i++)
    {
        line.p1.x = x1[i];
        line.p1.y = y1[i];
        line.p1.z = z1[i];
        line.p2.x = x2[i];
        line.p2.y = y2[i];
        line.p2.z = z2[i];
        listLines.push_back(line);
    }

    bool bFound = false;
    while(1)
    {
        // Quit if lines left < 3, 
        if (listLines.size() < 3)
            break;
        
        // Find a contour
        std::vector<plunc::PNT3D> contour;
        contour.clear();
        while(1)
        {
            bFound = false;
            for (auto iter = listLines.begin();iter!= listLines.end();)
            {
                if (contour.size() <= 0)
                {
                    contour.push_back(iter->p1);
                    contour.push_back(iter->p2);
                    listLines.erase(iter++);
                    bFound = true;
                }
                else if(iter->p1.x == contour[contour.size() - 1].x && iter->p1.y == contour[contour.size() - 1].y)
                {
                    contour.push_back(iter->p2);
                    listLines.erase(iter++);
                    bFound = true;
                }
                else if(iter->p2.x == contour[contour.size() - 1].x && iter->p2.y == contour[contour.size() - 1].y)
                {
                    contour.push_back(iter->p1);
                    listLines.erase(iter++);
                    bFound = true;
                }
                else
                {
                    ++iter;
                }
            }
            if (!bFound)
            {
                break;
            }
        }
        if (contour.size() >= 3)
        {
            plunc::CONTOUR_STC pluncContour;
            if(PointVector2Contour(contour, slice_num, slice_thickness, &pluncContour) != -1)
            {
                pVecContour->push_back(pluncContour);   // qian copy
            }
        }
    }

    if (pVecContour->size() >= 1)
    {
        return 0;
    }
    else
    {
        printf("\n Point2Contour Failed. \n");
        return -1;
    }
}


// not used yet
static int LineSegments2Contour2(std::vector<STC_LINE_SEGMENT>& vecLineSegment, 
                                 int slice_num, float slice_thickness, 
                                 std::vector<plunc::CONTOUR_STC>* pVecContour)
{
    if (pVecContour == nullptr)
    {
        return -1;
    }
    pVecContour->clear();

    // Convert to lines list - for dynamic remove
    std::list<STC_LINE_SEGMENT> listLines;
    for (int i = 0; i < vecLineSegment.size(); i++)
    {
        listLines.push_back(vecLineSegment[i]);
    }

    while(1)
    {
        // Quit if lines left < 3, 
        if (listLines.size() < 3)
            break;
        
        // Find a contour
        std::vector<plunc::PNT3D> contour;
        contour.clear();
        while(1)
        {
            bool bFound = false;
            for (auto iter = listLines.begin();iter!= listLines.end();)
            {
                if (contour.size() <= 0)
                {
                    contour.push_back(iter->p1);
                    contour.push_back(iter->p2);
                    listLines.erase(iter++);
                    bFound = true;
                }
                else if(iter->p1.x == contour[contour.size() - 1].x && iter->p1.y == contour[contour.size() - 1].y)
                {
                    contour.push_back(iter->p2);
                    listLines.erase(iter++);
                    bFound = true;
                }
                else if(iter->p2.x == contour[contour.size() - 1].x && iter->p2.y == contour[contour.size() - 1].y)
                {
                    contour.push_back(iter->p1);
                    listLines.erase(iter++);
                    bFound = true;
                }
                else
                {
                    ++iter;
                }
            }
            if (!bFound)
            {
                break;
            }
        }
        if (contour.size() >= 3)
        {
            plunc::CONTOUR_STC pluncContour;
            if(PointVector2Contour(contour, slice_num, slice_thickness, &pluncContour) != -1)
            {
                pVecContour->push_back(pluncContour);   // qian copy
            }
        }
    }

    if (pVecContour->size() >= 1)
    {
        return 0;
    }
    else
    {
        printf("\n Point2Contour Failed. \n");
        return -1;
    }
}

static int find_connected_contour(plunc::ANASTRUCT *pAna, int con_num, int first, int last)
{
    first;
    int     i, j;
    int     s1, s2;
    int     c1, c2;
    plunc::CONTOUR_STC* con1, *con2;

    con1 = pAna->contours + con_num;
    if (con1->slice_number == last)
    {
        return  (-1);
    }

    /* The current slice and the next slice */
    s1 = con1->slice_number;
    s2 = s1 + 1;

    /* Determine how many contours are on each of these two slices */
    c1 = c2 = 0;
    for (i = 0; i < pAna->contour_count; i++)
    {
        con2 = pAna->contours + i;
        if (con2->slice_number == s1) c1++;
        if (con2->slice_number == s2) c2++;
    }
    /* The next slice has no contours */
    if (c2 == 0)
    {
        return  (-2);
    }

    /* Both slices have just one contour each */
    if (c1 == 1 && c2 == 1) 
    {
        for (i = 0; i < pAna->contour_count; i++) 
        {
            con2 = pAna->contours + i;
            if (con2->slice_number == s2)
            {
                return  (i);
            }
        }
    }

    float   lx1, lx2, hx1, hx2;
    float   ly1, ly2, hy1, hy2;
    float   cx1, cx2, cx, cy1, cy2, cy;
    lx1 = con1->min.x; 
    ly1 = con1->min.y;
    hx1 = con1->max.x; 
    hy1 = con1->max.y;
    cx1 = 0.5 * (lx1 + hx1);
    cy1 = 0.5 * (ly1 + hy1);

    /* Find the contour that best matches this contour */
    int min_j = -1;
    cx = 0.0;
    cy = 0.0;
    float   dx, dy, dist;
    float   min_dist = 10000.0;
    for (j = 0; j < pAna->contour_count; j++) 
    {
        con2 = pAna->contours + j;
        if (con2->slice_number != s2) 
            continue;
        lx2 = con2->min.x; ly2 = con2->min.y;
        hx2 = con2->max.x; hy2 = con2->max.y;
        cx2 = 0.5*(lx2 + hx2);
        cy2 = 0.5*(ly2 + hy2);
        dx = cx2 - cx1;
        dy = cy1 - cy2;
        dist = dx * dx + dy * dy;
        if (min_dist > dist) 
        {
            min_dist = dist;
            min_j = j;
            cx = cx2;
            cy = cy2;
        }
    }
    
    if (cx < lx1 || cx > hx1) return(-1);
    if (cy < ly1 || cy > hy1) return(-1);
    return  (min_j);
}

static void normalize(plunc::PNT3D *pt)
{   
    float len2 = pt->x * pt->x + pt->y * pt->y + pt->z * pt->z;
    if (len2 == 0.0) 
        return;
    float len = 1.0/sqrt(len2);
    pt->x *= len;
    pt->y *= len;
    pt->z *= len;
}

static void cross_product(plunc::PNT3D p1, plunc::PNT3D p2, plunc::PNT3D p3, plunc::PNT3D *cross)
{   
    float           len;
    plunc::PNT3D    d1, d2;

    cross->x = cross->y = cross->z = 0.0;

    d1.x = p1.x - p2.x;
    d1.y = p1.y - p2.y;
    d1.z = p1.z - p2.z;
    len = d1.x * d1.x + d1.y * d1.y + d1.z * d1.z;
    if (len == 0.0) 
        return;
    len = 1.0/sqrt(len);
    d1.x *= len;
    d1.y *= len;
    d1.z *= len;

    d2.x = p3.x - p2.x;
    d2.y = p3.y - p2.y;
    d2.z = p3.z - p2.z;
    len = d2.x * d2.x + d2.y * d2.y + d2.z * d2.z;
    if (len == 0.0) 
        return;
    len = 1.0/sqrt(len);
    d2.x *= len;
    d2.y *= len;
    d2.z *= len;

    cross->x = d1.y*d2.z - d1.z*d2.y;
    cross->y = d1.z*d2.x - d1.x*d2.z;
    cross->z = d1.x*d2.y - d1.y*d2.x;
    len = cross->x * cross->x + cross->y * cross->y + cross->z * cross->z;
    if (len == 0.0) 
        return;
    len = 1.0/sqrt(len);
    cross->x *= len;
    cross->y *= len;
    cross->z *= len;
}

static int generate_prism_tiles(plunc::CONTOUR_STC* con, STC_STRIP* strip, int bBottomProcessed)
{
    int count = 2 * (con->vertex_count + 1);
    strip->count = count;
    strip->pt = (plunc::PNT3D *)malloc(count * sizeof(plunc::PNT3D));
    strip->dpt = (plunc::PNT3D *)malloc(count * sizeof(plunc::PNT3D));
    strip->vert = (int *)malloc(count * sizeof(int));
    strip->norm = (plunc::PNT3D *)malloc(count * sizeof(plunc::PNT3D));

    int     i, j, k, m;
    plunc::PNT3D   norm;
    j = 0;
    for (i = 0; i <= con->vertex_count; i++, j += 2) 
    {
        k = i;
        if (k == con->vertex_count) k = 0;

        strip->pt[j].x = con->x[k];
        strip->pt[j].y = con->y[k];

        if (bBottomProcessed == 1)      // the bottom part has already been processed, so start from the contour central ine
        {
            //printf("generate_prism_tiles: the bottom part has already been processed, so start from the contour central ine.\n");
            strip->pt[j].z = con->z;
        }
        else
        {
            strip->pt[j].z = con->min.z;
        }

        strip->pt[j + 1].x = con->x[k];
        strip->pt[j + 1].y = con->y[k];
        strip->pt[j + 1].z = con->max.z;

        strip->dpt[j] = strip->pt[j];
        strip->dpt[j + 1] = strip->pt[j+1];

        strip->vert[j] = k;
        strip->vert[j + 1] = k;
        
        // Previous point
        m = i - 1;
        if (m < 0) m += con->vertex_count;

        // Next point
        k = i + 1;
        if (k >= con->vertex_count) k -= con->vertex_count;

        // ??? 
        norm.x = -(con->y[k] - con->y[m]);
        norm.y =  (con->x[k] - con->x[m]);
        norm.z = 0.0;
        normalize(&norm);
        strip->norm[j] = norm;
        strip->norm[j+1] = norm;
    }
    return(0);
}

static int find_closest(plunc::CONTOUR_STC *con1, int loop1, plunc::CONTOUR_STC *con2)
{   
    int     loop, min_loop = -1;
    float   dx, dy, dist;

    float min_dist = -1.0f;
    for (loop = 0; loop < con2->vertex_count; loop++) 
    {
        dx = con2->x[loop] - con1->x[loop1];
        dy = con2->y[loop] - con1->y[loop1];
        dist = dx*dx + dy*dy;
        if (min_dist < 0)     // first assignment
        {
            min_dist = dist;
            min_loop = loop;
        }
        else if (min_dist > dist) 
        {
            min_dist = dist;
            min_loop = loop;
        }
    }
    return (min_loop);
}

static int find_next(plunc::CONTOUR_STC* con1, int *loop1, int *cnt1,
                     plunc::CONTOUR_STC *con2, int *loop2, int *cnt2)
{   
    int     i, j;
    int     min_i;
    int     i1, i2, j1, j2;
    float   dx, dy, d1, d2;
    float   min_dist;

    i1 = *loop1;
    i2 = *loop2;
    j1 = i1 + 1;
    j2 = i2 + 1;
    if (j1 == con1->vertex_count) j1 = 0;
    if (j2 == con2->vertex_count) j2 = 0;

    if (*cnt1 > con1->vertex_count) 
    {
        *loop2 = j2;
        (*cnt2)++;
        return(*cnt2 >= con2->vertex_count);
    }
    if (*cnt2 > con2->vertex_count) 
    {
        *loop1 = j1;
        (*cnt1)++;
        return(*cnt1 >= con1->vertex_count);
    }

    min_dist = -1.0f;
    for (i = 0; i < (con2->vertex_count>>1); i++) 
    {
        j = i2 + i + 1;
        if (j >= con2->vertex_count)
            j -= con2->vertex_count;
        dx = con2->x[j] - con1->x[i1];
        dy = con2->y[j] - con1->y[i1];
        d1 = dx*dx + dy*dy;
        if (min_dist < 0)
        {
            min_dist = d1;
            min_i = j;
        }
        else if (min_dist > d1) 
        {
            min_dist = d1;
            min_i = j;
        }
    }
    d1 = min_dist;

    min_dist = -1.0f;
    for (i = 0; i < (con1->vertex_count>>1); i++) 
    {
        j = i1 + i + 1;
        if (j >= con1->vertex_count) 
            j -= con1->vertex_count;
        dx = con1->x[j] - con2->x[i2];
        dy = con1->y[j] - con2->y[i2];
        d2 = dx*dx + dy*dy;
        if (min_dist < 0)
        {
            min_dist = d2;
            min_i = j;
        }
        else if (min_dist > d2) 
        {
            min_dist = d2;
            min_i = j;
        }
    }
    d2 = min_dist;

    if (d1 > d2) 
    {
        *loop1 = j1;
        (*cnt1)++;
    }
    else 
    {
        *loop2 = j2;
        (*cnt2)++;
    }

    return(*cnt1 > con1->vertex_count && *cnt2 > con2->vertex_count);
}

static void make_norm(plunc::CONTOUR_STC *con1, plunc::CONTOUR_STC *con2, int v1, int v2, int inc, plunc::PNT3D *n)
{   
    int     j;
    plunc::PNT3D   p1, p2, p3;
    plunc::PNT3D   n1, n2;

    p1.x = con1->x[v1];
    p1.y = con1->y[v1];
    p1.z = con1->z;

    p2.x = con2->x[v2];
    p2.y = con2->y[v2];
    p2.z = con2->z;

    j = v1 + inc;
    if (j < 0) j += con1->vertex_count;
    if (j >= con1->vertex_count) j -= con1->vertex_count;
    p3.x = con1->x[j];
    p3.y = con1->y[j];
    p3.z = con1->z;
    cross_product(p3, p1, p2, &n1);

    j = v1 - inc;
    if (j < 0) j += con1->vertex_count;
    if (j >= con1->vertex_count) j -= con1->vertex_count;
    p3.x = con1->x[j];
    p3.y = con1->y[j];
    p3.z = con1->z;
    cross_product(p2, p1, p3, &n2);

    n->x = n1.x + n2.x;
    n->y = n1.y + n2.y;
    n->z = n1.z + n2.z;
    normalize(n);
}

static int generate_smooth_tiles(plunc::CONTOUR_STC* con1, plunc::CONTOUR_STC* con2,
                                 STC_STRIP* strip,  int first_slice, int last_slice)
{
    first_slice, last_slice;
    int     i;
    int     vloop1, vloop2;
    int     loop1, loop2;
    int     cnt1 = 0, cnt2 = 0;
    int     count = 0;
    int     done = FALSE;
    int     ivert[10000];
    plunc::PNT3D    p1, p2;
    plunc::PNT3D    vert[10000];
    //plunc::PNT3D    norm[10000];

    p1.z = con1->z;
    p2.z = con2->z;
    vloop1 = 0;
    vloop2 = find_closest(con1, vloop1, con2);
    while (1) 
    {
        if (vloop1 == con1->vertex_count) loop1 = 0;
        else loop1 = vloop1;
        if (vloop2 == con2->vertex_count) loop2 = 0;
        else loop2 = vloop2;

        p1.x = con1->x[loop1];
        p1.y = con1->y[loop1];
        p2.x = con2->x[loop2];
        p2.y = con2->y[loop2];
        vert[count] = p1;
        vert[count+1] = p2;
        ivert[count] = loop1;
        ivert[count+1] = loop2;

        count += 2;
        if (done) break;
        done = find_next(con1, &vloop1, &cnt1, con2, &vloop2, &cnt2);
    }

    strip->count = count;
    strip->pt = (plunc::PNT3D *)malloc(count * sizeof(plunc::PNT3D));
    strip->dpt = (plunc::PNT3D *)malloc(count * sizeof(plunc::PNT3D));
    strip->vert = (int *)malloc(count * sizeof(int));
    strip->norm = (plunc::PNT3D *)malloc(count * sizeof(plunc::PNT3D));

    for (i = 0; i < count; i++) 
    {
        strip->pt[i] = vert[i];
        strip->dpt[i] = vert[i];
        strip->vert[i] = ivert[i];
    }
    for (i = 0; i < count; i += 2) 
    {
        make_norm(con1, con2, strip->vert[i], strip->vert[i+1], -1, &strip->norm[i]);
    }
    for (i = 1; i < count; i += 2) 
    {
        make_norm(con2, con1, strip->vert[i], strip->vert[i-1], 1, &strip->norm[i]);
    }
    return(0);
}

static void reverse_contour(plunc::CONTOUR_STC *con)
{
    int     i;
    //float   x[1000];
    //float   y[1000];
    if (con->vertex_count <= 0)
        return;

    float* x = new float[con->vertex_count];
    float* y = new float[con->vertex_count];

    for (i = 0; i < con->vertex_count; i++) 
    {
        x[i] = con->x[i];
        y[i] = con->y[i];
    }
    for (i = 0; i < con->vertex_count; i++) 
    {
        con->x[i] = x[con->vertex_count - 1 - i];
        con->y[i] = y[con->vertex_count - 1 - i];
    }

    delete[] x;
    delete[] y;
}

// 1: CCW_SENSE
//-1: CW_SENSE
static void fix_contour_sense(plunc::CONTOUR_STC *con, int sense_flag)
{   
    int     i, j;
    int     sense = 0;
    float   nx, ny, len;

    // Find the center
    float   cx = 0.0, cy = 0.0;
    for (i = 0; i < con->vertex_count; i++) 
    {
        cx += con->x[i];
        cy += con->y[i];
    }
    cx /= con->vertex_count;
    cy /= con->vertex_count;

    for (i = 0; i < con->vertex_count; i++) 
    {
        j = i + 1;
        if (j == con->vertex_count) j = 0;
        nx = con->y[i] - con->y[j];
        ny = con->x[j] - con->x[i];
        len = nx * nx + ny * ny;
        if (len == 0.0) continue;
        len = 1.0/sqrt(len);
        nx *= len;
        ny *= len;
        if (nx * (con->x[i] - cx) + ny * (con->y[i] - cy) > 0) sense++;
        else sense--;
    }
    if (sense * sense_flag > 0) 
    {
        reverse_contour(con);
    }
}

static int contour_min_max(plunc::CONTOUR_STC *c)
{
    float *ptrx, *ptry;
    int loop;

    if (c->vertex_count == 0) return(0);
    ptrx = c->x;
    c->min.x = c->max.x = *ptrx++;
    ptry = c->y;
    c->min.y = c->max.y = *ptry++;
    for (loop = 1; loop < c->vertex_count; loop++, ptrx++, ptry++) 
    {
        if (*ptrx > c->max.x) c->max.x = *ptrx;
        if (*ptrx < c->min.x) c->min.x = *ptrx;
        if (*ptry > c->max.y) c->max.y = *ptry;
        if (*ptry < c->min.y) c->min.y = *ptry;
    }
    return(0);
}

static int anastruct_min_max(plunc::ANASTRUCT *ana)
{
    int loop;
    //ana->min.x = 10000.0;
    //ana->min.y = 10000.0;
    //ana->min.z = 10000.0;
    //ana->max.x = -10000.0;
    //ana->max.y = -10000.0;
    //ana->max.z = -10000.0;

    contour_min_max(ana->contours);
    ana->min = ana->contours->min;
    ana->max = ana->contours->max;

    for (loop = 0; loop < ana->contour_count; loop++) 
    {
        contour_min_max(ana->contours + loop);

        if (ana->contours[loop].min.x < ana->min.x)
            ana->min.x = ana->contours[loop].min.x;
        if (ana->contours[loop].min.y < ana->min.y)
            ana->min.y = ana->contours[loop].min.y;
        if (ana->contours[loop].min.z < ana->min.z)
            ana->min.z = ana->contours[loop].min.z;
        if (ana->contours[loop].max.x > ana->max.x)
            ana->max.x = ana->contours[loop].max.x;
        if (ana->contours[loop].max.y > ana->max.y)
            ana->max.y = ana->contours[loop].max.y;
        if (ana->contours[loop].max.z > ana->max.z)
            ana->max.z = ana->contours[loop].max.z;
    }

    return (0);
}

static void smooth_strip(STC_STRIP *strip1, STC_STRIP *strip2)
{   
    int     i, j;
    int     cnt, cnt1;
    float   len;
    plunc::PNT3D   norm;

    // Find the max number of vertex in STRIP1
    cnt = 0;
    for (i = 1; i < strip1->count; i += 2) 
    {
        if (cnt < strip1->vert[i])
            cnt = strip1->vert[i];
    }
    
    for (j = 0; j <= cnt; j++) 
    {
        cnt1 = 0;
        norm.x = norm.y = norm.z = 0.0;
        for (i = 1; i < strip1->count; i += 2) 
        {
            if (strip1->vert[i] == j) 
            {
                cnt1++;
                norm.x += strip1->norm[i].x;
                norm.y += strip1->norm[i].y;
                norm.z += strip1->norm[i].z;
            }
        }

        for (i = 0; i < strip2->count; i += 2) 
        {
            if (strip2->vert[i] == j) 
            {
                cnt1++;
                norm.x += strip2->norm[i].x;
                norm.y += strip2->norm[i].y;
                norm.z += strip2->norm[i].z;
            }
        }

        if (cnt1 < 2) continue;

        len = norm.x*norm.x + norm.y*norm.y + norm.z*norm.z;
        if (len == 0.0) continue;
        len = 1.0/sqrt(len);
        norm.x *= len;
        norm.y *= len;
        norm.z *= len;
        for (i = 1; i < strip1->count; i += 2) 
        {
            if (strip1->vert[i] == j) 
            {
                strip1->norm[i] = norm;
            }
        }
        for (i = 0; i < strip2->count; i += 2) 
        {
            if (strip2->vert[i] == j) 
            {
                strip2->norm[i] = norm;
            }
        }
    } // end for each vertex in STRIP1
}

static int generate_tiles(plunc::ANASTRUCT* pAna, STC_STRIPS *pStrips)
{
    if (pStrips == nullptr || pAna == nullptr)
    {
        return -1;
    }
    if (pAna->contour_count <= 0)
    {
        pStrips->nCount = 0;
        return 0;
    }

    pStrips->nCount = pAna->contour_count;
    pStrips->pStrip = (STC_STRIP*)malloc(pStrips->nCount * sizeof(STC_STRIP));
    memset(pStrips->pStrip, 0, pStrips->nCount * sizeof(STC_STRIP));


    int i, j, next_con;
    int first_slice = 10000;
    int last_slice = -1000;

    plunc::CONTOUR_STC* con1, *con2;
    STC_STRIP*  strip;


    /* First do some sanity checking on the contours */
    for (j = 0; j < pAna->contour_count; j++) {
        con1 = pAna->contours + j;
        fix_contour_sense(con1, 1);    // -1: CW_SENSE   1: CCW_SENSE       because our Zslice1 < Zslice0
    }

    for (j = 0; j < pAna->contour_count; j++) {
        con1 = pAna->contours + j;
        if (first_slice > con1->slice_number) first_slice = con1->slice_number;
        if (last_slice < con1->slice_number) last_slice = con1->slice_number;
    }
    anastruct_min_max(pAna);

    int* pTopFlag = (int*)malloc(pAna->contour_count * sizeof(int));    // 1 表示contour的上半部已处理
    int* pBottomFlag = (int*)malloc(pAna->contour_count * sizeof(int)); // 1 表示contour的下半部已处理
    memset(pTopFlag, 0, pAna->contour_count * sizeof(int));
    memset(pBottomFlag, 0, pAna->contour_count * sizeof(int));

    for (j = 0; j < pAna->contour_count; j++) 
    {
        strip = pStrips->pStrip + j;
        con1 = pAna->contours + j;
        next_con = find_connected_contour(pAna, j, first_slice, last_slice);
        if (next_con == -1) {
            strip->con_num1 = strip->con_num2 = j;
            if (con1->slice_number < last_slice || last_slice == 0)
            {
                generate_prism_tiles(con1, strip, pBottomFlag[j]);
            }
        }
        else if (next_con != -2) 
        {
            con2 = pAna->contours + next_con;
            strip->con_num1 = j;
            strip->con_num2 = next_con;
            generate_smooth_tiles(con1, con2, strip,first_slice, last_slice);
            pBottomFlag[next_con] = true;
        }
    }

    STC_STRIP *strip1, *strip2;
    for (j = 0; j < pStrips->nCount-1; j++) 
    {
        strip1 = pStrips->pStrip + j;
        for (i = j + 1; i < pStrips->nCount; i++) 
        {
            strip2 = pStrips->pStrip + i;
            if (strip1->con_num2 == strip2->con_num1) 
            {
                smooth_strip(strip1, strip2);
            }
        }
    }

    free (pTopFlag);
    free (pBottomFlag);

    return(0);
}

static void add_line_segment(float fDoseLine,
                             float x1, float y1, float z1, float x2, float y2, float z2,
                             int i, int j, int k, float dose1, float dose2, float dose3, float dose4,
                             Mcsf::Matrix4f& matGrid2World, 
                             std::vector<float>& vecX1, std::vector<float>& vecY1, std::vector<float>& vecZ1, 
                             std::vector<float>& vecX2, std::vector<float>& vecY2, std::vector<float>& vecZ2)
{
    k;  // not used now
    Mcsf::Point3f pt1Vol = matGrid2World.Transform(Mcsf::Point3f(x1, y1, z1));
    Mcsf::Point3f pt2Vol = matGrid2World.Transform(Mcsf::Point3f(x2, y2, z2));
    if (y1 == y2) 
    //if (1)
    {
        vecX1.push_back(pt1Vol.GetX());
        vecY1.push_back(pt1Vol.GetY());
        vecZ1.push_back(pt1Vol.GetZ());

        vecX2.push_back(pt2Vol.GetX());
        vecY2.push_back(pt2Vol.GetY());
        vecZ2.push_back(pt2Vol.GetZ());
    }
    else
    {
        float yMid = (y1 + y2) * 0.5;
        float xMid = (x1 + x2) * 0.5;
        float zMid = (z1 + z2) * 0.5;
        float dyMid = yMid - j;
        float fL = (1 - dyMid) * dose1 + dyMid * dose4;
        float fR = (1 - dyMid) * dose2 + dyMid * dose3;
        //if (fR != fL) 
        xMid = i + 2;
        if (fabs(fR - fL) > 0.001)
        {
            xMid = (fDoseLine - fL) / (fR - fL) + i;
        }

        //if (xMid >= xGridDim - 1)
        if (xMid <= i || xMid >= (i + 1))
        {
            vecX1.push_back(pt1Vol.GetX());
            vecY1.push_back(pt1Vol.GetY());
            vecZ1.push_back(pt1Vol.GetZ());

            vecX2.push_back(pt2Vol.GetX());
            vecY2.push_back(pt2Vol.GetY());
            vecZ2.push_back(pt2Vol.GetZ());
        }
        else
        {
            Mcsf::Point3f pt3Vol = matGrid2World.Transform(Mcsf::Point3f(xMid, yMid, zMid));
            vecX1.push_back(pt1Vol.GetX());
            vecY1.push_back(pt1Vol.GetY());
            vecZ1.push_back(pt1Vol.GetZ());

            vecX2.push_back(pt3Vol.GetX());
            vecY2.push_back(pt3Vol.GetY());
            vecZ2.push_back(pt3Vol.GetZ());

            vecX1.push_back(pt3Vol.GetX());
            vecY1.push_back(pt3Vol.GetY());
            vecZ1.push_back(pt3Vol.GetZ());

            vecX2.push_back(pt2Vol.GetX());
            vecY2.push_back(pt2Vol.GetY());
            vecZ2.push_back(pt2Vol.GetZ());
        }
    }
}

int TpsDoseMeshHelper::GenerateMeshFromDose(TpsImage3DEntity* pImage3D, RtDosegrid* pDoseGrid,
    float fDoseMin, Mcsf::Matrix4f& matPat2World, STC_STRIPS* pMesh, plunc::ANASTRUCT* pAna)
{
    if (pMesh == nullptr) {
        return 0;
    }

    // Current Image
    if (nullptr == pImage3D){
        TPS_LOG_DEV_ERROR<<"TpsDoseMeshHelper::GenerateMeshFromDose(): Failed to get image";
        return -1;
    }

    RtImage3DHeader* pImageHeader = nullptr;
    if (!pImage3D->GetImage3DHeader(pImageHeader) || pImageHeader == nullptr){
        TPS_LOG_DEV_ERROR<<"TpsDoseMeshHelper::GenerateMeshFromDose(): Failed to get header of image: "<<pImage3D->GetUID();
        return -1;
    }
    //Mcsf::Matrix4f matVolume2Pat;
    //pImage3D->GetVolumeToPatientMatrix(matVolume2Pat);
    //Mcsf::Matrix4f matPat2Volume = matVolume2Pat.Inverse();
    //int xDim = pImageHeader->m_iXDim;
    //int yDim = pImageHeader->m_iYDim;
    //int zDim = pImageHeader->m_iSliceCount;

    db_Vector3f& vecXOrt = pImageHeader->m_ImageXOrientation;
    Mcsf::Vector3f vXAxis(vecXOrt.x, vecXOrt.y, vecXOrt.z);
    db_Vector3f& vecYOrt = pImageHeader->m_ImageYOrientation;
    Mcsf::Vector3f vYAxis(vecYOrt.x, vecYOrt.y, vecYOrt.z);
    float fImageSliceSpacing = pImageHeader->m_fSliceSpacing;

    // Current Plan and Dose Grid
    if (pDoseGrid == nullptr) {
        TPS_LOG_DEV_ERROR << "TpsVoiHelper::GenerateVoiFromDose(): Failed to find DoseGrid for plan: ";
        return -1;
    }
    Mcsf::Matrix4f matGrid2Pat;
    for (int n=0; n<4; n++) {
        for (int m=0; m<4; m++) {
            matGrid2Pat.SetElem(n,m,pDoseGrid->get_grid_to_pat_t()[n*4+m]);
        }
    }
    Mcsf::Matrix4f matPat2Grid = matGrid2Pat.Inverse();
    int xGridDim = pDoseGrid->get_xcount();
    int yGridDim = pDoseGrid->get_ycount();
    int zGridDim = pDoseGrid->get_zcount();
    //const 
    float* pDoseGridData = pDoseGrid->get_dosegrid_buffer();
    Mcsf::Point3f pt3fGridStart(pDoseGrid->get_xcoordinate3dstart(),
        pDoseGrid->get_ycoordinate3dstart(), pDoseGrid->get_zcoordinate3dstart());    // start in DICOM patient coordinate system
    Mcsf::Point3f pt3fGridInc(pDoseGrid->get_xcoordinate3dinc(),
        pDoseGrid->get_ycoordinate3dinc(), pDoseGrid->get_zcoordinate3dinc());        // spacing in DICOM patient coordinate system

    Mcsf::Vector3f vXInc = vXAxis * pt3fGridInc.GetX();                 // grid step in X in DICOM patient CS
    Mcsf::Vector3f vYInc = vYAxis * pt3fGridInc.GetY();                 // grid step in Y in DICOM patient CS

    // Check dose grid if all dose in 0
    bool bAllZero = true;
    bool bAllBelowMin = true;
    for (int i = 0; i < xGridDim * yGridDim * zGridDim; i++)
    {
        if (pDoseGridData[i] > 0)
        {
            bAllZero = false;
            break;
        }
    }
    if (bAllZero)
    {
        printf("All dose are zero!!!!\n");
        return -1;
    }

    for (int i = 0; i < xGridDim * yGridDim * zGridDim; i++)
    {
        if (pDoseGridData[i] > fDoseMin)
        {
            bAllBelowMin = false;
            break;
        }
    }
    if (bAllBelowMin)
    {
        printf("All dose less than  DoseMin!!!!\n");
        return -1;
    }

    // VOI from doseline 
    float dose1, dose2, dose3, dose4;
    float fDoseLine = fDoseMin;
    int index = 0;
    int hit = 0;
    float x[4], y[4], z[4];

    float slope = 0.0;

    //Mcsf::Matrix4f matGrid2Volume = matPat2Volume * matGrid2Pat;
    //Mcsf::Matrix4f matVolume2Grid = matGrid2Volume.Inverse();

    Mcsf::Matrix4f matGrid2World = matPat2World * matGrid2Pat;
    float fGridSliceThickness = 0.0;
    Mcsf::Point3f pt1InWorld = matGrid2World.Transform(Mcsf::Point3f(0, 0, 0));
    Mcsf::Point3f pt2InWorld = matGrid2World.Transform(Mcsf::Point3f(0, 0, 1));
    fGridSliceThickness = fabs(pt2InWorld.GetZ() - pt1InWorld.GetZ());

    std::vector<float> vecX1, vecY1, vecZ1;
    std::vector<float> vecX2, vecY2, vecZ2;

    int i, j, k;
    // Fake data 
    // all -> 0
    //for (k = 0; k < zGridDim; k++)
    //{
    //    for (j = 0; j < yGridDim; j++)
    //    {
    //        for (i = 0; i < xGridDim; i++)
    //        {
    //            index = k * xGridDim * yGridDim + j * xGridDim + i;
    //            pDoseGridData[index] = 0.0;
    //        }
    //    }
    //}

    // cylinder dose 
    //for (k = zGridDim / 4; k < zGridDim * 3 / 4; k++)
    //{
    //    for (j = yGridDim / 4; j < yGridDim * 3 / 4; j++)
    //    {
    //        for (i = xGridDim / 4; i < xGridDim * 3 / 4; i++)
    //        {
    //            index = k * xGridDim * yGridDim + j * xGridDim + i;
    //            pDoseGridData[index] = fDoseMin + 0.02;
    //        }
    //    }
    //}
    // sphere dose
    //float radius = (zGridDim * zGridDim + yGridDim * yGridDim + xGridDim * xGridDim) / 16.0;
    //for (k = 0; k < zGridDim ; k++)
    //{
    //    for (j = 0; j < yGridDim; j++)
    //    {
    //        for (i = 0; i < xGridDim; i++)
    //        {
    //            index = k * xGridDim * yGridDim + j * xGridDim + i;
    //            float distance = (k - zGridDim / 2) * (k - zGridDim / 2) + 
    //                             (j - yGridDim / 2) * (j - yGridDim / 2) + 
    //                             (i - xGridDim / 2) * (i - xGridDim / 2);
    //            if (distance < radius)
    //            {
    //                pDoseGridData[index] = fDoseMin + 0.02;
    //            }
    //        }
    //    }
    //}
    // quater sphere dose
    //float radius = (zGridDim * zGridDim + yGridDim * yGridDim + xGridDim * xGridDim) / 16.0;
    //for (k = 0; k < zGridDim ; k++)
    //{
    //    if( k > zGridDim / 2) continue;
    //    for (j = 0; j < yGridDim; j++)
    //    {
    //        if (j > yGridDim / 2) continue;
    //        for (i = 0; i < xGridDim; i++)
    //        {
    //            if (i > xGridDim /2) continue;
    //            index = k * xGridDim * yGridDim + j * xGridDim + i;
    //            float distance = (k - zGridDim / 2) * (k - zGridDim / 2) + 
    //                             (j - yGridDim / 2) * (j - yGridDim / 2) + 
    //                             (i - xGridDim / 2) * (i - xGridDim / 2);
    //            if (distance < radius)
    //            {
    //                pDoseGridData[index] = fDoseMin + 0.02;
    //            }
    //        }
    //    }
    //}

    // sphere dose

    int nSamples = 1;
    if ( pt3fGridInc.GetZ() > fImageSliceSpacing && fImageSliceSpacing > 0)
    {
        nSamples = 2.0 * pt3fGridInc.GetZ() / fImageSliceSpacing + 0.5;
    }
    if (nSamples < 1)
        nSamples = 1;
    fGridSliceThickness /= nSamples;

    std::vector<plunc::CONTOUR_STC> doseAna;
    float* pSliceDoseData = nullptr;
    float *pInterDoseData = (float*)malloc(xGridDim * yGridDim * sizeof(float));
    float fK = 0.0;         // actual k when sampling
for (k = 0; k < zGridDim; k++)
{
    for (int s = 0; s < nSamples; s++)
    {
        fK = (float)k;
        if (s == 0)
        {
            pSliceDoseData = pDoseGridData + k * xGridDim * yGridDim;
        }
        else
        {
            int k1 = k + 1;
            if (k1 >= zGridDim)
            {
                k1 = zGridDim - 1;
            }
            float dk = s;
            dk /= (float) nSamples;
            fK += dk;
            for (j = 0; j < yGridDim; j++)
            {
                for (i = 0; i < xGridDim; i++)
                {
                    pInterDoseData[j * xGridDim + i] = (1 - dk) * pDoseGridData[k * xGridDim * yGridDim + j * xGridDim + i] +
                                                       dk * pDoseGridData[k1 * xGridDim * yGridDim + j * xGridDim + i];
                }
            }
            pSliceDoseData = pInterDoseData;
        }


        for (j = 0; j < yGridDim - 1; j++)
        {
            for (i = 0; i < xGridDim - 1; i++)
            {
                hit = 0;
                //
                //    4  -------  3
                //      |       |
                //      |       |
                //    1  -------  2
                //
                //index = k * xGridDim * yGridDim + j * xGridDim + i;
                //dose1 = pDoseGridData[index];
                //dose2 = pDoseGridData[index + 1];
                //dose3 = pDoseGridData[index + xGridDim + 1];
                //dose4 = pDoseGridData[index + xGridDim];

                index = j * xGridDim + i;
                dose1 = pSliceDoseData[index];
                dose2 = pSliceDoseData[index + 1];
                dose3 = pSliceDoseData[index + xGridDim + 1];
                dose4 = pSliceDoseData[index + xGridDim];

                if ((dose1 <= fDoseLine && fDoseLine < dose2) || (dose2 <= fDoseLine && fDoseLine < dose1))
                {
                    slope = (fDoseLine - dose1) / (dose2 - dose1);
                    x[hit] = i + slope;
                    y[hit] = j;
                    z[hit] = fK;
                    //CheckGridDose(pDoseGridData, xGridDim, yGridDim, zGridDim, i + slope, j, fK, fDoseMin);
                    //Mcsf::Point3f ptVol = matGrid2Volume.Transform(Mcsf::Point3f(i + slope, j, fK));
                    //x[hit] = ptVol.GetX();
                    //y[hit] = ptVol.GetY();
                    //z[hit] = ptVol.GetZ();
                    //CheckVolumeDose(pDoseGridData, xGridDim, yGridDim, zGridDim, x[hit], y[hit], z[hit], matVolume2Grid, fDoseMin);
                    hit++;
                }

                if ((dose2 <= fDoseLine && fDoseLine < dose3) || (dose3 <= fDoseLine && fDoseLine < dose2))
                {
                    slope = (fDoseLine - dose2) / (dose3 - dose2);
                    x[hit] = i + 1;
                    y[hit] = j + slope;
                    z[hit] = fK;
                    //CheckGridDose(pDoseGridData, xGridDim, yGridDim, zGridDim, i + 1, j + slope, fK, fDoseMin);
                    //Mcsf::Point3f ptVol = matGrid2Volume.Transform(Mcsf::Point3f(i + 1, j + slope, fK));
                    //x[hit] = ptVol.GetX();
                    //y[hit] = ptVol.GetY();
                    //z[hit] = ptVol.GetZ();
                    //CheckVolumeDose(pDoseGridData, xGridDim, yGridDim, zGridDim, x[hit], y[hit], z[hit], matVolume2Grid, fDoseMin);
                    hit++;
                }
                if (hit == 2)
                {
                    add_line_segment(fDoseLine, x[0], y[0], z[0], x[1], y[1], z[1], i, j, k, dose1, dose2, dose3, dose4, matGrid2World,
                                     vecX1, vecY1, vecZ1, vecX2, vecY2, vecZ2);
                    hit = 0;
                }

                if ((dose3 <= fDoseLine && fDoseLine < dose4) || (dose4 <= fDoseLine && fDoseLine < dose3))
                {
                    slope = (fDoseLine - dose4) / (dose3 - dose4);
                    x[hit] = i + slope;
                    y[hit] = j + 1;
                    z[hit] = fK;
                    //CheckGridDose(pDoseGridData, xGridDim, yGridDim, zGridDim, i + slope, j + 1, fK, fDoseMin);
                    //Mcsf::Point3f ptVol = matGrid2Volume.Transform(Mcsf::Point3f(i + slope, j + 1, fK));
                    //x[hit] = ptVol.GetX();
                    //y[hit] = ptVol.GetY();
                    //z[hit] = ptVol.GetZ();
                    //CheckVolumeDose(pDoseGridData, xGridDim, yGridDim, zGridDim, x[hit], y[hit], z[hit], matVolume2Grid, fDoseMin);
                    hit++;
                }
                if (hit == 2)
                {
                    add_line_segment(fDoseLine, x[0], y[0], z[0], x[1], y[1], z[1], i, j, k, dose1, dose2, dose3, dose4, matGrid2World,
                                     vecX1, vecY1, vecZ1, vecX2, vecY2, vecZ2);
                    hit = 0;
                }

                if ((dose4 <= fDoseLine && fDoseLine < dose1) || (dose1 <= fDoseLine && fDoseLine < dose4))
                {
                    slope = (fDoseLine - dose1) / (dose4 - dose1);
                    x[hit] = i;
                    y[hit] = j + slope;
                    z[hit] = fK;
                    //CheckGridDose(pDoseGridData, xGridDim, yGridDim, zGridDim, i, j + slope, fK, fDoseMin);
                    //Mcsf::Point3f ptVol = matGrid2Volume.Transform(Mcsf::Point3f(i, j + slope, fK));
                    //x[hit] = ptVol.GetX();
                    //y[hit] = ptVol.GetY();
                    //z[hit] = ptVol.GetZ();
                    //CheckVolumeDose(pDoseGridData, xGridDim, yGridDim, zGridDim, x[hit], y[hit], z[hit], matVolume2Grid, fDoseMin);
                    hit++;
                }
                if (hit == 2)
                {
                    add_line_segment(fDoseLine, x[0], y[0], z[0], x[1], y[1], z[1], i, j, k, dose1, dose2, dose3, dose4, matGrid2World,
                                     vecX1, vecY1, vecZ1, vecX2, vecY2, vecZ2);
                    hit = 0;
                }
            } // for x
        } // for y
        

        // after finish each slice of dose grid, convert the dose point to contour
        if (vecX1.size() >= 3)
        {
            std::vector<plunc::CONTOUR_STC> vecContours;
            int nResult = DiscretePoint2Contour(vecX1, vecY1, vecZ1, vecX2, vecY2, vecZ2, k * nSamples + s, fGridSliceThickness, &vecContours);
            if (nResult == -1)
            {
                printf("Failed to Point2Contour.\n");
            }
            else
            {
                for (int t = 0; t < vecContours.size(); t++)
                {
                    doseAna.push_back(vecContours[t]);
                }
                //plunc::CONTOUR_STC fakeContour;
                //fakeContour.vertex_count = 4;
                //fakeContour.x = (float*)malloc(4 * sizeof(float));
                //fakeContour.y = (float*)malloc(4 * sizeof(float));
                //fakeContour.x[0] = -0.50;
                //fakeContour.x[1] =  0.0;
                //fakeContour.x[2] = 0.50;
                //fakeContour.x[3] =  0.0;

                //fakeContour.y[0] = -0.0;
                //fakeContour.y[1] = -0.50;
                //fakeContour.y[2] =  0.0;
                //fakeContour.y[3] =  0.50;

                //fakeContour.slice_number = k;
                //fakeContour.z = vecContours[0].z;
                //doseAna.push_back(fakeContour);

                //for (int t = 0; t < vecContours.size(); t++)
                //{
                //    plunc::CONTOUR_STC* pCon = &(vecContours[t]);
                //    free(pCon->x);
                //    free(pCon->y);
                //}
            }
        }

        vecX1.clear();
        vecY1.clear();
        vecZ1.clear();
        vecX2.clear();
        vecY2.clear();
        vecZ2.clear();
    } // for multi-samples
} // for k

    plunc::ANASTRUCT ana;
    ana.contour_count = doseAna.size();
    ana.contours = (plunc::CONTOUR_STC*)malloc(ana.contour_count * sizeof(plunc::CONTOUR_STC));
    for (int t = 0; t < ana.contour_count; t++)
    {
        ana.contours[t] = doseAna[t];
    }
    generate_tiles(&ana, pMesh);

    if (pAna != nullptr)
    {
        *pAna = ana;
    }
    else
    {
        for (int t = 0; t < ana.contour_count; t++)
        {
            plunc::CONTOUR_STC* pCon = ana.contours + t;
            free(pCon->x);
            free(pCon->y);
        }
        free(ana.contours);
    }
    return 0;
}

TPS_END_NAMESPACE
