//////////////////////////////////////////////////////////////////////////
/// \def group Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object_base.h
///  \brief     Rt database base class
/// 
///
///  \version 1.0
///  \date    Oct. 11, 2016
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_BASE_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_BASE_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "boost/archive/xml_iarchive.hpp"
#include "boost/archive/xml_oarchive.hpp"
#include  "boost/serialization/array.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;


class RT_DB_EXPORT RtDatabaseObject
{
public:

    RtDatabaseObject();
    ~RtDatabaseObject();

    /////////////////////////////////////////////////////////////////
    ///  \brief         check one filed in database
    ///  \param[in]     unsigned int field
    ///  \param[out]    None
    ///  \return        bool 
    ///                 true -- has 
    ///                 false -- has not
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    bool has_field(unsigned int field) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief         disable one field in database
    ///                 enable_field will be called internal while setting a value
    ///  \param[in]     unsigned int field
    ///  \param[out]    None
    ///  \return        None 
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    void disable_field(unsigned int field);

    /////////////////////////////////////////////////////////////////
    ///  \brief         enable one field in database
    ///  \param[in]     unsigned int field
    ///  \param[out]    None
    ///  \return        None 
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    void enable_field(unsigned int field);

    /////////////////////////////////////////////////////////////////
    ///  \brief         disable all fields in database
    ///                 
    ///  \param[in]     unsigned int field
    ///  \param[out]    None
    ///  \return        None 
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    void disable_all_fields();

    /////////////////////////////////////////////////////////////////
    ///  \brief         check one filed is dirty or not 
    ///  \param[in]     unsigned int field
    ///  \param[out]    None
    ///  \return        bool 
    ///                 true    -- dirty 
    ///                 false   -- not dirty
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    bool is_dirty(unsigned int field) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief         check if any filed is dirty or not 
    ///  \param[in]     None
    ///  \param[out]    None
    ///  \return        bool 
    ///                 true    -- dirty 
    ///                 false   -- not dirty
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    bool is_any_dirty() const;

    /////////////////////////////////////////////////////////////////
    ///  \brief         set dirty for one field in database
    ///                 enable_field will be called internal while setting a value
    ///  \param[in]     unsigned int field, bool isdirty
    ///  \param[out]    None
    ///  \return        None 
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    void setdirty_field(unsigned int field, bool isdirty);

    /////////////////////////////////////////////////////////////////
    ///  \brief         set dirty for all fields in database
    ///            
    ///  \param[in]     bool isdirty
    ///  \param[out]    None
    ///  \return        None 
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    void setdirty_all_fields(bool isdirty);

    /////////////////////////////////////////////////////////////////
    ///  \brief         check one filed is nullable or not 
    ///  \param[in]     unsigned int field
    ///  \param[out]    None
    ///  \return        bool 
    ///                 true    -- not null 
    ///                 false   -- null
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    bool is_notnull(unsigned int field) const;

protected:

    RtDatabaseObject(const RtDatabaseObject& obj);

    RtDatabaseObject& operator = (const RtDatabaseObject& obj);

    void set_flags(unsigned int field_size);

    void set_field_null(char* field_null);
    void set_field_notnull(unsigned int field);

private:

	friend class boost::serialization::access;


	template<class Archive>
	void serialize(Archive &ar, const unsigned int version) {
		version;
		ar & BOOST_SERIALIZATION_NVP(m_field_size);
		if (Archive::is_loading())
		{
			m_flags_has = new char[m_field_size];
			m_flags_dirty = new char[m_field_size];
			m_flags_notnull = new char[m_field_size];
		}

		ar & boost::serialization::make_nvp(BOOST_PP_STRINGIZE(m_flags_has), boost::serialization::make_array(m_flags_has, m_field_size));
		ar & boost::serialization::make_nvp(BOOST_PP_STRINGIZE(m_flags_dirty), boost::serialization::make_array(m_flags_dirty, m_field_size));
		ar & boost::serialization::make_nvp(BOOST_PP_STRINGIZE(m_flags_notnull), boost::serialization::make_array(m_flags_notnull, m_field_size));
	}

    char*               m_flags_has;
    char*               m_flags_dirty;
    char*               m_flags_notnull;
    unsigned int        m_field_size;
};


RT_TPS_DATABASE_END_NAMESPACE
#endif