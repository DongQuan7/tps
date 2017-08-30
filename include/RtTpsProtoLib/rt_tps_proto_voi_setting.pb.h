// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rt_tps_proto_voi_setting.proto

#ifndef PROTOBUF_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto__INCLUDED
#define PROTOBUF_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto__INCLUDED

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

namespace tps {
namespace proto {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto();
void protobuf_AssignDesc_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto();
void protobuf_ShutdownFile_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto();

class RtTpsProtoVoiSetting;
class RtTpsProtoRepeatedVoiSetting;

// ===================================================================

class RtTpsProtoVoiSetting : public ::google::protobuf::Message {
 public:
  RtTpsProtoVoiSetting();
  virtual ~RtTpsProtoVoiSetting();
  
  RtTpsProtoVoiSetting(const RtTpsProtoVoiSetting& from);
  
  inline RtTpsProtoVoiSetting& operator=(const RtTpsProtoVoiSetting& from) {
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
  static const RtTpsProtoVoiSetting& default_instance();
  
  void Swap(RtTpsProtoVoiSetting* other);
  
  // implements Message ----------------------------------------------
  
  RtTpsProtoVoiSetting* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RtTpsProtoVoiSetting& from);
  void MergeFrom(const RtTpsProtoVoiSetting& from);
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
  
  // optional string site = 2;
  inline bool has_site() const;
  inline void clear_site();
  static const int kSiteFieldNumber = 2;
  inline const ::std::string& site() const;
  inline void set_site(const ::std::string& value);
  inline void set_site(const char* value);
  inline void set_site(const char* value, size_t size);
  inline ::std::string* mutable_site();
  inline ::std::string* release_site();
  
  // optional string roiname = 3;
  inline bool has_roiname() const;
  inline void clear_roiname();
  static const int kRoinameFieldNumber = 3;
  inline const ::std::string& roiname() const;
  inline void set_roiname(const ::std::string& value);
  inline void set_roiname(const char* value);
  inline void set_roiname(const char* value, size_t size);
  inline ::std::string* mutable_roiname();
  inline ::std::string* release_roiname();
  
  // optional string description = 4;
  inline bool has_description() const;
  inline void clear_description();
  static const int kDescriptionFieldNumber = 4;
  inline const ::std::string& description() const;
  inline void set_description(const ::std::string& value);
  inline void set_description(const char* value);
  inline void set_description(const char* value, size_t size);
  inline ::std::string* mutable_description();
  inline ::std::string* release_description();
  
  // optional string chndescription = 5;
  inline bool has_chndescription() const;
  inline void clear_chndescription();
  static const int kChndescriptionFieldNumber = 5;
  inline const ::std::string& chndescription() const;
  inline void set_chndescription(const ::std::string& value);
  inline void set_chndescription(const char* value);
  inline void set_chndescription(const char* value, size_t size);
  inline ::std::string* mutable_chndescription();
  inline ::std::string* release_chndescription();
  
  // optional int32 planningRole = 6;
  inline bool has_planningrole() const;
  inline void clear_planningrole();
  static const int kPlanningRoleFieldNumber = 6;
  inline ::google::protobuf::int32 planningrole() const;
  inline void set_planningrole(::google::protobuf::int32 value);
  
  // optional int32 biologyRole = 7;
  inline bool has_biologyrole() const;
  inline void clear_biologyrole();
  static const int kBiologyRoleFieldNumber = 7;
  inline ::google::protobuf::int32 biologyrole() const;
  inline void set_biologyrole(::google::protobuf::int32 value);
  
  // optional string roicolor = 8;
  inline bool has_roicolor() const;
  inline void clear_roicolor();
  static const int kRoicolorFieldNumber = 8;
  inline const ::std::string& roicolor() const;
  inline void set_roicolor(const ::std::string& value);
  inline void set_roicolor(const char* value);
  inline void set_roicolor(const char* value, size_t size);
  inline ::std::string* mutable_roicolor();
  inline ::std::string* release_roicolor();
  
  // @@protoc_insertion_point(class_scope:tps.proto.RtTpsProtoVoiSetting)
 private:
  inline void set_has_uid();
  inline void clear_has_uid();
  inline void set_has_site();
  inline void clear_has_site();
  inline void set_has_roiname();
  inline void clear_has_roiname();
  inline void set_has_description();
  inline void clear_has_description();
  inline void set_has_chndescription();
  inline void clear_has_chndescription();
  inline void set_has_planningrole();
  inline void clear_has_planningrole();
  inline void set_has_biologyrole();
  inline void clear_has_biologyrole();
  inline void set_has_roicolor();
  inline void clear_has_roicolor();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* uid_;
  ::std::string* site_;
  ::std::string* roiname_;
  ::std::string* description_;
  ::std::string* chndescription_;
  ::google::protobuf::int32 planningrole_;
  ::google::protobuf::int32 biologyrole_;
  ::std::string* roicolor_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(8 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto();
  
  void InitAsDefaultInstance();
  static RtTpsProtoVoiSetting* default_instance_;
};
// -------------------------------------------------------------------

class RtTpsProtoRepeatedVoiSetting : public ::google::protobuf::Message {
 public:
  RtTpsProtoRepeatedVoiSetting();
  virtual ~RtTpsProtoRepeatedVoiSetting();
  
  RtTpsProtoRepeatedVoiSetting(const RtTpsProtoRepeatedVoiSetting& from);
  
  inline RtTpsProtoRepeatedVoiSetting& operator=(const RtTpsProtoRepeatedVoiSetting& from) {
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
  static const RtTpsProtoRepeatedVoiSetting& default_instance();
  
  void Swap(RtTpsProtoRepeatedVoiSetting* other);
  
  // implements Message ----------------------------------------------
  
  RtTpsProtoRepeatedVoiSetting* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RtTpsProtoRepeatedVoiSetting& from);
  void MergeFrom(const RtTpsProtoRepeatedVoiSetting& from);
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
  
  // repeated .tps.proto.RtTpsProtoVoiSetting voiSetting = 1;
  inline int voisetting_size() const;
  inline void clear_voisetting();
  static const int kVoiSettingFieldNumber = 1;
  inline const ::tps::proto::RtTpsProtoVoiSetting& voisetting(int index) const;
  inline ::tps::proto::RtTpsProtoVoiSetting* mutable_voisetting(int index);
  inline ::tps::proto::RtTpsProtoVoiSetting* add_voisetting();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoVoiSetting >&
      voisetting() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoVoiSetting >*
      mutable_voisetting();
  
  // @@protoc_insertion_point(class_scope:tps.proto.RtTpsProtoRepeatedVoiSetting)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoVoiSetting > voisetting_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto();
  
  void InitAsDefaultInstance();
  static RtTpsProtoRepeatedVoiSetting* default_instance_;
};
// ===================================================================


// ===================================================================

// RtTpsProtoVoiSetting

// optional string uid = 1;
inline bool RtTpsProtoVoiSetting::has_uid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RtTpsProtoVoiSetting::set_has_uid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RtTpsProtoVoiSetting::clear_has_uid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RtTpsProtoVoiSetting::clear_uid() {
  if (uid_ != &::google::protobuf::internal::kEmptyString) {
    uid_->clear();
  }
  clear_has_uid();
}
inline const ::std::string& RtTpsProtoVoiSetting::uid() const {
  return *uid_;
}
inline void RtTpsProtoVoiSetting::set_uid(const ::std::string& value) {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  uid_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_uid(const char* value) {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  uid_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_uid(const char* value, size_t size) {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  uid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoVoiSetting::mutable_uid() {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  return uid_;
}
inline ::std::string* RtTpsProtoVoiSetting::release_uid() {
  clear_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = uid_;
    uid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string site = 2;
inline bool RtTpsProtoVoiSetting::has_site() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void RtTpsProtoVoiSetting::set_has_site() {
  _has_bits_[0] |= 0x00000002u;
}
inline void RtTpsProtoVoiSetting::clear_has_site() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void RtTpsProtoVoiSetting::clear_site() {
  if (site_ != &::google::protobuf::internal::kEmptyString) {
    site_->clear();
  }
  clear_has_site();
}
inline const ::std::string& RtTpsProtoVoiSetting::site() const {
  return *site_;
}
inline void RtTpsProtoVoiSetting::set_site(const ::std::string& value) {
  set_has_site();
  if (site_ == &::google::protobuf::internal::kEmptyString) {
    site_ = new ::std::string;
  }
  site_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_site(const char* value) {
  set_has_site();
  if (site_ == &::google::protobuf::internal::kEmptyString) {
    site_ = new ::std::string;
  }
  site_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_site(const char* value, size_t size) {
  set_has_site();
  if (site_ == &::google::protobuf::internal::kEmptyString) {
    site_ = new ::std::string;
  }
  site_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoVoiSetting::mutable_site() {
  set_has_site();
  if (site_ == &::google::protobuf::internal::kEmptyString) {
    site_ = new ::std::string;
  }
  return site_;
}
inline ::std::string* RtTpsProtoVoiSetting::release_site() {
  clear_has_site();
  if (site_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = site_;
    site_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string roiname = 3;
inline bool RtTpsProtoVoiSetting::has_roiname() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void RtTpsProtoVoiSetting::set_has_roiname() {
  _has_bits_[0] |= 0x00000004u;
}
inline void RtTpsProtoVoiSetting::clear_has_roiname() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void RtTpsProtoVoiSetting::clear_roiname() {
  if (roiname_ != &::google::protobuf::internal::kEmptyString) {
    roiname_->clear();
  }
  clear_has_roiname();
}
inline const ::std::string& RtTpsProtoVoiSetting::roiname() const {
  return *roiname_;
}
inline void RtTpsProtoVoiSetting::set_roiname(const ::std::string& value) {
  set_has_roiname();
  if (roiname_ == &::google::protobuf::internal::kEmptyString) {
    roiname_ = new ::std::string;
  }
  roiname_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_roiname(const char* value) {
  set_has_roiname();
  if (roiname_ == &::google::protobuf::internal::kEmptyString) {
    roiname_ = new ::std::string;
  }
  roiname_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_roiname(const char* value, size_t size) {
  set_has_roiname();
  if (roiname_ == &::google::protobuf::internal::kEmptyString) {
    roiname_ = new ::std::string;
  }
  roiname_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoVoiSetting::mutable_roiname() {
  set_has_roiname();
  if (roiname_ == &::google::protobuf::internal::kEmptyString) {
    roiname_ = new ::std::string;
  }
  return roiname_;
}
inline ::std::string* RtTpsProtoVoiSetting::release_roiname() {
  clear_has_roiname();
  if (roiname_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = roiname_;
    roiname_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string description = 4;
inline bool RtTpsProtoVoiSetting::has_description() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void RtTpsProtoVoiSetting::set_has_description() {
  _has_bits_[0] |= 0x00000008u;
}
inline void RtTpsProtoVoiSetting::clear_has_description() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void RtTpsProtoVoiSetting::clear_description() {
  if (description_ != &::google::protobuf::internal::kEmptyString) {
    description_->clear();
  }
  clear_has_description();
}
inline const ::std::string& RtTpsProtoVoiSetting::description() const {
  return *description_;
}
inline void RtTpsProtoVoiSetting::set_description(const ::std::string& value) {
  set_has_description();
  if (description_ == &::google::protobuf::internal::kEmptyString) {
    description_ = new ::std::string;
  }
  description_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_description(const char* value) {
  set_has_description();
  if (description_ == &::google::protobuf::internal::kEmptyString) {
    description_ = new ::std::string;
  }
  description_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_description(const char* value, size_t size) {
  set_has_description();
  if (description_ == &::google::protobuf::internal::kEmptyString) {
    description_ = new ::std::string;
  }
  description_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoVoiSetting::mutable_description() {
  set_has_description();
  if (description_ == &::google::protobuf::internal::kEmptyString) {
    description_ = new ::std::string;
  }
  return description_;
}
inline ::std::string* RtTpsProtoVoiSetting::release_description() {
  clear_has_description();
  if (description_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = description_;
    description_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string chndescription = 5;
inline bool RtTpsProtoVoiSetting::has_chndescription() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void RtTpsProtoVoiSetting::set_has_chndescription() {
  _has_bits_[0] |= 0x00000010u;
}
inline void RtTpsProtoVoiSetting::clear_has_chndescription() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void RtTpsProtoVoiSetting::clear_chndescription() {
  if (chndescription_ != &::google::protobuf::internal::kEmptyString) {
    chndescription_->clear();
  }
  clear_has_chndescription();
}
inline const ::std::string& RtTpsProtoVoiSetting::chndescription() const {
  return *chndescription_;
}
inline void RtTpsProtoVoiSetting::set_chndescription(const ::std::string& value) {
  set_has_chndescription();
  if (chndescription_ == &::google::protobuf::internal::kEmptyString) {
    chndescription_ = new ::std::string;
  }
  chndescription_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_chndescription(const char* value) {
  set_has_chndescription();
  if (chndescription_ == &::google::protobuf::internal::kEmptyString) {
    chndescription_ = new ::std::string;
  }
  chndescription_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_chndescription(const char* value, size_t size) {
  set_has_chndescription();
  if (chndescription_ == &::google::protobuf::internal::kEmptyString) {
    chndescription_ = new ::std::string;
  }
  chndescription_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoVoiSetting::mutable_chndescription() {
  set_has_chndescription();
  if (chndescription_ == &::google::protobuf::internal::kEmptyString) {
    chndescription_ = new ::std::string;
  }
  return chndescription_;
}
inline ::std::string* RtTpsProtoVoiSetting::release_chndescription() {
  clear_has_chndescription();
  if (chndescription_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = chndescription_;
    chndescription_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional int32 planningRole = 6;
inline bool RtTpsProtoVoiSetting::has_planningrole() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void RtTpsProtoVoiSetting::set_has_planningrole() {
  _has_bits_[0] |= 0x00000020u;
}
inline void RtTpsProtoVoiSetting::clear_has_planningrole() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void RtTpsProtoVoiSetting::clear_planningrole() {
  planningrole_ = 0;
  clear_has_planningrole();
}
inline ::google::protobuf::int32 RtTpsProtoVoiSetting::planningrole() const {
  return planningrole_;
}
inline void RtTpsProtoVoiSetting::set_planningrole(::google::protobuf::int32 value) {
  set_has_planningrole();
  planningrole_ = value;
}

// optional int32 biologyRole = 7;
inline bool RtTpsProtoVoiSetting::has_biologyrole() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void RtTpsProtoVoiSetting::set_has_biologyrole() {
  _has_bits_[0] |= 0x00000040u;
}
inline void RtTpsProtoVoiSetting::clear_has_biologyrole() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void RtTpsProtoVoiSetting::clear_biologyrole() {
  biologyrole_ = 0;
  clear_has_biologyrole();
}
inline ::google::protobuf::int32 RtTpsProtoVoiSetting::biologyrole() const {
  return biologyrole_;
}
inline void RtTpsProtoVoiSetting::set_biologyrole(::google::protobuf::int32 value) {
  set_has_biologyrole();
  biologyrole_ = value;
}

// optional string roicolor = 8;
inline bool RtTpsProtoVoiSetting::has_roicolor() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void RtTpsProtoVoiSetting::set_has_roicolor() {
  _has_bits_[0] |= 0x00000080u;
}
inline void RtTpsProtoVoiSetting::clear_has_roicolor() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void RtTpsProtoVoiSetting::clear_roicolor() {
  if (roicolor_ != &::google::protobuf::internal::kEmptyString) {
    roicolor_->clear();
  }
  clear_has_roicolor();
}
inline const ::std::string& RtTpsProtoVoiSetting::roicolor() const {
  return *roicolor_;
}
inline void RtTpsProtoVoiSetting::set_roicolor(const ::std::string& value) {
  set_has_roicolor();
  if (roicolor_ == &::google::protobuf::internal::kEmptyString) {
    roicolor_ = new ::std::string;
  }
  roicolor_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_roicolor(const char* value) {
  set_has_roicolor();
  if (roicolor_ == &::google::protobuf::internal::kEmptyString) {
    roicolor_ = new ::std::string;
  }
  roicolor_->assign(value);
}
inline void RtTpsProtoVoiSetting::set_roicolor(const char* value, size_t size) {
  set_has_roicolor();
  if (roicolor_ == &::google::protobuf::internal::kEmptyString) {
    roicolor_ = new ::std::string;
  }
  roicolor_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoVoiSetting::mutable_roicolor() {
  set_has_roicolor();
  if (roicolor_ == &::google::protobuf::internal::kEmptyString) {
    roicolor_ = new ::std::string;
  }
  return roicolor_;
}
inline ::std::string* RtTpsProtoVoiSetting::release_roicolor() {
  clear_has_roicolor();
  if (roicolor_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = roicolor_;
    roicolor_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// -------------------------------------------------------------------

// RtTpsProtoRepeatedVoiSetting

// repeated .tps.proto.RtTpsProtoVoiSetting voiSetting = 1;
inline int RtTpsProtoRepeatedVoiSetting::voisetting_size() const {
  return voisetting_.size();
}
inline void RtTpsProtoRepeatedVoiSetting::clear_voisetting() {
  voisetting_.Clear();
}
inline const ::tps::proto::RtTpsProtoVoiSetting& RtTpsProtoRepeatedVoiSetting::voisetting(int index) const {
  return voisetting_.Get(index);
}
inline ::tps::proto::RtTpsProtoVoiSetting* RtTpsProtoRepeatedVoiSetting::mutable_voisetting(int index) {
  return voisetting_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoVoiSetting* RtTpsProtoRepeatedVoiSetting::add_voisetting() {
  return voisetting_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoVoiSetting >&
RtTpsProtoRepeatedVoiSetting::voisetting() const {
  return voisetting_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoVoiSetting >*
RtTpsProtoRepeatedVoiSetting::mutable_voisetting() {
  return &voisetting_;
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

#endif  // PROTOBUF_rt_5ftps_5fproto_5fvoi_5fsetting_2eproto__INCLUDED