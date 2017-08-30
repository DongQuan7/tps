////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_fw_common_datastructure.h
/// 
///  \brief common data stucture
/// 
///  \version 1.0
/// 
///  \date    Aug.4,2014
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_COMMON_DATASTRUCTURE_H_
#define TPS_FW_COMMON_DATASTRUCTURE_H_


#include "boost/archive/xml_iarchive.hpp"
#include "boost/archive/xml_oarchive.hpp"
//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
//TPS
#include "tps_fw_defs.h"

#ifndef TPS_BITSALLOCATED
#define TPS_BITSALLOCATED          32
#endif

namespace Mcsf
{
    class Point3f;
}

TPS_BEGIN_NAMESPACE

typedef struct CONTOUR_struct
{
    int vertex_count;
    int slice_number;   /* number of associated CT slice, start with 0*/
    //::Mcsf::Point3f max;  /* extent of prism represented by this contour */
    //::Mcsf::Point3f min;
    ::Mcsf::Point3f* pt;

    CONTOUR_struct(): vertex_count(0), slice_number(-1), pt(nullptr){}
    ~CONTOUR_struct()
    {
        if (nullptr != pt) { delete[] pt; pt = nullptr; }
    }
    CONTOUR_struct(const CONTOUR_struct& contour)
    {
        pt = new ::Mcsf::Point3f[contour.vertex_count];
        vertex_count = contour.vertex_count;
        memcpy(pt, contour.pt, vertex_count * sizeof(::Mcsf::Point3f));

        slice_number = contour.slice_number;
    }

    CONTOUR_struct& operator =(const CONTOUR_struct& contour)
    {
        if (vertex_count != contour.vertex_count || pt == nullptr)
        {
            if (pt) delete[] pt;
            pt = new ::Mcsf::Point3f[contour.vertex_count];
        }
        vertex_count = contour.vertex_count;
        memcpy(pt, contour.pt, vertex_count * sizeof(::Mcsf::Point3f));

        slice_number = contour.slice_number;
        return *this;
    }
} TPS_CONTOUR_STC;

struct TPS_ANASTRUCT {
    int         slice_count;
    int*        contour_count;
    char*       is_dirty;
    TPS_CONTOUR_STC** contours;

    float       ana_to_pat[4][4];
    float       pat_to_ana[4][4];
    float       color[4];
    bool        is_visible;
    bool        is_shaded;
    bool        is_skin;

    void Clear(){
        if (contours) {
            int i = 0;
            while(i < slice_count){
                if (nullptr != contours[i]){
                    delete[] contours[i];
                    contours[i] = nullptr;
                }
                ++i;
            }
            delete[] contours;
            contours = nullptr;
        }
        if (nullptr != contour_count) {
            delete[] contour_count;
            contour_count = nullptr;
        }
        if (nullptr != is_dirty) {
            delete[] is_dirty;
            is_dirty = nullptr;
        }
        slice_count = 0;
    }
    TPS_ANASTRUCT(): slice_count(0), contour_count(nullptr), contours(nullptr),
        is_dirty(nullptr), is_visible(true), is_shaded(false), is_skin(false){}

    ~TPS_ANASTRUCT(){ Clear(); }

    TPS_ANASTRUCT(const TPS_ANASTRUCT& ana){
        slice_count = ana.slice_count;
        contour_count = new int[slice_count];
        memcpy(contour_count, ana.contour_count, slice_count * sizeof(int));
        is_dirty = new char[slice_count];
        memcpy(is_dirty, ana.is_dirty, slice_count * sizeof(char));
        contours = new TPS_CONTOUR_STC*[slice_count];
        for (int i = 0; i < slice_count; ++i){
            contours[i] = new TPS_CONTOUR_STC[contour_count[i]];
            for (int j = 0; j < contour_count[i]; ++j){
                contours[i][j] = ana.contours[i][j];
            }
        }

        is_visible = ana.is_visible;
        is_shaded = ana.is_shaded;
        is_skin = ana.is_skin;
        memcpy(ana_to_pat, ana.ana_to_pat, 16 * sizeof(float));
        memcpy(pat_to_ana, ana.pat_to_ana, 16 * sizeof(float));
        memcpy(color, ana.color, 4 * sizeof(float));
    }

    TPS_ANASTRUCT& operator =(const TPS_ANASTRUCT& ana){
        Clear();
        slice_count = ana.slice_count;
        contour_count = new int[slice_count];
        memcpy(contour_count, ana.contour_count, slice_count * sizeof(int));
        is_dirty = new char[slice_count];
        memcpy(is_dirty, ana.is_dirty, slice_count * sizeof(char));
        contours = new TPS_CONTOUR_STC*[slice_count];
        for (int i = 0; i < slice_count; ++i){
            contours[i] = new TPS_CONTOUR_STC[contour_count[i]];
            for (int j = 0; j < contour_count[i]; ++j){
                contours[i][j] = ana.contours[i][j];
            }
        }

        is_visible = ana.is_visible;
        is_shaded = ana.is_shaded;
        is_skin = ana.is_skin;
        memcpy(ana_to_pat, ana.ana_to_pat, 16 * sizeof(float));
        memcpy(pat_to_ana, ana.pat_to_ana, 16 * sizeof(float));
        memcpy(color, ana.color, 4 * sizeof(float));
        return *this;
    }

    bool TPS_ANASTRUCT::IsAnyDirty(){
        bool Ret = false;
        for (int i=0; i<slice_count; ++i){
            if (is_dirty[i]){
                Ret = true;
                break;
            }
        }
        return Ret;
    }

    int TPS_ANASTRUCT::GetTotalContoursCount(){
        int count = 0;
        for (int i=0; i<slice_count; ++i)
            count += contour_count[i];
        return count;
    }

};


class RtMachine;
class RtTray;
class RtApplicator;
class RtAccessory;
class RtCommissionedunit;
class RtMcphocommissionedunit;
class RtGoldenstt;
class RtAccessorychunk;
class RtContour;
class RtKernelweight;
class RtMeasureddata;
class RtWedgefactor;
class RtCommissionedunit_tray;

class MachineTemplate
{
public:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version) {
		version;
		ar & BOOST_SERIALIZATION_NVP(vMachine);
		ar & BOOST_SERIALIZATION_NVP(vTray);
		ar & BOOST_SERIALIZATION_NVP(vElectronApplicator);
		ar & BOOST_SERIALIZATION_NVP(vAccessory);
		ar & BOOST_SERIALIZATION_NVP(vCommissionUnit);
		ar & BOOST_SERIALIZATION_NVP(vMcphocommissionedunit);
		ar & BOOST_SERIALIZATION_NVP(vGoldsst);
		ar & BOOST_SERIALIZATION_NVP(vAccessoryChunk);
		ar & BOOST_SERIALIZATION_NVP(vContour);
		ar & BOOST_SERIALIZATION_NVP(vKernelWeight);
		ar & BOOST_SERIALIZATION_NVP(vMeasuerData);
		ar & BOOST_SERIALIZATION_NVP(vWedgeFactor);
		ar & BOOST_SERIALIZATION_NVP(vTrayFactor);
	}

	std::vector<RtMachine> vMachine;
	std::vector<RtTray> vTray;
	std::vector<RtApplicator> vElectronApplicator;
	std::vector<RtAccessory> vAccessory;
	std::vector<RtCommissionedunit> vCommissionUnit;
	std::vector<RtMcphocommissionedunit> vMcphocommissionedunit;
	std::vector<RtGoldenstt> vGoldsst;
	std::vector<RtAccessorychunk>  vAccessoryChunk;
	std::vector<RtContour> vContour;
	std::vector<RtKernelweight>  vKernelWeight;
	std::vector<RtMeasureddata>  vMeasuerData;
	std::vector<RtWedgefactor> vWedgeFactor;
	std::vector<RtCommissionedunit_tray> vTrayFactor;
};

TPS_END_NAMESPACE
#endif  //TPS_FW_COMMON_DATASTRUCTURE_H_
