// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rt_tps_optinput.proto

#ifndef PROTOBUF_rt_5ftps_5foptinput_2eproto__INCLUDED
#define PROTOBUF_rt_5ftps_5foptinput_2eproto__INCLUDED

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
#include "rt_tps_optgoals.pb.h"
// @@protoc_insertion_point(includes)

namespace tps {
namespace proto {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_rt_5ftps_5foptinput_2eproto();
void protobuf_AssignDesc_rt_5ftps_5foptinput_2eproto();
void protobuf_ShutdownFile_rt_5ftps_5foptinput_2eproto();

class RT_TPS_Optinput;

// ===================================================================

class RT_TPS_Optinput : public ::google::protobuf::Message {
 public:
  RT_TPS_Optinput();
  virtual ~RT_TPS_Optinput();
  
  RT_TPS_Optinput(const RT_TPS_Optinput& from);
  
  inline RT_TPS_Optinput& operator=(const RT_TPS_Optinput& from) {
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
  static const RT_TPS_Optinput& default_instance();
  
  void Swap(RT_TPS_Optinput* other);
  
  // implements Message ----------------------------------------------
  
  RT_TPS_Optinput* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RT_TPS_Optinput& from);
  void MergeFrom(const RT_TPS_Optinput& from);
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
  
  // optional string OptNgUid = 1;
  inline bool has_optnguid() const;
  inline void clear_optnguid();
  static const int kOptNgUidFieldNumber = 1;
  inline const ::std::string& optnguid() const;
  inline void set_optnguid(const ::std::string& value);
  inline void set_optnguid(const char* value);
  inline void set_optnguid(const char* value, size_t size);
  inline ::std::string* mutable_optnguid();
  inline ::std::string* release_optnguid();
  
  // repeated string BasedOnNgUids = 2;
  inline int basedonnguids_size() const;
  inline void clear_basedonnguids();
  static const int kBasedOnNgUidsFieldNumber = 2;
  inline const ::std::string& basedonnguids(int index) const;
  inline ::std::string* mutable_basedonnguids(int index);
  inline void set_basedonnguids(int index, const ::std::string& value);
  inline void set_basedonnguids(int index, const char* value);
  inline void set_basedonnguids(int index, const char* value, size_t size);
  inline ::std::string* add_basedonnguids();
  inline void add_basedonnguids(const ::std::string& value);
  inline void add_basedonnguids(const char* value);
  inline void add_basedonnguids(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& basedonnguids() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_basedonnguids();
  
  // repeated .tps.proto.RT_TPS_Optgoal goals = 3;
  inline int goals_size() const;
  inline void clear_goals();
  static const int kGoalsFieldNumber = 3;
  inline const ::tps::proto::RT_TPS_Optgoal& goals(int index) const;
  inline ::tps::proto::RT_TPS_Optgoal* mutable_goals(int index);
  inline ::tps::proto::RT_TPS_Optgoal* add_goals();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Optgoal >&
      goals() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Optgoal >*
      mutable_goals();
  
  // optional int32 endCriterialType = 4;
  inline bool has_endcriterialtype() const;
  inline void clear_endcriterialtype();
  static const int kEndCriterialTypeFieldNumber = 4;
  inline ::google::protobuf::int32 endcriterialtype() const;
  inline void set_endcriterialtype(::google::protobuf::int32 value);
  
  // optional int32 endCriterialValue = 5;
  inline bool has_endcriterialvalue() const;
  inline void clear_endcriterialvalue();
  static const int kEndCriterialValueFieldNumber = 5;
  inline ::google::protobuf::int32 endcriterialvalue() const;
  inline void set_endcriterialvalue(::google::protobuf::int32 value);
  
  // @@protoc_insertion_point(class_scope:tps.proto.RT_TPS_Optinput)
 private:
  inline void set_has_optnguid();
  inline void clear_has_optnguid();
  inline void set_has_endcriterialtype();
  inline void clear_has_endcriterialtype();
  inline void set_has_endcriterialvalue();
  inline void clear_has_endcriterialvalue();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* optnguid_;
  ::google::protobuf::RepeatedPtrField< ::std::string> basedonnguids_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Optgoal > goals_;
  ::google::protobuf::int32 endcriterialtype_;
  ::google::protobuf::int32 endcriterialvalue_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5foptinput_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5foptinput_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5foptinput_2eproto();
  
  void InitAsDefaultInstance();
  static RT_TPS_Optinput* default_instance_;
};
// ===================================================================


// ===================================================================

// RT_TPS_Optinput

// optional string OptNgUid = 1;
inline bool RT_TPS_Optinput::has_optnguid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RT_TPS_Optinput::set_has_optnguid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RT_TPS_Optinput::clear_has_optnguid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RT_TPS_Optinput::clear_optnguid() {
  if (optnguid_ != &::google::protobuf::internal::kEmptyString) {
    optnguid_->clear();
  }
  clear_has_optnguid();
}
inline const ::std::string& RT_TPS_Optinput::optnguid() const {
  return *optnguid_;
}
inline void RT_TPS_Optinput::set_optnguid(const ::std::string& value) {
  set_has_optnguid();
  if (optnguid_ == &::google::protobuf::internal::kEmptyString) {
    optnguid_ = new ::std::string;
  }
  optnguid_->assign(value);
}
inline void RT_TPS_Optinput::set_optnguid(const char* value) {
  set_has_optnguid();
  if (optnguid_ == &::google::protobuf::internal::kEmptyString) {
    optnguid_ = new ::std::string;
  }
  optnguid_->assign(value);
}
inline void RT_TPS_Optinput::set_optnguid(const char* value, size_t size) {
  set_has_optnguid();
  if (optnguid_ == &::google::protobuf::internal::kEmptyString) {
    optnguid_ = new ::std::string;
  }
  optnguid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_Optinput::mutable_optnguid() {
  set_has_optnguid();
  if (optnguid_ == &::google::protobuf::internal::kEmptyString) {
    optnguid_ = new ::std::string;
  }
  return optnguid_;
}
inline ::std::string* RT_TPS_Optinput::release_optnguid() {
  clear_has_optnguid();
  if (optnguid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = optnguid_;
    optnguid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// repeated string BasedOnNgUids = 2;
inline int RT_TPS_Optinput::basedonnguids_size() const {
  return basedonnguids_.size();
}
inline void RT_TPS_Optinput::clear_basedonnguids() {
  basedonnguids_.Clear();
}
inline const ::std::string& RT_TPS_Optinput::basedonnguids(int index) const {
  return basedonnguids_.Get(index);
}
inline ::std::string* RT_TPS_Optinput::mutable_basedonnguids(int index) {
  return basedonnguids_.Mutable(index);
}
inline void RT_TPS_Optinput::set_basedonnguids(int index, const ::std::string& value) {
  basedonnguids_.Mutable(index)->assign(value);
}
inline void RT_TPS_Optinput::set_basedonnguids(int index, const char* value) {
  basedonnguids_.Mutable(index)->assign(value);
}
inline void RT_TPS_Optinput::set_basedonnguids(int index, const char* value, size_t size) {
  basedonnguids_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_Optinput::add_basedonnguids() {
  return basedonnguids_.Add();
}
inline void RT_TPS_Optinput::add_basedonnguids(const ::std::string& value) {
  basedonnguids_.Add()->assign(value);
}
inline void RT_TPS_Optinput::add_basedonnguids(const char* value) {
  basedonnguids_.Add()->assign(value);
}
inline void RT_TPS_Optinput::add_basedonnguids(const char* value, size_t size) {
  basedonnguids_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
RT_TPS_Optinput::basedonnguids() const {
  return basedonnguids_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
RT_TPS_Optinput::mutable_basedonnguids() {
  return &basedonnguids_;
}

// repeated .tps.proto.RT_TPS_Optgoal goals = 3;
inline int RT_TPS_Optinput::goals_size() const {
  return goals_.size();
}
inline void RT_TPS_Optinput::clear_goals() {
  goals_.Clear();
}
inline const ::tps::proto::RT_TPS_Optgoal& RT_TPS_Optinput::goals(int index) const {
  return goals_.Get(index);
}
inline ::tps::proto::RT_TPS_Optgoal* RT_TPS_Optinput::mutable_goals(int index) {
  return goals_.Mutable(index);
}
inline ::tps::proto::RT_TPS_Optgoal* RT_TPS_Optinput::add_goals() {
  return goals_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Optgoal >&
RT_TPS_Optinput::goals() const {
  return goals_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Optgoal >*
RT_TPS_Optinput::mutable_goals() {
  return &goals_;
}

// optional int32 endCriterialType = 4;
inline bool RT_TPS_Optinput::has_endcriterialtype() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void RT_TPS_Optinput::set_has_endcriterialtype() {
  _has_bits_[0] |= 0x00000008u;
}
inline void RT_TPS_Optinput::clear_has_endcriterialtype() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void RT_TPS_Optinput::clear_endcriterialtype() {
  endcriterialtype_ = 0;
  clear_has_endcriterialtype();
}
inline ::google::protobuf::int32 RT_TPS_Optinput::endcriterialtype() const {
  return endcriterialtype_;
}
inline void RT_TPS_Optinput::set_endcriterialtype(::google::protobuf::int32 value) {
  set_has_endcriterialtype();
  endcriterialtype_ = value;
}

// optional int32 endCriterialValue = 5;
inline bool RT_TPS_Optinput::has_endcriterialvalue() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void RT_TPS_Optinput::set_has_endcriterialvalue() {
  _has_bits_[0] |= 0x00000010u;
}
inline void RT_TPS_Optinput::clear_has_endcriterialvalue() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void RT_TPS_Optinput::clear_endcriterialvalue() {
  endcriterialvalue_ = 0;
  clear_has_endcriterialvalue();
}
inline ::google::protobuf::int32 RT_TPS_Optinput::endcriterialvalue() const {
  return endcriterialvalue_;
}
inline void RT_TPS_Optinput::set_endcriterialvalue(::google::protobuf::int32 value) {
  set_has_endcriterialvalue();
  endcriterialvalue_ = value;
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

#endif  // PROTOBUF_rt_5ftps_5foptinput_2eproto__INCLUDED