//////////////////////////////////////////////////////////////////////////
/// \def group Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object_base.cpp
///  \brief     Rt database base class
/// 
///
///  \version 1.0
///  \date    Oct. 11, 2016
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_base.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

//////////////////////////////////////////////////////////////////////////

RtDatabaseObject::RtDatabaseObject() 
    :m_flags_has(nullptr), 
    m_flags_dirty(nullptr), 
    m_flags_notnull(nullptr),
    m_field_size(0)
{

}

RtDatabaseObject::~RtDatabaseObject()
{
    if (nullptr != m_flags_has)
    {
        delete[] m_flags_has;
        m_flags_has = nullptr;
    }
    if (nullptr != m_flags_dirty)
    {
        delete[] m_flags_dirty;
        m_flags_dirty = nullptr;
    }    
    if (nullptr != m_flags_notnull)
    {
        delete[] m_flags_notnull;
        m_flags_notnull = nullptr;
    }
}

RtDatabaseObject::RtDatabaseObject(const RtDatabaseObject& obj):
    m_field_size(obj.m_field_size)
{
    if (m_field_size < 1)
    {
        return;
    }
    m_flags_has = new char[m_field_size];
    if (nullptr != m_flags_has && nullptr !=obj.m_flags_has)
    {
        memcpy(m_flags_has, obj.m_flags_has, sizeof(char) * m_field_size);
    }
    else
    {
        memset(m_flags_has, 0, sizeof(char) * m_field_size);
    }

    m_flags_dirty = new char[m_field_size];
    if (nullptr != m_flags_dirty && nullptr !=obj.m_flags_dirty)
    {
        memcpy(m_flags_dirty, obj.m_flags_dirty, sizeof(char) * m_field_size);
    }
    else
    {
        memset(m_flags_dirty, 0, sizeof(char) * m_field_size);
    }

    m_flags_notnull = new char[m_field_size];
    if (nullptr != m_flags_notnull && nullptr !=obj.m_flags_notnull)
    {
        memcpy(m_flags_notnull, obj.m_flags_notnull, sizeof(char) * m_field_size);
    }
    else
    {
        memset(m_flags_notnull, 0, sizeof(char) * m_field_size);
    }
}

RtDatabaseObject& RtDatabaseObject::operator = (const RtDatabaseObject& obj)
{
     if (this != &obj)
     {
         this->m_field_size = obj.m_field_size;
         if (nullptr != this->m_flags_has && nullptr !=obj.m_flags_has && this->m_field_size > 0)
         {
             memcpy(this->m_flags_has, obj.m_flags_has, sizeof(char) * this->m_field_size);
         }

         if (nullptr != this->m_flags_dirty && nullptr !=obj.m_flags_dirty && this->m_field_size > 0)
         {
             memcpy(this->m_flags_dirty, obj.m_flags_dirty, sizeof(char) * this->m_field_size);
         }

         if (nullptr != this->m_flags_notnull && nullptr !=obj.m_flags_notnull && this->m_field_size > 0)
         {
             memcpy(this->m_flags_notnull, obj.m_flags_notnull, sizeof(char) * this->m_field_size);
         }
     }
     return *this;
}

bool RtDatabaseObject::has_field(unsigned int field) const
{
    if (field >= m_field_size)
    {
        return false;
    }

    return 0 != m_flags_has[field];
}

void RtDatabaseObject::disable_field(unsigned int field)
{
    if (field < m_field_size)
    {
        if(0 == m_flags_notnull[field]) m_flags_has[field] = 0;
         m_flags_dirty[field] = 1;
    }
}

void RtDatabaseObject::disable_all_fields()
{
    if (m_field_size > 0)
    {
        //memset(m_flags_has, 0, sizeof(char) * m_field_size);
        for (unsigned int i(0); i < m_field_size; ++i)
        {
            if(0 == m_flags_notnull[i]) m_flags_has[i] = 0;
             m_flags_dirty[i] = 1;
        }
    }
}

void RtDatabaseObject::enable_field(unsigned int field)
{
    if (field < m_field_size)
    {
        m_flags_has[field] = 1;
        m_flags_dirty[field] = 1;
    }
}

void RtDatabaseObject::set_flags(unsigned int field_size)
{
    if (field_size < 1)
    {
        return;
    }
    m_field_size = field_size;

    if (nullptr != m_flags_has)
    {
        delete[] m_flags_has;
        m_flags_has = nullptr;
    }

    m_flags_has = new char[m_field_size];
    memset(m_flags_has, 0, sizeof(char) * m_field_size);

    if (nullptr != m_flags_dirty)
    {
        delete[] m_flags_dirty;
        m_flags_dirty = nullptr;
    }
    m_flags_dirty = new char[m_field_size];
    memset(m_flags_dirty, 1, sizeof(char) * m_field_size);

    if (nullptr != m_flags_notnull)
    {
        delete[] m_flags_notnull;
        m_flags_notnull = nullptr;
    }
    m_flags_notnull = new char[m_field_size];
    memset(m_flags_notnull, 0, sizeof(char) * m_field_size);
}


//////////////////////////////////////////////////////////////////////////
// dirty flags
bool RtDatabaseObject::is_dirty(unsigned int field) const
{
    if (field >= m_field_size)
    {
        return false;
    }

    return 0 != m_flags_dirty[field];
}

bool RtDatabaseObject::is_any_dirty() const
{
    for (unsigned int i(0); i < m_field_size; ++i)
    {
        if (0 != m_flags_dirty[i])
        {
            return true;
        } 
    }
    return false;
}

void RtDatabaseObject::setdirty_field(unsigned int field, bool isdirty)
{
    if (field < m_field_size)
    {
        m_flags_dirty[field] = isdirty ? 1 : 0;
    }
}

void RtDatabaseObject::setdirty_all_fields(bool isdirty)
{
    if (m_field_size > 0)
    {
        const int iDirty = isdirty ? 1 : 0 ;
        memset(m_flags_dirty, iDirty, sizeof(char) * m_field_size);
    }
}

//////////////////////////////////////////////////////////////////////////
// not null
void RtDatabaseObject::set_field_null(char* field_null)
{
    if (m_field_size > 0 && nullptr != field_null && nullptr != m_flags_notnull)
    {
        //memcpy(m_flags_notnull, field_null, sizeof(char) * m_field_size); wrong
        for (unsigned int i(0); i < m_field_size; ++i)
        {
            //m_flags_notnull[i] = '0' == field_null[i] ? 0 : 1;
            if ('0' == field_null[i])
            {
                m_flags_notnull[i] = 0;
            } 
            else
            {
                m_flags_notnull[i] = 1;
                m_flags_has[i] = 1;
            }
        }
    }
}

void RtDatabaseObject::set_field_notnull(unsigned int field)
{
    if (field < m_field_size)
    {                              
        m_flags_notnull[field] = 1; 
        m_flags_has[field] = 1;
    }
}

bool RtDatabaseObject::is_notnull(unsigned int field) const
{
    if (field >= m_field_size)
    {
        return false;
    }

    return 0 != m_flags_notnull[field];
}
RT_TPS_DATABASE_END_NAMESPACE