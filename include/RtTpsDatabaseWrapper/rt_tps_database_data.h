//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_data.h
///  \brief     basic data eg.point3f 
///
///  \version 1.0
///  \date    Nov. 2,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_DATA_H_
#define RT_TPS_DATABASE_DATA_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "boost/archive/xml_iarchive.hpp"
#include "boost/archive/xml_oarchive.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;


struct db_Vector3f;
struct RT_DB_EXPORT db_Point3f
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version) {
		version;
		ar & BOOST_SERIALIZATION_NVP(x);
		ar & BOOST_SERIALIZATION_NVP(y);
		ar & BOOST_SERIALIZATION_NVP(z);
	}
    float x;
    float y;
    float z;

    db_Point3f::db_Point3f() : x(0.f), y(0.f), z(0.f)
    {
    }

    db_Point3f::db_Point3f(const db_Point3f& pt3f)
        : x(pt3f.x), y(pt3f.y), z(pt3f.z)
    {

    }

    db_Point3f::db_Point3f(const float _x, const float _y, const float _z)
        : x(_x), y(_y), z(_z)
    {
    }

    db_Point3f& db_Point3f::operator = (const db_Point3f& pt3f)
    {
        if (this != &pt3f){
            x = pt3f.x;
            y = pt3f.y;
            z = pt3f.z;
        }
        return *this;
    }

    db_Vector3f db_Point3f::operator - (const db_Point3f& pt3f);
};

struct RT_DB_EXPORT db_Point3d
{
    double x;
    double y;
    double z;

    db_Point3d::db_Point3d()
    {
        x = 0.f; y = 0.f; z = 0.f;
    }

    db_Point3d::db_Point3d(double _x, double _y, double _z)
    {
        x = _x; y = _y; z = _z;
    }

    db_Point3d& db_Point3d::operator = (const db_Point3d& pt3d)
    {
        if (this != &pt3d){
            this->x = pt3d.x;
            this->y = pt3d.y;
            this->z = pt3d.z;
        }
        return *this;
    }
};

struct RT_DB_EXPORT db_Point2f
{
    float x;
    float y;

    db_Point2f::db_Point2f()
    {
        x = 0.f; y = 0.f;
    }

    db_Point2f::db_Point2f(float _x, float _y)
    {
        x = _x; y = _y;
    }

    db_Point2f& db_Point2f::operator = (const db_Point2f& pt2f)
    {
        if (this != &pt2f){
            this->x = pt2f.x;
            this->y = pt2f.y;
        }
        return *this;
    }
};

struct RT_DB_EXPORT db_Point2d
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive &ar, const unsigned int version) {
		version;
		ar & BOOST_SERIALIZATION_NVP(x);
		ar & BOOST_SERIALIZATION_NVP(y);
	}
    double x;
    double y;

    db_Point2d::db_Point2d()
    {
        x = 0.f; y = 0.f;
    }

    db_Point2d::db_Point2d(double _x, double _y)
    {
        x = _x; y = _y;
    }

    db_Point2d& db_Point2d::operator = (const db_Point2d& pt2d)
    {
        if (this != &pt2d){
            this->x = pt2d.x;
            this->y = pt2d.y;
        }
        return *this;
    }
};

struct RT_DB_EXPORT db_Vector3f
{
    float x;
    float y;
    float z;

    db_Vector3f::db_Vector3f() : x(0.f), y(0.f), z(0.f)
    {
    }

    db_Vector3f::db_Vector3f(const float _x, const float _y, const float _z)
        : x(_x), y(_y), z(_z)
    {
    }

    db_Vector3f::db_Vector3f(db_Point3f _3f)
        : x(_3f.x), y(_3f.y), z(_3f.z)
    {
    }

    db_Vector3f::db_Vector3f( const db_Vector3f& vec3f)
        : x(vec3f.x), y(vec3f.y), z(vec3f.z)
    {
    }

    db_Vector3f & db_Vector3f::operator =( const db_Vector3f &vec )
    {
        if (this == &vec)
        {
            return *this;
        }
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }

    db_Vector3f & db_Vector3f::operator =( const db_Point3f &pnt )
    {
        x = pnt.x;
        y = pnt.y;
        z = pnt.z;
        return *this;
    }

    const db_Vector3f db_Vector3f::operator -( const db_Vector3f &vec ) const
    {
        return db_Vector3f(this->x - vec.x, this->y - vec.y, this->z - vec.z);
    }

    float db_Vector3f::Magnitude() const;

    db_Vector3f db_Vector3f::GetNormalize()
    {
        float mag = this->Magnitude();
        return db_Vector3f(this->x / mag, this->y / mag, this->z / mag);
    }
};


RT_TPS_DATABASE_END_NAMESPACE
#endif