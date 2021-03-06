// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rt_tps_drrsetting.proto

#ifndef PROTOBUF_rt_5ftps_5fdrrsetting_2eproto__INCLUDED
#define PROTOBUF_rt_5ftps_5fdrrsetting_2eproto__INCLUDED

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
void  protobuf_AddDesc_rt_5ftps_5fdrrsetting_2eproto();
void protobuf_AssignDesc_rt_5ftps_5fdrrsetting_2eproto();
void protobuf_ShutdownFile_rt_5ftps_5fdrrsetting_2eproto();

class RtTpsDRRSetting;

// ===================================================================

class RtTpsDRRSetting : public ::google::protobuf::Message {
 public:
  RtTpsDRRSetting();
  virtual ~RtTpsDRRSetting();
  
  RtTpsDRRSetting(const RtTpsDRRSetting& from);
  
  inline RtTpsDRRSetting& operator=(const RtTpsDRRSetting& from) {
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
  static const RtTpsDRRSetting& default_instance();
  
  void Swap(RtTpsDRRSetting* other);
  
  // implements Message ----------------------------------------------
  
  RtTpsDRRSetting* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RtTpsDRRSetting& from);
  void MergeFrom(const RtTpsDRRSetting& from);
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
  
  // optional string CT2DensityTableName = 1;
  inline bool has_ct2densitytablename() const;
  inline void clear_ct2densitytablename();
  static const int kCT2DensityTableNameFieldNumber = 1;
  inline const ::std::string& ct2densitytablename() const;
  inline void set_ct2densitytablename(const ::std::string& value);
  inline void set_ct2densitytablename(const char* value);
  inline void set_ct2densitytablename(const char* value, size_t size);
  inline ::std::string* mutable_ct2densitytablename();
  inline ::std::string* release_ct2densitytablename();
  
  // optional string CT2DensityTableUid = 2;
  inline bool has_ct2densitytableuid() const;
  inline void clear_ct2densitytableuid();
  static const int kCT2DensityTableUidFieldNumber = 2;
  inline const ::std::string& ct2densitytableuid() const;
  inline void set_ct2densitytableuid(const ::std::string& value);
  inline void set_ct2densitytableuid(const char* value);
  inline void set_ct2densitytableuid(const char* value, size_t size);
  inline ::std::string* mutable_ct2densitytableuid();
  inline ::std::string* release_ct2densitytableuid();
  
  // optional float ElectronEnergy = 3;
  inline bool has_electronenergy() const;
  inline void clear_electronenergy();
  static const int kElectronEnergyFieldNumber = 3;
  inline float electronenergy() const;
  inline void set_electronenergy(float value);
  
  // optional string seriesUid = 4;
  inline bool has_seriesuid() const;
  inline void clear_seriesuid();
  static const int kSeriesUidFieldNumber = 4;
  inline const ::std::string& seriesuid() const;
  inline void set_seriesuid(const ::std::string& value);
  inline void set_seriesuid(const char* value);
  inline void set_seriesuid(const char* value, size_t size);
  inline ::std::string* mutable_seriesuid();
  inline ::std::string* release_seriesuid();
  
  // optional string normalgroupUid = 5;
  inline bool has_normalgroupuid() const;
  inline void clear_normalgroupuid();
  static const int kNormalgroupUidFieldNumber = 5;
  inline const ::std::string& normalgroupuid() const;
  inline void set_normalgroupuid(const ::std::string& value);
  inline void set_normalgroupuid(const char* value);
  inline void set_normalgroupuid(const char* value, size_t size);
  inline ::std::string* mutable_normalgroupuid();
  inline ::std::string* release_normalgroupuid();
  
  // optional float ctthresholdMin = 6;
  inline bool has_ctthresholdmin() const;
  inline void clear_ctthresholdmin();
  static const int kCtthresholdMinFieldNumber = 6;
  inline float ctthresholdmin() const;
  inline void set_ctthresholdmin(float value);
  
  // optional float ctthresholdMax = 7;
  inline bool has_ctthresholdmax() const;
  inline void clear_ctthresholdmax();
  static const int kCtthresholdMaxFieldNumber = 7;
  inline float ctthresholdmax() const;
  inline void set_ctthresholdmax(float value);
  
  // @@protoc_insertion_point(class_scope:tps.proto.RtTpsDRRSetting)
 private:
  inline void set_has_ct2densitytablename();
  inline void clear_has_ct2densitytablename();
  inline void set_has_ct2densitytableuid();
  inline void clear_has_ct2densitytableuid();
  inline void set_has_electronenergy();
  inline void clear_has_electronenergy();
  inline void set_has_seriesuid();
  inline void clear_has_seriesuid();
  inline void set_has_normalgroupuid();
  inline void clear_has_normalgroupuid();
  inline void set_has_ctthresholdmin();
  inline void clear_has_ctthresholdmin();
  inline void set_has_ctthresholdmax();
  inline void clear_has_ctthresholdmax();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* ct2densitytablename_;
  ::std::string* ct2densitytableuid_;
  ::std::string* seriesuid_;
  float electronenergy_;
  float ctthresholdmin_;
  ::std::string* normalgroupuid_;
  float ctthresholdmax_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(7 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5fdrrsetting_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5fdrrsetting_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5fdrrsetting_2eproto();
  
  void InitAsDefaultInstance();
  static RtTpsDRRSetting* default_instance_;
};
// ===================================================================


// ===================================================================

// RtTpsDRRSetting

// optional string CT2DensityTableName = 1;
inline bool RtTpsDRRSetting::has_ct2densitytablename() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RtTpsDRRSetting::set_has_ct2densitytablename() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RtTpsDRRSetting::clear_has_ct2densitytablename() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RtTpsDRRSetting::clear_ct2densitytablename() {
  if (ct2densitytablename_ != &::google::protobuf::internal::kEmptyString) {
    ct2densitytablename_->clear();
  }
  clear_has_ct2densitytablename();
}
inline const ::std::string& RtTpsDRRSetting::ct2densitytablename() const {
  return *ct2densitytablename_;
}
inline void RtTpsDRRSetting::set_ct2densitytablename(const ::std::string& value) {
  set_has_ct2densitytablename();
  if (ct2densitytablename_ == &::google::protobuf::internal::kEmptyString) {
    ct2densitytablename_ = new ::std::string;
  }
  ct2densitytablename_->assign(value);
}
inline void RtTpsDRRSetting::set_ct2densitytablename(const char* value) {
  set_has_ct2densitytablename();
  if (ct2densitytablename_ == &::google::protobuf::internal::kEmptyString) {
    ct2densitytablename_ = new ::std::string;
  }
  ct2densitytablename_->assign(value);
}
inline void RtTpsDRRSetting::set_ct2densitytablename(const char* value, size_t size) {
  set_has_ct2densitytablename();
  if (ct2densitytablename_ == &::google::protobuf::internal::kEmptyString) {
    ct2densitytablename_ = new ::std::string;
  }
  ct2densitytablename_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsDRRSetting::mutable_ct2densitytablename() {
  set_has_ct2densitytablename();
  if (ct2densitytablename_ == &::google::protobuf::internal::kEmptyString) {
    ct2densitytablename_ = new ::std::string;
  }
  return ct2densitytablename_;
}
inline ::std::string* RtTpsDRRSetting::release_ct2densitytablename() {
  clear_has_ct2densitytablename();
  if (ct2densitytablename_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = ct2densitytablename_;
    ct2densitytablename_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string CT2DensityTableUid = 2;
inline bool RtTpsDRRSetting::has_ct2densitytableuid() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void RtTpsDRRSetting::set_has_ct2densitytableuid() {
  _has_bits_[0] |= 0x00000002u;
}
inline void RtTpsDRRSetting::clear_has_ct2densitytableuid() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void RtTpsDRRSetting::clear_ct2densitytableuid() {
  if (ct2densitytableuid_ != &::google::protobuf::internal::kEmptyString) {
    ct2densitytableuid_->clear();
  }
  clear_has_ct2densitytableuid();
}
inline const ::std::string& RtTpsDRRSetting::ct2densitytableuid() const {
  return *ct2densitytableuid_;
}
inline void RtTpsDRRSetting::set_ct2densitytableuid(const ::std::string& value) {
  set_has_ct2densitytableuid();
  if (ct2densitytableuid_ == &::google::protobuf::internal::kEmptyString) {
    ct2densitytableuid_ = new ::std::string;
  }
  ct2densitytableuid_->assign(value);
}
inline void RtTpsDRRSetting::set_ct2densitytableuid(const char* value) {
  set_has_ct2densitytableuid();
  if (ct2densitytableuid_ == &::google::protobuf::internal::kEmptyString) {
    ct2densitytableuid_ = new ::std::string;
  }
  ct2densitytableuid_->assign(value);
}
inline void RtTpsDRRSetting::set_ct2densitytableuid(const char* value, size_t size) {
  set_has_ct2densitytableuid();
  if (ct2densitytableuid_ == &::google::protobuf::internal::kEmptyString) {
    ct2densitytableuid_ = new ::std::string;
  }
  ct2densitytableuid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsDRRSetting::mutable_ct2densitytableuid() {
  set_has_ct2densitytableuid();
  if (ct2densitytableuid_ == &::google::protobuf::internal::kEmptyString) {
    ct2densitytableuid_ = new ::std::string;
  }
  return ct2densitytableuid_;
}
inline ::std::string* RtTpsDRRSetting::release_ct2densitytableuid() {
  clear_has_ct2densitytableuid();
  if (ct2densitytableuid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = ct2densitytableuid_;
    ct2densitytableuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional float ElectronEnergy = 3;
inline bool RtTpsDRRSetting::has_electronenergy() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void RtTpsDRRSetting::set_has_electronenergy() {
  _has_bits_[0] |= 0x00000004u;
}
inline void RtTpsDRRSetting::clear_has_electronenergy() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void RtTpsDRRSetting::clear_electronenergy() {
  electronenergy_ = 0;
  clear_has_electronenergy();
}
inline float RtTpsDRRSetting::electronenergy() const {
  return electronenergy_;
}
inline void RtTpsDRRSetting::set_electronenergy(float value) {
  set_has_electronenergy();
  electronenergy_ = value;
}

// optional string seriesUid = 4;
inline bool RtTpsDRRSetting::has_seriesuid() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void RtTpsDRRSetting::set_has_seriesuid() {
  _has_bits_[0] |= 0x00000008u;
}
inline void RtTpsDRRSetting::clear_has_seriesuid() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void RtTpsDRRSetting::clear_seriesuid() {
  if (seriesuid_ != &::google::protobuf::internal::kEmptyString) {
    seriesuid_->clear();
  }
  clear_has_seriesuid();
}
inline const ::std::string& RtTpsDRRSetting::seriesuid() const {
  return *seriesuid_;
}
inline void RtTpsDRRSetting::set_seriesuid(const ::std::string& value) {
  set_has_seriesuid();
  if (seriesuid_ == &::google::protobuf::internal::kEmptyString) {
    seriesuid_ = new ::std::string;
  }
  seriesuid_->assign(value);
}
inline void RtTpsDRRSetting::set_seriesuid(const char* value) {
  set_has_seriesuid();
  if (seriesuid_ == &::google::protobuf::internal::kEmptyString) {
    seriesuid_ = new ::std::string;
  }
  seriesuid_->assign(value);
}
inline void RtTpsDRRSetting::set_seriesuid(const char* value, size_t size) {
  set_has_seriesuid();
  if (seriesuid_ == &::google::protobuf::internal::kEmptyString) {
    seriesuid_ = new ::std::string;
  }
  seriesuid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsDRRSetting::mutable_seriesuid() {
  set_has_seriesuid();
  if (seriesuid_ == &::google::protobuf::internal::kEmptyString) {
    seriesuid_ = new ::std::string;
  }
  return seriesuid_;
}
inline ::std::string* RtTpsDRRSetting::release_seriesuid() {
  clear_has_seriesuid();
  if (seriesuid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = seriesuid_;
    seriesuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string normalgroupUid = 5;
inline bool RtTpsDRRSetting::has_normalgroupuid() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void RtTpsDRRSetting::set_has_normalgroupuid() {
  _has_bits_[0] |= 0x00000010u;
}
inline void RtTpsDRRSetting::clear_has_normalgroupuid() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void RtTpsDRRSetting::clear_normalgroupuid() {
  if (normalgroupuid_ != &::google::protobuf::internal::kEmptyString) {
    normalgroupuid_->clear();
  }
  clear_has_normalgroupuid();
}
inline const ::std::string& RtTpsDRRSetting::normalgroupuid() const {
  return *normalgroupuid_;
}
inline void RtTpsDRRSetting::set_normalgroupuid(const ::std::string& value) {
  set_has_normalgroupuid();
  if (normalgroupuid_ == &::google::protobuf::internal::kEmptyString) {
    normalgroupuid_ = new ::std::string;
  }
  normalgroupuid_->assign(value);
}
inline void RtTpsDRRSetting::set_normalgroupuid(const char* value) {
  set_has_normalgroupuid();
  if (normalgroupuid_ == &::google::protobuf::internal::kEmptyString) {
    normalgroupuid_ = new ::std::string;
  }
  normalgroupuid_->assign(value);
}
inline void RtTpsDRRSetting::set_normalgroupuid(const char* value, size_t size) {
  set_has_normalgroupuid();
  if (normalgroupuid_ == &::google::protobuf::internal::kEmptyString) {
    normalgroupuid_ = new ::std::string;
  }
  normalgroupuid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsDRRSetting::mutable_normalgroupuid() {
  set_has_normalgroupuid();
  if (normalgroupuid_ == &::google::protobuf::internal::kEmptyString) {
    normalgroupuid_ = new ::std::string;
  }
  return normalgroupuid_;
}
inline ::std::string* RtTpsDRRSetting::release_normalgroupuid() {
  clear_has_normalgroupuid();
  if (normalgroupuid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = normalgroupuid_;
    normalgroupuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional float ctthresholdMin = 6;
inline bool RtTpsDRRSetting::has_ctthresholdmin() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void RtTpsDRRSetting::set_has_ctthresholdmin() {
  _has_bits_[0] |= 0x00000020u;
}
inline void RtTpsDRRSetting::clear_has_ctthresholdmin() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void RtTpsDRRSetting::clear_ctthresholdmin() {
  ctthresholdmin_ = 0;
  clear_has_ctthresholdmin();
}
inline float RtTpsDRRSetting::ctthresholdmin() const {
  return ctthresholdmin_;
}
inline void RtTpsDRRSetting::set_ctthresholdmin(float value) {
  set_has_ctthresholdmin();
  ctthresholdmin_ = value;
}

// optional float ctthresholdMax = 7;
inline bool RtTpsDRRSetting::has_ctthresholdmax() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void RtTpsDRRSetting::set_has_ctthresholdmax() {
  _has_bits_[0] |= 0x00000040u;
}
inline void RtTpsDRRSetting::clear_has_ctthresholdmax() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void RtTpsDRRSetting::clear_ctthresholdmax() {
  ctthresholdmax_ = 0;
  clear_has_ctthresholdmax();
}
inline float RtTpsDRRSetting::ctthresholdmax() const {
  return ctthresholdmax_;
}
inline void RtTpsDRRSetting::set_ctthresholdmax(float value) {
  set_has_ctthresholdmax();
  ctthresholdmax_ = value;
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

#endif  // PROTOBUF_rt_5ftps_5fdrrsetting_2eproto__INCLUDED
