//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_factory.h
///  \brief     rt tps database factory
///
///  \version 1.0
///  \date    July 10,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_FACTORY_H_
#define RT_TPS_DATABASE_FACTORY_H_

#include <string>
#include <memory>

#include "rt_tps_database_defs.h"
#include "rt_tps_database_interface.h"

//////////////////////////////////////////////////////////////////////////
RT_TPS_DATABASE_BEGIN_NAMESPACE;

/// \class IDatabaseFactory 
/// \brief create database wrapper  object 
///
///
/// \par  Usage of this class:
/// \code
///     Mcsf::IDatabaseFactory::IDBPointer m_pDBPointer
///     Mcsf::IDatabasePtr m_pDatabase
///         m_pDBPointer = Mcsf::IDatabaseFactory::Instance();
///         m_pDatabase = m_pDBPointer->CreateDBWrapper();
///         if(! m_pDatabase->Initialize())//Initialize
///         {
///             TPS_PRINTF_DEBUG("Cann't connect to database! \n");
///             return ;//connect to DB error!
///         }
/// \endcode
////
class RT_DB_EXPORT IDatabaseFactory
{
public:

    /////////////////////////////////////////////////////////////////
    ///  \brief Constructor
    ///
    ///  \param[in]     None
    ///  \param[out]   None
    ///  \return          None
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    IDatabaseFactory();

    /////////////////////////////////////////////////////////////////
    ///  \brief destructor
    ///
    ///  \param[in]     None
    ///  \param[out]   None
    ///  \return          None
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    virtual ~IDatabaseFactory();

    /////////////////////////////////////////////////////////////////
    ///  \brief Create database wrapper object
    ///     NOTE:  The user should not delete the pointer, as using boost::shared_ptr
    ///     IDatabase is not a security class, please new object in each thread.
    ///        All interfaces don't throw exceptions.
    ///  \param[in]     None
    ///  \param[out]   None
    ///  \return          IDatabasePtr
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    IRtDatabaseWrapper* CreateDBWrapper(void) const;


private:

   // RT_TPS_DATABASE_DISALLOW_COPY(IDatabaseFactory)
};

RT_TPS_DATABASE_END_NAMESPACE

#undef EXPORTLIBRARYDB

#endif 
    ///\}
