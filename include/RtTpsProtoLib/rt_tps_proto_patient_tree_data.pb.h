// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rt_tps_proto_patient_tree_data.proto

#ifndef PROTOBUF_rt_5ftps_5fproto_5fpatient_5ftree_5fdata_2eproto__INCLUDED
#define PROTOBUF_rt_5ftps_5fproto_5fpatient_5ftree_5fdata_2eproto__INCLUDED

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
#include "rt_tps_proto_patient.pb.h"
#include "rt_tps_proto_course.pb.h"
#include "rt_tps_proto_series.pb.h"
#include "rt_tps_proto_voi.pb.h"
#include "rt_tps_proto_plan.pb.h"
#include "rt_tps_proto_poi.pb.h"
#include "rt_tps_proto_normalizationgroup.pb.h"
#include "rt_tps_proto_beam.pb.h"
#include "rt_tps_proto_segment.pb.h"
#include "rt_tps_proto_prescription.pb.h"
#include "rt_tps_proto_constraint.pb.h"
#include "rt_tps_proto_doseline.pb.h"
#include "rt_tps_proto_doselinesetting.pb.h"
#include "rt_tps_optgoals.pb.h"
#include "rt_tps_proto_table.pb.h"
#include "rt_tps_proto_series_group.pb.h"
#include "rt_tps_referenceimage.pb.h"
#include "rt_tps_proto_site.pb.h"
#include "rt_tps_dvhdata.pb.h"
// @@protoc_insertion_point(includes)

namespace tps {
namespace proto {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_rt_5ftps_5fproto_5fpatient_5ftree_5fdata_2eproto();
void protobuf_AssignDesc_rt_5ftps_5fproto_5fpatient_5ftree_5fdata_2eproto();
void protobuf_ShutdownFile_rt_5ftps_5fproto_5fpatient_5ftree_5fdata_2eproto();

class RtTpsProtoPatientTreeData;

// ===================================================================

class RtTpsProtoPatientTreeData : public ::google::protobuf::Message {
 public:
  RtTpsProtoPatientTreeData();
  virtual ~RtTpsProtoPatientTreeData();
  
  RtTpsProtoPatientTreeData(const RtTpsProtoPatientTreeData& from);
  
  inline RtTpsProtoPatientTreeData& operator=(const RtTpsProtoPatientTreeData& from) {
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
  static const RtTpsProtoPatientTreeData& default_instance();
  
  void Swap(RtTpsProtoPatientTreeData* other);
  
  // implements Message ----------------------------------------------
  
  RtTpsProtoPatientTreeData* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RtTpsProtoPatientTreeData& from);
  void MergeFrom(const RtTpsProtoPatientTreeData& from);
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
  
  // optional .tps.proto.RtTpsProtoPatient patient = 1;
  inline bool has_patient() const;
  inline void clear_patient();
  static const int kPatientFieldNumber = 1;
  inline const ::tps::proto::RtTpsProtoPatient& patient() const;
  inline ::tps::proto::RtTpsProtoPatient* mutable_patient();
  inline ::tps::proto::RtTpsProtoPatient* release_patient();
  
  // repeated .tps.proto.RtTpsProtoCourse courseList = 2;
  inline int courselist_size() const;
  inline void clear_courselist();
  static const int kCourseListFieldNumber = 2;
  inline const ::tps::proto::RtTpsProtoCourse& courselist(int index) const;
  inline ::tps::proto::RtTpsProtoCourse* mutable_courselist(int index);
  inline ::tps::proto::RtTpsProtoCourse* add_courselist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoCourse >&
      courselist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoCourse >*
      mutable_courselist();
  
  // repeated .tps.proto.RtTpsProtoSeries seriesList = 3;
  inline int serieslist_size() const;
  inline void clear_serieslist();
  static const int kSeriesListFieldNumber = 3;
  inline const ::tps::proto::RtTpsProtoSeries& serieslist(int index) const;
  inline ::tps::proto::RtTpsProtoSeries* mutable_serieslist(int index);
  inline ::tps::proto::RtTpsProtoSeries* add_serieslist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSeries >&
      serieslist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSeries >*
      mutable_serieslist();
  
  // repeated .tps.proto.RtTpsProtoVoi voiList = 4;
  inline int voilist_size() const;
  inline void clear_voilist();
  static const int kVoiListFieldNumber = 4;
  inline const ::tps::proto::RtTpsProtoVoi& voilist(int index) const;
  inline ::tps::proto::RtTpsProtoVoi* mutable_voilist(int index);
  inline ::tps::proto::RtTpsProtoVoi* add_voilist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoVoi >&
      voilist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoVoi >*
      mutable_voilist();
  
  // repeated .tps.proto.RtTpsProtoPlan planList = 5;
  inline int planlist_size() const;
  inline void clear_planlist();
  static const int kPlanListFieldNumber = 5;
  inline const ::tps::proto::RtTpsProtoPlan& planlist(int index) const;
  inline ::tps::proto::RtTpsProtoPlan* mutable_planlist(int index);
  inline ::tps::proto::RtTpsProtoPlan* add_planlist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPlan >&
      planlist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPlan >*
      mutable_planlist();
  
  // repeated .tps.proto.RtTpsProtoPoi poiList = 6;
  inline int poilist_size() const;
  inline void clear_poilist();
  static const int kPoiListFieldNumber = 6;
  inline const ::tps::proto::RtTpsProtoPoi& poilist(int index) const;
  inline ::tps::proto::RtTpsProtoPoi* mutable_poilist(int index);
  inline ::tps::proto::RtTpsProtoPoi* add_poilist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPoi >&
      poilist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPoi >*
      mutable_poilist();
  
  // repeated .tps.proto.RtTpsProtoNormalizationGroup normalizationGroupList = 7;
  inline int normalizationgrouplist_size() const;
  inline void clear_normalizationgrouplist();
  static const int kNormalizationGroupListFieldNumber = 7;
  inline const ::tps::proto::RtTpsProtoNormalizationGroup& normalizationgrouplist(int index) const;
  inline ::tps::proto::RtTpsProtoNormalizationGroup* mutable_normalizationgrouplist(int index);
  inline ::tps::proto::RtTpsProtoNormalizationGroup* add_normalizationgrouplist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoNormalizationGroup >&
      normalizationgrouplist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoNormalizationGroup >*
      mutable_normalizationgrouplist();
  
  // repeated .tps.proto.RtTpsProtoBeam beamList = 8;
  inline int beamlist_size() const;
  inline void clear_beamlist();
  static const int kBeamListFieldNumber = 8;
  inline const ::tps::proto::RtTpsProtoBeam& beamlist(int index) const;
  inline ::tps::proto::RtTpsProtoBeam* mutable_beamlist(int index);
  inline ::tps::proto::RtTpsProtoBeam* add_beamlist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoBeam >&
      beamlist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoBeam >*
      mutable_beamlist();
  
  // repeated .tps.proto.RtTpsProtoSegment segmentList = 9;
  inline int segmentlist_size() const;
  inline void clear_segmentlist();
  static const int kSegmentListFieldNumber = 9;
  inline const ::tps::proto::RtTpsProtoSegment& segmentlist(int index) const;
  inline ::tps::proto::RtTpsProtoSegment* mutable_segmentlist(int index);
  inline ::tps::proto::RtTpsProtoSegment* add_segmentlist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSegment >&
      segmentlist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSegment >*
      mutable_segmentlist();
  
  // repeated .tps.proto.RtTpsProtoPrescription prescriptionList = 10;
  inline int prescriptionlist_size() const;
  inline void clear_prescriptionlist();
  static const int kPrescriptionListFieldNumber = 10;
  inline const ::tps::proto::RtTpsProtoPrescription& prescriptionlist(int index) const;
  inline ::tps::proto::RtTpsProtoPrescription* mutable_prescriptionlist(int index);
  inline ::tps::proto::RtTpsProtoPrescription* add_prescriptionlist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPrescription >&
      prescriptionlist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPrescription >*
      mutable_prescriptionlist();
  
  // repeated .tps.proto.RtTpsProtoConstraint constraintList = 11;
  inline int constraintlist_size() const;
  inline void clear_constraintlist();
  static const int kConstraintListFieldNumber = 11;
  inline const ::tps::proto::RtTpsProtoConstraint& constraintlist(int index) const;
  inline ::tps::proto::RtTpsProtoConstraint* mutable_constraintlist(int index);
  inline ::tps::proto::RtTpsProtoConstraint* add_constraintlist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoConstraint >&
      constraintlist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoConstraint >*
      mutable_constraintlist();
  
  // repeated .tps.proto.RtTpsProtoDoseLine doseLineList = 12;
  inline int doselinelist_size() const;
  inline void clear_doselinelist();
  static const int kDoseLineListFieldNumber = 12;
  inline const ::tps::proto::RtTpsProtoDoseLine& doselinelist(int index) const;
  inline ::tps::proto::RtTpsProtoDoseLine* mutable_doselinelist(int index);
  inline ::tps::proto::RtTpsProtoDoseLine* add_doselinelist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoDoseLine >&
      doselinelist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoDoseLine >*
      mutable_doselinelist();
  
  // repeated .tps.proto.RT_TPS_Optgoal optGoalList = 13;
  inline int optgoallist_size() const;
  inline void clear_optgoallist();
  static const int kOptGoalListFieldNumber = 13;
  inline const ::tps::proto::RT_TPS_Optgoal& optgoallist(int index) const;
  inline ::tps::proto::RT_TPS_Optgoal* mutable_optgoallist(int index);
  inline ::tps::proto::RT_TPS_Optgoal* add_optgoallist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Optgoal >&
      optgoallist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Optgoal >*
      mutable_optgoallist();
  
  // repeated .tps.proto.RtTpsProtoTable tpsTable = 14;
  inline int tpstable_size() const;
  inline void clear_tpstable();
  static const int kTpsTableFieldNumber = 14;
  inline const ::tps::proto::RtTpsProtoTable& tpstable(int index) const;
  inline ::tps::proto::RtTpsProtoTable* mutable_tpstable(int index);
  inline ::tps::proto::RtTpsProtoTable* add_tpstable();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoTable >&
      tpstable() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoTable >*
      mutable_tpstable();
  
  // repeated .tps.proto.RtTpsProtoSeriesGroup seriesGroup = 15;
  inline int seriesgroup_size() const;
  inline void clear_seriesgroup();
  static const int kSeriesGroupFieldNumber = 15;
  inline const ::tps::proto::RtTpsProtoSeriesGroup& seriesgroup(int index) const;
  inline ::tps::proto::RtTpsProtoSeriesGroup* mutable_seriesgroup(int index);
  inline ::tps::proto::RtTpsProtoSeriesGroup* add_seriesgroup();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSeriesGroup >&
      seriesgroup() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSeriesGroup >*
      mutable_seriesgroup();
  
  // repeated .tps.proto.ReferenceImage rtImageList = 16;
  inline int rtimagelist_size() const;
  inline void clear_rtimagelist();
  static const int kRtImageListFieldNumber = 16;
  inline const ::tps::proto::ReferenceImage& rtimagelist(int index) const;
  inline ::tps::proto::ReferenceImage* mutable_rtimagelist(int index);
  inline ::tps::proto::ReferenceImage* add_rtimagelist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::ReferenceImage >&
      rtimagelist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::ReferenceImage >*
      mutable_rtimagelist();
  
  // repeated .tps.proto.RtTpsProtoSite siteList = 17;
  inline int sitelist_size() const;
  inline void clear_sitelist();
  static const int kSiteListFieldNumber = 17;
  inline const ::tps::proto::RtTpsProtoSite& sitelist(int index) const;
  inline ::tps::proto::RtTpsProtoSite* mutable_sitelist(int index);
  inline ::tps::proto::RtTpsProtoSite* add_sitelist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSite >&
      sitelist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSite >*
      mutable_sitelist();
  
  // repeated .tps.proto.RT_TPS_DVHData dvhList = 18;
  inline int dvhlist_size() const;
  inline void clear_dvhlist();
  static const int kDvhListFieldNumber = 18;
  inline const ::tps::proto::RT_TPS_DVHData& dvhlist(int index) const;
  inline ::tps::proto::RT_TPS_DVHData* mutable_dvhlist(int index);
  inline ::tps::proto::RT_TPS_DVHData* add_dvhlist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_DVHData >&
      dvhlist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_DVHData >*
      mutable_dvhlist();
  
  // repeated .tps.proto.RtTpsProtoPoi maxPointList = 19;
  inline int maxpointlist_size() const;
  inline void clear_maxpointlist();
  static const int kMaxPointListFieldNumber = 19;
  inline const ::tps::proto::RtTpsProtoPoi& maxpointlist(int index) const;
  inline ::tps::proto::RtTpsProtoPoi* mutable_maxpointlist(int index);
  inline ::tps::proto::RtTpsProtoPoi* add_maxpointlist();
  inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPoi >&
      maxpointlist() const;
  inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPoi >*
      mutable_maxpointlist();
  
  // @@protoc_insertion_point(class_scope:tps.proto.RtTpsProtoPatientTreeData)
 private:
  inline void set_has_patient();
  inline void clear_has_patient();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::tps::proto::RtTpsProtoPatient* patient_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoCourse > courselist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSeries > serieslist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoVoi > voilist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPlan > planlist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPoi > poilist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoNormalizationGroup > normalizationgrouplist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoBeam > beamlist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSegment > segmentlist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPrescription > prescriptionlist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoConstraint > constraintlist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoDoseLine > doselinelist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Optgoal > optgoallist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoTable > tpstable_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSeriesGroup > seriesgroup_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::ReferenceImage > rtimagelist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSite > sitelist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_DVHData > dvhlist_;
  ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPoi > maxpointlist_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(19 + 31) / 32];
  
  friend void  protobuf_AddDesc_rt_5ftps_5fproto_5fpatient_5ftree_5fdata_2eproto();
  friend void protobuf_AssignDesc_rt_5ftps_5fproto_5fpatient_5ftree_5fdata_2eproto();
  friend void protobuf_ShutdownFile_rt_5ftps_5fproto_5fpatient_5ftree_5fdata_2eproto();
  
  void InitAsDefaultInstance();
  static RtTpsProtoPatientTreeData* default_instance_;
};
// ===================================================================


// ===================================================================

// RtTpsProtoPatientTreeData

// optional .tps.proto.RtTpsProtoPatient patient = 1;
inline bool RtTpsProtoPatientTreeData::has_patient() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RtTpsProtoPatientTreeData::set_has_patient() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RtTpsProtoPatientTreeData::clear_has_patient() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RtTpsProtoPatientTreeData::clear_patient() {
  if (patient_ != NULL) patient_->::tps::proto::RtTpsProtoPatient::Clear();
  clear_has_patient();
}
inline const ::tps::proto::RtTpsProtoPatient& RtTpsProtoPatientTreeData::patient() const {
  return patient_ != NULL ? *patient_ : *default_instance_->patient_;
}
inline ::tps::proto::RtTpsProtoPatient* RtTpsProtoPatientTreeData::mutable_patient() {
  set_has_patient();
  if (patient_ == NULL) patient_ = new ::tps::proto::RtTpsProtoPatient;
  return patient_;
}
inline ::tps::proto::RtTpsProtoPatient* RtTpsProtoPatientTreeData::release_patient() {
  clear_has_patient();
  ::tps::proto::RtTpsProtoPatient* temp = patient_;
  patient_ = NULL;
  return temp;
}

// repeated .tps.proto.RtTpsProtoCourse courseList = 2;
inline int RtTpsProtoPatientTreeData::courselist_size() const {
  return courselist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_courselist() {
  courselist_.Clear();
}
inline const ::tps::proto::RtTpsProtoCourse& RtTpsProtoPatientTreeData::courselist(int index) const {
  return courselist_.Get(index);
}
inline ::tps::proto::RtTpsProtoCourse* RtTpsProtoPatientTreeData::mutable_courselist(int index) {
  return courselist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoCourse* RtTpsProtoPatientTreeData::add_courselist() {
  return courselist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoCourse >&
RtTpsProtoPatientTreeData::courselist() const {
  return courselist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoCourse >*
RtTpsProtoPatientTreeData::mutable_courselist() {
  return &courselist_;
}

// repeated .tps.proto.RtTpsProtoSeries seriesList = 3;
inline int RtTpsProtoPatientTreeData::serieslist_size() const {
  return serieslist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_serieslist() {
  serieslist_.Clear();
}
inline const ::tps::proto::RtTpsProtoSeries& RtTpsProtoPatientTreeData::serieslist(int index) const {
  return serieslist_.Get(index);
}
inline ::tps::proto::RtTpsProtoSeries* RtTpsProtoPatientTreeData::mutable_serieslist(int index) {
  return serieslist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoSeries* RtTpsProtoPatientTreeData::add_serieslist() {
  return serieslist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSeries >&
RtTpsProtoPatientTreeData::serieslist() const {
  return serieslist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSeries >*
RtTpsProtoPatientTreeData::mutable_serieslist() {
  return &serieslist_;
}

// repeated .tps.proto.RtTpsProtoVoi voiList = 4;
inline int RtTpsProtoPatientTreeData::voilist_size() const {
  return voilist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_voilist() {
  voilist_.Clear();
}
inline const ::tps::proto::RtTpsProtoVoi& RtTpsProtoPatientTreeData::voilist(int index) const {
  return voilist_.Get(index);
}
inline ::tps::proto::RtTpsProtoVoi* RtTpsProtoPatientTreeData::mutable_voilist(int index) {
  return voilist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoVoi* RtTpsProtoPatientTreeData::add_voilist() {
  return voilist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoVoi >&
RtTpsProtoPatientTreeData::voilist() const {
  return voilist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoVoi >*
RtTpsProtoPatientTreeData::mutable_voilist() {
  return &voilist_;
}

// repeated .tps.proto.RtTpsProtoPlan planList = 5;
inline int RtTpsProtoPatientTreeData::planlist_size() const {
  return planlist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_planlist() {
  planlist_.Clear();
}
inline const ::tps::proto::RtTpsProtoPlan& RtTpsProtoPatientTreeData::planlist(int index) const {
  return planlist_.Get(index);
}
inline ::tps::proto::RtTpsProtoPlan* RtTpsProtoPatientTreeData::mutable_planlist(int index) {
  return planlist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoPlan* RtTpsProtoPatientTreeData::add_planlist() {
  return planlist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPlan >&
RtTpsProtoPatientTreeData::planlist() const {
  return planlist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPlan >*
RtTpsProtoPatientTreeData::mutable_planlist() {
  return &planlist_;
}

// repeated .tps.proto.RtTpsProtoPoi poiList = 6;
inline int RtTpsProtoPatientTreeData::poilist_size() const {
  return poilist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_poilist() {
  poilist_.Clear();
}
inline const ::tps::proto::RtTpsProtoPoi& RtTpsProtoPatientTreeData::poilist(int index) const {
  return poilist_.Get(index);
}
inline ::tps::proto::RtTpsProtoPoi* RtTpsProtoPatientTreeData::mutable_poilist(int index) {
  return poilist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoPoi* RtTpsProtoPatientTreeData::add_poilist() {
  return poilist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPoi >&
RtTpsProtoPatientTreeData::poilist() const {
  return poilist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPoi >*
RtTpsProtoPatientTreeData::mutable_poilist() {
  return &poilist_;
}

// repeated .tps.proto.RtTpsProtoNormalizationGroup normalizationGroupList = 7;
inline int RtTpsProtoPatientTreeData::normalizationgrouplist_size() const {
  return normalizationgrouplist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_normalizationgrouplist() {
  normalizationgrouplist_.Clear();
}
inline const ::tps::proto::RtTpsProtoNormalizationGroup& RtTpsProtoPatientTreeData::normalizationgrouplist(int index) const {
  return normalizationgrouplist_.Get(index);
}
inline ::tps::proto::RtTpsProtoNormalizationGroup* RtTpsProtoPatientTreeData::mutable_normalizationgrouplist(int index) {
  return normalizationgrouplist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoNormalizationGroup* RtTpsProtoPatientTreeData::add_normalizationgrouplist() {
  return normalizationgrouplist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoNormalizationGroup >&
RtTpsProtoPatientTreeData::normalizationgrouplist() const {
  return normalizationgrouplist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoNormalizationGroup >*
RtTpsProtoPatientTreeData::mutable_normalizationgrouplist() {
  return &normalizationgrouplist_;
}

// repeated .tps.proto.RtTpsProtoBeam beamList = 8;
inline int RtTpsProtoPatientTreeData::beamlist_size() const {
  return beamlist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_beamlist() {
  beamlist_.Clear();
}
inline const ::tps::proto::RtTpsProtoBeam& RtTpsProtoPatientTreeData::beamlist(int index) const {
  return beamlist_.Get(index);
}
inline ::tps::proto::RtTpsProtoBeam* RtTpsProtoPatientTreeData::mutable_beamlist(int index) {
  return beamlist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoBeam* RtTpsProtoPatientTreeData::add_beamlist() {
  return beamlist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoBeam >&
RtTpsProtoPatientTreeData::beamlist() const {
  return beamlist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoBeam >*
RtTpsProtoPatientTreeData::mutable_beamlist() {
  return &beamlist_;
}

// repeated .tps.proto.RtTpsProtoSegment segmentList = 9;
inline int RtTpsProtoPatientTreeData::segmentlist_size() const {
  return segmentlist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_segmentlist() {
  segmentlist_.Clear();
}
inline const ::tps::proto::RtTpsProtoSegment& RtTpsProtoPatientTreeData::segmentlist(int index) const {
  return segmentlist_.Get(index);
}
inline ::tps::proto::RtTpsProtoSegment* RtTpsProtoPatientTreeData::mutable_segmentlist(int index) {
  return segmentlist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoSegment* RtTpsProtoPatientTreeData::add_segmentlist() {
  return segmentlist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSegment >&
RtTpsProtoPatientTreeData::segmentlist() const {
  return segmentlist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSegment >*
RtTpsProtoPatientTreeData::mutable_segmentlist() {
  return &segmentlist_;
}

// repeated .tps.proto.RtTpsProtoPrescription prescriptionList = 10;
inline int RtTpsProtoPatientTreeData::prescriptionlist_size() const {
  return prescriptionlist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_prescriptionlist() {
  prescriptionlist_.Clear();
}
inline const ::tps::proto::RtTpsProtoPrescription& RtTpsProtoPatientTreeData::prescriptionlist(int index) const {
  return prescriptionlist_.Get(index);
}
inline ::tps::proto::RtTpsProtoPrescription* RtTpsProtoPatientTreeData::mutable_prescriptionlist(int index) {
  return prescriptionlist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoPrescription* RtTpsProtoPatientTreeData::add_prescriptionlist() {
  return prescriptionlist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPrescription >&
RtTpsProtoPatientTreeData::prescriptionlist() const {
  return prescriptionlist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPrescription >*
RtTpsProtoPatientTreeData::mutable_prescriptionlist() {
  return &prescriptionlist_;
}

// repeated .tps.proto.RtTpsProtoConstraint constraintList = 11;
inline int RtTpsProtoPatientTreeData::constraintlist_size() const {
  return constraintlist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_constraintlist() {
  constraintlist_.Clear();
}
inline const ::tps::proto::RtTpsProtoConstraint& RtTpsProtoPatientTreeData::constraintlist(int index) const {
  return constraintlist_.Get(index);
}
inline ::tps::proto::RtTpsProtoConstraint* RtTpsProtoPatientTreeData::mutable_constraintlist(int index) {
  return constraintlist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoConstraint* RtTpsProtoPatientTreeData::add_constraintlist() {
  return constraintlist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoConstraint >&
RtTpsProtoPatientTreeData::constraintlist() const {
  return constraintlist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoConstraint >*
RtTpsProtoPatientTreeData::mutable_constraintlist() {
  return &constraintlist_;
}

// repeated .tps.proto.RtTpsProtoDoseLine doseLineList = 12;
inline int RtTpsProtoPatientTreeData::doselinelist_size() const {
  return doselinelist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_doselinelist() {
  doselinelist_.Clear();
}
inline const ::tps::proto::RtTpsProtoDoseLine& RtTpsProtoPatientTreeData::doselinelist(int index) const {
  return doselinelist_.Get(index);
}
inline ::tps::proto::RtTpsProtoDoseLine* RtTpsProtoPatientTreeData::mutable_doselinelist(int index) {
  return doselinelist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoDoseLine* RtTpsProtoPatientTreeData::add_doselinelist() {
  return doselinelist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoDoseLine >&
RtTpsProtoPatientTreeData::doselinelist() const {
  return doselinelist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoDoseLine >*
RtTpsProtoPatientTreeData::mutable_doselinelist() {
  return &doselinelist_;
}

// repeated .tps.proto.RT_TPS_Optgoal optGoalList = 13;
inline int RtTpsProtoPatientTreeData::optgoallist_size() const {
  return optgoallist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_optgoallist() {
  optgoallist_.Clear();
}
inline const ::tps::proto::RT_TPS_Optgoal& RtTpsProtoPatientTreeData::optgoallist(int index) const {
  return optgoallist_.Get(index);
}
inline ::tps::proto::RT_TPS_Optgoal* RtTpsProtoPatientTreeData::mutable_optgoallist(int index) {
  return optgoallist_.Mutable(index);
}
inline ::tps::proto::RT_TPS_Optgoal* RtTpsProtoPatientTreeData::add_optgoallist() {
  return optgoallist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Optgoal >&
RtTpsProtoPatientTreeData::optgoallist() const {
  return optgoallist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_Optgoal >*
RtTpsProtoPatientTreeData::mutable_optgoallist() {
  return &optgoallist_;
}

// repeated .tps.proto.RtTpsProtoTable tpsTable = 14;
inline int RtTpsProtoPatientTreeData::tpstable_size() const {
  return tpstable_.size();
}
inline void RtTpsProtoPatientTreeData::clear_tpstable() {
  tpstable_.Clear();
}
inline const ::tps::proto::RtTpsProtoTable& RtTpsProtoPatientTreeData::tpstable(int index) const {
  return tpstable_.Get(index);
}
inline ::tps::proto::RtTpsProtoTable* RtTpsProtoPatientTreeData::mutable_tpstable(int index) {
  return tpstable_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoTable* RtTpsProtoPatientTreeData::add_tpstable() {
  return tpstable_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoTable >&
RtTpsProtoPatientTreeData::tpstable() const {
  return tpstable_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoTable >*
RtTpsProtoPatientTreeData::mutable_tpstable() {
  return &tpstable_;
}

// repeated .tps.proto.RtTpsProtoSeriesGroup seriesGroup = 15;
inline int RtTpsProtoPatientTreeData::seriesgroup_size() const {
  return seriesgroup_.size();
}
inline void RtTpsProtoPatientTreeData::clear_seriesgroup() {
  seriesgroup_.Clear();
}
inline const ::tps::proto::RtTpsProtoSeriesGroup& RtTpsProtoPatientTreeData::seriesgroup(int index) const {
  return seriesgroup_.Get(index);
}
inline ::tps::proto::RtTpsProtoSeriesGroup* RtTpsProtoPatientTreeData::mutable_seriesgroup(int index) {
  return seriesgroup_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoSeriesGroup* RtTpsProtoPatientTreeData::add_seriesgroup() {
  return seriesgroup_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSeriesGroup >&
RtTpsProtoPatientTreeData::seriesgroup() const {
  return seriesgroup_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSeriesGroup >*
RtTpsProtoPatientTreeData::mutable_seriesgroup() {
  return &seriesgroup_;
}

// repeated .tps.proto.ReferenceImage rtImageList = 16;
inline int RtTpsProtoPatientTreeData::rtimagelist_size() const {
  return rtimagelist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_rtimagelist() {
  rtimagelist_.Clear();
}
inline const ::tps::proto::ReferenceImage& RtTpsProtoPatientTreeData::rtimagelist(int index) const {
  return rtimagelist_.Get(index);
}
inline ::tps::proto::ReferenceImage* RtTpsProtoPatientTreeData::mutable_rtimagelist(int index) {
  return rtimagelist_.Mutable(index);
}
inline ::tps::proto::ReferenceImage* RtTpsProtoPatientTreeData::add_rtimagelist() {
  return rtimagelist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::ReferenceImage >&
RtTpsProtoPatientTreeData::rtimagelist() const {
  return rtimagelist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::ReferenceImage >*
RtTpsProtoPatientTreeData::mutable_rtimagelist() {
  return &rtimagelist_;
}

// repeated .tps.proto.RtTpsProtoSite siteList = 17;
inline int RtTpsProtoPatientTreeData::sitelist_size() const {
  return sitelist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_sitelist() {
  sitelist_.Clear();
}
inline const ::tps::proto::RtTpsProtoSite& RtTpsProtoPatientTreeData::sitelist(int index) const {
  return sitelist_.Get(index);
}
inline ::tps::proto::RtTpsProtoSite* RtTpsProtoPatientTreeData::mutable_sitelist(int index) {
  return sitelist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoSite* RtTpsProtoPatientTreeData::add_sitelist() {
  return sitelist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSite >&
RtTpsProtoPatientTreeData::sitelist() const {
  return sitelist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoSite >*
RtTpsProtoPatientTreeData::mutable_sitelist() {
  return &sitelist_;
}

// repeated .tps.proto.RT_TPS_DVHData dvhList = 18;
inline int RtTpsProtoPatientTreeData::dvhlist_size() const {
  return dvhlist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_dvhlist() {
  dvhlist_.Clear();
}
inline const ::tps::proto::RT_TPS_DVHData& RtTpsProtoPatientTreeData::dvhlist(int index) const {
  return dvhlist_.Get(index);
}
inline ::tps::proto::RT_TPS_DVHData* RtTpsProtoPatientTreeData::mutable_dvhlist(int index) {
  return dvhlist_.Mutable(index);
}
inline ::tps::proto::RT_TPS_DVHData* RtTpsProtoPatientTreeData::add_dvhlist() {
  return dvhlist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_DVHData >&
RtTpsProtoPatientTreeData::dvhlist() const {
  return dvhlist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RT_TPS_DVHData >*
RtTpsProtoPatientTreeData::mutable_dvhlist() {
  return &dvhlist_;
}

// repeated .tps.proto.RtTpsProtoPoi maxPointList = 19;
inline int RtTpsProtoPatientTreeData::maxpointlist_size() const {
  return maxpointlist_.size();
}
inline void RtTpsProtoPatientTreeData::clear_maxpointlist() {
  maxpointlist_.Clear();
}
inline const ::tps::proto::RtTpsProtoPoi& RtTpsProtoPatientTreeData::maxpointlist(int index) const {
  return maxpointlist_.Get(index);
}
inline ::tps::proto::RtTpsProtoPoi* RtTpsProtoPatientTreeData::mutable_maxpointlist(int index) {
  return maxpointlist_.Mutable(index);
}
inline ::tps::proto::RtTpsProtoPoi* RtTpsProtoPatientTreeData::add_maxpointlist() {
  return maxpointlist_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPoi >&
RtTpsProtoPatientTreeData::maxpointlist() const {
  return maxpointlist_;
}
inline ::google::protobuf::RepeatedPtrField< ::tps::proto::RtTpsProtoPoi >*
RtTpsProtoPatientTreeData::mutable_maxpointlist() {
  return &maxpointlist_;
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

#endif  // PROTOBUF_rt_5ftps_5fproto_5fpatient_5ftree_5fdata_2eproto__INCLUDED