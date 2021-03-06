// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "rt_tps_proto_prescription.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace tps {
namespace proto {

namespace {

const ::google::protobuf::Descriptor* RtTpsProtoPrescription_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  RtTpsProtoPrescription_reflection_ = NULL;
const ::google::protobuf::Descriptor* RtTpsProtoRepeatedPrescription_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  RtTpsProtoRepeatedPrescription_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_rt_5ftps_5fproto_5fprescription_2eproto() {
  protobuf_AddDesc_rt_5ftps_5fproto_5fprescription_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "rt_tps_proto_prescription.proto");
  GOOGLE_CHECK(file != NULL);
  RtTpsProtoPrescription_descriptor_ = file->message_type(0);
  static const int RtTpsProtoPrescription_offsets_[11] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, prescriptionuid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, prescriptionname_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, doseperfraction_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, fractionsnumber_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, courseuid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, constraintlist_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, isgating_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, siteuid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, binmode_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, beamonrangestart_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, beamonrangeend_),
  };
  RtTpsProtoPrescription_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      RtTpsProtoPrescription_descriptor_,
      RtTpsProtoPrescription::default_instance_,
      RtTpsProtoPrescription_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoPrescription, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(RtTpsProtoPrescription));
  RtTpsProtoRepeatedPrescription_descriptor_ = file->message_type(1);
  static const int RtTpsProtoRepeatedPrescription_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoRepeatedPrescription, prescriptionlist_),
  };
  RtTpsProtoRepeatedPrescription_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      RtTpsProtoRepeatedPrescription_descriptor_,
      RtTpsProtoRepeatedPrescription::default_instance_,
      RtTpsProtoRepeatedPrescription_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoRepeatedPrescription, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RtTpsProtoRepeatedPrescription, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(RtTpsProtoRepeatedPrescription));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_rt_5ftps_5fproto_5fprescription_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    RtTpsProtoPrescription_descriptor_, &RtTpsProtoPrescription::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    RtTpsProtoRepeatedPrescription_descriptor_, &RtTpsProtoRepeatedPrescription::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_rt_5ftps_5fproto_5fprescription_2eproto() {
  delete RtTpsProtoPrescription::default_instance_;
  delete RtTpsProtoPrescription_reflection_;
  delete RtTpsProtoRepeatedPrescription::default_instance_;
  delete RtTpsProtoRepeatedPrescription_reflection_;
}

void protobuf_AddDesc_rt_5ftps_5fproto_5fprescription_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::tps::proto::protobuf_AddDesc_rt_5ftps_5fproto_5fconstraint_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\037rt_tps_proto_prescription.proto\022\ttps.p"
    "roto\032\035rt_tps_proto_constraint.proto\"\257\002\n\026"
    "RtTpsProtoPrescription\022\027\n\017prescriptionUI"
    "D\030\001 \001(\t\022\030\n\020prescriptionName\030\002 \001(\t\022\027\n\017dos"
    "ePerFraction\030\003 \001(\002\022\027\n\017fractionsNumber\030\004 "
    "\001(\005\022\021\n\tcourseUID\030\005 \001(\t\0227\n\016constraintList"
    "\030\006 \003(\0132\037.tps.proto.RtTpsProtoConstraint\022"
    "\020\n\010isgating\030\007 \001(\010\022\017\n\007siteUID\030\010 \001(\t\022\017\n\007bi"
    "nMode\030\t \001(\005\022\030\n\020beamOnRangeStart\030\n \001(\001\022\026\n"
    "\016beamOnRangeEnd\030\013 \001(\001\"]\n\036RtTpsProtoRepea"
    "tedPrescription\022;\n\020prescriptionList\030\001 \003("
    "\0132!.tps.proto.RtTpsProtoPrescription", 476);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "rt_tps_proto_prescription.proto", &protobuf_RegisterTypes);
  RtTpsProtoPrescription::default_instance_ = new RtTpsProtoPrescription();
  RtTpsProtoRepeatedPrescription::default_instance_ = new RtTpsProtoRepeatedPrescription();
  RtTpsProtoPrescription::default_instance_->InitAsDefaultInstance();
  RtTpsProtoRepeatedPrescription::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_rt_5ftps_5fproto_5fprescription_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_rt_5ftps_5fproto_5fprescription_2eproto {
  StaticDescriptorInitializer_rt_5ftps_5fproto_5fprescription_2eproto() {
    protobuf_AddDesc_rt_5ftps_5fproto_5fprescription_2eproto();
  }
} static_descriptor_initializer_rt_5ftps_5fproto_5fprescription_2eproto_;


// ===================================================================

#ifndef _MSC_VER
const int RtTpsProtoPrescription::kPrescriptionUIDFieldNumber;
const int RtTpsProtoPrescription::kPrescriptionNameFieldNumber;
const int RtTpsProtoPrescription::kDosePerFractionFieldNumber;
const int RtTpsProtoPrescription::kFractionsNumberFieldNumber;
const int RtTpsProtoPrescription::kCourseUIDFieldNumber;
const int RtTpsProtoPrescription::kConstraintListFieldNumber;
const int RtTpsProtoPrescription::kIsgatingFieldNumber;
const int RtTpsProtoPrescription::kSiteUIDFieldNumber;
const int RtTpsProtoPrescription::kBinModeFieldNumber;
const int RtTpsProtoPrescription::kBeamOnRangeStartFieldNumber;
const int RtTpsProtoPrescription::kBeamOnRangeEndFieldNumber;
#endif  // !_MSC_VER

RtTpsProtoPrescription::RtTpsProtoPrescription()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void RtTpsProtoPrescription::InitAsDefaultInstance() {
}

RtTpsProtoPrescription::RtTpsProtoPrescription(const RtTpsProtoPrescription& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void RtTpsProtoPrescription::SharedCtor() {
  _cached_size_ = 0;
  prescriptionuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  prescriptionname_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  doseperfraction_ = 0;
  fractionsnumber_ = 0;
  courseuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  isgating_ = false;
  siteuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  binmode_ = 0;
  beamonrangestart_ = 0;
  beamonrangeend_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

RtTpsProtoPrescription::~RtTpsProtoPrescription() {
  SharedDtor();
}

void RtTpsProtoPrescription::SharedDtor() {
  if (prescriptionuid_ != &::google::protobuf::internal::kEmptyString) {
    delete prescriptionuid_;
  }
  if (prescriptionname_ != &::google::protobuf::internal::kEmptyString) {
    delete prescriptionname_;
  }
  if (courseuid_ != &::google::protobuf::internal::kEmptyString) {
    delete courseuid_;
  }
  if (siteuid_ != &::google::protobuf::internal::kEmptyString) {
    delete siteuid_;
  }
  if (this != default_instance_) {
  }
}

void RtTpsProtoPrescription::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* RtTpsProtoPrescription::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return RtTpsProtoPrescription_descriptor_;
}

const RtTpsProtoPrescription& RtTpsProtoPrescription::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_rt_5ftps_5fproto_5fprescription_2eproto();  return *default_instance_;
}

RtTpsProtoPrescription* RtTpsProtoPrescription::default_instance_ = NULL;

RtTpsProtoPrescription* RtTpsProtoPrescription::New() const {
  return new RtTpsProtoPrescription;
}

void RtTpsProtoPrescription::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_prescriptionuid()) {
      if (prescriptionuid_ != &::google::protobuf::internal::kEmptyString) {
        prescriptionuid_->clear();
      }
    }
    if (has_prescriptionname()) {
      if (prescriptionname_ != &::google::protobuf::internal::kEmptyString) {
        prescriptionname_->clear();
      }
    }
    doseperfraction_ = 0;
    fractionsnumber_ = 0;
    if (has_courseuid()) {
      if (courseuid_ != &::google::protobuf::internal::kEmptyString) {
        courseuid_->clear();
      }
    }
    isgating_ = false;
    if (has_siteuid()) {
      if (siteuid_ != &::google::protobuf::internal::kEmptyString) {
        siteuid_->clear();
      }
    }
  }
  if (_has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    binmode_ = 0;
    beamonrangestart_ = 0;
    beamonrangeend_ = 0;
  }
  constraintlist_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool RtTpsProtoPrescription::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string prescriptionUID = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_prescriptionuid()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->prescriptionuid().data(), this->prescriptionuid().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_prescriptionName;
        break;
      }
      
      // optional string prescriptionName = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_prescriptionName:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_prescriptionname()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->prescriptionname().data(), this->prescriptionname().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(29)) goto parse_dosePerFraction;
        break;
      }
      
      // optional float dosePerFraction = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED32) {
         parse_dosePerFraction:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &doseperfraction_)));
          set_has_doseperfraction();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(32)) goto parse_fractionsNumber;
        break;
      }
      
      // optional int32 fractionsNumber = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_fractionsNumber:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &fractionsnumber_)));
          set_has_fractionsnumber();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_courseUID;
        break;
      }
      
      // optional string courseUID = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_courseUID:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_courseuid()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->courseuid().data(), this->courseuid().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(50)) goto parse_constraintList;
        break;
      }
      
      // repeated .tps.proto.RtTpsProtoConstraint constraintList = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_constraintList:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_constraintlist()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(50)) goto parse_constraintList;
        if (input->ExpectTag(56)) goto parse_isgating;
        break;
      }
      
      // optional bool isgating = 7;
      case 7: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_isgating:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &isgating_)));
          set_has_isgating();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(66)) goto parse_siteUID;
        break;
      }
      
      // optional string siteUID = 8;
      case 8: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_siteUID:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_siteuid()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->siteuid().data(), this->siteuid().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(72)) goto parse_binMode;
        break;
      }
      
      // optional int32 binMode = 9;
      case 9: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_binMode:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &binmode_)));
          set_has_binmode();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(81)) goto parse_beamOnRangeStart;
        break;
      }
      
      // optional double beamOnRangeStart = 10;
      case 10: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
         parse_beamOnRangeStart:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   double, ::google::protobuf::internal::WireFormatLite::TYPE_DOUBLE>(
                 input, &beamonrangestart_)));
          set_has_beamonrangestart();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(89)) goto parse_beamOnRangeEnd;
        break;
      }
      
      // optional double beamOnRangeEnd = 11;
      case 11: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
         parse_beamOnRangeEnd:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   double, ::google::protobuf::internal::WireFormatLite::TYPE_DOUBLE>(
                 input, &beamonrangeend_)));
          set_has_beamonrangeend();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void RtTpsProtoPrescription::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional string prescriptionUID = 1;
  if (has_prescriptionuid()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->prescriptionuid().data(), this->prescriptionuid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->prescriptionuid(), output);
  }
  
  // optional string prescriptionName = 2;
  if (has_prescriptionname()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->prescriptionname().data(), this->prescriptionname().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->prescriptionname(), output);
  }
  
  // optional float dosePerFraction = 3;
  if (has_doseperfraction()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(3, this->doseperfraction(), output);
  }
  
  // optional int32 fractionsNumber = 4;
  if (has_fractionsnumber()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(4, this->fractionsnumber(), output);
  }
  
  // optional string courseUID = 5;
  if (has_courseuid()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->courseuid().data(), this->courseuid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      5, this->courseuid(), output);
  }
  
  // repeated .tps.proto.RtTpsProtoConstraint constraintList = 6;
  for (int i = 0; i < this->constraintlist_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      6, this->constraintlist(i), output);
  }
  
  // optional bool isgating = 7;
  if (has_isgating()) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(7, this->isgating(), output);
  }
  
  // optional string siteUID = 8;
  if (has_siteuid()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->siteuid().data(), this->siteuid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      8, this->siteuid(), output);
  }
  
  // optional int32 binMode = 9;
  if (has_binmode()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(9, this->binmode(), output);
  }
  
  // optional double beamOnRangeStart = 10;
  if (has_beamonrangestart()) {
    ::google::protobuf::internal::WireFormatLite::WriteDouble(10, this->beamonrangestart(), output);
  }
  
  // optional double beamOnRangeEnd = 11;
  if (has_beamonrangeend()) {
    ::google::protobuf::internal::WireFormatLite::WriteDouble(11, this->beamonrangeend(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* RtTpsProtoPrescription::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional string prescriptionUID = 1;
  if (has_prescriptionuid()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->prescriptionuid().data(), this->prescriptionuid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->prescriptionuid(), target);
  }
  
  // optional string prescriptionName = 2;
  if (has_prescriptionname()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->prescriptionname().data(), this->prescriptionname().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->prescriptionname(), target);
  }
  
  // optional float dosePerFraction = 3;
  if (has_doseperfraction()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(3, this->doseperfraction(), target);
  }
  
  // optional int32 fractionsNumber = 4;
  if (has_fractionsnumber()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(4, this->fractionsnumber(), target);
  }
  
  // optional string courseUID = 5;
  if (has_courseuid()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->courseuid().data(), this->courseuid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        5, this->courseuid(), target);
  }
  
  // repeated .tps.proto.RtTpsProtoConstraint constraintList = 6;
  for (int i = 0; i < this->constraintlist_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        6, this->constraintlist(i), target);
  }
  
  // optional bool isgating = 7;
  if (has_isgating()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(7, this->isgating(), target);
  }
  
  // optional string siteUID = 8;
  if (has_siteuid()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->siteuid().data(), this->siteuid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        8, this->siteuid(), target);
  }
  
  // optional int32 binMode = 9;
  if (has_binmode()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(9, this->binmode(), target);
  }
  
  // optional double beamOnRangeStart = 10;
  if (has_beamonrangestart()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteDoubleToArray(10, this->beamonrangestart(), target);
  }
  
  // optional double beamOnRangeEnd = 11;
  if (has_beamonrangeend()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteDoubleToArray(11, this->beamonrangeend(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int RtTpsProtoPrescription::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional string prescriptionUID = 1;
    if (has_prescriptionuid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->prescriptionuid());
    }
    
    // optional string prescriptionName = 2;
    if (has_prescriptionname()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->prescriptionname());
    }
    
    // optional float dosePerFraction = 3;
    if (has_doseperfraction()) {
      total_size += 1 + 4;
    }
    
    // optional int32 fractionsNumber = 4;
    if (has_fractionsnumber()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->fractionsnumber());
    }
    
    // optional string courseUID = 5;
    if (has_courseuid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->courseuid());
    }
    
    // optional bool isgating = 7;
    if (has_isgating()) {
      total_size += 1 + 1;
    }
    
    // optional string siteUID = 8;
    if (has_siteuid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->siteuid());
    }
    
  }
  if (_has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    // optional int32 binMode = 9;
    if (has_binmode()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->binmode());
    }
    
    // optional double beamOnRangeStart = 10;
    if (has_beamonrangestart()) {
      total_size += 1 + 8;
    }
    
    // optional double beamOnRangeEnd = 11;
    if (has_beamonrangeend()) {
      total_size += 1 + 8;
    }
    
  }
  // repeated .tps.proto.RtTpsProtoConstraint constraintList = 6;
  total_size += 1 * this->constraintlist_size();
  for (int i = 0; i < this->constraintlist_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->constraintlist(i));
  }
  
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void RtTpsProtoPrescription::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const RtTpsProtoPrescription* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const RtTpsProtoPrescription*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void RtTpsProtoPrescription::MergeFrom(const RtTpsProtoPrescription& from) {
  GOOGLE_CHECK_NE(&from, this);
  constraintlist_.MergeFrom(from.constraintlist_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_prescriptionuid()) {
      set_prescriptionuid(from.prescriptionuid());
    }
    if (from.has_prescriptionname()) {
      set_prescriptionname(from.prescriptionname());
    }
    if (from.has_doseperfraction()) {
      set_doseperfraction(from.doseperfraction());
    }
    if (from.has_fractionsnumber()) {
      set_fractionsnumber(from.fractionsnumber());
    }
    if (from.has_courseuid()) {
      set_courseuid(from.courseuid());
    }
    if (from.has_isgating()) {
      set_isgating(from.isgating());
    }
    if (from.has_siteuid()) {
      set_siteuid(from.siteuid());
    }
  }
  if (from._has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    if (from.has_binmode()) {
      set_binmode(from.binmode());
    }
    if (from.has_beamonrangestart()) {
      set_beamonrangestart(from.beamonrangestart());
    }
    if (from.has_beamonrangeend()) {
      set_beamonrangeend(from.beamonrangeend());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void RtTpsProtoPrescription::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RtTpsProtoPrescription::CopyFrom(const RtTpsProtoPrescription& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RtTpsProtoPrescription::IsInitialized() const {
  
  for (int i = 0; i < constraintlist_size(); i++) {
    if (!this->constraintlist(i).IsInitialized()) return false;
  }
  return true;
}

void RtTpsProtoPrescription::Swap(RtTpsProtoPrescription* other) {
  if (other != this) {
    std::swap(prescriptionuid_, other->prescriptionuid_);
    std::swap(prescriptionname_, other->prescriptionname_);
    std::swap(doseperfraction_, other->doseperfraction_);
    std::swap(fractionsnumber_, other->fractionsnumber_);
    std::swap(courseuid_, other->courseuid_);
    constraintlist_.Swap(&other->constraintlist_);
    std::swap(isgating_, other->isgating_);
    std::swap(siteuid_, other->siteuid_);
    std::swap(binmode_, other->binmode_);
    std::swap(beamonrangestart_, other->beamonrangestart_);
    std::swap(beamonrangeend_, other->beamonrangeend_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata RtTpsProtoPrescription::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = RtTpsProtoPrescription_descriptor_;
  metadata.reflection = RtTpsProtoPrescription_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int RtTpsProtoRepeatedPrescription::kPrescriptionListFieldNumber;
#endif  // !_MSC_VER

RtTpsProtoRepeatedPrescription::RtTpsProtoRepeatedPrescription()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void RtTpsProtoRepeatedPrescription::InitAsDefaultInstance() {
}

RtTpsProtoRepeatedPrescription::RtTpsProtoRepeatedPrescription(const RtTpsProtoRepeatedPrescription& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void RtTpsProtoRepeatedPrescription::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

RtTpsProtoRepeatedPrescription::~RtTpsProtoRepeatedPrescription() {
  SharedDtor();
}

void RtTpsProtoRepeatedPrescription::SharedDtor() {
  if (this != default_instance_) {
  }
}

void RtTpsProtoRepeatedPrescription::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* RtTpsProtoRepeatedPrescription::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return RtTpsProtoRepeatedPrescription_descriptor_;
}

const RtTpsProtoRepeatedPrescription& RtTpsProtoRepeatedPrescription::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_rt_5ftps_5fproto_5fprescription_2eproto();  return *default_instance_;
}

RtTpsProtoRepeatedPrescription* RtTpsProtoRepeatedPrescription::default_instance_ = NULL;

RtTpsProtoRepeatedPrescription* RtTpsProtoRepeatedPrescription::New() const {
  return new RtTpsProtoRepeatedPrescription;
}

void RtTpsProtoRepeatedPrescription::Clear() {
  prescriptionlist_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool RtTpsProtoRepeatedPrescription::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .tps.proto.RtTpsProtoPrescription prescriptionList = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_prescriptionList:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_prescriptionlist()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_prescriptionList;
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void RtTpsProtoRepeatedPrescription::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated .tps.proto.RtTpsProtoPrescription prescriptionList = 1;
  for (int i = 0; i < this->prescriptionlist_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->prescriptionlist(i), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* RtTpsProtoRepeatedPrescription::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .tps.proto.RtTpsProtoPrescription prescriptionList = 1;
  for (int i = 0; i < this->prescriptionlist_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->prescriptionlist(i), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int RtTpsProtoRepeatedPrescription::ByteSize() const {
  int total_size = 0;
  
  // repeated .tps.proto.RtTpsProtoPrescription prescriptionList = 1;
  total_size += 1 * this->prescriptionlist_size();
  for (int i = 0; i < this->prescriptionlist_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->prescriptionlist(i));
  }
  
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void RtTpsProtoRepeatedPrescription::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const RtTpsProtoRepeatedPrescription* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const RtTpsProtoRepeatedPrescription*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void RtTpsProtoRepeatedPrescription::MergeFrom(const RtTpsProtoRepeatedPrescription& from) {
  GOOGLE_CHECK_NE(&from, this);
  prescriptionlist_.MergeFrom(from.prescriptionlist_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void RtTpsProtoRepeatedPrescription::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RtTpsProtoRepeatedPrescription::CopyFrom(const RtTpsProtoRepeatedPrescription& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RtTpsProtoRepeatedPrescription::IsInitialized() const {
  
  for (int i = 0; i < prescriptionlist_size(); i++) {
    if (!this->prescriptionlist(i).IsInitialized()) return false;
  }
  return true;
}

void RtTpsProtoRepeatedPrescription::Swap(RtTpsProtoRepeatedPrescription* other) {
  if (other != this) {
    prescriptionlist_.Swap(&other->prescriptionlist_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata RtTpsProtoRepeatedPrescription::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = RtTpsProtoRepeatedPrescription_descriptor_;
  metadata.reflection = RtTpsProtoRepeatedPrescription_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace proto
}  // namespace tps

// @@protoc_insertion_point(global_scope)
