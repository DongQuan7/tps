////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song   song.li@united-imaging.com
/// 
///  \file tps_rc_voi_helper.h
/// 
///  \brief class TpsVoiHelper declaration 
/// 
///  \version 1.0
/// 
///  \date    Dec.10,2013
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_ap_voi_helper.h"
#include <string.h>     // for memset, memcpy etc
#include <stdio.h>      // for printf etc

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
#include "RtTpsFramework/tps_fw_common_enums.h"

#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"

#include "RtTpsFramework/tps_fw_safelock.h"             // for TpsSafeLock
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"

#include <omp.h>
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"

TPS_BEGIN_NAMESPACE

//#define DEBUG_VOI_FROM_DOSE

int TpsVoiHelper::GenerateVoiViaMirror(
    TpsImage3DEntity* image3D,
    McsfGeometry::SubVolume *srcSV, 
    int mirrorDirection,
    McsfGeometry::SubVolume* tgtSV)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    if (srcSV == nullptr || tgtSV == nullptr || image3D == nullptr || 
        srcSV == tgtSV)
    {
        return TPS_ER_INPUT_INVALID;
    }

    RtImage3DHeader* header = nullptr;
    image3D->GetImage3DHeader(header);
    if (header == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Image header is null.";
        return TPS_ER_FAILURE;
    }
    size_t dim[3] = { header->m_iXDim, header->m_iYDim, header->m_iSliceCount };

    int srcSize[3];
    srcSV->GetSize(srcSize[0], srcSize[1], srcSize[2]);
    int srcOffset[3];
    srcSV->GetOffSet(srcOffset[0], srcOffset[1], srcOffset[2]);
    char* srcData = srcSV->GetData();

    Mcsf::Matrix4f matV2P;
    image3D->GetVolumeToPatientMatrix(matV2P);

    //calculate roi center
    Mcsf::Point3f center;
    if (TPS_ER_SUCCESS != SubVolumeHelperAPI::GetSubVolumeCenter(*srcSV, center))
    {
        TPS_LOG_DEV_ERROR<<"Failed to calculate sub-volume center.";
        return TPS_ER_FAILURE;
    }
    center = matV2P.Transform(center);

    Mcsf::Vector3f vecMirror = 
        mirrorDirection == LR ? Mcsf::Vector3f(-1.f, 1.f, 1.f) : 
        (mirrorDirection == HF ? Mcsf::Vector3f(1.f, 1.f, -1.f) : 
        Mcsf::Vector3f(1.f, -1.f, 1.f));
    Mcsf::Matrix4f matToCenter = MakeTranslate(Mcsf::Vector3f(-1 * center));
    Mcsf::Matrix4f matSV2MirrorV = MakeTranslate(
        Mcsf::Vector3f(srcOffset[0], srcOffset[1], srcOffset[2]));
    matSV2MirrorV.Prepend(matV2P);
    matSV2MirrorV.Prepend(matToCenter);
    matSV2MirrorV.Prepend(MakeScale(vecMirror));
    matSV2MirrorV.Prepend(matToCenter.Inverse());
    matSV2MirrorV.Prepend(matV2P.Inverse());

    Mcsf::Point3f vertices[8] = 
    {
        Mcsf::Point3f(-0.5, -0.5, -0.5),
        Mcsf::Point3f(srcSize[0] - 0.5, -0.5, -0.5),
        Mcsf::Point3f(srcSize[0] - 0.5, srcSize[1] - 0.5, -0.5),
        Mcsf::Point3f(-0.5, srcSize[1] - 0.5, -0.5),
        Mcsf::Point3f(-0.5, -0.5, srcSize[2] - 0.5),
        Mcsf::Point3f(srcSize[0] - 0.5, -0.5, srcSize[2] - 0.5),
        Mcsf::Point3f(srcSize[0] - 0.5, srcSize[1] - 0.5, srcSize[2] - 0.5),
        Mcsf::Point3f(-0.5, srcSize[1] - 0.5, srcSize[2] - 0.5),
    };
    int bdry[6] = 
    {
        dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1, 0
    };
    Mcsf::Point3f p;
    for (int i = 0; i < 8; ++i)
    {
        p = matSV2MirrorV.Transform(vertices[i]);
        bdry[0] = TPS_CLAMP(bdry[0], 0, TpsUtility::Floor(p[0]));
        bdry[1] = TPS_CLAMP(bdry[1], TpsUtility::Ceil(p[0]), dim[0] - 1);
        bdry[2] = TPS_CLAMP(bdry[2], 0, TpsUtility::Floor(p[1]));
        bdry[3] = TPS_CLAMP(bdry[3], TpsUtility::Ceil(p[1]), dim[1] - 1);
        bdry[4] = TPS_CLAMP(bdry[4], 0, TpsUtility::Floor(p[2]));
        bdry[5] = TPS_CLAMP(bdry[5], TpsUtility::Ceil(p[2]), dim[2] - 1);
    }
    int tgtSize[3] = 
    {
        bdry[1] - bdry[0] + 1,
        bdry[3] - bdry[2] + 1,
        bdry[5] - bdry[4] + 1,
    };

    tgtSV->SetOffSet(bdry[0], bdry[2], bdry[4]);
    tgtSV->SetSize(tgtSize[0], tgtSize[1], tgtSize[2]);
    char* tgtData = tgtSV->GetData();
    memset(tgtData, 0, tgtSize[0] * tgtSize[1] * tgtSize[2] * sizeof(char));

    Mcsf::Matrix4f matMirrorSV2OrgSV = matSV2MirrorV.Inverse();
    matMirrorSV2OrgSV.Append(MakeTranslate(Mcsf::Vector3f(bdry[0], bdry[2], bdry[4])));

    Mcsf::Point3f pt;
    int ix, iy, iz;
#pragma omp parallel for private(p, pt, ix, iy, iz)
    for (int z = 0; z < tgtSize[2]; ++z)
    {
        p.SetZ(z);
        for (int y = 0; y < tgtSize[1]; ++y)
        {
            p.SetY(y);
            for (int x = 0; x < tgtSize[0]; ++x)
            {
                p.SetX(x);
                pt = matMirrorSV2OrgSV.Transform(p);
                //here we use nearest interpolation
                ix = TPS_ROUND(pt[0]);
                iy = TPS_ROUND(pt[1]);
                iz = TPS_ROUND(pt[2]);
                if (ix < 0 || ix >= srcSize[0] || 
                    iy < 0 || iy >= srcSize[1] || 
                    iz < 0 || iz >= srcSize[2])
                {
                    continue;
                }
                tgtData[x + y * tgtSize[0] + z * tgtSize[0] * tgtSize[1]] = 
                    srcData[ix + iy * srcSize[0] + iz * srcSize[0] * srcSize[1]];
            }
        }
    }

    TEST_PERFORMANCE_END("GenerateVoiViaMirror")
    return TPS_ER_SUCCESS;
}

struct ex_sv
{
    char* buffer;
    int size[3];
    int offset[3];
    inline char& operator()(int x, int y, int z)
    {
        x -= offset[0];
        y -= offset[1];
        z -= offset[2];
        return buffer[size[0] * size[1] * z + size[0] * y + x];
    }

    inline bool IsXOut(int x)
    {
        x -= offset[0];
        return x < 0 || x >= size[0];
    }

    inline bool IsYOut(int y)
    {
        y -= offset[1];
        return y < 0 || y >= size[1];
    }

    inline bool IsZOut(int z)
    {
        z -= offset[2];
        return z < 0 || z >= size[2];
    }
};

int TpsVoiHelper::GenerateVoiViaExpansion(
    McsfGeometry::SubVolume *srcSV, 
    bool bMode2D,
    bool isExpansion,
    int delta[6],
    size_t volumeDim[3],
    McsfGeometry::SubVolume* tgtSV)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    if (srcSV == nullptr || tgtSV == nullptr || srcSV == tgtSV)
    {
        return TPS_ER_INPUT_INVALID;
    }
    ex_sv sv0;
    srcSV->GetSize(sv0.size[0], sv0.size[1], sv0.size[2]);
    srcSV->GetOffSet(sv0.offset[0], sv0.offset[1], sv0.offset[2]);
    sv0.buffer = srcSV->GetData();
    int bdry[6] = 
    {
        sv0.offset[0], sv0.offset[0] + sv0.size[0] - 1, 
        sv0.offset[1], sv0.offset[1] + sv0.size[1] - 1, 
        sv0.offset[2], sv0.offset[2] + sv0.size[2] - 1 
    };

    //prepare target sub-volume
    int shift[2] = {1, -1};
    if (isExpansion) 
    {
        shift[0] = 0; 
        shift[1] = 0;
    }
    int newBdry[6];
    for (int i = 0; i < 6; ++i)
    {
        newBdry[i] = bdry[i] + delta[i + shift[i%2]];
    };
    if (newBdry[1] < newBdry[0] || newBdry[3] < newBdry[2] || 
        newBdry[5] < newBdry[4])
    {
        tgtSV->SetOffSet(0, 0, 0);
        tgtSV->SetSize(0, 0, 0);
        return TPS_ER_SUCCESS;
    }
    for (int i = 0; i < 6; ++i)
    {
        newBdry[i] = TPS_CLAMP(newBdry[i], 0, volumeDim[i/2] - 1);
    }
    if (memcmp(bdry, newBdry, 6 * sizeof(int)) == 0)
    {
        tgtSV->SetOffSet(sv0.offset[0], sv0.offset[1], sv0.offset[2]);
        tgtSV->SetSize(sv0.size[0], sv0.size[1], sv0.size[2]);
        char* data = tgtSV->GetData();
        memcpy(data, sv0.buffer, sv0.size[0] * sv0.size[1] * sv0.size[2] * sizeof(char));
        return TPS_ER_SUCCESS;
    }

    ex_sv sv1;
    sv1.size[0] = newBdry[1] - newBdry[0] + 1;
    sv1.size[1] = newBdry[3] - newBdry[2] + 1;
    sv1.size[2] = newBdry[5] - newBdry[4] + 1;
    sv1.offset[0] = newBdry[0];
    sv1.offset[1] = newBdry[2];
    sv1.offset[2] = newBdry[4];
    tgtSV->SetOffSet(sv1.offset[0], sv1.offset[1], sv1.offset[2]);
    tgtSV->SetSize(sv1.size[0], sv1.size[1], sv1.size[2]);
    sv1.buffer = tgtSV->GetData();
    memset(sv1.buffer, 0, sv1.size[0] * sv1.size[1] * sv1.size[2] * sizeof(char));

    //copy overlapped region buffer
    int overlap[6] = 
    {
        TPS_MAX(bdry[0], newBdry[0]),
        TPS_MIN(bdry[1], newBdry[1]),
        TPS_MAX(bdry[2], newBdry[2]),
        TPS_MIN(bdry[3], newBdry[3]),
        TPS_MAX(bdry[4], newBdry[4]),
        TPS_MIN(bdry[5], newBdry[5])
    };
    for (int z = overlap[4]; z <= overlap[5]; ++z)
    {
        for (int y = overlap[2]; y <= overlap[3]; ++y)
        {
            memcpy(
                &sv1(overlap[0], y, z), 
                &sv0(overlap[0], y, z),
                (overlap[1] - overlap[0] + 1) * sizeof(char));
        }
    }

    int real_delta[6];
    for (int i = 0; i < 6; ++i)
    {
        real_delta[i + shift[i%2]] = newBdry[i] - bdry[i];
    };

    //make kernal
    ex_sv kernal;
    kernal.size[0] = real_delta[1] - real_delta[0] + 1;
    kernal.size[1] = real_delta[3] - real_delta[2] + 1;
    kernal.size[2] = real_delta[5] - real_delta[4] + 1;
    kernal.offset[0] = real_delta[0];
    kernal.offset[1] = real_delta[2];
    kernal.offset[2] = real_delta[4];
    int size = kernal.size[0] * kernal.size[1] * kernal.size[2];
    kernal.buffer = new char[size];
    memset(kernal.buffer, 0, size * sizeof(char));

    if (bMode2D)
    {
        for (int i = real_delta[0]; i <= real_delta[1]; ++i)
        {
            kernal(i, 0, 0) = 1;
        }
        for (int j = real_delta[2]; j <= real_delta[3]; ++j)
        {
            kernal(0, j, 0) = 1;
        }
        for (int k = real_delta[4]; k <= real_delta[5]; ++k)
        {
            kernal(0, 0, k) = 1;
        }
    }
    else
    {
        float a, b, c;
        for (int k = real_delta[4]; k <= real_delta[5]; ++k)
        {
            c = k <= 0 ? real_delta[4] : real_delta[5];
            if (c != 0) c = 1.f / (c * c);
            for (int j = real_delta[2]; j <= real_delta[3]; ++j)
            {
                b = j <= 0 ? real_delta[2] : real_delta[3];
                if (b != 0) b = 1.f / (b * b);
                for (int i = real_delta[0]; i <= real_delta[1]; ++i)
                {
                    a = i <= 0 ? real_delta[0] : real_delta[1];
                    if (a != 0) a = 1.f / (a * a);
                    if (i*i*a + j*j*b + k*k*c <= 1.f)
                    {
                        kernal(i, j, k) = 1;
                    }
                }
            }
        }
    }

    //do expansion or shrinkage
    char mask = isExpansion ? 1 : 0;
    int window[6];
    char neighbors[6];

#pragma omp parallel for private(window, neighbors)
    for (int z = bdry[4]; z <= bdry[5]; ++z)
    {
        for (int y = bdry[2]; y <= bdry[3]; ++y)
        {
            for (int x = bdry[0]; x <= bdry[1]; ++x)
            {
                if (sv0(x, y, z) != mask) continue;
                neighbors[0] = sv0.IsXOut(x-1) ? 0 : sv0(x-1, y, z);
                neighbors[1] = sv0.IsXOut(x+1) ? 0 : sv0(x+1, y, z); 
                neighbors[2] = sv0.IsYOut(y-1) ? 0 : sv0(x, y-1, z); 
                neighbors[3] = sv0.IsYOut(y+1) ? 0 : sv0(x, y+1, z); 
                neighbors[4] = sv0.IsZOut(z-1) ? 0 : sv0(x, y, z-1); 
                neighbors[5] = sv0.IsZOut(z+1) ? 0 : sv0(x, y, z+1);

                bool handle = false;
                memset(window, 0, 6 * sizeof(int));
                for (int i = 0; i < 6; ++i)
                {
                    if (real_delta[i] == 0|| (neighbors[i] == mask))
                    {
                        continue;
                    }
                    window[i] = real_delta[i];
                    handle = true;
                }
                if (!handle) continue;
                window[0] = TPS_MAX(window[0] + x, newBdry[0]);
                window[1] = TPS_MIN(window[1] + x, newBdry[1]);
                window[2] = TPS_MAX(window[2] + y, newBdry[2]);
                window[3] = TPS_MIN(window[3] + y, newBdry[3]);
                window[4] = TPS_MAX(window[4] + z, newBdry[4]);
                window[5] = TPS_MIN(window[5] + z, newBdry[5]);

                for (int k = window[4]; k <= window[5]; ++k)
                {
                    for (int j = window[2]; j <= window[3]; ++j)
                    {
                        for (int i = window[0]; i <= window[1]; ++i)
                        {
                            if (kernal(i - x, j - y, k - z))
                            {
                                sv1(i, j, k) = mask;
                            }
                        }
                    }
                }
            }
        }
    }

    delete[] kernal.buffer;
    TEST_PERFORMANCE_END("GenerateVoiViaExpansion");
    return TPS_ER_SUCCESS;
}

int TpsVoiHelper::GenerateVoiViaCombination(
    McsfGeometry::SubVolume* srcSV, 
    McsfGeometry::SubVolume* combineSV, 
    int combinationMode,
    McsfGeometry::SubVolume* tgtSV)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    if (srcSV == nullptr || combineSV == nullptr || tgtSV == nullptr || 
        srcSV == tgtSV || combineSV == tgtSV)
    {
        return TPS_ER_INPUT_INVALID;
    }

    int iSizeX = 0, iSizeY = 0, iSizeZ = 0;
    srcSV->GetSize(iSizeX, iSizeY, iSizeZ);
    int iOffsetX = 0, iOffsetY = 0, iOffsetZ = 0;
    srcSV->GetOffSet(iOffsetX, iOffsetY, iOffsetZ);
    char* pSrcData = srcSV->GetData();

    int iPeerSizeX = 0, iPeerSizeY = 0, iPeerSizeZ = 0;
    combineSV->GetSize(iPeerSizeX, iPeerSizeY, iPeerSizeZ);
    int iPeerOffsetX = 0, iPeerOffsetY = 0, iPeerOffsetZ = 0;
    combineSV->GetOffSet(iPeerOffsetX, iPeerOffsetY, iPeerOffsetZ);
    char* pPeerData = combineSV->GetData();

    int iDstOffsetX = iOffsetX;
    int iDstOffsetY = iOffsetY;
    int iDstOffsetZ = iOffsetZ;
    int iDstSizeX = iSizeX;
    int iDstSizeY = iSizeY;
    int iDstSizeZ = iSizeZ;

    // Union or XOR
    if (combinationMode == Union || combinationMode == XOR ) 
    {
        iDstOffsetX = min(iOffsetX, iPeerOffsetX);// the smaller as begin
        iDstOffsetY = min(iOffsetY, iPeerOffsetY);
        iDstOffsetZ = min(iOffsetZ, iPeerOffsetZ);

        int iEndX = max(iOffsetX + iSizeX, iPeerOffsetX + iPeerSizeX);
        int iEndY = max(iOffsetY + iSizeY, iPeerOffsetY + iPeerSizeY);
        int iEndZ = max(iOffsetZ + iSizeZ, iPeerOffsetZ + iPeerSizeZ);

        iDstSizeX = iEndX - iDstOffsetX;
        iDstSizeY = iEndY - iDstOffsetY;
        iDstSizeZ = iEndZ - iDstOffsetZ;
    }
    else if(combinationMode == Intersect || combinationMode == Subtract)
    {
        // for Intersect and Substract, use the size of the source subvolume
    }

    //{
    //    std::string strCombinationMode[4] = {"Union", "Intersect", "Subtract", "XOR"};

    //    printf("\n%s: Src(%d->%d %d->%d %d->%d), Peer(%d->%d %d->%d %d->%d), Dst(%d->%d %d->%d %d->%d)\n", strCombinationMode[iCombinationMode].c_str(),
    //                   iOffsetX, iSizeX, iOffsetY, iSizeY, iOffsetZ, iSizeZ,
    //                   iPeerOffsetX, iPeerSizeX, iPeerOffsetY, iPeerSizeY, iPeerOffsetZ, iPeerSizeZ,
    //                   iDstOffsetX, iDstSizeX, iDstOffsetY, iDstSizeY, iDstOffsetZ, iDstSizeZ);
    //}

    tgtSV->SetSize(iDstSizeX, iDstSizeY, iDstSizeZ);                       // will allocate memory
    tgtSV->SetOffSet(iDstOffsetX, iDstOffsetY, iDstOffsetZ);
    char* pDstData = tgtSV->GetData();
    if (pDstData == nullptr)
    {
        TPS_LOG_DEV_ERROR << "AddVoiViaCombination error: failed to allocate memory for the new VOI.";
        return TPS_ER_FAILURE;
    }
    memset(pDstData, 0, iDstSizeX * iDstSizeY * iDstSizeZ * sizeof(char));

    int i = 0, j = 0, k = 0;
    //int iDstRange = iDstSizeX * iDstSizeY * iDstSizeZ - 1;
    //int iSrcRange = iSizeX * iSizeY * iSizeZ - 1;
    //int iPeerRange = iPeerSizeX * iPeerSizeY * iPeerSizeZ - 1;
    int iPeerBegin = max(iPeerOffsetX, iDstOffsetX);
    int iPeerEnd = min(iPeerOffsetX + iPeerSizeX, iDstOffsetX + iDstSizeX);
    //int iPeerCount = min(iPeerSizeX, iDstOffsetX + iDstSizeX - iPeerOffsetX);
    //int iPeerYCount = min(iPeerSizeY, iDstOffsetY + iDstSizeY - iPeerOffsetY);
    int kSrc, kPeer;
    int jSrc, jPeer;
    for (k = 0; k < iDstSizeZ; k++)
    {
        kSrc = iDstOffsetZ + k - iOffsetZ;
        kPeer = iDstOffsetZ + k - iPeerOffsetZ;
        bool bSliceInSrc = (0 <= kSrc && kSrc < iSizeZ);
        bool bSliceInPeer = (0 <= kPeer && kPeer < iPeerSizeZ);

        // this slice is within both voi
        if (bSliceInSrc && bSliceInPeer)  {                  
            for (j = 0; j < iDstSizeY; j++) {
                jSrc = j + iDstOffsetY - iOffsetY;
                jPeer = j + iDstOffsetY - iPeerOffsetY;
                bool bLineInSrc = (0 <= jSrc && jSrc < iSizeY);
                bool bLineInPeer = (0 <= jPeer && jPeer < iPeerSizeY);

                int srcIndex = kSrc * iSizeX * iSizeY + jSrc * iSizeX;
                int peerIndex = kPeer * iPeerSizeX * iPeerSizeY + jPeer * iPeerSizeX;

                // line within src
                if(bLineInSrc) {
                    int dstIndex = k * iDstSizeX * iDstSizeY + j * iDstSizeX + iOffsetX - iDstOffsetX;
                    //if (( dstIndex + iSizeX - 1) > iDstRange)
                    //    printf("MemcopySrc dst out of heap1 \n");
                    //if ((srcIndex + iSizeX - 1) > iSrcRange)
                    //    printf("MemcopySrc src out of heap2 \n");
                    memcpy (pDstData + dstIndex,
                        pSrcData + srcIndex,
                        iSizeX);
                }

                // line within peer
                if(bLineInPeer) {
                    int dstIndex = k * iDstSizeX * iDstSizeY + j * iDstSizeX;// + iPeerOffsetX - iDstOffsetX;
                    if (combinationMode == Union) {
                        for (i = iPeerBegin; i < iPeerEnd; i++) {
                            pDstData[dstIndex + i - iDstOffsetX] |= pPeerData[peerIndex + i - iPeerOffsetX];
                            //if (( dstIndex + i) > iDstRange)
                            //    printf("Union dst out of heap \n");
                            //if ((peerIndex + i) > iPeerRange)
                            //    printf("Union peer out of heap \n");
                        }
                    }
                    if (combinationMode == Intersect) {
                        for (i = iPeerBegin; i < iPeerEnd; i++) {
                            //if (pDstData[ dstIndex + i  - iDstOffsetX] >= 2)
                            //    printf("\n intersect src error");
                            //if (pPeerData[peerIndex + i - iPeerOffsetX] >= 2)
                            //    printf("\n intersect src error");
                            pDstData[ dstIndex + i  - iDstOffsetX] += pPeerData[peerIndex + i - iPeerOffsetX];
                            //if (( dstIndex + i) > iDstRange)
                            //    printf("Intersect/XOR dst out of heap \n");
                            //if ((peerIndex + i) > iPeerRange)
                            //    printf("Intersect/XOR peer out of heap \n");
                        }
                    }
                    if (combinationMode == Subtract) {
                        for (i = iPeerBegin; i < iPeerEnd; i++) {
                            pDstData[ dstIndex + i - iDstOffsetX] &= ~pPeerData[peerIndex + i - iPeerOffsetX];
                            //if (( dstIndex + i) > iDstRange)
                            //    printf("Subtract dst out of heap \n");
                            //if ((peerIndex + i) > iPeerRange)
                            //    printf("Subtract peer out of heap \n");
                        }
                    }
                    if (combinationMode == XOR) {
                        for (i = iPeerBegin; i < iPeerEnd; i++) {
                            pDstData[ dstIndex + i  - iDstOffsetX] ^= pPeerData[peerIndex + i - iPeerOffsetX];
                            //if (( dstIndex + i) > iDstRange)
                            //    printf("Intersect/XOR dst out of heap \n");
                            //if ((peerIndex + i) > iPeerRange)
                            //    printf("Intersect/XOR peer out of heap \n");
                        }
                    }
                }

                if(combinationMode == Intersect) {
                    int dstIndex = k * iDstSizeX * iDstSizeY + j * iDstSizeX;
                    for (i = 0; i < iDstSizeX; i++) {
                        pDstData[dstIndex] /= 2;
                        //if (dstIndex > iDstRange)
                        //    printf("Intersect dst out of heap4 \n");
                        dstIndex++;
                    }
                }
                //else if(iCombinationMode == 3) {
                //    int dstIndex = k * iDstSizeX * iDstSizeY + j * iDstSizeX;
                //    for (i = 0; i < iDstSizeX; i++) {
                //        pDstData[dstIndex] &= 1;
                //        //if (dstIndex > iDstRange)
                //        //    printf("XOR dst out of heap5 \n");
                //        dstIndex++;
                //    }
                //}
            }
        }

        // only in src voi
        else if(bSliceInSrc) {
            // A (union) 0 = A, A (intersect) 0 = 0, A (subtract) 0 = A, A (xor) 0 = A
            if (combinationMode == Union || combinationMode == Subtract || combinationMode == XOR) {
                for (j = 0; j < iSizeY; j++) {
                    memcpy (pDstData + k * iDstSizeX * iDstSizeY + (j + iOffsetY - iDstOffsetY) * iDstSizeX + iOffsetX - iDstOffsetX,
                        pSrcData + kSrc * iSizeX * iSizeY + j * iSizeX,
                        iSizeX);
                }
            }
        }

        // only in 2nd voi
        else if(bSliceInPeer) {   
            // 0 (union) B = B, 0 (intersect) B = 0, 0 (subtract) B = 0, 0 (xor) B = B
            if (combinationMode == Union || combinationMode == XOR) {
                for (j = 0; j < iPeerSizeY; j++) {
                    memcpy (pDstData + k * iDstSizeX * iDstSizeY + (j + iPeerOffsetY - iDstOffsetY) * iDstSizeX + iPeerOffsetX - iDstOffsetX,
                        pPeerData + kPeer * iPeerSizeX * iPeerSizeY + j * iPeerSizeX,
                        iPeerSizeX);
                }
            }
        }
    } // end loop of z

    //if (!IsSubvolumeEmpty(pDstVolume))
    //{
        //SubVolumeHelperAPI::RefineSubvolume(pDstVolume);
    //}
    TEST_PERFORMANCE_END("GenerateVoiViaCombination");
    return 0;//ChopSubvolumeToFitImage(pDstVolume, strImageUID);
}

int TpsVoiHelper::GenerateVoiFromDose(
    TpsImage3DEntity* pImage3D, 
    RtDosegrid* pDoseGrid, 
    float fDoseMin, 
    float fDoseMax, 
    McsfGeometry::SubVolume* sv)
{
    TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;

    if (pImage3D == nullptr || pDoseGrid == nullptr || 
        sv == nullptr || fDoseMin - fDoseMax >= -1e-6)
    {
        TPS_LOG_DEV_ERROR<<"GenerateVoiFromDose_ error, invalid parameters.";
        return TPS_ER_INPUT_INVALID;
    }

    RtImage3DHeader* header = nullptr;
    pImage3D->GetImage3DHeader(header);
    if (header == nullptr)
    {
        return TPS_ER_INPUT_INVALID;
    }

    int gridSize[3] = 
    {
        pDoseGrid->get_xcount(), pDoseGrid->get_ycount(), pDoseGrid->get_zcount()
    };
    bool hasValidValue = false;
    float* gridBuff = pDoseGrid->get_dosegrid_buffer();
    for (int i = 0; i < gridSize[0] * gridSize[1] * gridSize[2]; ++i)
    {
        if (gridBuff[i] >= fDoseMin && gridBuff[i] <= fDoseMax)
        {
            hasValidValue = true;
            break;
        }
    }
    if (!hasValidValue)
    {
        sv->SetOffSet(0, 0, 0);
        sv->SetSize(0, 0, 0);
        return TPS_ER_SUCCESS;
    }

    int volSize[3] = 
    {
        header->m_iXDim, header->m_iYDim, header->m_iSliceCount
    };

    Mcsf::Matrix4f matV2P;
    pImage3D->GetVolumeToPatientMatrix(matV2P);
    auto matG2P = TpsArithmeticConverter::ConvertToMatrix4f(
        pDoseGrid->get_grid_to_pat_t());
    auto matG2V = matG2P;
    matG2V.Prepend(matV2P.Inverse());

    int bdry[6] = {volSize[0] - 1, 0, volSize[1] - 1, 0, volSize[2] - 1, 0};
    Mcsf::Point3f vertices[8] = 
    {
        Mcsf::Point3f(-0.5, -0.5, -0.5),
        Mcsf::Point3f(-0.5, -0.5, gridSize[2] - 0.5),
        Mcsf::Point3f(-0.5, gridSize[1] - 0.5, -0.5),
        Mcsf::Point3f(-0.5, gridSize[1] - 0.5, gridSize[2] - 0.5),
        Mcsf::Point3f(gridSize[0] - 0.5, -0.5, -0.5),
        Mcsf::Point3f(gridSize[0] - 0.5, -0.5, gridSize[2] - 0.5),
        Mcsf::Point3f(gridSize[0] - 0.5, gridSize[1] - 0.5, -0.5),
        Mcsf::Point3f(gridSize[0] - 0.5, gridSize[1] - 0.5, gridSize[2] - 0.5)
    };
    for (int i = 0; i < 8; ++i)
    {
        vertices[i] = matG2V.Transform(vertices[i]);
        bdry[0] = TPS_CLAMP(floor(vertices[i][0]), 0, bdry[0]);
        bdry[1] = TPS_CLAMP(ceil(vertices[i][0]), bdry[1], volSize[0] - 1);
        bdry[2] = TPS_CLAMP(floor(vertices[i][1]), 0, bdry[2]);
        bdry[3] = TPS_CLAMP(ceil(vertices[i][1]), bdry[3], volSize[1] - 1);
        bdry[4] = TPS_CLAMP(floor(vertices[i][2]), 0, bdry[4]);
        bdry[5] = TPS_CLAMP(ceil(vertices[i][2]), bdry[5], volSize[2] - 1);
    }
    int svSize[3] = 
    {
        bdry[1] - bdry[0] + 1,
        bdry[3] - bdry[2] + 1,
        bdry[5] - bdry[4] + 1
    };
    sv->SetOffSet(bdry[0], bdry[2], bdry[4]);
    sv->SetSize(svSize[0], svSize[1], svSize[2]);
    int totalSize = svSize[0] * svSize[1] * svSize[2];
    char* buffer = sv->GetData();
    memset(buffer, 0, totalSize * sizeof(char));

    Mcsf::Matrix4f matSV2G;
    matSV2G.SetIdentity();
    matSV2G.SetCol3(Mcsf::Vector4f(bdry[0], bdry[2], bdry[4], 1.f));
    matSV2G.Prepend(matG2V.Inverse());

    int interSize[3] = 
    {
        svSize[0] + 2, svSize[1] + 2, svSize[2] + 2
    };
    float* interBuff = new float[interSize[0] * interSize[1] * interSize[2]];

    TEST_PERFORMANCE_BEGIN;
    Mcsf::Point3f p0, p1;
    int planeSize = interSize[0] * interSize[1];
#pragma omp parallel for private(p0, p1)
    for (int z = 0; z < interSize[2]; ++z)
    {
        p1.SetZ(z-1);
        for (int y = 0; y < interSize[1]; ++y)
        {
            p1.SetY(y-1);
            for (int x = 0; x < interSize[0]; ++x)
            {
                p1.SetX(x-1);
                p0 = matSV2G.Transform(p1);
                interBuff[x + y * interSize[0] + z * planeSize] = 
                    TrilinearInterpolate(gridBuff, gridSize, p0);
            }
        }
    }
    TEST_PERFORMANCE_END("TrilinearInterpolate LOOP");

    int shifts[26] = 
    {
        -1-interSize[0], -interSize[0], -interSize[0]+1, 
        -1, 1, 
        -1+interSize[0], interSize[0], interSize[0]+1,

        -1-interSize[0]-planeSize, -interSize[0]-planeSize, -interSize[0]+1-planeSize, 
        -1-planeSize, -planeSize, 1-planeSize, 
        -1+interSize[0]-planeSize, interSize[0]-planeSize, interSize[0]+1-planeSize,

        -1-interSize[0]+planeSize, -interSize[0]+planeSize, -interSize[0]+1+planeSize, 
        -1+planeSize, planeSize, 1+planeSize, 
        -1+interSize[0]+planeSize, interSize[0]+planeSize, interSize[0]+1+planeSize,
    };

    int idxDose, idxSV;
    float dose0, dose1;
    bool smallerThanMin;
    int svPlaneSize = svSize[0] * svSize[1];
    TEST_PERFORMANCE_BEGIN;
#pragma omp parallel for private(idxDose, idxSV, dose0, dose1, smallerThanMin)
    for (int z = 1; z <= svSize[2]; ++z)
    {
        for (int y = 1; y <= svSize[1]; ++y)
        {
            for (int x = 1; x <= svSize[0]; ++x)
            {
                idxDose = z * planeSize + y * interSize[0] + x;
                dose0 = interBuff[idxDose];
                if (fabs(dose0 - INVALID_DOSE_VALUE) <= 1e-6)
                {
                    continue;
                }

                idxSV = (z - 1) * svPlaneSize + (y - 1) * svSize[0] + x - 1;
                if (dose0 >= fDoseMin && dose0 <= fDoseMax)
                {
                    buffer[idxSV] = 1;
                    continue;
                }

                smallerThanMin = dose0 < fDoseMin;
                for (int i = 0; i < 26; ++i)
                {
                    dose1 = interBuff[idxDose + shifts[i]];
                    if (fabs(dose1 - INVALID_DOSE_VALUE) <= 1e-6 || (
                        dose1 >= fDoseMin && dose1 <= fDoseMax))
                    {
                        continue;
                    }

                    if (smallerThanMin && dose1 > fDoseMax && 
                        (fDoseMin - dose0) < (dose1 - fDoseMax))
                    {
                        buffer[idxSV] = 1;
                        break;
                    }
                    if (!smallerThanMin && dose1 < fDoseMin && 
                        (dose0 - fDoseMax) < (fDoseMin - dose1))
                    {
                        buffer[idxSV] = 1;
                        break;
                    }
                }
            }
        }
    }
    TEST_PERFORMANCE_END("SET ROI LOOP");

    TPS_DEL_ARRAY(interBuff);
    //TEST_PERFORMANCE_END("GenerateVoiFromDose");
    return TPS_ER_SUCCESS;
}

float TpsVoiHelper::TrilinearInterpolate(float* buffer, int dim[3], const Mcsf::Point3f& point)
{
    float x = point.GetX();
    float y = point.GetY();
    float z = point.GetZ();
    if (x < -0.5 || x > dim[0] - 0.5 || 
        y < -0.5 || y > dim[1] - 0.5 || 
        z < -0.5 || z > dim[2] - 0.5)
    {
        return INVALID_DOSE_VALUE;
    }

    int x0 = TpsUtility::Floor(x);
    int y0 = TpsUtility::Floor(y);
    int z0 = TpsUtility::Floor(z);
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    int z1 = z0 + 1;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (z0 < 0) z0 = 0;
    if (x1 >= dim[0]) x1 = dim[0] - 1;
    if (y1 >= dim[1]) y1 = dim[1] - 1;
    if (z1 >= dim[2]) z1 = dim[2] - 1;
    float dx = x - x0;
    float dy = y - y0;
    float dz = z - z0;

    float v000 = buffer[z0 * dim[0] * dim[1] + y0 * dim[0] + x0];
    float v100 = buffer[z0 * dim[0] * dim[1] + y0 * dim[0] + x1];
    float v110 = buffer[z0 * dim[0] * dim[1] + y1 * dim[0] + x1];
    float v010 = buffer[z0 * dim[0] * dim[1] + y1 * dim[0] + x0];
    float v001 = buffer[z1 * dim[0] * dim[1] + y0 * dim[0] + x0];
    float v101 = buffer[z1 * dim[0] * dim[1] + y0 * dim[0] + x1];
    float v111 = buffer[z1 * dim[0] * dim[1] + y1 * dim[0] + x1];
    float v011 = buffer[z1 * dim[0] * dim[1] + y1 * dim[0] + x0];

    float v0 = (v000 * (1-dx) + v100 * dx) * (1-dy) + (v010 * (1-dx) + v110 * dx) * dy;
    float v1 = (v001 * (1-dx) + v101 * dx) * (1-dy) + (v011 * (1-dx) + v111 * dx) * dy;
    return v0 * (1-dz) + v1 * dz;
}

int TpsVoiHelper::ChopSubvolumeToFitImage(McsfGeometry::SubVolume *pSubvolume, TpsImage3DEntity* pImage3D) 
{
    if (nullptr == pImage3D){
        TPS_LOG_DEV_ERROR<<"TpsVoiHelper::ChopSubvolumeToFitImage(): Failed to get image";
        return -1;
    }

    RtImage3DHeader* pImageHeader = nullptr;
    if (!pImage3D->GetImage3DHeader(pImageHeader) || pImageHeader == nullptr){
        TPS_LOG_DEV_ERROR<<"TpsVoiHelper::ChopSubvolumeToFitImage(): Failed to get header of image: "<<pImage3D->GetUID();
        return -1;
    }

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    int xDim = pImageHeader->m_iXDim;
    int yDim = pImageHeader->m_iYDim;
    int zDim = pImageHeader->m_iSliceCount;

    int iSizeX = 0, iSizeY = 0, iSizeZ = 0;
    pSubvolume->GetSize(iSizeX, iSizeY, iSizeZ);
    int iOffsetX = 0, iOffsetY = 0, iOffsetZ = 0;
    pSubvolume->GetOffSet(iOffsetX, iOffsetY, iOffsetZ);

    if (iOffsetX >= 0 && iOffsetY >=0 && iOffsetZ >= 0 && 
        (iOffsetX + iSizeX) <= xDim && (iOffsetY + iSizeY) <= yDim && (iOffsetZ + iSizeZ) <= zDim)
    {
        // good work
        return 0;
    }

    if (iOffsetX >= xDim || iOffsetY >= yDim || iOffsetZ >= zDim)
    {
        // complete out of range
        return -1;
    }

    int iXStart = iOffsetX > 0 ? iOffsetX : 0;
    int iYStart = iOffsetY > 0 ? iOffsetY : 0;
    int iZStart = iOffsetZ > 0 ? iOffsetZ : 0;

    int iX = min(xDim, iSizeX + iOffsetX) - iXStart;
    int iY = min(yDim, iSizeY + iOffsetY) - iYStart;
    int iZ = min(zDim, iSizeZ + iOffsetZ) - iZStart;
    if (iX < 0 || iY < 0 || iZ < 0)
    {
        // complete out of range
        return -1;
    }
    unsigned char* pBuffer = (unsigned char*)malloc(iX * iY * iZ * sizeof(unsigned char));
    unsigned char* pSrcBuffer = (unsigned char*)pSubvolume->GetData();

    int k = 0, j = 0, iSrcIndex = 0;
    for (k = 0; k < iZ; k++)
    {
        for (j = 0; j < iY; j++)
        {
            iSrcIndex = (k + iZStart - iOffsetZ) * iSizeX * iSizeY + (j + iYStart - iOffsetY) * iSizeX + iXStart - iOffsetX;
            memcpy(pBuffer + k * iX * iY + j * iX, 
                   pSrcBuffer + iSrcIndex,
                   iX);
        }
    }

    pSubvolume->SetSize(iX, iY, iZ);
    pSubvolume->SetOffSet(iXStart, iYStart, iZStart);
    memcpy(pSubvolume->GetData(), pBuffer, iX * iY * iZ * sizeof(unsigned char));
    free(pBuffer);
    TEST_PERFORMANCE_END("ChopSubvolumeToFitImage");
    return 0;
}

int TpsVoiHelper::PopulateVoiContourGivenSubvolume(ROIEntity* /*pVoi*/, const TpsImage3DEntity* /*pImage3D*/, RtCt2density* /*ct2density*/) {
    // Get image information
    //std::string strImageUID = pVoi->GetSeriesUid();

    //if (nullptr == pImage3D){
    //    TPS_LOG_DEV_ERROR<<"no image3d with the image UID:"<<strImageUID;
    //    return -1;
    //}

    //Mcsf::Matrix4f matP2V;
    //if (!pImage3D->GetVolumeToPatientMatrix(matP2V)){
    //    TPS_LOG_DEV_ERROR<<"Failed to get volume to patient matrix of image:"<<strImageUID;
    //    return -1;
    //}
    //matP2V = matP2V.Inverse();
    ////float* matData = nullptr;
    //float matData[16];
    //memset(matData, 0, sizeof(matData));
    //matP2V.GetElements(matData);
    //pVoi->SetPat2VolumeMatrix(matData);

    //RtImage3DHeader* header = nullptr;
    //if (!pImage3D->GetImage3DHeader(header) || header == nullptr){
    //    TPS_LOG_DEV_ERROR<<"Failed to get image header of image: "<<strImageUID;
    //    return -1;
    //}

    //McsfGeometry::SubVolume *pVolume = pVoi->GetSubVolume();
    //if (pVolume == nullptr) {
    //    TPS_LOG_DEV_ERROR << "PopulateVoiContourGivenSubvolume error: failed to find subvolue of VOI.";
    //    return -1;
    //}

    //TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;
    //{
    //    TpsSafeLock<VOIEntity> lock(pVoi);
    //    pVoi->InitializeAna(header->m_iSliceCount);
    //}
    //TEST_PERFORMANCE_END("InitializeAna");

    //int iSizeX = 0, iSizeY = 0, iSizeZ = 0;
    //pVolume->GetSize(iSizeX, iSizeY, iSizeZ);
    //int iOffsetX = 0, iOffsetY = 0, iOffsetZ = 0;
    //pVolume->GetOffSet(iOffsetX, iOffsetY, iOffsetZ);

    //int z1 = iOffsetZ;
    //int z2 = iSizeZ;
    //z2 += z1;
    ////for(int i = z1 + 1; i <= z2; ++i) {
    ////    //plus 1 in the AddVoiContourSlices function
    ////    pVoi->AddVoiContourSlices(i - 1);
    ////}

    //TEST_PERFORMANCE_BEGIN;
    ////create a thread to generate contours of all slices
    //{
    //    TpsSafeLock<VOIEntity> lockAna(pVoi);
    //    pVoi->SetSlicesDirtyForAna(z1, z2 -1);
    //}
    //TEST_PERFORMANCE_END("SetSlicesDirtyForAna");

    //TEST_PERFORMANCE_BEGIN;
    //if (!SubVolumeHelperAPI::UpdateDirtySlicesOfAnastruct(matP2V.Inverse(), pVoi)){
    //    TPS_LOG_DEV_ERROR<<"Failed to update ana-struct of VOI: "<<pVoi->GetVoiUid();
    //    TEST_PERFORMANCE_END("UpdateDirtySlicesOfAnastruct");
    //    return -1;
    //}
    //TEST_PERFORMANCE_END("UpdateDirtySlicesOfAnastruct");

    //TEST_PERFORMANCE_BEGIN;
    ////update voi statistical information
    //if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(ct2density, *pImage3D, pVoi)){
    //    TPS_LOG_DEV_ERROR<<"Failed to update statistical information.";
    //    TEST_PERFORMANCE_END("UpdateVOIStatisticalInformation");
    //    return -1;
    //}
    //TEST_PERFORMANCE_END("UpdateVOIStatisticalInformation");
    return 0;
}

bool TpsVoiHelper::IsSubvolumeEmpty(McsfGeometry::SubVolume *pVolume)
{
    if (pVolume == nullptr)
    {
        return true;
    }

    int iSizeX = 0, iSizeY = 0, iSizeZ = 0;
    pVolume->GetSize(iSizeX, iSizeY, iSizeZ);
    if (iSizeX == 0 || iSizeY == 0 || iSizeZ == 0) 
    {
        return true;
    }

    char* pBuffer = pVolume->GetData();
    int nBytesOfSlice = iSizeX * iSizeY;
    char* tmpBuffer = new char[nBytesOfSlice];
    memset(tmpBuffer, 0, nBytesOfSlice * sizeof(char));
    for (int i = 0; i < iSizeZ; i++)
    {
        if(memcmp(pBuffer + i * nBytesOfSlice, tmpBuffer, sizeof(char) * nBytesOfSlice) != 0)
        {
            delete[] tmpBuffer;
            return false;
        }
    }
    
    delete[] tmpBuffer;
    return true;
}

bool TpsVoiHelper::ConvertSeriesTableToVoi(
	const RtTable& rtTable,
	const TpsImage3DEntity& image3D,
	const SeriesTable& seriesTable, ROIEntity* & voiEntity)
{
    rtTable;
    image3D;
    seriesTable;
    voiEntity;

    return true;

	//Mcsf::Matrix4f matVolToPat;
	//if (!image3D.GetVolumeToPatientMatrix(matVolToPat))
	//{
	//	TPS_LOG_DEV_ERROR << "Failed to get volume to patient matrix.";
	//	return false;
	//}

	//if (voiEntity == nullptr)
	//{
	//	TPS_LOG_DEV_ERROR << "voiEntity is null.";
	//	return false;
	//}

	//voiEntity->SetSeriesUid(image3D.GetUID());
	//voiEntity->SetVisibility(true);
	//voiEntity->SetIsQA(false);
	//voiEntity->SetPlanningRole(RtDbDef::PLANNING_ROLE_SUPPORT);
	//voiEntity->SetSubRole(RtDbDef::SUBROLE_TABLE);
	//voiEntity->SetLockType(RtDbDef::NORMALLOCK);
	//voiEntity->SetIsOverrided(true);
	//voiEntity->SetOverrideMassDensity(seriesTable.GetDensity());
	//voiEntity->SetOverrideReferenceEDensity(seriesTable.GetReferenceEDensity());

	//Mcsf::Matrix4f matPat2Vol = matVolToPat.Inverse();
	//float matrixArray[16];
	//matPat2Vol.GetElements(matrixArray);
	//voiEntity->SetPat2VolumeMatrix(matrixArray);

	//Mcsf::Matrix4f matTblToPat = seriesTable.GetTableToPatient(rtTable.GetCenter());
	//Mcsf::Matrix4f matTblVolToPatVol = rtTable.GetTableToMaskVolume().Inverse();
	//matTblVolToPatVol.Prepend(matTblToPat);
	//matTblVolToPatVol.Prepend(matVolToPat.Inverse());

	////get the bounding box of table volume in patient volume
	//unsigned char* tblMask = rtTable.GetMask();
	//unsigned int dim[3];
	//rtTable.GetMaskDim(dim);
	//Mcsf::Point3f vertices[8] =
	//{
	//	Mcsf::Point3f(0.f, 0.f, 0.f),
	//	Mcsf::Point3f(dim[0] - 1, 0.f, 0.f),
	//	Mcsf::Point3f(dim[0] - 1, dim[1] - 1, 0.f),
	//	Mcsf::Point3f(0.f, dim[1] - 1, 0.f),
	//	Mcsf::Point3f(0.f, 0.f, dim[2] - 1),
	//	Mcsf::Point3f(dim[0] - 1, 0.f, dim[2] - 1),
	//	Mcsf::Point3f(dim[0] - 1, dim[1] - 1, dim[2] - 1),
	//	Mcsf::Point3f(0.f, dim[1] - 1, dim[2] - 1)
	//};
	//float tblBdry[6] =
	//{
	//	std::numeric_limits<float>::max(),
	//	std::numeric_limits<float>::lowest(),
	//	std::numeric_limits<float>::max(),
	//	std::numeric_limits<float>::lowest(),
	//	std::numeric_limits<float>::max(),
	//	std::numeric_limits<float>::lowest()
	//};
	//for (int i = 0; i < 8; ++i)
	//{
	//	vertices[i] = matTblVolToPatVol.Transform(vertices[i]);
	//	tblBdry[0] = TPS_MIN(vertices[i].GetX(), tblBdry[0]);
	//	tblBdry[1] = TPS_MAX(vertices[i].GetX(), tblBdry[1]);
	//	tblBdry[2] = TPS_MIN(vertices[i].GetY(), tblBdry[2]);
	//	tblBdry[3] = TPS_MAX(vertices[i].GetY(), tblBdry[3]);
	//	tblBdry[4] = TPS_MIN(vertices[i].GetZ(), tblBdry[4]);
	//	tblBdry[5] = TPS_MAX(vertices[i].GetZ(), tblBdry[5]);
	//}

	//size_t image3DDim[3];
	//image3D.GetDimension(image3DDim);

	////for sub-volume definition, we need leave one slice of "0" around the sub-volume 
	////at each direction if not out of original range
	//tblBdry[0] = TPS_CLAMP(tblBdry[0] - 1, 0, image3DDim[0] - 1);
	//tblBdry[1] = TPS_CLAMP(tblBdry[1] + 1, 0, image3DDim[0] - 1);
	//tblBdry[2] = TPS_CLAMP(tblBdry[2] - 1, 0, image3DDim[1] - 1);
	//tblBdry[3] = TPS_CLAMP(tblBdry[3] + 1, 0, image3DDim[1] - 1);
	//tblBdry[4] = TPS_CLAMP(tblBdry[4] - 1, 0, image3DDim[2] - 1);
	//tblBdry[5] = TPS_CLAMP(tblBdry[5] + 1, 0, image3DDim[2] - 1);

	//int intBdry[6] =
	//{
	//	static_cast<int>(floor(tblBdry[0])),
	//	static_cast<int>(ceil(tblBdry[1])),
	//	static_cast<int>(floor(tblBdry[2])),
	//	static_cast<int>(ceil(tblBdry[3])),
	//	static_cast<int>(floor(tblBdry[4])),
	//	static_cast<int>(ceil(tblBdry[5]))
	//};
	//int svSize[3] = {
	//	intBdry[1] - intBdry[0] + 1,
	//	intBdry[3] - intBdry[2] + 1,
	//	intBdry[5] - intBdry[4] + 1 };
	//int svOffset[3] = { intBdry[0], intBdry[2], intBdry[4] };


	//auto subVolume = voiEntity->GetSubVolume();
	//subVolume->SetSize(svSize[0], svSize[1], svSize[2]);
	//subVolume->SetOffSet(svOffset[0], svOffset[1], svOffset[2]);
	//auto svBuffer = (unsigned char*)subVolume->GetData();
	//memset(svBuffer, 0, svSize[0] * svSize[1] * svSize[2] * sizeof(unsigned char));

	//Mcsf::Matrix4f matPatVol2TblVol = matTblVolToPatVol.Inverse();
	//Mcsf::Point3f planeVerticesPatVol[4] =
	//{
	//	Mcsf::Point3f(intBdry[0], intBdry[2], 0),
	//	Mcsf::Point3f(intBdry[1], intBdry[2], 0),
	//	Mcsf::Point3f(intBdry[1], intBdry[3], 0),
	//	Mcsf::Point3f(intBdry[0], intBdry[3], 0)
	//};
	//Mcsf::Point3f planeVerticesTblVol[4];

	//RtImage3DHeader* header = nullptr;
	//if (!image3D.GetImage3DHeader(header) || header == nullptr)
	//{
	//	TPS_LOG_DEV_ERROR << "Failed to get image 3d header.";
	//	return false;
	//}

	//{
	//	TpsSafeLock<VOIEntity> lockAna(voiEntity);
	//	voiEntity->InitializeAna(header->m_iSliceCount);
	//}

	//int startSliceNumber = header->m_vScanInfoList[0].m_uiInstanceNumber;

	//unsigned char* targetBuffer;
	//std::vector<std::vector<Mcsf::Point2f>> vvContour;
	//std::vector<RtContour*> rtContours;
	//Mcsf::Point3f point;
	//for (int i = intBdry[4]; i <= intBdry[5]; ++i)
	//{
	//	for (int j = 0; j < 4; ++j)
	//	{
	//		planeVerticesPatVol[j].SetZ(i);
	//		planeVerticesTblVol[j] = matPatVol2TblVol.Transform(planeVerticesPatVol[j]);
	//	}
	//	targetBuffer = svBuffer + (i - intBdry[4]) * svSize[0] * svSize[1];
	//	if (TPS_ER_SUCCESS != SubVolumeHelperAPI::SliceSubVolume(tblMask, dim, planeVerticesTblVol,
	//		svSize[0], svSize[1], targetBuffer))
	//	{
	//		TPS_LOG_DEV_ERROR << "Failed to slice sub-volume.";
	//		for (int j = 0; j < rtContours.size(); ++j)
	//		{
	//			TPS_DEL_PTR(rtContours[j]);
	//		}
	//		return false;
	//	}
	//	vvContour.clear();
	//	if (TPS_ER_SUCCESS != SubVolumeHelperAPI::GetContoursFromMask(targetBuffer,
	//		svSize[0], svSize[1], vvContour))
	//	{
	//		TPS_LOG_DEV_ERROR << "Failed to get contours from mask.";
	//		for (int j = 0; j < rtContours.size(); ++j)
	//		{
	//			TPS_DEL_PTR(rtContours[j]);
	//		}
	//		return false;
	//	}
	//	if (vvContour.empty()) continue;

	//	TPS_ANASTRUCT* ana = voiEntity->GetAnastruct();
	//	int sliceIndex = i - 1 + startSliceNumber;
	//	int sliceContoursCount = vvContour.size();;
	//	ana->contour_count[sliceIndex] = sliceContoursCount;
	//	ana->contours[sliceIndex] = new TPS_CONTOUR_STC[sliceContoursCount];

	//	for (int j = 0; j < sliceContoursCount; ++j)
	//	{
	//		TpsUtility::RDPPolygonReduce(vvContour[j], 0.5f);
	//		if (vvContour[j].size() < 3) continue;

	//		TPS_CONTOUR_STC* contour = (ana->contours[sliceIndex] + j);
	//		int vertexCount = vvContour[j].size();

	//		contour->vertex_count = vertexCount;
	//		contour->slice_number = sliceIndex;

	//		contour->pt = new Mcsf::Point3f[vertexCount];
	//		for (int k = 0; k < vertexCount; ++k)
	//		{
	//			point = matVolToPat.Transform(Mcsf::Point3f(
	//				vvContour[j][k].GetX() + svOffset[0],
	//				vvContour[j][k].GetY() + svOffset[1], i));
	//			contour->pt[k].SetX(point.GetX());
	//			contour->pt[k].SetY(point.GetY());
	//			contour->pt[k].SetZ(point.GetZ());
	//		}
	//	}
	//}

	//return true;
}
TPS_END_NAMESPACE
