// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rt_tps_voieditinfo.proto

#ifndef PROTOBUF_rt_5ftps_5fvoieditinfo_2eproto__INCLUDED
#define PROTOBUF_rt_5ftps_5fvoieditinfo_2eproto__INCLUDED

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
void  protobuf_AddDesc_rt_5ftps_5fvoieditinfo_2eproto();
void protobuf_AssignDesc_rt_5ftps_5fvoieditinfo_2eproto();
void protobuf_ShutdownFile_rt_5ftps_5fvoieditinfo_2eproto();

class RT_TPS_EDIT_VOI_INFO;

// ===================================================================

class RT_TPS_EDIT_VOI_INFO : public ::google::protobuf::Message {
 public:
  RT_TPS_EDIT_VOI_INFO();
  virtual ~RT_TPS_EDIT_VOI_INFO();
  
  RT_TPS_EDIT_VOI_INFO(const RT_TPS_EDIT_VOI_INFO& from);
  
  inline RT_TPS_EDIT_VOI_INFO& operator=(const RT_TPS_EDIT_VOI_INFO& from) {
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
  static const RT_TPS_EDIT_VOI_INFO& default_instance();
  
  void Swap(RT_TPS_EDIT_VOI_INFO* other);
  
  // implements Message ----------------------------------------------
  
  RT_TPS_EDIT_VOI_INFO* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RT_TPS_EDIT_VOI_INFO& from);
  void MergeFrom(const RT_TPS_EDIT_VOI_INFO& from);
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
  
  // required double x = 1;
  inline bool has_x() const;
  inline void clear_x();
  static const int kXFieldNumber = 1;
  inline double x() const;
  inline void set_x(double value);
  
  // required double y = 2;
  inline bool has_y() const;
  inline void clear_y();
  static const int kYFieldNumber = 2;
  inline double y() const;
  inline void set_y(double value);
  
  // required int32 mouseActionType = 3;
  inline bool has_mouseactiontype() const;
  inline void clear_mouseactiontype();
  static const int kMouseActionTypeFieldNumber = 3;
  inline ::google::protobuf::int32 mouseactiontype() const;
  inline void set_mouseactiontype(::google::protobuf::int32 value);
  
  // required int32 lastMouseActionType = 4;
  inline bool has_lastmouseactiontype() const;
  inline void clear_lastmouseactiontype();
  static const int kLastMouseActionTypeFieldNumber = 4;
  inline ::google::protobuf::int32 lastmouseactiontype() const;
  inline void set_lastmouseactiontype(::google::protobuf::int32 value);
  
  // required int32 windowtype = 5;
  inline bool has_windowtype() const;
  inline void clear_windowtype();
  static const int kWindowtypeFieldNumber = 5;
  inline ::google::protobuf::int32 windowtype() const;
  inline void set_windowtype(::google::protobuf::int32 value);
  
  // required string windowuid = 6;
  inline bool has_windowuid() const;
  inline void clear_windowuid();
  static const int kWindowuidFieldNumber = 6;
  inline const ::std::string& windowuid() const;
  inline void set_windowuid(const ::std::string& value);
  inline void set_windowuid(const char* value);
  inline void set_windowuid(const char* value, size_t size);
  inline ::std::string* mutable_windowuid();
  inline ::std::string* release_windowuid();
  
  // required string voiUID = 7;
  inline bool has_voiuid() const;
  inline void clear_voiuid();
  static const int kVoiUIDFieldNumber = 7;
  inline const ::std::string& voiuid() const;
  inline void set_voiuid(const ::std::string& value);
  inline void set_voiuid(const char* value);
  inline void set_voiuid(const char* value, size_t size);
  inline ::std::string* mutable_voiuid();
  inline ::std::string* release_voiuid();
  
  // optional double radius = 8;
  inline bool has_radius() const;
  inline void clear_radius();
  static const int kRadiusFieldNumber = 8;
  inline double radius() const;
  inline void set_radius(double value);
  
  // optional int32 voieditingmode = 9;
  inline bool has_voieditingmode() const;
  inline void clear_voieditingmode();
  static const int kVoieditingmodeFieldNumber = 9;
  inline ::google::protobuf::int32 voieditingmode() const;
  inline void set_voieditingmode(::google::protobuf::int32 value);
  
  // optional bool isinterpolate = 10;
  inline bool has_isinterpolate() const;
  inline void clear_isinterpolate();
  static const int kIsinterpolateFieldNumber = 10;
  inline bool isinterpolate() const;
  inline void set_isinterpolate(bool value);
  
  // repeated string avoidvoiuid = 11;
  inline int avoidvoiuid_size() const;
  inline void clear_avoidvoiuid();
  static const int kAvoidvoiuidFieldNumber = 11;
  inline const ::std::string& avoidvoiuid(int index) const;
  inline ::std::string* mutable_avoidvoiuid(int index);
  inline void set_avoidvoiuid(int index, const ::std::string& value);
  inline void set_avoidvoiuid(int index, const char* value);
  inline void set_avoidvoiuid(int index, const char* value, size_t size);
  inline ::std::string* add_avoidvoiuid();
  inline void add_avoidvoiuid(const ::std::string& value);
  inline void add_avoidvoiuid(const char* value);
  inline void add_avoidvoiuid(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& avoidvoiuid() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_avoidvoiuid();
  
  // optional bool iseditstarted = 12;
  inline bool has_iseditstarted() const;
  inline void clear_iseditstarted();
  static const int kIseditstartedFieldNumber = 12;
  inline bool iseditstarted() const;
  inline void set_iseditstarted(bool value);
  
  // repeated double ptxlist = 13;
  inline int ptxlist_size() const;
  inline void clear_ptxlist();
  static const int kPtxlistFieldNumber = 13;
  inline double ptxlist(int index) const;
  inline void set_ptxlist(int index, double value);
  inline void add_ptxlist(double value);
  inline const ::google::protobuf::RepeatedField< double >&
      ptxlist() const;
  inline ::google::protobuf::RepeatedField< double >*
      mutable_ptxlist();
  
  // repeated double ptylist = 14;
  inline int ptylist_size() const;
  inline void clear_ptylist();
  static const int kPtylistFieldNumber = 14;
  inline double ptylist(int index) const;
  inline void set_ptylist(int index, double value);
  inline void add_ptylist(double value);
  inline const ::google::protobuf::RepeatedField< double >&
      ptylist() const;
  inline ::google::protobuf::RepeatedField< double >*
      mutable_ptylist();
  
  // @@protoc_insertion_point(class_scope:tps.proto.RT_TPS_EDIT_VOI_INFO)
 private:
  inline void set_has_x();
  inline void clear_has_x();
  inline void set_has_y();
  inline void clear_has_y();
  inline void set_has_mouseactiontype();
  inline void clear_has_mouseactiontype();
  inline void set_has_lastmouseactiontype();
  inline void clear_has_lastmouseactiontype();
  inline void set_has_windowtype();
  inline void clear_has_windowtype();
  inline void set_has_windowuid();
  inline void clear_has_windowuid();
  inline void set_has_voiuid();
  inline void clear_has_voiuid();
  inline void set_has_radius();
  inline void clear_has_radius();
  inline void set_has_voieditingmode();
  inline void clear_has_voieditingmode();
  inline void set_has_isinterpolate();
  inline void clear_has_isinterpolate();
  inline void set_has_iseditstarted();
  inline void clear_has_iseditstarted();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  double x_;
  double y_;
  ::google::protobuf::int32 mouseactiontype_;
  ::google::protobuf::int32 lastmouseactiontype_;
  ::std::string* windowuid_;
  ::std::string* voiuid_;
  ::google::protobuf::int32 windowtype_;
  ::google::protobuf::int32 voieditingmode_;
  double radius_;
  ::google::protobuf::RepeatedPtrField< ::std::string> avoidvoiuid_;
  ::google::protobuf::RepeatedField< double > ptxlist_;
  ::google::protobuf::RepeatedField< double > ptylist_;
  bool isinterpolate_;
  bool iseditstarted_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(14 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5fvoieditinfo_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5fvoieditinfo_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5fvoieditinfo_2eproto();
  
  void InitAsDefaultInstance();
  static RT_TPS_EDIT_VOI_INFO* default_instance_;
};
// ===================================================================


// ===================================================================

// RT_TPS_EDIT_VOI_INFO

// required double x = 1;
inline bool RT_TPS_EDIT_VOI_INFO::has_x() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RT_TPS_EDIT_VOI_INFO::set_has_x() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_has_x() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_x() {
  x_ = 0;
  clear_has_x();
}
inline double RT_TPS_EDIT_VOI_INFO::x() const {
  return x_;
}
inline void RT_TPS_EDIT_VOI_INFO::set_x(double value) {
  set_has_x();
  x_ = value;
}

// required double y = 2;
inline bool RT_TPS_EDIT_VOI_INFO::has_y() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void RT_TPS_EDIT_VOI_INFO::set_has_y() {
  _has_bits_[0] |= 0x00000002u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_has_y() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_y() {
  y_ = 0;
  clear_has_y();
}
inline double RT_TPS_EDIT_VOI_INFO::y() const {
  return y_;
}
inline void RT_TPS_EDIT_VOI_INFO::set_y(double value) {
  set_has_y();
  y_ = value;
}

// required int32 mouseActionType = 3;
inline bool RT_TPS_EDIT_VOI_INFO::has_mouseactiontype() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void RT_TPS_EDIT_VOI_INFO::set_has_mouseactiontype() {
  _has_bits_[0] |= 0x00000004u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_has_mouseactiontype() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_mouseactiontype() {
  mouseactiontype_ = 0;
  clear_has_mouseactiontype();
}
inline ::google::protobuf::int32 RT_TPS_EDIT_VOI_INFO::mouseactiontype() const {
  return mouseactiontype_;
}
inline void RT_TPS_EDIT_VOI_INFO::set_mouseactiontype(::google::protobuf::int32 value) {
  set_has_mouseactiontype();
  mouseactiontype_ = value;
}

// required int32 lastMouseActionType = 4;
inline bool RT_TPS_EDIT_VOI_INFO::has_lastmouseactiontype() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void RT_TPS_EDIT_VOI_INFO::set_has_lastmouseactiontype() {
  _has_bits_[0] |= 0x00000008u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_has_lastmouseactiontype() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_lastmouseactiontype() {
  lastmouseactiontype_ = 0;
  clear_has_lastmouseactiontype();
}
inline ::google::protobuf::int32 RT_TPS_EDIT_VOI_INFO::lastmouseactiontype() const {
  return lastmouseactiontype_;
}
inline void RT_TPS_EDIT_VOI_INFO::set_lastmouseactiontype(::google::protobuf::int32 value) {
  set_has_lastmouseactiontype();
  lastmouseactiontype_ = value;
}

// required int32 windowtype = 5;
inline bool RT_TPS_EDIT_VOI_INFO::has_windowtype() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void RT_TPS_EDIT_VOI_INFO::set_has_windowtype() {
  _has_bits_[0] |= 0x00000010u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_has_windowtype() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_windowtype() {
  windowtype_ = 0;
  clear_has_windowtype();
}
inline ::google::protobuf::int32 RT_TPS_EDIT_VOI_INFO::windowtype() const {
  return windowtype_;
}
inline void RT_TPS_EDIT_VOI_INFO::set_windowtype(::google::protobuf::int32 value) {
  set_has_windowtype();
  windowtype_ = value;
}

// required string windowuid = 6;
inline bool RT_TPS_EDIT_VOI_INFO::has_windowuid() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void RT_TPS_EDIT_VOI_INFO::set_has_windowuid() {
  _has_bits_[0] |= 0x00000020u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_has_windowuid() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_windowuid() {
  if (windowuid_ != &::google::protobuf::internal::kEmptyString) {
    windowuid_->clear();
  }
  clear_has_windowuid();
}
inline const ::std::string& RT_TPS_EDIT_VOI_INFO::windowuid() const {
  return *windowuid_;
}
inline void RT_TPS_EDIT_VOI_INFO::set_windowuid(const ::std::string& value) {
  set_has_windowuid();
  if (windowuid_ == &::google::protobuf::internal::kEmptyString) {
    windowuid_ = new ::std::string;
  }
  windowuid_->assign(value);
}
inline void RT_TPS_EDIT_VOI_INFO::set_windowuid(const char* value) {
  set_has_windowuid();
  if (windowuid_ == &::google::protobuf::internal::kEmptyString) {
    windowuid_ = new ::std::string;
  }
  windowuid_->assign(value);
}
inline void RT_TPS_EDIT_VOI_INFO::set_windowuid(const char* value, size_t size) {
  set_has_windowuid();
  if (windowuid_ == &::google::protobuf::internal::kEmptyString) {
    windowuid_ = new ::std::string;
  }
  windowuid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_EDIT_VOI_INFO::mutable_windowuid() {
  set_has_windowuid();
  if (windowuid_ == &::google::protobuf::internal::kEmptyString) {
    windowuid_ = new ::std::string;
  }
  return windowuid_;
}
inline ::std::string* RT_TPS_EDIT_VOI_INFO::release_windowuid() {
  clear_has_windowuid();
  if (windowuid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = windowuid_;
    windowuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// required string voiUID = 7;
inline bool RT_TPS_EDIT_VOI_INFO::has_voiuid() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void RT_TPS_EDIT_VOI_INFO::set_has_voiuid() {
  _has_bits_[0] |= 0x00000040u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_has_voiuid() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_voiuid() {
  if (voiuid_ != &::google::protobuf::internal::kEmptyString) {
    voiuid_->clear();
  }
  clear_has_voiuid();
}
inline const ::std::string& RT_TPS_EDIT_VOI_INFO::voiuid() const {
  return *voiuid_;
}
inline void RT_TPS_EDIT_VOI_INFO::set_voiuid(const ::std::string& value) {
  set_has_voiuid();
  if (voiuid_ == &::google::protobuf::internal::kEmptyString) {
    voiuid_ = new ::std::string;
  }
  voiuid_->assign(value);
}
inline void RT_TPS_EDIT_VOI_INFO::set_voiuid(const char* value) {
  set_has_voiuid();
  if (voiuid_ == &::google::protobuf::internal::kEmptyString) {
    voiuid_ = new ::std::string;
  }
  voiuid_->assign(value);
}
inline void RT_TPS_EDIT_VOI_INFO::set_voiuid(const char* value, size_t size) {
  set_has_voiuid();
  if (voiuid_ == &::google::protobuf::internal::kEmptyString) {
    voiuid_ = new ::std::string;
  }
  voiuid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_EDIT_VOI_INFO::mutable_voiuid() {
  set_has_voiuid();
  if (voiuid_ == &::google::protobuf::internal::kEmptyString) {
    voiuid_ = new ::std::string;
  }
  return voiuid_;
}
inline ::std::string* RT_TPS_EDIT_VOI_INFO::release_voiuid() {
  clear_has_voiuid();
  if (voiuid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = voiuid_;
    voiuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional double radius = 8;
inline bool RT_TPS_EDIT_VOI_INFO::has_radius() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void RT_TPS_EDIT_VOI_INFO::set_has_radius() {
  _has_bits_[0] |= 0x00000080u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_has_radius() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_radius() {
  radius_ = 0;
  clear_has_radius();
}
inline double RT_TPS_EDIT_VOI_INFO::radius() const {
  return radius_;
}
inline void RT_TPS_EDIT_VOI_INFO::set_radius(double value) {
  set_has_radius();
  radius_ = value;
}

// optional int32 voieditingmode = 9;
inline bool RT_TPS_EDIT_VOI_INFO::has_voieditingmode() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void RT_TPS_EDIT_VOI_INFO::set_has_voieditingmode() {
  _has_bits_[0] |= 0x00000100u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_has_voieditingmode() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_voieditingmode() {
  voieditingmode_ = 0;
  clear_has_voieditingmode();
}
inline ::google::protobuf::int32 RT_TPS_EDIT_VOI_INFO::voieditingmode() const {
  return voieditingmode_;
}
inline void RT_TPS_EDIT_VOI_INFO::set_voieditingmode(::google::protobuf::int32 value) {
  set_has_voieditingmode();
  voieditingmode_ = value;
}

// optional bool isinterpolate = 10;
inline bool RT_TPS_EDIT_VOI_INFO::has_isinterpolate() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void RT_TPS_EDIT_VOI_INFO::set_has_isinterpolate() {
  _has_bits_[0] |= 0x00000200u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_has_isinterpolate() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_isinterpolate() {
  isinterpolate_ = false;
  clear_has_isinterpolate();
}
inline bool RT_TPS_EDIT_VOI_INFO::isinterpolate() const {
  return isinterpolate_;
}
inline void RT_TPS_EDIT_VOI_INFO::set_isinterpolate(bool value) {
  set_has_isinterpolate();
  isinterpolate_ = value;
}

// repeated string avoidvoiuid = 11;
inline int RT_TPS_EDIT_VOI_INFO::avoidvoiuid_size() const {
  return avoidvoiuid_.size();
}
inline void RT_TPS_EDIT_VOI_INFO::clear_avoidvoiuid() {
  avoidvoiuid_.Clear();
}
inline const ::std::string& RT_TPS_EDIT_VOI_INFO::avoidvoiuid(int index) const {
  return avoidvoiuid_.Get(index);
}
inline ::std::string* RT_TPS_EDIT_VOI_INFO::mutable_avoidvoiuid(int index) {
  return avoidvoiuid_.Mutable(index);
}
inline void RT_TPS_EDIT_VOI_INFO::set_avoidvoiuid(int index, const ::std::string& value) {
  avoidvoiuid_.Mutable(index)->assign(value);
}
inline void RT_TPS_EDIT_VOI_INFO::set_avoidvoiuid(int index, const char* value) {
  avoidvoiuid_.Mutable(index)->assign(value);
}
inline void RT_TPS_EDIT_VOI_INFO::set_avoidvoiuid(int index, const char* value, size_t size) {
  avoidvoiuid_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RT_TPS_EDIT_VOI_INFO::add_avoidvoiuid() {
  return avoidvoiuid_.Add();
}
inline void RT_TPS_EDIT_VOI_INFO::add_avoidvoiuid(const ::std::string& value) {
  avoidvoiuid_.Add()->assign(value);
}
inline void RT_TPS_EDIT_VOI_INFO::add_avoidvoiuid(const char* value) {
  avoidvoiuid_.Add()->assign(value);
}
inline void RT_TPS_EDIT_VOI_INFO::add_avoidvoiuid(const char* value, size_t size) {
  avoidvoiuid_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
RT_TPS_EDIT_VOI_INFO::avoidvoiuid() const {
  return avoidvoiuid_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
RT_TPS_EDIT_VOI_INFO::mutable_avoidvoiuid() {
  return &avoidvoiuid_;
}

// optional bool iseditstarted = 12;
inline bool RT_TPS_EDIT_VOI_INFO::has_iseditstarted() const {
  return (_has_bits_[0] & 0x00000800u) != 0;
}
inline void RT_TPS_EDIT_VOI_INFO::set_has_iseditstarted() {
  _has_bits_[0] |= 0x00000800u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_has_iseditstarted() {
  _has_bits_[0] &= ~0x00000800u;
}
inline void RT_TPS_EDIT_VOI_INFO::clear_iseditstarted() {
  iseditstarted_ = false;
  clear_has_iseditstarted();
}
inline bool RT_TPS_EDIT_VOI_INFO::iseditstarted() const {
  return iseditstarted_;
}
inline void RT_TPS_EDIT_VOI_INFO::set_iseditstarted(bool value) {
  set_has_iseditstarted();
  iseditstarted_ = value;
}

// repeated double ptxlist = 13;
inline int RT_TPS_EDIT_VOI_INFO::ptxlist_size() const {
  return ptxlist_.size();
}
inline void RT_TPS_EDIT_VOI_INFO::clear_ptxlist() {
  ptxlist_.Clear();
}
inline double RT_TPS_EDIT_VOI_INFO::ptxlist(int index) const {
  return ptxlist_.Get(index);
}
inline void RT_TPS_EDIT_VOI_INFO::set_ptxlist(int index, double value) {
  ptxlist_.Set(index, value);
}
inline void RT_TPS_EDIT_VOI_INFO::add_ptxlist(double value) {
  ptxlist_.Add(value);
}
inline const ::google::protobuf::RepeatedField< double >&
RT_TPS_EDIT_VOI_INFO::ptxlist() const {
  return ptxlist_;
}
inline ::google::protobuf::RepeatedField< double >*
RT_TPS_EDIT_VOI_INFO::mutable_ptxlist() {
  return &ptxlist_;
}

// repeated double ptylist = 14;
inline int RT_TPS_EDIT_VOI_INFO::ptylist_size() const {
  return ptylist_.size();
}
inline void RT_TPS_EDIT_VOI_INFO::clear_ptylist() {
  ptylist_.Clear();
}
inline double RT_TPS_EDIT_VOI_INFO::ptylist(int index) const {
  return ptylist_.Get(index);
}
inline void RT_TPS_EDIT_VOI_INFO::set_ptylist(int index, double value) {
  ptylist_.Set(index, value);
}
inline void RT_TPS_EDIT_VOI_INFO::add_ptylist(double value) {
  ptylist_.Add(value);
}
inline const ::google::protobuf::RepeatedField< double >&
RT_TPS_EDIT_VOI_INFO::ptylist() const {
  return ptylist_;
}
inline ::google::protobuf::RepeatedField< double >*
RT_TPS_EDIT_VOI_INFO::mutable_ptylist() {
  return &ptylist_;
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

#endif  // PROTOBUF_rt_5ftps_5fvoieditinfo_2eproto__INCLUDED