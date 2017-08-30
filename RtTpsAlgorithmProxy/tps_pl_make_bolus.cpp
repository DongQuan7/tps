////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_make_bolus.cpp
/// 
///  \brief implementations for generating bolus, mostly borrowed 
///         from PLUNC 
/// 
///  \version 1.0
/// 
///  \date    2014/05/12
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_pl_make_bolus.h"

//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "McsfAlgorithms/McsfAppLib/McsfAlgoAutoContour.h"

#include "RtTpsAlgorithmProxy/tps_pl_autoshape_func.h"
//#include "tps_rl_autoshape_beam.h"
#include "RtTpsAlgorithmProxy/tps_pl_utility.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"
#include "RtTpsAlgorithmProxy/tps_pl_collimator.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"


#include "tps_logger.h"
using namespace tps::plunc;

TPS_BEGIN_NAMESPACE  // begin namespace tps

struct cord_stc {
    int     pt_num_start;
    int     pt_num_end;
    float   xstart;
    float   ystart;
    float   xend;
    float   yend;
} *stack, *stack_ptr;

static void push(int index1, int index2, float x1, float y1, float x2, float y2) {
    stack_ptr->pt_num_start = index1;
    stack_ptr->pt_num_end = index2;
    stack_ptr->xstart = x1;
    stack_ptr->ystart = y1;
    stack_ptr->xend = x2;
    stack_ptr->yend = y2;
    stack_ptr++;
}

static void pop(struct cord_stc *next_cord) {
    *next_cord = *--stack_ptr;
}


/*
    distance routine is stolen from Julian Rosenman's BASIC implementation
    of this algorithm.  The distance is defined as the perpendicular distance
    between the cord and the point if the perpendicular intersects the cord
    between its endpoints.  Otherwise the distance is the distance between the
    point and the nearest endpoint of the cord.  In short, it is the shortest
    distance between the cord and the point in a very literal sense.
    */

    static float dist(float x, float y, struct cord_stc *this_cord) {
        float c1, c2, c3, c4;
        float determinant, x_perp, y_perp, p, distance;

        c1 = this_cord->ystart - this_cord->yend;
        c2 = this_cord->xend - this_cord->xstart;
        c3 = this_cord->xend * this_cord->ystart - this_cord->xstart * this_cord->yend;
        c4 = x*c2 - y*c1;

        determinant = (float)(-c1*c1 - c2*c2);

        if (determinant != 0) {
            x_perp = ((float)(-c1*c3 - c2*c4))/determinant;
            y_perp = ((float)(c1*c4 - c3*c2))/determinant;
        }
        else {
            return(0.0);
        }

        if (c2) p = (x_perp - this_cord->xstart)/(float)c2;
        else    p = (y_perp - this_cord->ystart)/(float)-c1;

        if (p < 0.0)
            distance = (x - this_cord->xstart) * (x - this_cord->xstart) +
            (y - this_cord->ystart) * (y - this_cord->ystart);
        else if (p > 1.0)
            distance = (x - this_cord->xend) * (x - this_cord->xend) +
            (y - this_cord->yend) * (y- this_cord->yend);
        else
            distance = (x - x_perp)*(x - x_perp) + (y - y_perp)*(y - y_perp);
        return(distance);
}

/*
    Here's the guts.  Pull the next cord off the stack and find the point
    which lies between the endpoints of the cord which is furthest away
    (perpendicularly speaking).  If the distance to the furthest point
    is greater than or equal to the criterion distance, create two new
    cords and push them onto the stack.  If not, add the first endpoint of
    the current cord to the list of contour points which have passed muster.
    */

    static void cord_test(float criterion,
    float *xptr,
    float *yptr,
    int   count,
    float *new_xptr,
    float *new_yptr,
    int   *new_count) {
        struct cord_stc next_cord;
        int loop;
        int new_point;
        float max_dist;
        float perp;

        pop(&next_cord);

        max_dist = 0.0;
        new_point = -1;
        for (loop = (next_cord.pt_num_start + 1)%count; loop != next_cord.pt_num_end; loop = (loop + 1) % count) {
            perp = dist(xptr[loop], yptr[loop], &next_cord);
            if (perp > max_dist) {
                new_point = loop;
                max_dist = perp;
            }
        }

        if (new_point == -1 || max_dist < criterion) {
            new_xptr[*new_count] = next_cord.xstart;
            new_yptr[(*new_count)++] = next_cord.ystart;
        }
        else {
            push(new_point, next_cord.pt_num_end, xptr[new_point],
                yptr[new_point], next_cord.xend, next_cord.yend);
            push(next_cord.pt_num_start, new_point, next_cord.xstart,
                next_cord.ystart, xptr[new_point], yptr[new_point]);
        }
}

static void reduce(float criterion, float *xptr, float *yptr, int count,
    float *new_xptr, float *new_yptr, int *new_count) {
        int middle;

        stack = (struct cord_stc *)malloc(count*sizeof(struct cord_stc));
        stack_ptr = stack;

        /*
            We have made the arbitrary decision to choose the first and
            middle points to define the starting cords of the polygon.
            I'm sure there are other equally valid choices.
            */
            middle = count/2;
        push(middle, 0, xptr[middle], yptr[middle], xptr[0], yptr[0]);
        push(0, middle, xptr[0], yptr[0], xptr[middle], yptr[middle]);

        criterion *= criterion;
        *new_count = 0;

        while (stack_ptr != stack) {
            cord_test(criterion, xptr, yptr, count, new_xptr, new_yptr, new_count);
        }
        free(stack);
}

//void reduce_con(float criterion, plunc::CONTOUR_STC *con) {   
//    int i;
//    int count;
//    float *xptr;
//    float *yptr;
//    int new_count;
//    float *new_xptr;
//    float *new_yptr;
//
//    xptr = con->x;
//    yptr = con->y;
//    count = con->vertex_count;
//    if (count < 4) return;
//
//    new_xptr = (float *)malloc(count*sizeof(float));
//    new_yptr = (float *)malloc(count*sizeof(float));
//    reduce(criterion, xptr, yptr, count, new_xptr, new_yptr, &new_count);
//
//    for (i = 0; i < new_count; i++) {
//        con->x[i] = new_xptr[i];
//        con->y[i] = new_yptr[i];
//    }
//    free(new_xptr);
//    free(new_yptr);
//    con->vertex_count = new_count;
//}

static void reverse_contour(plunc::CONTOUR_STC *con)
{
    for (int i = 0; i <= int(con->vertex_count/2); i++)
    {
        swap(con->x[i], con->x[con->vertex_count - 1 - i]);
        swap(con->y[i], con->y[con->vertex_count - 1 - i]);
    }
}

void fix_contour_sense(plunc::CONTOUR_STC *con, int sense_flag) {   
    int i, j;
    int sense = 0;
    float   cx = 0.0, cy = 0.0;
    float   nx, ny, len;

    for (i = 0; i < con->vertex_count; i++) {
        cx += con->x[i];
        cy += con->y[i];
    }
    cx /= con->vertex_count;
    cy /= con->vertex_count;

    for (i = 0; i < con->vertex_count; i++) {
        j = i + 1;
        if (j == con->vertex_count) j = 0;
        nx = con->y[i] - con->y[j];
        ny = con->x[j] - con->x[i];
        len = nx*nx + ny*ny;
        if (len == 0.0) continue;
        len = 1.0/sqrt(len);
        nx *= len;
        ny *= len;
        if (nx*(con->x[i]-cx) + ny*(con->y[i]-cy) > 0) sense++;
        else sense--;
    }
    if (sense*sense_flag > 0) {
        reverse_contour(con);
    }
}


static void fill_contour(plunc::CONTOUR_STC *in_con, plunc::CONTOUR_STC *out_con) {   
    int     j, k, cnt = 0;
    int     steps;
    float   dx, dy;
    float   len, t, dt, tlen = 0.0;

    CONTOUR_STC old_con;
    old_con = *in_con;
    old_con.x = (float*)malloc(in_con->vertex_count * sizeof(float));
    old_con.y = (float*)malloc(in_con->vertex_count * sizeof(float));
    for (j = 0; j < in_con->vertex_count; j++) {
        old_con.x[j] = in_con->x[j];
        old_con.y[j] = in_con->y[j];
    }
    reduce_con(1.0f, &old_con);             // 0.1 cm -> 1.0 mm

    std::vector<float> vec_x, vec_y;
    vec_x.reserve(1000);
    vec_y.reserve(1000);
    vec_x.push_back(old_con.x[0]);
    vec_y.push_back(old_con.y[0]);
    for (j = 0; j < old_con.vertex_count - 1; j++) {
        k = j + 1;
        if (k == old_con.vertex_count) k = 0;
        dx = old_con.x[k] - old_con.x[j];
        dy = old_con.y[k] - old_con.y[j];
        len = sqrt(dx*dx + dy*dy);
        tlen += len;
        if (len < 3.0) continue;                // 0.3 cm -> 3.0 mm
        if (len <= 10.0) {                      // 1 cm -> 10.0 mm
            vec_x.push_back(old_con.x[k]);
            vec_y.push_back(old_con.y[k]);
            continue;
        }
        steps = ((int)(len / 10)) + 1;          // len -> len / 10
        dt = 1.0/steps;
        if (dt < 0.001) {
            printf("j,vc,cnt,dx,dy,len,dt: %d %d %d %f %f %f %f\n",
                j, old_con.vertex_count, cnt, dx, dy, len, dt);
            continue;
        }

        for (t = dt; t <= 1.01; t += dt) {
            vec_x.push_back(old_con.x[j] + t * dx);
            vec_y.push_back(old_con.y[j] + t * dy);
        }
    }
    free(old_con.x); old_con.x = NULL;
    free(old_con.y); old_con.y = NULL;

    *out_con = old_con;
    out_con->x = (float*)malloc(vec_x.size() * sizeof(float));
    out_con->y = (float*)malloc(vec_y.size() * sizeof(float));
    memcpy(out_con->x, vec_x._Myfirst, vec_x.size() * sizeof(float));
    memcpy(out_con->y, vec_y._Myfirst, vec_y.size() * sizeof(float));
    out_con->vertex_count = vec_x.size();
    fix_contour_sense(out_con, -1);   //CW_SENSE
}

/*
 ---------------------------------------------------------------------------
 
   NAME
   prism_ntrsc - line-polygon intersection
 
   SYNOPSIS
 
 
   DESCRIPTION
    This routine finds all intersections of the line described by
    p1 and p2 (not just the ray) and the polygon in con, and
    returns a list of the intersection coordinates and
    parameterized distances from p1 (alphas, defined as fraction
    of distance from p1 to p2).  As an added bonus the boolean
    inside tells whether point 1 is inside or outside the polygon.

    Only the first FI_LIMIT intersects and alphas are returned.
    If the return value is greater than FI_LIMIT, the calling
    routine needs to cope.

    The logic of this routine is a minor variation on FindNtrsc,
    the variation being that FindNtrsc returns only intersections
    with the ray.

    The basic strategy is to transform the polygon into a
    coordinate system where p1 is at the origin and the p1-p2 ray
    lies along the positive x-axis.  Intersection of a facet with
    the ray is indicated by opposite signs for the y coordinates
    of the two endpoints of the transformed facet.  The x
    coordinate of the intersection point is the (signed) distance
    from p1 in the direction of p2.  Some special cases are also
    dealt with.
 
 ---------------------------------------------------------------------------
*/

int inout(plunc::CONTOUR_STC *con, PNT3D point) {
    float p1[2], p2[2], intersects[2][TPS_FI_LIMIT], distances[TPS_FI_LIMIT];
    int int_count;

    p1[0] = point.x;
    p1[1] = point.y;
    p2[0] = 1000.0;
    p2[1] = point.y;

    int_count = FindNtrsc(con, p1, p2, intersects, distances);

    return(int_count & 1);
}

int prism_ntrsc(CONTOUR_STC *con, PNT3D p1, PNT3D p2, 
                float intersects[2][TPS_FI_LIMIT], float int_alpha[TPS_FI_LIMIT], int *inside) {
    int vertex_count;
    int index, loop, InnerLoop, OuterLoop;
    float *xTrans, *yTrans;
    float theta, sinTheta, cosTheta;
    float x1, x2, x3, x1r, x2r, x3r;
    float y1, y2, y3, y1r, y2r, y3r;
    float temp;
    float fx;
    PNT3D p3, p4;
    int IntersectCount;
    float P2x, P2y;
    float ray_length;

/*
  Initialize inside to FALSE.  Toggle the sense with each intersect we
  find that has positive alpha.  If the count is odd, the point is
  inside, if even the point if outside.
*/
    *inside = FALSE;
    vertex_count = con->vertex_count;
    if (vertex_count == 0) return(0);

/*
  Translate con so that p1 is the origin
*/
    xTrans = (float *)Malloc(2*vertex_count*sizeof(float));
    yTrans = xTrans + vertex_count;
    for (loop = 0; loop < vertex_count; loop++) {
        xTrans[loop] = con->x[loop] - p1.x;
        yTrans[loop] = con->y[loop] - p1.y;
    }

/*
  Next, deduce the clockwise angle through which the whole system
  needs to rotate in order for the line segment to lie along the
  positive x-axis.
*/
    P2x = p2.x - p1.x;
    P2y = p2.y - p1.y;
    if (P2x == 0.0 && P2y == 0.0) {
    if (p2.z == p1.z) {
        Free(xTrans);
        return(0);
    }
    if (inout(con, p1)) {
        int_alpha[0] = (con->min.z - p1.z)/(p2.z - p1.z);
        int_alpha[1] = (con->max.z - p1.z)/(p2.z - p1.z);
        intersects[0][0] = p1.x;
        intersects[1][0] = p1.y;
        intersects[0][1] = p1.x;
        intersects[1][1] = p1.y;
        *inside = TRUE;
        if (int_alpha[0] > int_alpha[1]) {
            temp = int_alpha[0];
            int_alpha[0] = int_alpha[1];
            int_alpha[1] = temp;
        }
        Free(xTrans);
        return(2);
    }
    Free(xTrans);
    return(0);
    }

    if (fabs((double) P2x) < 0.0001) {
        if (P2y > 0.0f) theta = (float)PI / 2.0f;
        else theta = (float)(-PI / 2.0f);
    }
    else {
        if (fabs((double) P2y) < 0.0001) {
            if (P2x > 0.0) theta = 0.0;
            else theta = (float)PI;
        }
        else {
            /*
              Beware:  The man pages for Ultrix 2.*, BSD4.3, and Stellix1.5 differ
              as to the argument list of atan2.  Ultrix says (x,y), BSD says
              (y,x), Stellix doesn't say.  I believe (y,x) is what we want and I
              have demonstrated that all three give the (same) desired result for
              the following line.  Don't say I didn't warn you.
            */
            theta = (float) atan2((double) P2y, (double) P2x);
        }
    }

#ifdef DEBUG
    printf("prism_ntrsc -> theta = %f\n", theta);
#endif

    sinTheta = (float) sin((double) theta);
    cosTheta = (float) cos((double) theta);

/*
  Compute the ray length for later use.
*/
    ray_length = P2x * cosTheta + P2y * sinTheta;

/*
  As we step through the line segments of the polygon we must track
  three corners at a time in order to detect singular intersects with
  corners.  If the line passes through a vertex then we make sure that
  the two polygon facets which attach to the corner are on opposite
  sides of the line.  Otherwise it is ignored.
*/
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

    IntersectCount = 0;
    for (loop = 0; loop < vertex_count; loop++) {
    if (y2r == 0.0) {
        /*
          Vertex of polygon lies along ray defined by line segment
          */
        if (y1r == 0.0) {
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
#define TAD (0.0005)
        p3.x = p1.x + ((p1.x < 0.0) ? TAD : -TAD);
        p3.y = p1.y + ((p1.y < 0.0) ? TAD : -TAD);
        p4.x = p2.x + ((p2.x < 0.0) ? TAD : -TAD);
        p4.y = p2.y + ((p2.y < 0.0) ? TAD : -TAD);
        p3.z = p4.z = 0.0;
        Free(xTrans);
        return (prism_ntrsc(con, p3, p4, intersects,
                int_alpha, inside));
        }
        else {
/*
  Make sure we don't include a singularity - vertices adjacent to
  current vertex (#2) must be on opposite sides of x-axis for point
  to be included.
*/
            if (y1r * y3r < 0.0) {
                if (x2r > 0.0) *inside = !(*inside);
                if (IntersectCount < TPS_FI_LIMIT) {
                    int_alpha[IntersectCount] = x2r / ray_length;
                    intersects[0][IntersectCount] = x2r * cosTheta + p1.x;
                    intersects[1][IntersectCount] = x2r * sinTheta + p1.y;
                }
                IntersectCount++;
            }
        }
    }
    else {
/*
  Special cases are taken care of  -  check for plain ole intersect
*/
        if (y1r * y2r < 0.0) {
        temp = interp(0, y1r, y2r, 0.0, x1r, x2r, &fx);
        if (temp > 0.0)
            *inside = !(*inside);
        if (IntersectCount < TPS_FI_LIMIT) {
            int_alpha[IntersectCount] = temp / ray_length;
            intersects[0][IntersectCount] = temp * cosTheta + p1.x;
            intersects[1][IntersectCount] = temp * sinTheta + p1.y;
        }
        IntersectCount++;
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

#ifdef DEBUG
    printf("prism_ntrsc ->  Got to sort with %d intersects\n", IntersectCount);
#endif

    index = (IntersectCount > TPS_FI_LIMIT) ? TPS_FI_LIMIT: IntersectCount;

    for (OuterLoop = 0; OuterLoop < index - 1; OuterLoop++) {
        for (InnerLoop = OuterLoop + 1; InnerLoop < index; InnerLoop++) {
            if (int_alpha[InnerLoop] < int_alpha[OuterLoop]) {
                temp = int_alpha[InnerLoop];
                int_alpha[InnerLoop] = int_alpha[OuterLoop];
                int_alpha[OuterLoop] = temp;

                temp = intersects[0][InnerLoop];
                intersects[0][InnerLoop] = intersects[0][OuterLoop];
                intersects[0][OuterLoop] = temp;

                temp = intersects[1][InnerLoop];
                intersects[1][InnerLoop] = intersects[1][OuterLoop];
                intersects[1][OuterLoop] = temp;
            }
        }
    }
    Free(xTrans);
    return(IntersectCount);
}


static int blocked_by_con(CONTOUR_STC *con, PNT3D p1, PNT3D p2) {   
    int num;
    float ints[2][TPS_FI_LIMIT], int_alpha[TPS_FI_LIMIT];
    int inside;

    num = prism_ntrsc(con, p1, p2, ints, int_alpha, &inside);
    if (num == 0) return(0);
    if (int_alpha[0] < 0.99) return(1);
    else return(0);
}

static int make_norm(CONTOUR_STC *con, int i1, int i2, PNT3D *pt) {   
    float   len;
    if (i1 == con->vertex_count) i1 = 0;
    if (i1 == -1) i1 = con->vertex_count - 1;
    if (i2 == con->vertex_count) i2 = 0;
    if (i2 == -1) i2 = con->vertex_count - 1;
    pt->x = con->y[i1] - con->y[i2];
    pt->y = con->x[i2] - con->x[i1];
    pt->z = 0.0;
    len = pt->x * pt->x + pt->y * pt->y;
    if (len == 0.0) return(1);
    len = 1.0/sqrt(len);
    pt->x *= len;
    pt->y *= len;
    return(0);
}

static int average_norm(PNT3D p1, PNT3D p2, PNT3D *p3) {
    float len;
    p3->x = p1.x + p2.x;
    p3->y = p1.y + p2.y;
    p3->z = p1.z + p2.z;
    if (p3->z > 0.0) return(1);
    len = p3->x * p3->x + p3->y * p3->y + p3->z * p3->z;
    if (len == 0.0) return(1);
    len = 1.0/sqrt(len);
    p3->x *= len;
    p3->y *= len;
    p3->z *= len;
    return(0);
}

//int contour_min_max(CONTOUR_STC *c) {
//    float *ptrx, *ptry;
//    int loop;
//
//    if (c->vertex_count == 0) return(0);
//    ptrx = c->x;
//    c->min.x = c->max.x = *ptrx++;
//    ptry = c->y;
//    c->min.y = c->max.y = *ptry++;
//    for (loop = 1; loop < c->vertex_count; loop++, ptrx++, ptry++) {
//        if (*ptrx > c->max.x) c->max.x = *ptrx;
//        if (*ptrx < c->min.x) c->min.x = *ptrx;
//        if (*ptry > c->max.y) c->max.y = *ptry;
//        if (*ptry < c->min.y) c->min.y = *ptry;
//    }
//    return(0);
//}

//int anastruct_min_max(ANASTRUCT *ana) {
//    int loop;
//
//    ana->min.x = 10000.0;
//    ana->min.y = 10000.0;
//    ana->min.z = 10000.0;
//    ana->max.x = -10000.0;
//    ana->max.y = -10000.0;
//    ana->max.z = -10000.0;
//
//    for (loop = 0; loop < ana->contour_count; loop++) {
//        contour_min_max(ana->contours + loop);
//        if (ana->contours[loop].min.x < ana->min.x)
//            ana->min.x = ana->contours[loop].min.x;
//        if (ana->contours[loop].min.y < ana->min.y)
//            ana->min.y = ana->contours[loop].min.y;
//        if (ana->contours[loop].min.z < ana->min.z)
//            ana->min.z = ana->contours[loop].min.z;
//        if (ana->contours[loop].max.x > ana->max.x)
//            ana->max.x = ana->contours[loop].max.x;
//        if (ana->contours[loop].max.y > ana->max.y)
//            ana->max.y = ana->contours[loop].max.y;
//        if (ana->contours[loop].max.z > ana->max.z)
//            ana->max.z = ana->contours[loop].max.z;
//    }
//    return (0);
//}


static int crosses( float xi, float yi,
                    float Xi, float Yi,
                    float xj, float yj,
                    float Xj, float Yj,
                    float *x, float *y) {
    float dxi, dyi, dxj, dyj;
    float denom;
    float t1, t2;

    dxi = Xi - xi;
    dyi = Yi - yi;
    dxj = Xj - xj;
    dyj = Yj - yj;

    if (dyj == 0.0) {
        if (dxj == 0.0) return(0);

        if (dyi == 0.0) return(0);

        t1 = (yj - yi)/dyi;
        if (t1 < 0.0 || t1 > 1.0) return(0);

        if (dxj != 0.0) t2 = (xi + dxi*t1 - xj)/dxj;
        else t2 = (yi + dyi*t1 - yj)/dyj;
        if (t2 < 0.0 || t2 > 1.0) return(0);
        *x = xi + t1*dxi;
        *y = yi + t1*dyi;
        return(1);
    }

    denom = dxi*dyj - dxj*dyi;
    if (denom == 0.0) {
        return(0);
    }
    else {
        t1 = (dyj*xj - dxj*yj - dyj*xi + dxj*yi)/denom;
    }

    if (t1 < 0.0 || t1 > 1.0) return(0);

    if (dxj != 0.0) t2 = (xi + dxi*t1 - xj)/dxj;
    else t2 = (yi + dyi*t1 - yj)/dyj;
    if (t2 < 0.0 || t2 > 1.0) {
        return(0);
    }
    *x = xi + t1*dxi;
    *y = yi + t1*dyi;
    return(1);
}

static void remove_crossings(CONTOUR_STC *con, int start) {   
    int i, j, k;
    int num;
    int crossed;
    float x = 0.0f, y = 0.0f;

    for (j = start; j < con->vertex_count; j++) {
        crossed = FALSE;
        for (i = j-3; i >= start; i--) {
            if (crosses(con->x[i], con->y[i],
                        con->x[i+1], con->y[i+1],
                        con->x[j], con->y[j],
                        con->x[j-1], con->y[j-1],
                        &x, &y)) {
                crossed = TRUE;
                break; 
            }
        }
        if (!crossed) continue;
        /* They cross */
        /* Insert intersection pt */
        con->x[i+1] = x;
        con->y[i+1] = y;
        /* Remove points from i+2 thru j-1 */
        num = j - i - 2;
        con->vertex_count -= num;
        for (k = i+2; k < con->vertex_count; k++) {
            con->x[k] = con->x[k + num];
            con->y[k] = con->y[k + num];
        }
        /* Retry segment j */
        j = start;
   }
}


//static int SaveContour(const char* path, CONTOUR_STC* pContour)
//{
//    FILE* file = fopen(path, "wt");
//    if (file == NULL)
//    {
//        fprintf(stderr, "Can not open output file\n");
//        return -1;
//    }
//
//    for (int i = 0; i < pContour->vertex_count; i++)
//    {
//        fprintf(file, "%.3f    %.3f\n", pContour->x[i], pContour->y[i]);
//    }
//
//    // and close file
//    fclose ( file );
//    return 0;
//}

static void remove_slivers(CONTOUR_STC *con) {   
    int i, j, k;
    float dx1, dy1, dx2, dy2;
    float len, dot;

    for (j = 0; j < con->vertex_count; j++) {
        i = j - 1;
        if (i < 0) i = con->vertex_count - 1;
        k = j + 1;
        if (k == con->vertex_count) k = 0;
        dx1 = con->x[i] - con->x[j];
        dy1 = con->y[i] - con->y[j];
        dx2 = con->x[k] - con->x[j];
        dy2 = con->y[k] - con->y[j];
        len = sqrt(dx1*dx1 + dy1*dy1);
        if (len == 0.0) continue;
        dx1 /= len;
        dy1 /= len;
        len = sqrt(dx2*dx2 + dy2*dy2);
        if (len == 0.0) continue;
        dx2 /= len;
        dy2 /= len;

        dot = dx1*dx2 + dy1*dy2;
        if (dot > 0.95) {
            /* Remove point j */
            con->vertex_count--;
            for (i = j; i < con->vertex_count; i++) {
                con->x[i] = con->x[i+1];
                con->y[i] = con->y[i+1];
            }
            j--;
        }
    }
}

void make_bolus(const TpsImage3DEntity& image3D,
                ROIEntity* pVOISkin,
                CONTOUR_STC* pBeamOutline,
                float matPat2Beam[4][4],
                float matBeam2Pat[4][4],
                float fBolusThickness, 
                float fBorder[4],
                ROIEntity* pVOIBolus) 
{
    ANASTRUCT skin;
    ANASTRUCT anaBolus;
    memset(&anaBolus, 0, sizeof(anaBolus));
    ANASTRUCT* pAnaBolus = &anaBolus;

    //从voi中的TPS_ANA转成ana，保证contour以slice_num有序
    SubVolumeHelperAPI::VOIToPluncAnastruct(pVOISkin, &skin);

    CONTOUR_STC* in_con;
    CONTOUR_STC out_con;

    int i, j, k;
    int cnt, cnt2, old_cnt;
    int *hit = NULL, *v = NULL;
    float xo, yo, xf, yf;
    float px, py;
    float min_z;
    PNT3D p1, p2, p3, beam_src;
    /*
      The bolus will be created as an anastruct. It will have the same
      coordinate system as the skin.
    */
    contour_min_max(pBeamOutline);
    xo = pBeamOutline->min.x - fBorder[0];
    xf = pBeamOutline->max.x + fBorder[1];
    yo = pBeamOutline->min.y - fBorder[2];
    yf = pBeamOutline->max.y + fBorder[3];

    p1.x = p1.y = p1.z = 0.0;
    transform_point(p1, matBeam2Pat, &beam_src);

    int last_slice_num = -1;
    float last_min_len = 1000000.f;
    min_z = 1000.0;

    Mcsf::Matrix4f matV2B;
    image3D.GetVolumeToPatientMatrix(matV2B);
    Mcsf::Matrix4f matP2B = TpsArithmeticConverter::ConvertToMatrix4f(&matPat2Beam[0][0]);
    matV2B.Prepend(matP2B);
    auto vecAxisZ = matV2B.Transform(Mcsf::Vector3f(0, 0, 1));
    vecAxisZ.SetZ(0.f);
    float radX = TPS_MIN(vecAxisZ.AngleBetween(Mcsf::Vector3f(1, 0, 0)), 
        vecAxisZ.AngleBetween(Mcsf::Vector3f(-1, 0, 0)));
    float radY = TPS_MIN(vecAxisZ.AngleBetween(Mcsf::Vector3f(0, 1, 0)), 
        vecAxisZ.AngleBetween(Mcsf::Vector3f(0, -1, 0)));
    bool judgeX = radX >= radY;

    for (j = 0; j < skin.contour_count; j++) {
        int min_i = 0, max_i = 0, start, end;
        float min_len, max_len;
        in_con = skin.contours + j;

        //char fileName[256];
        //sprintf_s(fileName, "d:\\temp\\skin_contour_%d.txt", j);
        //SaveContour(fileName, in_con);

        fill_contour(in_con, &out_con);
        //sprintf_s(fileName, "d:\\temp\\skin_contour_%d_filled.txt", j);
        //SaveContour(fileName, &out_con);
        in_con = &out_con;
        hit = (int*)malloc(in_con->vertex_count * sizeof(int));
        memset(hit, 0, in_con->vertex_count * sizeof(int));
        cnt = 0;
        min_len = 10000.0;
        max_len = 0.0;
        float boundary[2] = {10000.f, -10000.f};
        float val;
        for (i = 0; i < in_con->vertex_count; i++) {
            p1.x = in_con->x[i];
            p1.y = in_con->y[i];
            p1.z = in_con->z;
            transform_point(p1, matPat2Beam, &p2);
            px = p2.x * (-1000.0f) / p2.z;
            py = p2.y * (-1000.0f) / p2.z;
            if (px < xo || px > xf || py < yo || py > yf) continue;

            /* Record which points are closest and farthest */
            if (min_len > -p2.z) {
                min_len = -p2.z;
                min_i = i;
            }
            if (max_len < -p2.z) {
                max_len = -p2.z;
                max_i = i;
            }
            //beam c.s.下的X,Y包围盒
            val = judgeX ? p2.x : p2.y;
            boundary[0] = TPS_MIN(boundary[0], val);
            boundary[1] = TPS_MAX(boundary[1], val);

            hit[i] = 1;
            cnt++;
        }
        if (cnt < 2  || 
            //如果同一层有多个contour，选择最近的contour
            (last_slice_num == in_con->slice_number && last_min_len < min_len)) 
        {
            free(in_con->x); in_con->x = NULL;
            free(in_con->y); in_con->y = NULL;
            free(hit); hit = NULL;
            continue;
        }

        p1.x = in_con->x[min_i];
        p1.y = in_con->y[min_i];
        p1.z = in_con->z;
        if (blocked_by_con(in_con, beam_src, p1)) 
        {
            free(in_con->x); in_con->x = NULL;
            free(in_con->y); in_con->y = NULL;
            free(hit); hit = NULL;
            continue;
        }

        /* Using the closest point, find the first and last hit points */
        for (i = 0; i < in_con->vertex_count; i++) {
            k = min_i - i;
            if (k < 0) k += in_con->vertex_count;
            if (k >= in_con->vertex_count) k -= in_con->vertex_count;
            if (k == max_i) {
                i--;
                break;
            }

            if (!hit[k]) break;

            //如果到达投影边界，则停止
            p1.x = in_con->x[k];
            p1.y = in_con->y[k];
            p1.z = in_con->z;
            transform_point(p1, matPat2Beam, &p2);
            val = judgeX ? p2.x : p2.y;
            if (fabs(val - boundary[0]) <= 1e-6 || 
                fabs(val - boundary[1]) <= 1e-6 )
            {
                break;
            }
        }
        start = -i;

        for (i = 0; i < in_con->vertex_count; i++) {
            k = min_i + i;
            if (k < 0) k += in_con->vertex_count;
            if (k >= in_con->vertex_count) k -= in_con->vertex_count;
            if (k == max_i) {
                i --;
                break;
            }
            if (!hit[k]) break;

            //如果到达投影边界，则停止
            p1.x = in_con->x[k];
            p1.y = in_con->y[k];
            p1.z = in_con->z;
            transform_point(p1, matPat2Beam, &p2);
            val = judgeX ? p2.x : p2.y;
            if (fabs(val - boundary[0]) <= 1e-6 || 
                fabs(val - boundary[1]) <= 1e-6 )
            {
                break;
            }
        }
        end = i;
        cnt2 = end - start + 1;
        free(hit); hit = NULL;
        if (cnt2 < 2)
        {
            free(in_con->x); in_con->x = NULL;
            free(in_con->y); in_con->y = NULL;
            continue;
        }

        /* Form the segment from start to end */
        //如果同一层，替换离source较远的contour，而不需要创建新contour
        CONTOUR_STC* con;
        if (last_slice_num == in_con->slice_number)
        {
            con = pAnaBolus->contours + pAnaBolus->contour_count - 1;
            free(con->x); con->x = NULL;
            free(con->y); con->y = NULL;
        }
        else
        {
            pAnaBolus->contour_count++;
            pAnaBolus->contours = (CONTOUR_STC *)realloc(pAnaBolus->contours, pAnaBolus->contour_count * sizeof(CONTOUR_STC));
            con = pAnaBolus->contours + pAnaBolus->contour_count - 1;
            *con = *in_con;
        }
        old_cnt = cnt2;
        con->vertex_count = cnt2;
        con->x = (float *)malloc(4 * con->vertex_count * sizeof(float));
        con->y = (float *)malloc(4 * con->vertex_count * sizeof(float));

        v = (int*)malloc(in_con->vertex_count * sizeof(int));
        memset(v, 0, in_con->vertex_count * sizeof(int));
        for (i = start; i <= end; i++) {
            k = min_i + i;
            if (k < 0) k += in_con->vertex_count;
            if (k >= in_con->vertex_count) k -= in_con->vertex_count;
            v[i - start] = k;
            con->x[i - start] = in_con->x[k];
            con->y[i - start] = in_con->y[k];
        }

        //sprintf_s(fileName, "d:\\temp\\bolus_half_contour_%d_filled.txt", j);
        //SaveContour(fileName, con);

        for (i = con->vertex_count - 1; i >= 0; i--) {
            float dx, dy;
            k = v[i];
            if (make_norm(in_con, k, k + 1, &p1)) continue;

            con->x[cnt2] = con->x[i] + fBolusThickness * p1.x;
            con->y[cnt2++] = con->y[i] + fBolusThickness * p1.y;
            if (make_norm(in_con, k - 1, k, &p2)) continue;
            dx = p2.x - p1.x;
            dy = p2.y - p1.y;
            if (dx * dx + dy * dy < 0.4) continue;  
            if (average_norm(p1, p2, &p3)) continue;
            con->x[cnt2] = con->x[i] + fBolusThickness * p3.x;
            con->y[cnt2++] = con->y[i] + fBolusThickness * p3.y;
            con->x[cnt2] = con->x[i] + fBolusThickness * p2.x;
            con->y[cnt2++] = con->y[i] + fBolusThickness * p2.y;
        }
        con->vertex_count = cnt2;
        free(in_con->x); in_con->x = NULL;
        free(in_con->y); in_con->y = NULL;
        free(v); v = NULL;
        //sprintf_s(fileName, "d:\\temp\\bolus_contour_%d_filled.txt", j);
        //SaveContour(fileName, con);

        remove_crossings(con, old_cnt);
        //sprintf_s(fileName, "d:\\temp\\bolus_reduced_contour_%d_filled.txt", j);
        //SaveContour(fileName, con);
        remove_slivers(con);
        //sprintf_s(fileName, "d:\\temp\\bolus_final_contour_%d_filled.txt", j);
        //SaveContour(fileName, con);
        //reduce_con(0.1, con, FALSE);
        contour_min_max(con);

        last_slice_num = con->slice_number;
        last_min_len = min_len;
    }
    anastruct_min_max(pAnaBolus);
    SubVolumeHelperAPI::PluncAnastructToVOI(pAnaBolus, image3D, pVOIBolus);

    //clear memories
    for (int i = 0; i < skin.contour_count; ++i){
        free(skin.contours[i].x);
        free(skin.contours[i].y);
    }
    free(skin.contours);
    for (int i = 0; i < anaBolus.contour_count; ++i){
        free(anaBolus.contours[i].x);
        free(anaBolus.contours[i].y);
    }
    free(anaBolus.contours);
}

TPS_END_NAMESPACE  // end namespace tps
