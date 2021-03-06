// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rt_tps_beam_template.proto

#ifndef PROTOBUF_rt_5ftps_5fbeam_5ftemplate_2eproto__INCLUDED
#define PROTOBUF_rt_5ftps_5fbeam_5ftemplate_2eproto__INCLUDED

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
#include "rt_tps_simplebeam.pb.h"
// @@protoc_insertion_point(includes)

namespace tps {
namespace proto {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_rt_5ftps_5fbeam_5ftemplate_2eproto();
void protobuf_AssignDesc_rt_5ftps_5fbeam_5ftemplate_2eproto();
void protobuf_ShutdownFile_rt_5ftps_5fbeam_5ftemplate_2eproto();

class RT_TPS_BeamTemplate;
class RT_TPS_BeamTemplate_List;

// ===================================================================

class RT_TPS_BeamTemplate : public ::google::protobuf::Message {
 public:
  RT_TPS_BeamTemplate();
  virtual ~RT_TPS_BeamTemplate();
  
  RT_TPS_BeamTemplate(const RT_TPS_BeamTemplate& from);
  
  inline RT_TPS_BeamTemplate& operator=(const RT_TPS_BeamTemplate& from) {
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
  static const RT_TPS_BeamTemplate& default_instance();
  
  void Swap(RT_TPS_BeamTemplate* other);
  
  // implements Message ----------------------------------------------
  
  RT_TPS_BeamTemplate* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RT_TPS_BeamTemplate& from);
  void MergeFrom(const RT_TPS_BeamTemplate& from);
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
  
  // optional string templateUID = 1;
  inline bool has_templateuid() const;
  inline void clear_templateuid();
  static const int kTemplateUIDFieldNumber = 1;
  inline const ::std::string& templateuid() const;
  inline void set_templateuid(const ::std::string& value);
  inline void set_templateuid(const char* value);
  inline void set_templateuid(const char* value, size_t size);
  inline ::std::string* mutable_templateuid();
  inline ::std::string* release_templateuid();
  
  // optional string templateName = 2;
  inline bool has_templatename() const;
  inline void clear_templatename();
  static const int kTemplateNameFieldNumber = 2;
  inline const ::std::string& templatename() const;
  inline void set_templatename(const ::std::string& value);
  inline void set_templatename(const char* value);
  inline void set_templatename(const char* value, size_t size);
  inline ::std::string* mutable_templatename();
  inline ::std::string* release_templatename();
  
  // optional string templateDescription = 3;
  inline bool has_templatedescription() const;
  inline void clear_templatedescription();
  static const int kTemplateDescriptionFieldNumber = 3;
  inline const ::std::string& templatedescription() const;
  inline void set_templatedescription(const ::std::string& value);
  inline void set_templatedescription(const char* value);
  inline void set_templatedescription(const char* value, size_t size);
  inline ::std::string* mutable_templatedescription();
  inline ::std::string* release_templatedescription();
  
  // repeated .tps.proto.RT_TPS_SimpleBeam simpleBeam = 4;
  inline int simplebeam_size() const;
  inline void clear_simplebeam();
  static const int kSimpleBeamFieldNumber = 4;
  inline const ::tps::proto::RT_TPS_SimpleBeam& simplebeam(int index) const;
  inline ::tps::proto::RT_TPS_SimpleBeam* mutable_simplebeam(int index);
  inline ::tps::proto::RT_TPS_SimpleBeam* add_simplebeam();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_SimpleBeam >&
      simplebeam() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_SimpleBeam >*
      mutable_simplebeam();
  
  // @@protoc_insertion_point(class_scope:tps.proto.RT_TPS_BeamTemplate)
 private:
  inline void set_has_templateuid();
  inline void clear_has_templateuid();
  inline void set_has_templatename();
  inline void clear_has_templatename();
  inline void set_has_templatedescription();
  inline void clear_has_templatedescription();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* templateuid_;
  ::std::string* templatename_;
  ::std::string* templatedescription_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_SimpleBeam > simplebeam_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5fbeam_5ftemplate_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5fbeam_5ftemplate_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5fbeam_5ftemplate_2eproto();
  
  void InitAsDefaultInstance();
  static RT_TPS_BeamTemplate* default_instance_;
};
// -------------------------------------------------------------------

class RT_TPS_BeamTemplate_List : public ::google::protobuf::Message {
 public:
  RT_TPS_BeamTemplate_List();
  virtual ~RT_TPS_BeamTemplate_List();
  
  RT_TPS_BeamTemplate_List(const RT_TPS_BeamTemplate_List& from);
  
  inline RT_TPS_BeamTemplate_List& operator=(const RT_TPS_BeamTemplate_List& from) {
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
  static const RT_TPS_BeamTemplate_List& default_instance();
  
  void Swap(RT_TPS_BeamTemplate_List* other);
  
  // implements Message ----------------------------------------------
  
  RT_TPS_BeamTemplate_List* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RT_TPS_BeamTemplate_List& from);
  void MergeFrom(const RT_TPS_BeamTemplate_List& from);
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
  
  // repeated .tps.proto.RT_TPS_BeamTemplate beamTemplate = 1;
  inline int beamtemplate_size() const;
  inline void clear_beamtemplate();
  static const int kBeamTemplateFieldNumber = 1;
  inline const ::tps::proto::RT_TPS_BeamTemplate& beamtemplate(int index) const;
  inline ::tps::proto::RT_TPS_BeamTemplate* mutable_beamtemplate(int index);
  inline ::tps::proto::RT_TPS_BeamTemplate* add_beamtemplate();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_BeamTemplate >&
      beamtemplate() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_BeamTemplate >*
      mutable_beamtemplate();
  
  // @@protoc_insertion_point(class_scope:tps.proto.RT_TPS_BeamTemplate_List)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_BeamTemplate > beamtemplate_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5fbeam_5ftemplate_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5fbeam_5ftemplate_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5fbeam_5ftemplate_2eproto();
  
  void InitAsDefaultInstance();
  static RT_TPS_BeamTemplate_List* default_instance_;
};
// ===================================================================


// ===================================================================

// RT_TPS_BeamTemplate

// optional string templateUID = 1;
inline bool RT_TPS_BeamTemplate::has_templateuid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RT_TPS_BeamTemplate::set_has_templateuid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RT_TPS_BeamTemplate::clear_has_templateuid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RT_TPS_BeamTemplate::clear_templateuid() {
  if (templateuid_ != &::google::protobuf::internal::kEmptyString) {
    templateuid_->clear();
  }
  clear_has_templateuid();
}
inline const ::std::string& RT_TPS_BeamTemplate::templateuid() const {
  return *templateuid_;
}
inline void RT_TPS_BeamTemplate::set_templateuid(const ::std::string& value) {
  set_has_templateuid();
  if (templateuid_ == &::google::protobuf::internal::kEmptyString) {
    templateuid_ = new ::std::string;
  }
  templateuid_->assign(value);
}
inline void RT_TPS_BeamTemplate::set_templateuid(const char* value) {
  set_has_templateuid();
  if (templateuid_ == &::google::protobuf::internal::kEmptyString) {
    templateuid_ = new ::std::string;
  }
  templateuid_->assign(value);
}
inline void RT_TPS_BeamTemplate::set_templateuid(const char* value, size_t size) {
  set_has_templateuid();
  if (templateuid_ == &::google::protobuf::internal::kEmptyString) {
    templateuid_ = new ::std::string;
  }
  templateuid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_BeamTemplate::mutable_templateuid() {
  set_has_templateuid();
  if (templateuid_ == &::google::protobuf::internal::kEmptyString) {
    templateuid_ = new ::std::string;
  }
  return templateuid_;
}
inline ::std::string* RT_TPS_BeamTemplate::release_templateuid() {
  clear_has_templateuid();
  if (templateuid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = templateuid_;
    templateuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string templateName = 2;
inline bool RT_TPS_BeamTemplate::has_templatename() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void RT_TPS_BeamTemplate::set_has_templatename() {
  _has_bits_[0] |= 0x00000002u;
}
inline void RT_TPS_BeamTemplate::clear_has_templatename() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void RT_TPS_BeamTemplate::clear_templatename() {
  if (templatename_ != &::google::protobuf::internal::kEmptyString) {
    templatename_->clear();
  }
  clear_has_templatename();
}
inline const ::std::string& RT_TPS_BeamTemplate::templatename() const {
  return *templatename_;
}
inline void RT_TPS_BeamTemplate::set_templatename(const ::std::string& value) {
  set_has_templatename();
  if (templatename_ == &::google::protobuf::internal::kEmptyString) {
    templatename_ = new ::std::string;
  }
  templatename_->assign(value);
}
inline void RT_TPS_BeamTemplate::set_templatename(const char* value) {
  set_has_templatename();
  if (templatename_ == &::google::protobuf::internal::kEmptyString) {
    templatename_ = new ::std::string;
  }
  templatename_->assign(value);
}
inline void RT_TPS_BeamTemplate::set_templatename(const char* value, size_t size) {
  set_has_templatename();
  if (templatename_ == &::google::protobuf::internal::kEmptyString) {
    templatename_ = new ::std::string;
  }
  templatename_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_BeamTemplate::mutable_templatename() {
  set_has_templatename();
  if (templatename_ == &::google::protobuf::internal::kEmptyString) {
    templatename_ = new ::std::string;
  }
  return templatename_;
}
inline ::std::string* RT_TPS_BeamTemplate::release_templatename() {
  clear_has_templatename();
  if (templatename_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = templatename_;
    templatename_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional string templateDescription = 3;
inline bool RT_TPS_BeamTemplate::has_templatedescription() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void RT_TPS_BeamTemplate::set_has_templatedescription() {
  _has_bits_[0] |= 0x00000004u;
}
inline void RT_TPS_BeamTemplate::clear_has_templatedescription() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void RT_TPS_BeamTemplate::clear_templatedescription() {
  if (templatedescription_ != &::google::protobuf::internal::kEmptyString) {
    templatedescription_->clear();
  }
  clear_has_templatedescription();
}
inline const ::std::string& RT_TPS_BeamTemplate::templatedescription() const {
  return *templatedescription_;
}
inline void RT_TPS_BeamTemplate::set_templatedescription(const ::std::string& value) {
  set_has_templatedescription();
  if (templatedescription_ == &::google::protobuf::internal::kEmptyString) {
    templatedescription_ = new ::std::string;
  }
  templatedescription_->assign(value);
}
inline void RT_TPS_BeamTemplate::set_templatedescription(const char* value) {
  set_has_templatedescription();
  if (templatedescription_ == &::google::protobuf::internal::kEmptyString) {
    templatedescription_ = new ::std::string;
  }
  templatedescription_->assign(value);
}
inline void RT_TPS_BeamTemplate::set_templatedescription(const char* value, size_t size) {
  set_has_templatedescription();
  if (templatedescription_ == &::google::protobuf::internal::kEmptyString) {
    templatedescription_ = new ::std::string;
  }
  templatedescription_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_BeamTemplate::mutable_templatedescription() {
  set_has_templatedescription();
  if (templatedescription_ == &::google::protobuf::internal::kEmptyString) {
    templatedescription_ = new ::std::string;
  }
  return templatedescription_;
}
inline ::std::string* RT_TPS_BeamTemplate::release_templatedescription() {
  clear_has_templatedescription();
  if (templatedescription_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = templatedescription_;
    templatedescription_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// repeated .tps.proto.RT_TPS_SimpleBeam simpleBeam = 4;
inline int RT_TPS_BeamTemplate::simplebeam_size() const {
  return simplebeam_.size();
}
inline void RT_TPS_BeamTemplate::clear_simplebeam() {
  simplebeam_.Clear();
}
inline const ::tps::proto::RT_TPS_SimpleBeam& RT_TPS_BeamTemplate::simplebeam(int index) const {
  return simplebeam_.Get(index);
}
inline ::tps::proto::RT_TPS_SimpleBeam* RT_TPS_BeamTemplate::mutable_simplebeam(int index) {
  return simplebeam_.Mutable(index);
}
inline ::tps::proto::RT_TPS_SimpleBeam* RT_TPS_BeamTemplate::add_simplebeam() {
  return simplebeam_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_SimpleBeam >&
RT_TPS_BeamTemplate::simplebeam() const {
  return simplebeam_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_SimpleBeam >*
RT_TPS_BeamTemplate::mutable_simplebeam() {
  return &simplebeam_;
}

// -------------------------------------------------------------------

// RT_TPS_BeamTemplate_List

// repeated .tps.proto.RT_TPS_BeamTemplate beamTemplate = 1;
inline int RT_TPS_BeamTemplate_List::beamtemplate_size() const {
  return beamtemplate_.size();
}
inline void RT_TPS_BeamTemplate_List::clear_beamtemplate() {
  beamtemplate_.Clear();
}
inline const ::tps::proto::RT_TPS_BeamTemplate& RT_TPS_BeamTemplate_List::beamtemplate(int index) const {
  return beamtemplate_.Get(index);
}
inline ::tps::proto::RT_TPS_BeamTemplate* RT_TPS_BeamTemplate_List::mutable_beamtemplate(int index) {
  return beamtemplate_.Mutable(index);
}
inline ::tps::proto::RT_TPS_BeamTemplate* RT_TPS_BeamTemplate_List::add_beamtemplate() {
  return beamtemplate_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_BeamTemplate >&
RT_TPS_BeamTemplate_List::beamtemplate() const {
  return beamtemplate_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_BeamTemplate >*
RT_TPS_BeamTemplate_List::mutable_beamtemplate() {
  return &beamtemplate_;
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

#endif  // PROTOBUF_rt_5ftps_5fbeam_5ftemplate_2eproto__INCLUDED
