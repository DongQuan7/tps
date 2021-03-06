// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rt_tps_proto_course.proto

#ifndef PROTOBUF_rt_5ftps_5fproto_5fcourse_2eproto__INCLUDED
#define PROTOBUF_rt_5ftps_5fproto_5fcourse_2eproto__INCLUDED

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
#include "rt_tps_proto_plan.pb.h"
#include "rt_tps_proto_prescription.pb.h"
// @@protoc_insertion_point(includes)

namespace tps {
namespace proto {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_rt_5ftps_5fproto_5fcourse_2eproto();
void protobuf_AssignDesc_rt_5ftps_5fproto_5fcourse_2eproto();
void protobuf_ShutdownFile_rt_5ftps_5fproto_5fcourse_2eproto();

class RtTpsProtoCourse;
class RtTpsProtoRepeatedCourse;

// ===================================================================

class RtTpsProtoCourse : public ::google::protobuf::Message {
 public:
  RtTpsProtoCourse();
  virtual ~RtTpsProtoCourse();
  
  RtTpsProtoCourse(const RtTpsProtoCourse& from);
  
  inline RtTpsProtoCourse& operator=(const RtTpsProtoCourse& from) {
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
  static const RtTpsProtoCourse& default_instance();
  
  void Swap(RtTpsProtoCourse* other);
  
  // implements Message ----------------------------------------------
  
  RtTpsProtoCourse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RtTpsProtoCourse& from);
  void MergeFrom(const RtTpsProtoCourse& from);
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
  
  // required string courseUID = 1;
  inline bool has_courseuid() const;
  inline void clear_courseuid();
  static const int kCourseUIDFieldNumber = 1;
  inline const ::std::string& courseuid() const;
  inline void set_courseuid(const ::std::string& value);
  inline void set_courseuid(const char* value);
  inline void set_courseuid(const char* value, size_t size);
  inline ::std::string* mutable_courseuid();
  inline ::std::string* release_courseuid();
  
  // optional string courseName = 2;
  inline bool has_coursename() const;
  inline void clear_coursename();
  static const int kCourseNameFieldNumber = 2;
  inline const ::std::string& coursename() const;
  inline void set_coursename(const ::std::string& value);
  inline void set_coursename(const char* value);
  inline void set_coursename(const char* value, size_t size);
  inline ::std::string* mutable_coursename();
  inline ::std::string* release_coursename();
  
  // optional string patientUID = 3;
  inline bool has_patientuid() const;
  inline void clear_patientuid();
  static const int kPatientUIDFieldNumber = 3;
  inline const ::std::string& patientuid() const;
  inline void set_patientuid(const ::std::string& value);
  inline void set_patientuid(const char* value);
  inline void set_patientuid(const char* value, size_t size);
  inline ::std::string* mutable_patientuid();
  inline ::std::string* release_patientuid();
  
  // repeated .tps.proto.RtTpsProtoPlan planList = 4;
  inline int planlist_size() const;
  inline void clear_planlist();
  static const int kPlanListFieldNumber = 4;
  inline const ::tps::proto::RtTpsProtoPlan& planlist(int index) const;
  inline ::tps::proto::RtTpsProtoPlan* mutable_planlist(int index);
  inline ::tps::proto::RtTpsProtoPlan* add_planlist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPlan >&
      planlist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPlan >*
      mutable_planlist();
  
  // @@protoc_insertion_point(class_scope:tps.proto.RtTpsProtoCourse)
 private:
  inline void set_has_courseuid();
  inline void clear_has_courseuid();
  inline void set_has_coursename();
  inline void clear_has_coursename();
  inline void set_has_patientuid();
  inline void clear_has_patientuid();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* courseuid_;
  ::std::string* coursename_;
  ::std::string* patientuid_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPlan > planlist_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5fproto_5fcourse_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5fproto_5fcourse_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5fproto_5fcourse_2eproto();
  
  void InitAsDefaultInstance();
  static RtTpsProtoCourse* default_instance_;
};
// -------------------------------------------------------------------

class RtTpsProtoRepeatedCourse : public ::google::protobuf::Message {
 public:
  RtTpsProtoRepeatedCourse();
  virtual ~RtTpsProtoRepeatedCourse();
  
  RtTpsProtoRepeatedCourse(const RtTpsProtoRepeatedCourse& from);
  
  inline RtTpsProtoRepeatedCourse& operator=(const RtTpsProtoRepeatedCourse& from) {
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
  static const RtTpsProtoRepeatedCourse& default_instance();
  
  void Swap(RtTpsProtoRepeatedCourse* other);
  
  // implements Message ----------------------------------------------
  
  RtTpsProtoRepeatedCourse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RtTpsProtoRepeatedCourse& from);
  void MergeFrom(const RtTpsProtoRepeatedCourse& from);
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
  
  // repeated .tps.proto.RtTpsProtoCourse courseList = 1;
  inline int courselist_size() const;
  inline void clear_courselist();
  static const int kCourseListFieldNumber = 1;
  inline const ::tps::proto::RtTpsProtoCourse& courselist(int index) const;
  inline ::tps::proto::RtTpsProtoCourse* mutable_courselist(int index);
  inline ::tps::proto::RtTpsProtoCourse* add_courselist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoCourse >&
      courselist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoCourse >*
      mutable_courselist();
  
  // @@protoc_insertion_point(class_scope:tps.proto.RtTpsProtoRepeatedCourse)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoCourse > courselist_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5fproto_5fcourse_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5fproto_5fcourse_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5fproto_5fcourse_2eproto();
  
  void InitAsDefaultInstance();
  static RtTpsProtoRepeatedCourse* default_instance_;
};
// ===================================================================


// ===================================================================

// RtTpsProtoCourse

// required string courseUID = 1;
inline bool RtTpsProtoCourse::has_courseuid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RtTpsProtoCourse::set_has_courseuid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RtTpsProtoCourse::clear_has_courseuid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RtTpsProtoCourse::clear_courseuid() {
  if (courseuid_ != &::google::protobuf::internal::kEmptyString) {
    courseuid_->clear();
  }
  clear_has_courseuid();
}
inline const ::std::string& RtTpsProtoCourse::courseuid() const {
  return *courseuid_;
}
inline void RtTpsProtoCourse::set_courseuid(const ::std::string& value) {
  set_has_courseuid();
  if (courseuid_ == &::google::protobuf::internal::kEmptyString) {
    courseuid_ = new ::std::string;
  }
  courseuid_->assign(value);
}
inline void RtTpsProtoCourse::set_courseuid(const char* value) {
  set_has_courseuid();
  if (courseuid_ == &::google::protobuf::internal::kEmptyString) {
    courseuid_ = new ::std::string;
  }
  courseuid_->assign(value);
}
inline void RtTpsProtoCourse::set_courseuid(const char* value, size_t size) {
  set_has_courseuid();
  if (courseuid_ == &::google::protobuf::internal::kEmptyString) {
    courseuid_ = new ::std::string;
  }
  courseuid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoCourse::mutable_courseuid() {
  set_has_courseuid();
  if (courseuid_ == &::google::protobuf::internal::kEmptyString) {
    courseuid_ = new ::std::string;
  }
  return courseuid_;
}
inline ::std::string* RtTpsProtoCourse::release_courseuid() {
  clear_has_courseuid();
  if (courseuid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = courseuid_;
    courseuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string courseName = 2;
inline bool RtTpsProtoCourse::has_coursename() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void RtTpsProtoCourse::set_has_coursename() {
  _has_bits_[0] |= 0x00000002u;
}
inline void RtTpsProtoCourse::clear_has_coursename() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void RtTpsProtoCourse::clear_coursename() {
  if (coursename_ != &::google::protobuf::internal::kEmptyString) {
    coursename_->clear();
  }
  clear_has_coursename();
}
inline const ::std::string& RtTpsProtoCourse::coursename() const {
  return *coursename_;
}
inline void RtTpsProtoCourse::set_coursename(const ::std::string& value) {
  set_has_coursename();
  if (coursename_ == &::google::protobuf::internal::kEmptyString) {
    coursename_ = new ::std::string;
  }
  coursename_->assign(value);
}
inline void RtTpsProtoCourse::set_coursename(const char* value) {
  set_has_coursename();
  if (coursename_ == &::google::protobuf::internal::kEmptyString) {
    coursename_ = new ::std::string;
  }
  coursename_->assign(value);
}
inline void RtTpsProtoCourse::set_coursename(const char* value, size_t size) {
  set_has_coursename();
  if (coursename_ == &::google::protobuf::internal::kEmptyString) {
    coursename_ = new ::std::string;
  }
  coursename_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoCourse::mutable_coursename() {
  set_has_coursename();
  if (coursename_ == &::google::protobuf::internal::kEmptyString) {
    coursename_ = new ::std::string;
  }
  return coursename_;
}
inline ::std::string* RtTpsProtoCourse::release_coursename() {
  clear_has_coursename();
  if (coursename_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = coursename_;
    coursename_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string patientUID = 3;
inline bool RtTpsProtoCourse::has_patientuid() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void RtTpsProtoCourse::set_has_patientuid() {
  _has_bits_[0] |= 0x00000004u;
}
inline void RtTpsProtoCourse::clear_has_patientuid() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void RtTpsProtoCourse::clear_patientuid() {
  if (patientuid_ != &::google::protobuf::internal::kEmptyString) {
    patientuid_->clear();
  }
  clear_has_patientuid();
}
inline const ::std::string& RtTpsProtoCourse::patientuid() const {
  return *patientuid_;
}
inline void RtTpsProtoCourse::set_patientuid(const ::std::string& value) {
  set_has_patientuid();
  if (patientuid_ == &::google::protobuf::internal::kEmptyString) {
    patientuid_ = new ::std::string;
  }
  patientuid_->assign(value);
}
inline void RtTpsProtoCourse::set_patientuid(const char* value) {
  set_has_patientuid();
  if (patientuid_ == &::google::protobuf::internal::kEmptyString) {
    patientuid_ = new ::std::string;
  }
  patientuid_->assign(value);
}
inline void RtTpsProtoCourse::set_patientuid(const char* value, size_t size) {
  set_has_patientuid();
  if (patientuid_ == &::google::protobuf::internal::kEmptyString) {
    patientuid_ = new ::std::string;
  }
  patientuid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoCourse::mutable_patientuid() {
  set_has_patientuid();
  if (patientuid_ == &::google::protobuf::internal::kEmptyString) {
    patientuid_ = new ::std::string;
  }
  return patientuid_;
}
inline ::std::string* RtTpsProtoCourse::release_patientuid() {
  clear_has_patientuid();
  if (patientuid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = patientuid_;
    patientuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// repeated .tps.proto.RtTpsProtoPlan planList = 4;
inline int RtTpsProtoCourse::planlist_size() const {
  return planlist_.size();
}
inline void RtTpsProtoCourse::clear_planlist() {
  planlist_.Clear();
}
inline const ::tps::proto::RtTpsProtoPlan& RtTpsProtoCourse::planlist(int index) const {
  return planlist_.Get(index);
}
inline ::tps::proto::RtTpsProtoPlan* RtTpsProtoCourse::mutable_planlist(int index) {
  return planlist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoPlan* RtTpsProtoCourse::add_planlist() {
  return planlist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPlan >&
RtTpsProtoCourse::planlist() const {
  return planlist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPlan >*
RtTpsProtoCourse::mutable_planlist() {
  return &planlist_;
}

// -------------------------------------------------------------------

// RtTpsProtoRepeatedCourse

// repeated .tps.proto.RtTpsProtoCourse courseList = 1;
inline int RtTpsProtoRepeatedCourse::courselist_size() const {
  return courselist_.size();
}
inline void RtTpsProtoRepeatedCourse::clear_courselist() {
  courselist_.Clear();
}
inline const ::tps::proto::RtTpsProtoCourse& RtTpsProtoRepeatedCourse::courselist(int index) const {
  return courselist_.Get(index);
}
inline ::tps::proto::RtTpsProtoCourse* RtTpsProtoRepeatedCourse::mutable_courselist(int index) {
  return courselist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoCourse* RtTpsProtoRepeatedCourse::add_courselist() {
  return courselist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoCourse >&
RtTpsProtoRepeatedCourse::courselist() const {
  return courselist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoCourse >*
RtTpsProtoRepeatedCourse::mutable_courselist() {
  return &courselist_;
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

#endif  // PROTOBUF_rt_5ftps_5fproto_5fcourse_2eproto__INCLUDED
