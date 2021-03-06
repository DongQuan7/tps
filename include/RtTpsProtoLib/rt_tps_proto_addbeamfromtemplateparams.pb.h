// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rt_tps_proto_addbeamfromtemplateparams.proto

#ifndef PROTOBUF_rt_5ftps_5fproto_5faddbeamfromtemplateparams_2eproto__INCLUDED
#define PROTOBUF_rt_5ftps_5fproto_5faddbeamfromtemplateparams_2eproto__INCLUDED

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
void  protobuf_AddDesc_rt_5ftps_5fproto_5faddbeamfromtemplateparams_2eproto();
void protobuf_AssignDesc_rt_5ftps_5fproto_5faddbeamfromtemplateparams_2eproto();
void protobuf_ShutdownFile_rt_5ftps_5fproto_5faddbeamfromtemplateparams_2eproto();

class RtTpsProtoAddBeamFromTemplateParams;

// ===================================================================

class RtTpsProtoAddBeamFromTemplateParams : public ::google::protobuf::Message {
 public:
  RtTpsProtoAddBeamFromTemplateParams();
  virtual ~RtTpsProtoAddBeamFromTemplateParams();
  
  RtTpsProtoAddBeamFromTemplateParams(const RtTpsProtoAddBeamFromTemplateParams& from);
  
  inline RtTpsProtoAddBeamFromTemplateParams& operator=(const RtTpsProtoAddBeamFromTemplateParams& from) {
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
  static const RtTpsProtoAddBeamFromTemplateParams& default_instance();
  
  void Swap(RtTpsProtoAddBeamFromTemplateParams* other);
  
  // implements Message ----------------------------------------------
  
  RtTpsProtoAddBeamFromTemplateParams* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RtTpsProtoAddBeamFromTemplateParams& from);
  void MergeFrom(const RtTpsProtoAddBeamFromTemplateParams& from);
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
  
  // optional string beamTemplateUID = 1;
  inline bool has_beamtemplateuid() const;
  inline void clear_beamtemplateuid();
  static const int kBeamTemplateUIDFieldNumber = 1;
  inline const ::std::string& beamtemplateuid() const;
  inline void set_beamtemplateuid(const ::std::string& value);
  inline void set_beamtemplateuid(const char* value);
  inline void set_beamtemplateuid(const char* value, size_t size);
  inline ::std::string* mutable_beamtemplateuid();
  inline ::std::string* release_beamtemplateuid();
  
  // optional string normalizationGroupUID = 2;
  inline bool has_normalizationgroupuid() const;
  inline void clear_normalizationgroupuid();
  static const int kNormalizationGroupUIDFieldNumber = 2;
  inline const ::std::string& normalizationgroupuid() const;
  inline void set_normalizationgroupuid(const ::std::string& value);
  inline void set_normalizationgroupuid(const char* value);
  inline void set_normalizationgroupuid(const char* value, size_t size);
  inline ::std::string* mutable_normalizationgroupuid();
  inline ::std::string* release_normalizationgroupuid();
  
  // optional string commissionedUnitUID = 3;
  inline bool has_commissionedunituid() const;
  inline void clear_commissionedunituid();
  static const int kCommissionedUnitUIDFieldNumber = 3;
  inline const ::std::string& commissionedunituid() const;
  inline void set_commissionedunituid(const ::std::string& value);
  inline void set_commissionedunituid(const char* value);
  inline void set_commissionedunituid(const char* value, size_t size);
  inline ::std::string* mutable_commissionedunituid();
  inline ::std::string* release_commissionedunituid();
  
  // optional string isocenterUID = 4;
  inline bool has_isocenteruid() const;
  inline void clear_isocenteruid();
  static const int kIsocenterUIDFieldNumber = 4;
  inline const ::std::string& isocenteruid() const;
  inline void set_isocenteruid(const ::std::string& value);
  inline void set_isocenteruid(const char* value);
  inline void set_isocenteruid(const char* value, size_t size);
  inline ::std::string* mutable_isocenteruid();
  inline ::std::string* release_isocenteruid();
  
  // @@protoc_insertion_point(class_scope:tps.proto.RtTpsProtoAddBeamFromTemplateParams)
 private:
  inline void set_has_beamtemplateuid();
  inline void clear_has_beamtemplateuid();
  inline void set_has_normalizationgroupuid();
  inline void clear_has_normalizationgroupuid();
  inline void set_has_commissionedunituid();
  inline void clear_has_commissionedunituid();
  inline void set_has_isocenteruid();
  inline void clear_has_isocenteruid();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* beamtemplateuid_;
  ::std::string* normalizationgroupuid_;
  ::std::string* commissionedunituid_;
  ::std::string* isocenteruid_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5fproto_5faddbeamfromtemplateparams_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5fproto_5faddbeamfromtemplateparams_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5fproto_5faddbeamfromtemplateparams_2eproto();
  
  void InitAsDefaultInstance();
  static RtTpsProtoAddBeamFromTemplateParams* default_instance_;
};
// ===================================================================


// ===================================================================

// RtTpsProtoAddBeamFromTemplateParams

// optional string beamTemplateUID = 1;
inline bool RtTpsProtoAddBeamFromTemplateParams::has_beamtemplateuid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_has_beamtemplateuid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RtTpsProtoAddBeamFromTemplateParams::clear_has_beamtemplateuid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RtTpsProtoAddBeamFromTemplateParams::clear_beamtemplateuid() {
  if (beamtemplateuid_ != &::google::protobuf::internal::kEmptyString) {
    beamtemplateuid_->clear();
  }
  clear_has_beamtemplateuid();
}
inline const ::std::string& RtTpsProtoAddBeamFromTemplateParams::beamtemplateuid() const {
  return *beamtemplateuid_;
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_beamtemplateuid(const ::std::string& value) {
  set_has_beamtemplateuid();
  if (beamtemplateuid_ == &::google::protobuf::internal::kEmptyString) {
    beamtemplateuid_ = new ::std::string;
  }
  beamtemplateuid_->assign(value);
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_beamtemplateuid(const char* value) {
  set_has_beamtemplateuid();
  if (beamtemplateuid_ == &::google::protobuf::internal::kEmptyString) {
    beamtemplateuid_ = new ::std::string;
  }
  beamtemplateuid_->assign(value);
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_beamtemplateuid(const char* value, size_t size) {
  set_has_beamtemplateuid();
  if (beamtemplateuid_ == &::google::protobuf::internal::kEmptyString) {
    beamtemplateuid_ = new ::std::string;
  }
  beamtemplateuid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoAddBeamFromTemplateParams::mutable_beamtemplateuid() {
  set_has_beamtemplateuid();
  if (beamtemplateuid_ == &::google::protobuf::internal::kEmptyString) {
    beamtemplateuid_ = new ::std::string;
  }
  return beamtemplateuid_;
}
inline ::std::string* RtTpsProtoAddBeamFromTemplateParams::release_beamtemplateuid() {
  clear_has_beamtemplateuid();
  if (beamtemplateuid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = beamtemplateuid_;
    beamtemplateuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string normalizationGroupUID = 2;
inline bool RtTpsProtoAddBeamFromTemplateParams::has_normalizationgroupuid() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_has_normalizationgroupuid() {
  _has_bits_[0] |= 0x00000002u;
}
inline void RtTpsProtoAddBeamFromTemplateParams::clear_has_normalizationgroupuid() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void RtTpsProtoAddBeamFromTemplateParams::clear_normalizationgroupuid() {
  if (normalizationgroupuid_ != &::google::protobuf::internal::kEmptyString) {
    normalizationgroupuid_->clear();
  }
  clear_has_normalizationgroupuid();
}
inline const ::std::string& RtTpsProtoAddBeamFromTemplateParams::normalizationgroupuid() const {
  return *normalizationgroupuid_;
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_normalizationgroupuid(const ::std::string& value) {
  set_has_normalizationgroupuid();
  if (normalizationgroupuid_ == &::google::protobuf::internal::kEmptyString) {
    normalizationgroupuid_ = new ::std::string;
  }
  normalizationgroupuid_->assign(value);
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_normalizationgroupuid(const char* value) {
  set_has_normalizationgroupuid();
  if (normalizationgroupuid_ == &::google::protobuf::internal::kEmptyString) {
    normalizationgroupuid_ = new ::std::string;
  }
  normalizationgroupuid_->assign(value);
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_normalizationgroupuid(const char* value, size_t size) {
  set_has_normalizationgroupuid();
  if (normalizationgroupuid_ == &::google::protobuf::internal::kEmptyString) {
    normalizationgroupuid_ = new ::std::string;
  }
  normalizationgroupuid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoAddBeamFromTemplateParams::mutable_normalizationgroupuid() {
  set_has_normalizationgroupuid();
  if (normalizationgroupuid_ == &::google::protobuf::internal::kEmptyString) {
    normalizationgroupuid_ = new ::std::string;
  }
  return normalizationgroupuid_;
}
inline ::std::string* RtTpsProtoAddBeamFromTemplateParams::release_normalizationgroupuid() {
  clear_has_normalizationgroupuid();
  if (normalizationgroupuid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = normalizationgroupuid_;
    normalizationgroupuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string commissionedUnitUID = 3;
inline bool RtTpsProtoAddBeamFromTemplateParams::has_commissionedunituid() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_has_commissionedunituid() {
  _has_bits_[0] |= 0x00000004u;
}
inline void RtTpsProtoAddBeamFromTemplateParams::clear_has_commissionedunituid() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void RtTpsProtoAddBeamFromTemplateParams::clear_commissionedunituid() {
  if (commissionedunituid_ != &::google::protobuf::internal::kEmptyString) {
    commissionedunituid_->clear();
  }
  clear_has_commissionedunituid();
}
inline const ::std::string& RtTpsProtoAddBeamFromTemplateParams::commissionedunituid() const {
  return *commissionedunituid_;
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_commissionedunituid(const ::std::string& value) {
  set_has_commissionedunituid();
  if (commissionedunituid_ == &::google::protobuf::internal::kEmptyString) {
    commissionedunituid_ = new ::std::string;
  }
  commissionedunituid_->assign(value);
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_commissionedunituid(const char* value) {
  set_has_commissionedunituid();
  if (commissionedunituid_ == &::google::protobuf::internal::kEmptyString) {
    commissionedunituid_ = new ::std::string;
  }
  commissionedunituid_->assign(value);
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_commissionedunituid(const char* value, size_t size) {
  set_has_commissionedunituid();
  if (commissionedunituid_ == &::google::protobuf::internal::kEmptyString) {
    commissionedunituid_ = new ::std::string;
  }
  commissionedunituid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoAddBeamFromTemplateParams::mutable_commissionedunituid() {
  set_has_commissionedunituid();
  if (commissionedunituid_ == &::google::protobuf::internal::kEmptyString) {
    commissionedunituid_ = new ::std::string;
  }
  return commissionedunituid_;
}
inline ::std::string* RtTpsProtoAddBeamFromTemplateParams::release_commissionedunituid() {
  clear_has_commissionedunituid();
  if (commissionedunituid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = commissionedunituid_;
    commissionedunituid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string isocenterUID = 4;
inline bool RtTpsProtoAddBeamFromTemplateParams::has_isocenteruid() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_has_isocenteruid() {
  _has_bits_[0] |= 0x00000008u;
}
inline void RtTpsProtoAddBeamFromTemplateParams::clear_has_isocenteruid() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void RtTpsProtoAddBeamFromTemplateParams::clear_isocenteruid() {
  if (isocenteruid_ != &::google::protobuf::internal::kEmptyString) {
    isocenteruid_->clear();
  }
  clear_has_isocenteruid();
}
inline const ::std::string& RtTpsProtoAddBeamFromTemplateParams::isocenteruid() const {
  return *isocenteruid_;
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_isocenteruid(const ::std::string& value) {
  set_has_isocenteruid();
  if (isocenteruid_ == &::google::protobuf::internal::kEmptyString) {
    isocenteruid_ = new ::std::string;
  }
  isocenteruid_->assign(value);
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_isocenteruid(const char* value) {
  set_has_isocenteruid();
  if (isocenteruid_ == &::google::protobuf::internal::kEmptyString) {
    isocenteruid_ = new ::std::string;
  }
  isocenteruid_->assign(value);
}
inline void RtTpsProtoAddBeamFromTemplateParams::set_isocenteruid(const char* value, size_t size) {
  set_has_isocenteruid();
  if (isocenteruid_ == &::google::protobuf::internal::kEmptyString) {
    isocenteruid_ = new ::std::string;
  }
  isocenteruid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RtTpsProtoAddBeamFromTemplateParams::mutable_isocenteruid() {
  set_has_isocenteruid();
  if (isocenteruid_ == &::google::protobuf::internal::kEmptyString) {
    isocenteruid_ = new ::std::string;
  }
  return isocenteruid_;
}
inline ::std::string* RtTpsProtoAddBeamFromTemplateParams::release_isocenteruid() {
  clear_has_isocenteruid();
  if (isocenteruid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = isocenteruid_;
    isocenteruid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
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

#endif  // PROTOBUF_rt_5ftps_5fproto_5faddbeamfromtemplateparams_2eproto__INCLUDED
