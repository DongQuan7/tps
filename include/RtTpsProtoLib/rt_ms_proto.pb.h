// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rt_ms_proto.proto

#ifndef PROTOBUF_rt_5fms_5fproto_2eproto__INCLUDED
#define PROTOBUF_rt_5fms_5fproto_2eproto__INCLUDED

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
// @@protoc_insertion_point(includes)

namespace ms {
namespace proto {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_rt_5fms_5fproto_2eproto();
void protobuf_AssignDesc_rt_5fms_5fproto_2eproto();
void protobuf_ShutdownFile_rt_5fms_5fproto_2eproto();

class RT_MS_Proto;

// ===================================================================

class RT_MS_Proto : public ::google::protobuf::Message {
 public:
  RT_MS_Proto();
  virtual ~RT_MS_Proto();
  
  RT_MS_Proto(const RT_MS_Proto& from);
  
  inline RT_MS_Proto& operator=(const RT_MS_Proto& from) {
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
  static const RT_MS_Proto& default_instance();
  
  void Swap(RT_MS_Proto* other);
  
  // implements Message ----------------------------------------------
  
  RT_MS_Proto* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RT_MS_Proto& from);
  void MergeFrom(const RT_MS_Proto& from);
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
  
  // optional string uid = 1;
  inline bool has_uid() const;
  inline void clear_uid();
  static const int kUidFieldNumber = 1;
  inline const ::std::string& uid() const;
  inline void set_uid(const ::std::string& value);
  inline void set_uid(const char* value);
  inline void set_uid(const char* value, size_t size);
  inline ::std::string* mutable_uid();
  inline ::std::string* release_uid();
  
  // optional string objectType = 2;
  inline bool has_objecttype() const;
  inline void clear_objecttype();
  static const int kObjectTypeFieldNumber = 2;
  inline const ::std::string& objecttype() const;
  inline void set_objecttype(const ::std::string& value);
  inline void set_objecttype(const char* value);
  inline void set_objecttype(const char* value, size_t size);
  inline ::std::string* mutable_objecttype();
  inline ::std::string* release_objecttype();
  
  // optional string parent_uid = 3;
  inline bool has_parent_uid() const;
  inline void clear_parent_uid();
  static const int kParentUidFieldNumber = 3;
  inline const ::std::string& parent_uid() const;
  inline void set_parent_uid(const ::std::string& value);
  inline void set_parent_uid(const char* value);
  inline void set_parent_uid(const char* value, size_t size);
  inline ::std::string* mutable_parent_uid();
  inline ::std::string* release_parent_uid();
  
  // optional int32 algType = 4;
  inline bool has_algtype() const;
  inline void clear_algtype();
  static const int kAlgTypeFieldNumber = 4;
  inline ::google::protobuf::int32 algtype() const;
  inline void set_algtype(::google::protobuf::int32 value);
  
  // repeated string filename = 5;
  inline int filename_size() const;
  inline void clear_filename();
  static const int kFilenameFieldNumber = 5;
  inline const ::std::string& filename(int index) const;
  inline ::std::string* mutable_filename(int index);
  inline void set_filename(int index, const ::std::string& value);
  inline void set_filename(int index, const char* value);
  inline void set_filename(int index, const char* value, size_t size);
  inline ::std::string* add_filename();
  inline void add_filename(const ::std::string& value);
  inline void add_filename(const char* value);
  inline void add_filename(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& filename() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_filename();
  
  // repeated string physicalwedgeuidlist = 6;
  inline int physicalwedgeuidlist_size() const;
  inline void clear_physicalwedgeuidlist();
  static const int kPhysicalwedgeuidlistFieldNumber = 6;
  inline const ::std::string& physicalwedgeuidlist(int index) const;
  inline ::std::string* mutable_physicalwedgeuidlist(int index);
  inline void set_physicalwedgeuidlist(int index, const ::std::string& value);
  inline void set_physicalwedgeuidlist(int index, const char* value);
  inline void set_physicalwedgeuidlist(int index, const char* value, size_t size);
  inline ::std::string* add_physicalwedgeuidlist();
  inline void add_physicalwedgeuidlist(const ::std::string& value);
  inline void add_physicalwedgeuidlist(const char* value);
  inline void add_physicalwedgeuidlist(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& physicalwedgeuidlist() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_physicalwedgeuidlist();
  
  // repeated string virtualwedgeuidlist = 7;
  inline int virtualwedgeuidlist_size() const;
  inline void clear_virtualwedgeuidlist();
  static const int kVirtualwedgeuidlistFieldNumber = 7;
  inline const ::std::string& virtualwedgeuidlist(int index) const;
  inline ::std::string* mutable_virtualwedgeuidlist(int index);
  inline void set_virtualwedgeuidlist(int index, const ::std::string& value);
  inline void set_virtualwedgeuidlist(int index, const char* value);
  inline void set_virtualwedgeuidlist(int index, const char* value, size_t size);
  inline ::std::string* add_virtualwedgeuidlist();
  inline void add_virtualwedgeuidlist(const ::std::string& value);
  inline void add_virtualwedgeuidlist(const char* value);
  inline void add_virtualwedgeuidlist(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& virtualwedgeuidlist() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_virtualwedgeuidlist();
  
  // optional string uid2 = 8;
  inline bool has_uid2() const;
  inline void clear_uid2();
  static const int kUid2FieldNumber = 8;
  inline const ::std::string& uid2() const;
  inline void set_uid2(const ::std::string& value);
  inline void set_uid2(const char* value);
  inline void set_uid2(const char* value, size_t size);
  inline ::std::string* mutable_uid2();
  inline ::std::string* release_uid2();
  
  // optional string uid3 = 9;
  inline bool has_uid3() const;
  inline void clear_uid3();
  static const int kUid3FieldNumber = 9;
  inline const ::std::string& uid3() const;
  inline void set_uid3(const ::std::string& value);
  inline void set_uid3(const char* value);
  inline void set_uid3(const char* value, size_t size);
  inline ::std::string* mutable_uid3();
  inline ::std::string* release_uid3();
  
  // optional float variant = 10;
  inline bool has_variant() const;
  inline void clear_variant();
  static const int kVariantFieldNumber = 10;
  inline float variant() const;
  inline void set_variant(float value);
  
  // @@protoc_insertion_point(class_scope:ms.proto.RT_MS_Proto)
 private:
  inline void set_has_uid();
  inline void clear_has_uid();
  inline void set_has_objecttype();
  inline void clear_has_objecttype();
  inline void set_has_parent_uid();
  inline void clear_has_parent_uid();
  inline void set_has_algtype();
  inline void clear_has_algtype();
  inline void set_has_uid2();
  inline void clear_has_uid2();
  inline void set_has_uid3();
  inline void clear_has_uid3();
  inline void set_has_variant();
  inline void clear_has_variant();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* uid_;
  ::std::string* objecttype_;
  ::std::string* parent_uid_;
  ::google::protobuf::RepeatedPtrField< ::std::string> filename_;
  ::google::protobuf::RepeatedPtrField< ::std::string> physicalwedgeuidlist_;
  ::google::protobuf::RepeatedPtrField< ::std::string> virtualwedgeuidlist_;
  ::google::protobuf::int32 algtype_;
  float variant_;
  ::std::string* uid2_;
  ::std::string* uid3_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(10 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5fms_5fproto_2eproto();
  friend void protobuf_AssignDesc_rt_5fms_5fproto_2eproto();
  friend void protobuf_ShutdownFile_rt_5fms_5fproto_2eproto();
  
  void InitAsDefaultInstance();
  static RT_MS_Proto* default_instance_;
};
// ===================================================================


// ===================================================================

// RT_MS_Proto

// optional string uid = 1;
inline bool RT_MS_Proto::has_uid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RT_MS_Proto::set_has_uid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RT_MS_Proto::clear_has_uid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RT_MS_Proto::clear_uid() {
  if (uid_ != &::google::protobuf::internal::kEmptyString) {
    uid_->clear();
  }
  clear_has_uid();
}
inline const ::std::string& RT_MS_Proto::uid() const {
  return *uid_;
}
inline void RT_MS_Proto::set_uid(const ::std::string& value) {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  uid_->assign(value);
}
inline void RT_MS_Proto::set_uid(const char* value) {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  uid_->assign(value);
}
inline void RT_MS_Proto::set_uid(const char* value, size_t size) {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  uid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_MS_Proto::mutable_uid() {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  return uid_;
}
inline ::std::string* RT_MS_Proto::release_uid() {
  clear_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = uid_;
    uid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string objectType = 2;
inline bool RT_MS_Proto::has_objecttype() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void RT_MS_Proto::set_has_objecttype() {
  _has_bits_[0] |= 0x00000002u;
}
inline void RT_MS_Proto::clear_has_objecttype() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void RT_MS_Proto::clear_objecttype() {
  if (objecttype_ != &::google::protobuf::internal::kEmptyString) {
    objecttype_->clear();
  }
  clear_has_objecttype();
}
inline const ::std::string& RT_MS_Proto::objecttype() const {
  return *objecttype_;
}
inline void RT_MS_Proto::set_objecttype(const ::std::string& value) {
  set_has_objecttype();
  if (objecttype_ == &::google::protobuf::internal::kEmptyString) {
    objecttype_ = new ::std::string;
  }
  objecttype_->assign(value);
}
inline void RT_MS_Proto::set_objecttype(const char* value) {
  set_has_objecttype();
  if (objecttype_ == &::google::protobuf::internal::kEmptyString) {
    objecttype_ = new ::std::string;
  }
  objecttype_->assign(value);
}
inline void RT_MS_Proto::set_objecttype(const char* value, size_t size) {
  set_has_objecttype();
  if (objecttype_ == &::google::protobuf::internal::kEmptyString) {
    objecttype_ = new ::std::string;
  }
  objecttype_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_MS_Proto::mutable_objecttype() {
  set_has_objecttype();
  if (objecttype_ == &::google::protobuf::internal::kEmptyString) {
    objecttype_ = new ::std::string;
  }
  return objecttype_;
}
inline ::std::string* RT_MS_Proto::release_objecttype() {
  clear_has_objecttype();
  if (objecttype_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = objecttype_;
    objecttype_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string parent_uid = 3;
inline bool RT_MS_Proto::has_parent_uid() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void RT_MS_Proto::set_has_parent_uid() {
  _has_bits_[0] |= 0x00000004u;
}
inline void RT_MS_Proto::clear_has_parent_uid() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void RT_MS_Proto::clear_parent_uid() {
  if (parent_uid_ != &::google::protobuf::internal::kEmptyString) {
    parent_uid_->clear();
  }
  clear_has_parent_uid();
}
inline const ::std::string& RT_MS_Proto::parent_uid() const {
  return *parent_uid_;
}
inline void RT_MS_Proto::set_parent_uid(const ::std::string& value) {
  set_has_parent_uid();
  if (parent_uid_ == &::google::protobuf::internal::kEmptyString) {
    parent_uid_ = new ::std::string;
  }
  parent_uid_->assign(value);
}
inline void RT_MS_Proto::set_parent_uid(const char* value) {
  set_has_parent_uid();
  if (parent_uid_ == &::google::protobuf::internal::kEmptyString) {
    parent_uid_ = new ::std::string;
  }
  parent_uid_->assign(value);
}
inline void RT_MS_Proto::set_parent_uid(const char* value, size_t size) {
  set_has_parent_uid();
  if (parent_uid_ == &::google::protobuf::internal::kEmptyString) {
    parent_uid_ = new ::std::string;
  }
  parent_uid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_MS_Proto::mutable_parent_uid() {
  set_has_parent_uid();
  if (parent_uid_ == &::google::protobuf::internal::kEmptyString) {
    parent_uid_ = new ::std::string;
  }
  return parent_uid_;
}
inline ::std::string* RT_MS_Proto::release_parent_uid() {
  clear_has_parent_uid();
  if (parent_uid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = parent_uid_;
    parent_uid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional int32 algType = 4;
inline bool RT_MS_Proto::has_algtype() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void RT_MS_Proto::set_has_algtype() {
  _has_bits_[0] |= 0x00000008u;
}
inline void RT_MS_Proto::clear_has_algtype() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void RT_MS_Proto::clear_algtype() {
  algtype_ = 0;
  clear_has_algtype();
}
inline ::google::protobuf::int32 RT_MS_Proto::algtype() const {
  return algtype_;
}
inline void RT_MS_Proto::set_algtype(::google::protobuf::int32 value) {
  set_has_algtype();
  algtype_ = value;
}

// repeated string filename = 5;
inline int RT_MS_Proto::filename_size() const {
  return filename_.size();
}
inline void RT_MS_Proto::clear_filename() {
  filename_.Clear();
}
inline const ::std::string& RT_MS_Proto::filename(int index) const {
  return filename_.Get(index);
}
inline ::std::string* RT_MS_Proto::mutable_filename(int index) {
  return filename_.Mutable(index);
}
inline void RT_MS_Proto::set_filename(int index, const ::std::string& value) {
  filename_.Mutable(index)->assign(value);
}
inline void RT_MS_Proto::set_filename(int index, const char* value) {
  filename_.Mutable(index)->assign(value);
}
inline void RT_MS_Proto::set_filename(int index, const char* value, size_t size) {
  filename_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_MS_Proto::add_filename() {
  return filename_.Add();
}
inline void RT_MS_Proto::add_filename(const ::std::string& value) {
  filename_.Add()->assign(value);
}
inline void RT_MS_Proto::add_filename(const char* value) {
  filename_.Add()->assign(value);
}
inline void RT_MS_Proto::add_filename(const char* value, size_t size) {
  filename_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
RT_MS_Proto::filename() const {
  return filename_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
RT_MS_Proto::mutable_filename() {
  return &filename_;
}

// repeated string physicalwedgeuidlist = 6;
inline int RT_MS_Proto::physicalwedgeuidlist_size() const {
  return physicalwedgeuidlist_.size();
}
inline void RT_MS_Proto::clear_physicalwedgeuidlist() {
  physicalwedgeuidlist_.Clear();
}
inline const ::std::string& RT_MS_Proto::physicalwedgeuidlist(int index) const {
  return physicalwedgeuidlist_.Get(index);
}
inline ::std::string* RT_MS_Proto::mutable_physicalwedgeuidlist(int index) {
  return physicalwedgeuidlist_.Mutable(index);
}
inline void RT_MS_Proto::set_physicalwedgeuidlist(int index, const ::std::string& value) {
  physicalwedgeuidlist_.Mutable(index)->assign(value);
}
inline void RT_MS_Proto::set_physicalwedgeuidlist(int index, const char* value) {
  physicalwedgeuidlist_.Mutable(index)->assign(value);
}
inline void RT_MS_Proto::set_physicalwedgeuidlist(int index, const char* value, size_t size) {
  physicalwedgeuidlist_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_MS_Proto::add_physicalwedgeuidlist() {
  return physicalwedgeuidlist_.Add();
}
inline void RT_MS_Proto::add_physicalwedgeuidlist(const ::std::string& value) {
  physicalwedgeuidlist_.Add()->assign(value);
}
inline void RT_MS_Proto::add_physicalwedgeuidlist(const char* value) {
  physicalwedgeuidlist_.Add()->assign(value);
}
inline void RT_MS_Proto::add_physicalwedgeuidlist(const char* value, size_t size) {
  physicalwedgeuidlist_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
RT_MS_Proto::physicalwedgeuidlist() const {
  return physicalwedgeuidlist_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
RT_MS_Proto::mutable_physicalwedgeuidlist() {
  return &physicalwedgeuidlist_;
}

// repeated string virtualwedgeuidlist = 7;
inline int RT_MS_Proto::virtualwedgeuidlist_size() const {
  return virtualwedgeuidlist_.size();
}
inline void RT_MS_Proto::clear_virtualwedgeuidlist() {
  virtualwedgeuidlist_.Clear();
}
inline const ::std::string& RT_MS_Proto::virtualwedgeuidlist(int index) const {
  return virtualwedgeuidlist_.Get(index);
}
inline ::std::string* RT_MS_Proto::mutable_virtualwedgeuidlist(int index) {
  return virtualwedgeuidlist_.Mutable(index);
}
inline void RT_MS_Proto::set_virtualwedgeuidlist(int index, const ::std::string& value) {
  virtualwedgeuidlist_.Mutable(index)->assign(value);
}
inline void RT_MS_Proto::set_virtualwedgeuidlist(int index, const char* value) {
  virtualwedgeuidlist_.Mutable(index)->assign(value);
}
inline void RT_MS_Proto::set_virtualwedgeuidlist(int index, const char* value, size_t size) {
  virtualwedgeuidlist_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_MS_Proto::add_virtualwedgeuidlist() {
  return virtualwedgeuidlist_.Add();
}
inline void RT_MS_Proto::add_virtualwedgeuidlist(const ::std::string& value) {
  virtualwedgeuidlist_.Add()->assign(value);
}
inline void RT_MS_Proto::add_virtualwedgeuidlist(const char* value) {
  virtualwedgeuidlist_.Add()->assign(value);
}
inline void RT_MS_Proto::add_virtualwedgeuidlist(const char* value, size_t size) {
  virtualwedgeuidlist_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
RT_MS_Proto::virtualwedgeuidlist() const {
  return virtualwedgeuidlist_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
RT_MS_Proto::mutable_virtualwedgeuidlist() {
  return &virtualwedgeuidlist_;
}

// optional string uid2 = 8;
inline bool RT_MS_Proto::has_uid2() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void RT_MS_Proto::set_has_uid2() {
  _has_bits_[0] |= 0x00000080u;
}
inline void RT_MS_Proto::clear_has_uid2() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void RT_MS_Proto::clear_uid2() {
  if (uid2_ != &::google::protobuf::internal::kEmptyString) {
    uid2_->clear();
  }
  clear_has_uid2();
}
inline const ::std::string& RT_MS_Proto::uid2() const {
  return *uid2_;
}
inline void RT_MS_Proto::set_uid2(const ::std::string& value) {
  set_has_uid2();
  if (uid2_ == &::google::protobuf::internal::kEmptyString) {
    uid2_ = new ::std::string;
  }
  uid2_->assign(value);
}
inline void RT_MS_Proto::set_uid2(const char* value) {
  set_has_uid2();
  if (uid2_ == &::google::protobuf::internal::kEmptyString) {
    uid2_ = new ::std::string;
  }
  uid2_->assign(value);
}
inline void RT_MS_Proto::set_uid2(const char* value, size_t size) {
  set_has_uid2();
  if (uid2_ == &::google::protobuf::internal::kEmptyString) {
    uid2_ = new ::std::string;
  }
  uid2_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_MS_Proto::mutable_uid2() {
  set_has_uid2();
  if (uid2_ == &::google::protobuf::internal::kEmptyString) {
    uid2_ = new ::std::string;
  }
  return uid2_;
}
inline ::std::string* RT_MS_Proto::release_uid2() {
  clear_has_uid2();
  if (uid2_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = uid2_;
    uid2_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string uid3 = 9;
inline bool RT_MS_Proto::has_uid3() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void RT_MS_Proto::set_has_uid3() {
  _has_bits_[0] |= 0x00000100u;
}
inline void RT_MS_Proto::clear_has_uid3() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void RT_MS_Proto::clear_uid3() {
  if (uid3_ != &::google::protobuf::internal::kEmptyString) {
    uid3_->clear();
  }
  clear_has_uid3();
}
inline const ::std::string& RT_MS_Proto::uid3() const {
  return *uid3_;
}
inline void RT_MS_Proto::set_uid3(const ::std::string& value) {
  set_has_uid3();
  if (uid3_ == &::google::protobuf::internal::kEmptyString) {
    uid3_ = new ::std::string;
  }
  uid3_->assign(value);
}
inline void RT_MS_Proto::set_uid3(const char* value) {
  set_has_uid3();
  if (uid3_ == &::google::protobuf::internal::kEmptyString) {
    uid3_ = new ::std::string;
  }
  uid3_->assign(value);
}
inline void RT_MS_Proto::set_uid3(const char* value, size_t size) {
  set_has_uid3();
  if (uid3_ == &::google::protobuf::internal::kEmptyString) {
    uid3_ = new ::std::string;
  }
  uid3_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_MS_Proto::mutable_uid3() {
  set_has_uid3();
  if (uid3_ == &::google::protobuf::internal::kEmptyString) {
    uid3_ = new ::std::string;
  }
  return uid3_;
}
inline ::std::string* RT_MS_Proto::release_uid3() {
  clear_has_uid3();
  if (uid3_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = uid3_;
    uid3_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional float variant = 10;
inline bool RT_MS_Proto::has_variant() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void RT_MS_Proto::set_has_variant() {
  _has_bits_[0] |= 0x00000200u;
}
inline void RT_MS_Proto::clear_has_variant() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void RT_MS_Proto::clear_variant() {
  variant_ = 0;
  clear_has_variant();
}
inline float RT_MS_Proto::variant() const {
  return variant_;
}
inline void RT_MS_Proto::set_variant(float value) {
  set_has_variant();
  variant_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace proto
}  // namespace ms

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_rt_5fms_5fproto_2eproto__INCLUDED
