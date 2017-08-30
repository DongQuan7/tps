// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rt_tps_unit.proto

#ifndef PROTOBUF_rt_5ftps_5funit_2eproto__INCLUDED
#define PROTOBUF_rt_5ftps_5funit_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
#include "rt_tps_applicator.pb.h"
#include "rt_tps_tray_factor.pb.h"
// @@protoc_insertion_point(includes)

namespace tps {
namespace proto {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_rt_5ftps_5funit_2eproto();
void protobuf_AssignDesc_rt_5ftps_5funit_2eproto();
void protobuf_ShutdownFile_rt_5ftps_5funit_2eproto();

class RT_TPS_Unit;

// ===================================================================

class RT_TPS_Unit : public ::google::protobuf::Message {
 public:
  RT_TPS_Unit();
  virtual ~RT_TPS_Unit();
  
  RT_TPS_Unit(const RT_TPS_Unit& from);
  
  inline RT_TPS_Unit& operator=(const RT_TPS_Unit& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const RT_TPS_Unit& default_instance();
  
  void Swap(RT_TPS_Unit* other);
  
  // implements Message ----------------------------------------------
  
  RT_TPS_Unit* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RT_TPS_Unit& from);
  void MergeFrom(const RT_TPS_Unit& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required string unitUid = 1;
  inline bool has_unituid() const;
  inline void clear_unituid();
  static const int kUnitUidFieldNumber = 1;
  inline const ::std::string& unituid() const;
  inline void set_unituid(const ::std::string& value);
  inline void set_unituid(const char* value);
  inline void set_unituid(const char* value, size_t size);
  inline ::std::string* mutable_unituid();
  inline ::std::string* release_unituid();
  
  // required string machineUid = 2;
  inline bool has_machineuid() const;
  inline void clear_machineuid();
  static const int kMachineUidFieldNumber = 2;
  inline const ::std::string& machineuid() const;
  inline void set_machineuid(const ::std::string& value);
  inline void set_machineuid(const char* value);
  inline void set_machineuid(const char* value, size_t size);
  inline ::std::string* mutable_machineuid();
  inline ::std::string* release_machineuid();
  
  // required string machineName = 3;
  inline bool has_machinename() const;
  inline void clear_machinename();
  static const int kMachineNameFieldNumber = 3;
  inline const ::std::string& machinename() const;
  inline void set_machinename(const ::std::string& value);
  inline void set_machinename(const char* value);
  inline void set_machinename(const char* value, size_t size);
  inline ::std::string* mutable_machinename();
  inline ::std::string* release_machinename();
  
  // required int32 radiationType = 4;
  inline bool has_radiationtype() const;
  inline void clear_radiationtype();
  static const int kRadiationTypeFieldNumber = 4;
  inline ::google::protobuf::int32 radiationtype() const;
  inline void set_radiationtype(::google::protobuf::int32 value);
  
  // repeated int32 techniqueType = 5;
  inline int techniquetype_size() const;
  inline void clear_techniquetype();
  static const int kTechniqueTypeFieldNumber = 5;
  inline ::google::protobuf::int32 techniquetype(int index) const;
  inline void set_techniquetype(int index, ::google::protobuf::int32 value);
  inline void add_techniquetype(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      techniquetype() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_techniquetype();
  
  // required string unitEnergy = 6;
  inline bool has_unitenergy() const;
  inline void clear_unitenergy();
  static const int kUnitEnergyFieldNumber = 6;
  inline const ::std::string& unitenergy() const;
  inline void set_unitenergy(const ::std::string& value);
  inline void set_unitenergy(const char* value);
  inline void set_unitenergy(const char* value, size_t size);
  inline ::std::string* mutable_unitenergy();
  inline ::std::string* release_unitenergy();
  
  // required int32 flatteningfiltermode = 7;
  inline bool has_flatteningfiltermode() const;
  inline void clear_flatteningfiltermode();
  static const int kFlatteningfiltermodeFieldNumber = 7;
  inline ::google::protobuf::int32 flatteningfiltermode() const;
  inline void set_flatteningfiltermode(::google::protobuf::int32 value);
  
  // required double gantry_min = 8;
  inline bool has_gantry_min() const;
  inline void clear_gantry_min();
  static const int kGantryMinFieldNumber = 8;
  inline double gantry_min() const;
  inline void set_gantry_min(double value);
  
  // required double gantry_max = 9;
  inline bool has_gantry_max() const;
  inline void clear_gantry_max();
  static const int kGantryMaxFieldNumber = 9;
  inline double gantry_max() const;
  inline void set_gantry_max(double value);
  
  // required double table_min = 10;
  inline bool has_table_min() const;
  inline void clear_table_min();
  static const int kTableMinFieldNumber = 10;
  inline double table_min() const;
  inline void set_table_min(double value);
  
  // required double table_max = 11;
  inline bool has_table_max() const;
  inline void clear_table_max();
  static const int kTableMaxFieldNumber = 11;
  inline double table_max() const;
  inline void set_table_max(double value);
  
  // required double collimator_min = 12;
  inline bool has_collimator_min() const;
  inline void clear_collimator_min();
  static const int kCollimatorMinFieldNumber = 12;
  inline double collimator_min() const;
  inline void set_collimator_min(double value);
  
  // required double collimator_max = 13;
  inline bool has_collimator_max() const;
  inline void clear_collimator_max();
  static const int kCollimatorMaxFieldNumber = 13;
  inline double collimator_max() const;
  inline void set_collimator_max(double value);
  
  // required double jaw_x1_min = 14;
  inline bool has_jaw_x1_min() const;
  inline void clear_jaw_x1_min();
  static const int kJawX1MinFieldNumber = 14;
  inline double jaw_x1_min() const;
  inline void set_jaw_x1_min(double value);
  
  // required double jaw_x1_max = 15;
  inline bool has_jaw_x1_max() const;
  inline void clear_jaw_x1_max();
  static const int kJawX1MaxFieldNumber = 15;
  inline double jaw_x1_max() const;
  inline void set_jaw_x1_max(double value);
  
  // required double jaw_x2_min = 16;
  inline bool has_jaw_x2_min() const;
  inline void clear_jaw_x2_min();
  static const int kJawX2MinFieldNumber = 16;
  inline double jaw_x2_min() const;
  inline void set_jaw_x2_min(double value);
  
  // required double jaw_x2_max = 17;
  inline bool has_jaw_x2_max() const;
  inline void clear_jaw_x2_max();
  static const int kJawX2MaxFieldNumber = 17;
  inline double jaw_x2_max() const;
  inline void set_jaw_x2_max(double value);
  
  // required double jaw_y1_min = 18;
  inline bool has_jaw_y1_min() const;
  inline void clear_jaw_y1_min();
  static const int kJawY1MinFieldNumber = 18;
  inline double jaw_y1_min() const;
  inline void set_jaw_y1_min(double value);
  
  // required double jaw_y1_max = 19;
  inline bool has_jaw_y1_max() const;
  inline void clear_jaw_y1_max();
  static const int kJawY1MaxFieldNumber = 19;
  inline double jaw_y1_max() const;
  inline void set_jaw_y1_max(double value);
  
  // required double jaw_y2_min = 20;
  inline bool has_jaw_y2_min() const;
  inline void clear_jaw_y2_min();
  static const int kJawY2MinFieldNumber = 20;
  inline double jaw_y2_min() const;
  inline void set_jaw_y2_min(double value);
  
  // required double jaw_y2_max = 21;
  inline bool has_jaw_y2_max() const;
  inline void clear_jaw_y2_max();
  static const int kJawY2MaxFieldNumber = 21;
  inline double jaw_y2_max() const;
  inline void set_jaw_y2_max(double value);
  
  // repeated .tps.proto.RT_TPS_Applicator applicators = 22;
  inline int applicators_size() const;
  inline void clear_applicators();
  static const int kApplicatorsFieldNumber = 22;
  inline const ::tps::proto::RT_TPS_Applicator& applicators(int index) const;
  inline ::tps::proto::RT_TPS_Applicator* mutable_applicators(int index);
  inline ::tps::proto::RT_TPS_Applicator* add_applicators();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Applicator >&
      applicators() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Applicator >*
      mutable_applicators();
  
  // repeated float doseRates = 23;
  inline int doserates_size() const;
  inline void clear_doserates();
  static const int kDoseRatesFieldNumber = 23;
  inline float doserates(int index) const;
  inline void set_doserates(int index, float value);
  inline void add_doserates(float value);
  inline const ::google::protobuf::RepeatedField< float >&
      doserates() const;
  inline ::google::protobuf::RepeatedField< float >*
      mutable_doserates();
  
  // repeated string trayuid = 24;
  inline int trayuid_size() const;
  inline void clear_trayuid();
  static const int kTrayuidFieldNumber = 24;
  inline const ::std::string& trayuid(int index) const;
  inline ::std::string* mutable_trayuid(int index);
  inline void set_trayuid(int index, const ::std::string& value);
  inline void set_trayuid(int index, const char* value);
  inline void set_trayuid(int index, const char* value, size_t size);
  inline ::std::string* add_trayuid();
  inline void add_trayuid(const ::std::string& value);
  inline void add_trayuid(const char* value);
  inline void add_trayuid(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& trayuid() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_trayuid();
  
  // repeated .tps.proto.RT_TPS_Tray_Factor tray_factor = 25;
  inline int tray_factor_size() const;
  inline void clear_tray_factor();
  static const int kTrayFactorFieldNumber = 25;
  inline const ::tps::proto::RT_TPS_Tray_Factor& tray_factor(int index) const;
  inline ::tps::proto::RT_TPS_Tray_Factor* mutable_tray_factor(int index);
  inline ::tps::proto::RT_TPS_Tray_Factor* add_tray_factor();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Tray_Factor >&
      tray_factor() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Tray_Factor >*
      mutable_tray_factor();
  
  // @@protoc_insertion_point(class_scope:tps.proto.RT_TPS_Unit)
 private:
  inline void set_has_unituid();
  inline void clear_has_unituid();
  inline void set_has_machineuid();
  inline void clear_has_machineuid();
  inline void set_has_machinename();
  inline void clear_has_machinename();
  inline void set_has_radiationtype();
  inline void clear_has_radiationtype();
  inline void set_has_unitenergy();
  inline void clear_has_unitenergy();
  inline void set_has_flatteningfiltermode();
  inline void clear_has_flatteningfiltermode();
  inline void set_has_gantry_min();
  inline void clear_has_gantry_min();
  inline void set_has_gantry_max();
  inline void clear_has_gantry_max();
  inline void set_has_table_min();
  inline void clear_has_table_min();
  inline void set_has_table_max();
  inline void clear_has_table_max();
  inline void set_has_collimator_min();
  inline void clear_has_collimator_min();
  inline void set_has_collimator_max();
  inline void clear_has_collimator_max();
  inline void set_has_jaw_x1_min();
  inline void clear_has_jaw_x1_min();
  inline void set_has_jaw_x1_max();
  inline void clear_has_jaw_x1_max();
  inline void set_has_jaw_x2_min();
  inline void clear_has_jaw_x2_min();
  inline void set_has_jaw_x2_max();
  inline void clear_has_jaw_x2_max();
  inline void set_has_jaw_y1_min();
  inline void clear_has_jaw_y1_min();
  inline void set_has_jaw_y1_max();
  inline void clear_has_jaw_y1_max();
  inline void set_has_jaw_y2_min();
  inline void clear_has_jaw_y2_min();
  inline void set_has_jaw_y2_max();
  inline void clear_has_jaw_y2_max();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* unituid_;
  ::std::string* machineuid_;
  ::std::string* machinename_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > techniquetype_;
  ::google::protobuf::int32 radiationtype_;
  ::google::protobuf::int32 flatteningfiltermode_;
  ::std::string* unitenergy_;
  double gantry_min_;
  double gantry_max_;
  double table_min_;
  double table_max_;
  double collimator_min_;
  double collimator_max_;
  double jaw_x1_min_;
  double jaw_x1_max_;
  double jaw_x2_min_;
  double jaw_x2_max_;
  double jaw_y1_min_;
  double jaw_y1_max_;
  double jaw_y2_min_;
  double jaw_y2_max_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Applicator > applicators_;
  ::google::protobuf::RepeatedField< float > doserates_;
  ::google::protobuf::RepeatedPtrField< ::std::string> trayuid_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Tray_Factor > tray_factor_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(25 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5funit_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5funit_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5funit_2eproto();
  
  void InitAsDefaultInstance();
  static RT_TPS_Unit* default_instance_;
};
// ===================================================================


// ===================================================================

// RT_TPS_Unit

// required string unitUid = 1;
inline bool RT_TPS_Unit::has_unituid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RT_TPS_Unit::set_has_unituid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RT_TPS_Unit::clear_has_unituid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RT_TPS_Unit::clear_unituid() {
  if (unituid_ != &::google::protobuf::internal::kEmptyString) {
    unituid_->clear();
  }
  clear_has_unituid();
}
inline const ::std::string& RT_TPS_Unit::unituid() const {
  return *unituid_;
}
inline void RT_TPS_Unit::set_unituid(const ::std::string& value) {
  set_has_unituid();
  if (unituid_ == &::google::protobuf::internal::kEmptyString) {
    unituid_ = new ::std::string;
  }
  unituid_->assign(value);
}
inline void RT_TPS_Unit::set_unituid(const char* value) {
  set_has_unituid();
  if (unituid_ == &::google::protobuf::internal::kEmptyString) {
    unituid_ = new ::std::string;
  }
  unituid_->assign(value);
}
inline void RT_TPS_Unit::set_unituid(const char* value, size_t size) {
  set_has_unituid();
  if (unituid_ == &::google::protobuf::internal::kEmptyString) {
    unituid_ = new ::std::string;
  }
  unituid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_Unit::mutable_unituid() {
  set_has_unituid();
  if (unituid_ == &::google::protobuf::internal::kEmptyString) {
    unituid_ = new ::std::string;
  }
  return unituid_;
}
inline ::std::string* RT_TPS_Unit::release_unituid() {
  clear_has_unituid();
  if (unituid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = unituid_;
    unituid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// required string machineUid = 2;
inline bool RT_TPS_Unit::has_machineuid() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void RT_TPS_Unit::set_has_machineuid() {
  _has_bits_[0] |= 0x00000002u;
}
inline void RT_TPS_Unit::clear_has_machineuid() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void RT_TPS_Unit::clear_machineuid() {
  if (machineuid_ != &::google::protobuf::internal::kEmptyString) {
    machineuid_->clear();
  }
  clear_has_machineuid();
}
inline const ::std::string& RT_TPS_Unit::machineuid() const {
  return *machineuid_;
}
inline void RT_TPS_Unit::set_machineuid(const ::std::string& value) {
  set_has_machineuid();
  if (machineuid_ == &::google::protobuf::internal::kEmptyString) {
    machineuid_ = new ::std::string;
  }
  machineuid_->assign(value);
}
inline void RT_TPS_Unit::set_machineuid(const char* value) {
  set_has_machineuid();
  if (machineuid_ == &::google::protobuf::internal::kEmptyString) {
    machineuid_ = new ::std::string;
  }
  machineuid_->assign(value);
}
inline void RT_TPS_Unit::set_machineuid(const char* value, size_t size) {
  set_has_machineuid();
  if (machineuid_ == &::google::protobuf::internal::kEmptyString) {
    machineuid_ = new ::std::string;
  }
  machineuid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_Unit::mutable_machineuid() {
  set_has_machineuid();
  if (machineuid_ == &::google::protobuf::internal::kEmptyString) {
    machineuid_ = new ::std::string;
  }
  return machineuid_;
}
inline ::std::string* RT_TPS_Unit::release_machineuid() {
  clear_has_machineuid();
  if (machineuid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = machineuid_;
    machineuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// required string machineName = 3;
inline bool RT_TPS_Unit::has_machinename() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void RT_TPS_Unit::set_has_machinename() {
  _has_bits_[0] |= 0x00000004u;
}
inline void RT_TPS_Unit::clear_has_machinename() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void RT_TPS_Unit::clear_machinename() {
  if (machinename_ != &::google::protobuf::internal::kEmptyString) {
    machinename_->clear();
  }
  clear_has_machinename();
}
inline const ::std::string& RT_TPS_Unit::machinename() const {
  return *machinename_;
}
inline void RT_TPS_Unit::set_machinename(const ::std::string& value) {
  set_has_machinename();
  if (machinename_ == &::google::protobuf::internal::kEmptyString) {
    machinename_ = new ::std::string;
  }
  machinename_->assign(value);
}
inline void RT_TPS_Unit::set_machinename(const char* value) {
  set_has_machinename();
  if (machinename_ == &::google::protobuf::internal::kEmptyString) {
    machinename_ = new ::std::string;
  }
  machinename_->assign(value);
}
inline void RT_TPS_Unit::set_machinename(const char* value, size_t size) {
  set_has_machinename();
  if (machinename_ == &::google::protobuf::internal::kEmptyString) {
    machinename_ = new ::std::string;
  }
  machinename_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_Unit::mutable_machinename() {
  set_has_machinename();
  if (machinename_ == &::google::protobuf::internal::kEmptyString) {
    machinename_ = new ::std::string;
  }
  return machinename_;
}
inline ::std::string* RT_TPS_Unit::release_machinename() {
  clear_has_machinename();
  if (machinename_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = machinename_;
    machinename_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// required int32 radiationType = 4;
inline bool RT_TPS_Unit::has_radiationtype() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void RT_TPS_Unit::set_has_radiationtype() {
  _has_bits_[0] |= 0x00000008u;
}
inline void RT_TPS_Unit::clear_has_radiationtype() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void RT_TPS_Unit::clear_radiationtype() {
  radiationtype_ = 0;
  clear_has_radiationtype();
}
inline ::google::protobuf::int32 RT_TPS_Unit::radiationtype() const {
  return radiationtype_;
}
inline void RT_TPS_Unit::set_radiationtype(::google::protobuf::int32 value) {
  set_has_radiationtype();
  radiationtype_ = value;
}

// repeated int32 techniqueType = 5;
inline int RT_TPS_Unit::techniquetype_size() const {
  return techniquetype_.size();
}
inline void RT_TPS_Unit::clear_techniquetype() {
  techniquetype_.Clear();
}
inline ::google::protobuf::int32 RT_TPS_Unit::techniquetype(int index) const {
  return techniquetype_.Get(index);
}
inline void RT_TPS_Unit::set_techniquetype(int index, ::google::protobuf::int32 value) {
  techniquetype_.Set(index, value);
}
inline void RT_TPS_Unit::add_techniquetype(::google::protobuf::int32 value) {
  techniquetype_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
RT_TPS_Unit::techniquetype() const {
  return techniquetype_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
RT_TPS_Unit::mutable_techniquetype() {
  return &techniquetype_;
}

// required string unitEnergy = 6;
inline bool RT_TPS_Unit::has_unitenergy() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void RT_TPS_Unit::set_has_unitenergy() {
  _has_bits_[0] |= 0x00000020u;
}
inline void RT_TPS_Unit::clear_has_unitenergy() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void RT_TPS_Unit::clear_unitenergy() {
  if (unitenergy_ != &::google::protobuf::internal::kEmptyString) {
    unitenergy_->clear();
  }
  clear_has_unitenergy();
}
inline const ::std::string& RT_TPS_Unit::unitenergy() const {
  return *unitenergy_;
}
inline void RT_TPS_Unit::set_unitenergy(const ::std::string& value) {
  set_has_unitenergy();
  if (unitenergy_ == &::google::protobuf::internal::kEmptyString) {
    unitenergy_ = new ::std::string;
  }
  unitenergy_->assign(value);
}
inline void RT_TPS_Unit::set_unitenergy(const char* value) {
  set_has_unitenergy();
  if (unitenergy_ == &::google::protobuf::internal::kEmptyString) {
    unitenergy_ = new ::std::string;
  }
  unitenergy_->assign(value);
}
inline void RT_TPS_Unit::set_unitenergy(const char* value, size_t size) {
  set_has_unitenergy();
  if (unitenergy_ == &::google::protobuf::internal::kEmptyString) {
    unitenergy_ = new ::std::string;
  }
  unitenergy_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_Unit::mutable_unitenergy() {
  set_has_unitenergy();
  if (unitenergy_ == &::google::protobuf::internal::kEmptyString) {
    unitenergy_ = new ::std::string;
  }
  return unitenergy_;
}
inline ::std::string* RT_TPS_Unit::release_unitenergy() {
  clear_has_unitenergy();
  if (unitenergy_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = unitenergy_;
    unitenergy_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// required int32 flatteningfiltermode = 7;
inline bool RT_TPS_Unit::has_flatteningfiltermode() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void RT_TPS_Unit::set_has_flatteningfiltermode() {
  _has_bits_[0] |= 0x00000040u;
}
inline void RT_TPS_Unit::clear_has_flatteningfiltermode() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void RT_TPS_Unit::clear_flatteningfiltermode() {
  flatteningfiltermode_ = 0;
  clear_has_flatteningfiltermode();
}
inline ::google::protobuf::int32 RT_TPS_Unit::flatteningfiltermode() const {
  return flatteningfiltermode_;
}
inline void RT_TPS_Unit::set_flatteningfiltermode(::google::protobuf::int32 value) {
  set_has_flatteningfiltermode();
  flatteningfiltermode_ = value;
}

// required double gantry_min = 8;
inline bool RT_TPS_Unit::has_gantry_min() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void RT_TPS_Unit::set_has_gantry_min() {
  _has_bits_[0] |= 0x00000080u;
}
inline void RT_TPS_Unit::clear_has_gantry_min() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void RT_TPS_Unit::clear_gantry_min() {
  gantry_min_ = 0;
  clear_has_gantry_min();
}
inline double RT_TPS_Unit::gantry_min() const {
  return gantry_min_;
}
inline void RT_TPS_Unit::set_gantry_min(double value) {
  set_has_gantry_min();
  gantry_min_ = value;
}

// required double gantry_max = 9;
inline bool RT_TPS_Unit::has_gantry_max() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void RT_TPS_Unit::set_has_gantry_max() {
  _has_bits_[0] |= 0x00000100u;
}
inline void RT_TPS_Unit::clear_has_gantry_max() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void RT_TPS_Unit::clear_gantry_max() {
  gantry_max_ = 0;
  clear_has_gantry_max();
}
inline double RT_TPS_Unit::gantry_max() const {
  return gantry_max_;
}
inline void RT_TPS_Unit::set_gantry_max(double value) {
  set_has_gantry_max();
  gantry_max_ = value;
}

// required double table_min = 10;
inline bool RT_TPS_Unit::has_table_min() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void RT_TPS_Unit::set_has_table_min() {
  _has_bits_[0] |= 0x00000200u;
}
inline void RT_TPS_Unit::clear_has_table_min() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void RT_TPS_Unit::clear_table_min() {
  table_min_ = 0;
  clear_has_table_min();
}
inline double RT_TPS_Unit::table_min() const {
  return table_min_;
}
inline void RT_TPS_Unit::set_table_min(double value) {
  set_has_table_min();
  table_min_ = value;
}

// required double table_max = 11;
inline bool RT_TPS_Unit::has_table_max() const {
  return (_has_bits_[0] & 0x00000400u) != 0;
}
inline void RT_TPS_Unit::set_has_table_max() {
  _has_bits_[0] |= 0x00000400u;
}
inline void RT_TPS_Unit::clear_has_table_max() {
  _has_bits_[0] &= ~0x00000400u;
}
inline void RT_TPS_Unit::clear_table_max() {
  table_max_ = 0;
  clear_has_table_max();
}
inline double RT_TPS_Unit::table_max() const {
  return table_max_;
}
inline void RT_TPS_Unit::set_table_max(double value) {
  set_has_table_max();
  table_max_ = value;
}

// required double collimator_min = 12;
inline bool RT_TPS_Unit::has_collimator_min() const {
  return (_has_bits_[0] & 0x00000800u) != 0;
}
inline void RT_TPS_Unit::set_has_collimator_min() {
  _has_bits_[0] |= 0x00000800u;
}
inline void RT_TPS_Unit::clear_has_collimator_min() {
  _has_bits_[0] &= ~0x00000800u;
}
inline void RT_TPS_Unit::clear_collimator_min() {
  collimator_min_ = 0;
  clear_has_collimator_min();
}
inline double RT_TPS_Unit::collimator_min() const {
  return collimator_min_;
}
inline void RT_TPS_Unit::set_collimator_min(double value) {
  set_has_collimator_min();
  collimator_min_ = value;
}

// required double collimator_max = 13;
inline bool RT_TPS_Unit::has_collimator_max() const {
  return (_has_bits_[0] & 0x00001000u) != 0;
}
inline void RT_TPS_Unit::set_has_collimator_max() {
  _has_bits_[0] |= 0x00001000u;
}
inline void RT_TPS_Unit::clear_has_collimator_max() {
  _has_bits_[0] &= ~0x00001000u;
}
inline void RT_TPS_Unit::clear_collimator_max() {
  collimator_max_ = 0;
  clear_has_collimator_max();
}
inline double RT_TPS_Unit::collimator_max() const {
  return collimator_max_;
}
inline void RT_TPS_Unit::set_collimator_max(double value) {
  set_has_collimator_max();
  collimator_max_ = value;
}

// required double jaw_x1_min = 14;
inline bool RT_TPS_Unit::has_jaw_x1_min() const {
  return (_has_bits_[0] & 0x00002000u) != 0;
}
inline void RT_TPS_Unit::set_has_jaw_x1_min() {
  _has_bits_[0] |= 0x00002000u;
}
inline void RT_TPS_Unit::clear_has_jaw_x1_min() {
  _has_bits_[0] &= ~0x00002000u;
}
inline void RT_TPS_Unit::clear_jaw_x1_min() {
  jaw_x1_min_ = 0;
  clear_has_jaw_x1_min();
}
inline double RT_TPS_Unit::jaw_x1_min() const {
  return jaw_x1_min_;
}
inline void RT_TPS_Unit::set_jaw_x1_min(double value) {
  set_has_jaw_x1_min();
  jaw_x1_min_ = value;
}

// required double jaw_x1_max = 15;
inline bool RT_TPS_Unit::has_jaw_x1_max() const {
  return (_has_bits_[0] & 0x00004000u) != 0;
}
inline void RT_TPS_Unit::set_has_jaw_x1_max() {
  _has_bits_[0] |= 0x00004000u;
}
inline void RT_TPS_Unit::clear_has_jaw_x1_max() {
  _has_bits_[0] &= ~0x00004000u;
}
inline void RT_TPS_Unit::clear_jaw_x1_max() {
  jaw_x1_max_ = 0;
  clear_has_jaw_x1_max();
}
inline double RT_TPS_Unit::jaw_x1_max() const {
  return jaw_x1_max_;
}
inline void RT_TPS_Unit::set_jaw_x1_max(double value) {
  set_has_jaw_x1_max();
  jaw_x1_max_ = value;
}

// required double jaw_x2_min = 16;
inline bool RT_TPS_Unit::has_jaw_x2_min() const {
  return (_has_bits_[0] & 0x00008000u) != 0;
}
inline void RT_TPS_Unit::set_has_jaw_x2_min() {
  _has_bits_[0] |= 0x00008000u;
}
inline void RT_TPS_Unit::clear_has_jaw_x2_min() {
  _has_bits_[0] &= ~0x00008000u;
}
inline void RT_TPS_Unit::clear_jaw_x2_min() {
  jaw_x2_min_ = 0;
  clear_has_jaw_x2_min();
}
inline double RT_TPS_Unit::jaw_x2_min() const {
  return jaw_x2_min_;
}
inline void RT_TPS_Unit::set_jaw_x2_min(double value) {
  set_has_jaw_x2_min();
  jaw_x2_min_ = value;
}

// required double jaw_x2_max = 17;
inline bool RT_TPS_Unit::has_jaw_x2_max() const {
  return (_has_bits_[0] & 0x00010000u) != 0;
}
inline void RT_TPS_Unit::set_has_jaw_x2_max() {
  _has_bits_[0] |= 0x00010000u;
}
inline void RT_TPS_Unit::clear_has_jaw_x2_max() {
  _has_bits_[0] &= ~0x00010000u;
}
inline void RT_TPS_Unit::clear_jaw_x2_max() {
  jaw_x2_max_ = 0;
  clear_has_jaw_x2_max();
}
inline double RT_TPS_Unit::jaw_x2_max() const {
  return jaw_x2_max_;
}
inline void RT_TPS_Unit::set_jaw_x2_max(double value) {
  set_has_jaw_x2_max();
  jaw_x2_max_ = value;
}

// required double jaw_y1_min = 18;
inline bool RT_TPS_Unit::has_jaw_y1_min() const {
  return (_has_bits_[0] & 0x00020000u) != 0;
}
inline void RT_TPS_Unit::set_has_jaw_y1_min() {
  _has_bits_[0] |= 0x00020000u;
}
inline void RT_TPS_Unit::clear_has_jaw_y1_min() {
  _has_bits_[0] &= ~0x00020000u;
}
inline void RT_TPS_Unit::clear_jaw_y1_min() {
  jaw_y1_min_ = 0;
  clear_has_jaw_y1_min();
}
inline double RT_TPS_Unit::jaw_y1_min() const {
  return jaw_y1_min_;
}
inline void RT_TPS_Unit::set_jaw_y1_min(double value) {
  set_has_jaw_y1_min();
  jaw_y1_min_ = value;
}

// required double jaw_y1_max = 19;
inline bool RT_TPS_Unit::has_jaw_y1_max() const {
  return (_has_bits_[0] & 0x00040000u) != 0;
}
inline void RT_TPS_Unit::set_has_jaw_y1_max() {
  _has_bits_[0] |= 0x00040000u;
}
inline void RT_TPS_Unit::clear_has_jaw_y1_max() {
  _has_bits_[0] &= ~0x00040000u;
}
inline void RT_TPS_Unit::clear_jaw_y1_max() {
  jaw_y1_max_ = 0;
  clear_has_jaw_y1_max();
}
inline double RT_TPS_Unit::jaw_y1_max() const {
  return jaw_y1_max_;
}
inline void RT_TPS_Unit::set_jaw_y1_max(double value) {
  set_has_jaw_y1_max();
  jaw_y1_max_ = value;
}

// required double jaw_y2_min = 20;
inline bool RT_TPS_Unit::has_jaw_y2_min() const {
  return (_has_bits_[0] & 0x00080000u) != 0;
}
inline void RT_TPS_Unit::set_has_jaw_y2_min() {
  _has_bits_[0] |= 0x00080000u;
}
inline void RT_TPS_Unit::clear_has_jaw_y2_min() {
  _has_bits_[0] &= ~0x00080000u;
}
inline void RT_TPS_Unit::clear_jaw_y2_min() {
  jaw_y2_min_ = 0;
  clear_has_jaw_y2_min();
}
inline double RT_TPS_Unit::jaw_y2_min() const {
  return jaw_y2_min_;
}
inline void RT_TPS_Unit::set_jaw_y2_min(double value) {
  set_has_jaw_y2_min();
  jaw_y2_min_ = value;
}

// required double jaw_y2_max = 21;
inline bool RT_TPS_Unit::has_jaw_y2_max() const {
  return (_has_bits_[0] & 0x00100000u) != 0;
}
inline void RT_TPS_Unit::set_has_jaw_y2_max() {
  _has_bits_[0] |= 0x00100000u;
}
inline void RT_TPS_Unit::clear_has_jaw_y2_max() {
  _has_bits_[0] &= ~0x00100000u;
}
inline void RT_TPS_Unit::clear_jaw_y2_max() {
  jaw_y2_max_ = 0;
  clear_has_jaw_y2_max();
}
inline double RT_TPS_Unit::jaw_y2_max() const {
  return jaw_y2_max_;
}
inline void RT_TPS_Unit::set_jaw_y2_max(double value) {
  set_has_jaw_y2_max();
  jaw_y2_max_ = value;
}

// repeated .tps.proto.RT_TPS_Applicator applicators = 22;
inline int RT_TPS_Unit::applicators_size() const {
  return applicators_.size();
}
inline void RT_TPS_Unit::clear_applicators() {
  applicators_.Clear();
}
inline const ::tps::proto::RT_TPS_Applicator& RT_TPS_Unit::applicators(int index) const {
  return applicators_.Get(index);
}
inline ::tps::proto::RT_TPS_Applicator* RT_TPS_Unit::mutable_applicators(int index) {
  return applicators_.Mutable(index);
}
inline ::tps::proto::RT_TPS_Applicator* RT_TPS_Unit::add_applicators() {
  return applicators_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Applicator >&
RT_TPS_Unit::applicators() const {
  return applicators_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Applicator >*
RT_TPS_Unit::mutable_applicators() {
  return &applicators_;
}

// repeated float doseRates = 23;
inline int RT_TPS_Unit::doserates_size() const {
  return doserates_.size();
}
inline void RT_TPS_Unit::clear_doserates() {
  doserates_.Clear();
}
inline float RT_TPS_Unit::doserates(int index) const {
  return doserates_.Get(index);
}
inline void RT_TPS_Unit::set_doserates(int index, float value) {
  doserates_.Set(index, value);
}
inline void RT_TPS_Unit::add_doserates(float value) {
  doserates_.Add(value);
}
inline const ::google::protobuf::RepeatedField< float >&
RT_TPS_Unit::doserates() const {
  return doserates_;
}
inline ::google::protobuf::RepeatedField< float >*
RT_TPS_Unit::mutable_doserates() {
  return &doserates_;
}

// repeated string trayuid = 24;
inline int RT_TPS_Unit::trayuid_size() const {
  return trayuid_.size();
}
inline void RT_TPS_Unit::clear_trayuid() {
  trayuid_.Clear();
}
inline const ::std::string& RT_TPS_Unit::trayuid(int index) const {
  return trayuid_.Get(index);
}
inline ::std::string* RT_TPS_Unit::mutable_trayuid(int index) {
  return trayuid_.Mutable(index);
}
inline void RT_TPS_Unit::set_trayuid(int index, const ::std::string& value) {
  trayuid_.Mutable(index)->assign(value);
}
inline void RT_TPS_Unit::set_trayuid(int index, const char* value) {
  trayuid_.Mutable(index)->assign(value);
}
inline void RT_TPS_Unit::set_trayuid(int index, const char* value, size_t size) {
  trayuid_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_Unit::add_trayuid() {
  return trayuid_.Add();
}
inline void RT_TPS_Unit::add_trayuid(const ::std::string& value) {
  trayuid_.Add()->assign(value);
}
inline void RT_TPS_Unit::add_trayuid(const char* value) {
  trayuid_.Add()->assign(value);
}
inline void RT_TPS_Unit::add_trayuid(const char* value, size_t size) {
  trayuid_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
RT_TPS_Unit::trayuid() const {
  return trayuid_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
RT_TPS_Unit::mutable_trayuid() {
  return &trayuid_;
}

// repeated .tps.proto.RT_TPS_Tray_Factor tray_factor = 25;
inline int RT_TPS_Unit::tray_factor_size() const {
  return tray_factor_.size();
}
inline void RT_TPS_Unit::clear_tray_factor() {
  tray_factor_.Clear();
}
inline const ::tps::proto::RT_TPS_Tray_Factor& RT_TPS_Unit::tray_factor(int index) const {
  return tray_factor_.Get(index);
}
inline ::tps::proto::RT_TPS_Tray_Factor* RT_TPS_Unit::mutable_tray_factor(int index) {
  return tray_factor_.Mutable(index);
}
inline ::tps::proto::RT_TPS_Tray_Factor* RT_TPS_Unit::add_tray_factor() {
  return tray_factor_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Tray_Factor >&
RT_TPS_Unit::tray_factor() const {
  return tray_factor_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Tray_Factor >*
RT_TPS_Unit::mutable_tray_factor() {
  return &tray_factor_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace proto
}  // namespace tps

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_rt_5ftps_5funit_2eproto__INCLUDED