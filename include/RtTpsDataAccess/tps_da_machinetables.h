
#ifndef TPS_DA_MACHINETABLES_H_
#define TPS_DA_MACHINETABLES_H_


#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include "tps_da_defs.h"
#include "tps_da_machinetablebase.hpp"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_commissionedunit.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_tray.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_applicator.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mcphocommissionedunit.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_measuredprofile.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_wedgefactor.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_kernelweight.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_measureddata.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_commissionedunit_tray.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_goldenstt.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessorychunk.h"


TPS_BEGIN_NAMESPACE

class IRtDatabaseWrapper;
class RtMachine;
class RtCommissionedunit;
class RtTray;
class RtApplicator;
class RtAccessory;

class RtMcphocommissionedunit;
class RtMeasuredprofile;
class RtWedgefactor;
class RtKernelweight;
class RtMeasureddata;
class RtCommissionedunit_tray;
class RtGoldenstt;
class RtContour;
class RtAccessorychunk;

struct Tag_UniQueKey{};
struct Tag_NonUniqueKey0{};
struct Tag_NonUniqueKey1{};
struct Tag_NonUniqueKey2{};
struct Tag_NonUniqueKey3{};

#define COMPOSIT_KEY_ONE(CLASS,TYPE,FUN) boost::multi_index::composite_key<CLASS,boost::multi_index::const_mem_fun<CLASS, TYPE, FUN >>
#define COMPOSIT_KEY_TWO(CLASS,TYPE1,FUN1,TYPE2,FUN2) boost::multi_index::composite_key<CLASS,boost::multi_index::const_mem_fun<CLASS, TYPE1, FUN1 >,boost::multi_index::const_mem_fun<CLASS, TYPE2, FUN2 >>
#define COMPOSIT_KEY_THREE(CLASS,TYPE1,FUN1,TYPE2,FUN2,TYPE3,FUN3) boost::multi_index::composite_key<CLASS,boost::multi_index::const_mem_fun<CLASS, TYPE1, FUN1 >,boost::multi_index::const_mem_fun<CLASS, TYPE2, FUN2 >,boost::multi_index::const_mem_fun<CLASS, TYPE3, FUN3 >>

#define ORDERED_UNIQUE_ONE(TAG, CLASS,TYPE,FUN) boost::multi_index::ordered_unique<boost::multi_index::tag<TAG>, COMPOSIT_KEY_ONE(CLASS,TYPE,FUN)>
#define ORDERED_UNIQUE_TWO(TAG, CLASS,TYPE1,FUN1,TYPE2,FUN2) boost::multi_index::ordered_unique<boost::multi_index::tag<TAG>, COMPOSIT_KEY_TWO(CLASS,TYPE1,FUN1,TYPE2,FUN2)>
#define ORDERED_UNIQUE_THREE(TAG, CLASS,TYPE1,FUN1,TYPE2,FUN2,TYPE3,FUN3) boost::multi_index::ordered_unique<boost::multi_index::tag<TAG>, COMPOSIT_KEY_THREE(CLASS,TYPE1,FUN1,TYPE2,FUN2,TYPE3,FUN3)>

#define ORDERED_NON_UNIQUE_ONE(TAG, CLASS,TYPE,FUN) boost::multi_index::ordered_non_unique<boost::multi_index::tag<TAG>, COMPOSIT_KEY_ONE(CLASS,TYPE,FUN)>
#define ORDERED_NON_UNIQUE_TWO(TAG, CLASS,TYPE1,FUN1,TYPE2,FUN2) boost::multi_index::ordered_non_unique<boost::multi_index::tag<TAG>, COMPOSIT_KEY_TWO(CLASS,TYPE1,FUN1,TYPE2,FUN2)>
#define ORDERED_NON_UNIQUE_THREE(TAG, CLASS,TYPE1,FUN1,TYPE2,FUN2,TYPE3,FUN3) boost::multi_index::ordered_non_unique<boost::multi_index::tag<TAG>, COMPOSIT_KEY_THREE(CLASS,TYPE1,FUN1,TYPE2,FUN2,TYPE3,FUN3)>

typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtMachine, std::string, &RtMachine::get_uid)
> RtMachineQuery;
class TPS_DA_DECLSPEC RtMachineTable :public RtTableBase < RtMachine, RtMachineQuery > {
public:
    RtMachineTable();
};

typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtCommissionedunit, std::string, &RtCommissionedunit::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtCommissionedunit, std::string, &RtCommissionedunit::get_machineuid)
> RtCommissionedunitQuery;
class TPS_DA_DECLSPEC RtCommissionedunitTable :public RtTableBase < RtCommissionedunit, RtCommissionedunitQuery > {
public:
    RtCommissionedunitTable();
};

typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtTray, std::string, &RtTray::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtTray, std::string, &RtTray::get_machineuid)
> RtTrayQuery;
class TPS_DA_DECLSPEC RtTrayTable :public RtTableBase < RtTray, RtTrayQuery > {
public:
    RtTrayTable();
};

typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtApplicator, std::string, &RtApplicator::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtApplicator, std::string, &RtApplicator::get_machineuid)
> RtApplicatorQuery;
class TPS_DA_DECLSPEC RtApplicatorTable :public RtTableBase < RtApplicator, RtApplicatorQuery > {
public:
    RtApplicatorTable();
};
typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtAccessory, std::string, &RtAccessory::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtAccessory, std::string, &RtAccessory::get_machineuid),
    ORDERED_NON_UNIQUE_TWO(Tag_NonUniqueKey1, RtAccessory, std::string, &RtAccessory::get_machineuid, std::string, &RtAccessory::get_name)
> RtAccessoryQuery;
class TPS_DA_DECLSPEC RtAccessoryTable :public RtTableBase < RtAccessory, RtAccessoryQuery > {
public:
    RtAccessoryTable();
};

typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtMcphocommissionedunit, std::string, &RtMcphocommissionedunit::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtMcphocommissionedunit, std::string, &RtMcphocommissionedunit::get_commissionedunituid)
> RtMcphocommissionedunitQuery;
class TPS_DA_DECLSPEC RtMcphocommissionedunitTable :public RtTableBase < RtMcphocommissionedunit, RtMcphocommissionedunitQuery > {
public:
    RtMcphocommissionedunitTable();
};

typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtMeasuredprofile, std::string, &RtMeasuredprofile::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtMeasuredprofile, std::string, &RtMeasuredprofile::get_commissionedunituid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey1, RtMeasuredprofile, std::string, &RtMeasuredprofile::get_accessoryuid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey2, RtMeasuredprofile, std::string, &RtMeasuredprofile::get_applicatoruid)
> RtMeasuredprofileQuery;
class TPS_DA_DECLSPEC RtMeasuredprofileTable :public RtTableBase < RtMeasuredprofile, RtMeasuredprofileQuery > {
public:
    RtMeasuredprofileTable();
};
typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtWedgefactor, std::string, &RtWedgefactor::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtWedgefactor, std::string, &RtWedgefactor::get_commissionedunituid)
> RtWedgefactorQuery;
class TPS_DA_DECLSPEC RtWedgefactorTable :public RtTableBase < RtWedgefactor, RtWedgefactorQuery > {
public:
    RtWedgefactorTable();
};

typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtKernelweight, std::string, &RtKernelweight::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtKernelweight, std::string, &RtKernelweight::get_commissionedunituid)
> RtKernelweightQuery;
class TPS_DA_DECLSPEC RtKernelweightTable :public RtTableBase < RtKernelweight, RtKernelweightQuery > {
public:
    RtKernelweightTable();
};
typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtMeasureddata, std::string, &RtMeasureddata::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtMeasureddata, std::string, &RtMeasureddata::get_commissionedunituid)
> RtMeasureddataQuery;
class TPS_DA_DECLSPEC RtMeasureddataTable :public RtTableBase < RtMeasureddata, RtMeasureddataQuery > {
public:
    RtMeasureddataTable();
};

typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_TWO(Tag_UniQueKey, RtCommissionedunit_tray, std::string, &RtCommissionedunit_tray::get_commissionedunituid, std::string, &RtCommissionedunit_tray::get_trayuid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtCommissionedunit_tray, std::string, &RtCommissionedunit_tray::get_commissionedunituid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey1, RtCommissionedunit_tray, std::string, &RtCommissionedunit_tray::get_trayuid)
> RtCommissionedunit_trayQuery;
class TPS_DA_DECLSPEC RtCommissionedunit_trayTable :public RtTableBase < RtCommissionedunit_tray, RtCommissionedunit_trayQuery > {
public:
    RtCommissionedunit_trayTable();
};

typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtGoldenstt, std::string, &RtGoldenstt::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtGoldenstt, std::string, &RtGoldenstt::get_commissionedunituid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey1, RtGoldenstt, std::string, &RtGoldenstt::get_accessoryuid),
    ORDERED_NON_UNIQUE_TWO(Tag_NonUniqueKey2, RtGoldenstt, std::string, &RtGoldenstt::get_commissionedunituid, std::string, &RtGoldenstt::get_accessoryuid)
> RtGoldensttQuery;
class TPS_DA_DECLSPEC RtGoldensttTable :public RtTableBase < RtGoldenstt, RtGoldensttQuery > {
public:
    RtGoldensttTable();
};

typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtContour, std::string, &RtContour::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtContour, std::string, &RtContour::get_accessorychunkuid)
> RtContourQuery;
class TPS_DA_DECLSPEC RtContourTable :public RtTableBase < RtContour, RtContourQuery > {
public:
    RtContourTable();
};

typedef boost::multi_index::indexed_by <
    ORDERED_UNIQUE_ONE(Tag_UniQueKey, RtAccessorychunk, std::string, &RtAccessorychunk::get_uid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey0, RtAccessorychunk, std::string, &RtAccessorychunk::get_commissionedunituid),
    ORDERED_NON_UNIQUE_ONE(Tag_NonUniqueKey1, RtAccessorychunk, std::string, &RtAccessorychunk::get_accessoryuid),
    ORDERED_NON_UNIQUE_TWO(Tag_NonUniqueKey2, RtAccessorychunk, std::string, &RtAccessorychunk::get_accessoryuid, std::string, &RtAccessorychunk::get_commissionedunituid),
    ORDERED_NON_UNIQUE_THREE(Tag_NonUniqueKey3, RtAccessorychunk, std::string, &RtAccessorychunk::get_accessoryuid, std::string, &RtAccessorychunk::get_commissionedunituid, int, &RtAccessorychunk::get_algorithmtype)
> RtAccessorychunkQuery;
class TPS_DA_DECLSPEC RtAccessorychunkTable :public RtTableBase < RtAccessorychunk, RtAccessorychunkQuery > {
public:
    RtAccessorychunkTable();
};

TPS_END_NAMESPACE

#endif