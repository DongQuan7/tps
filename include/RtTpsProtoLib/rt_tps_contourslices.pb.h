// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rt_tps_contourslices.proto

#ifndef PROTOBUF_rt_5ftps_5fcontourslices_2eproto__INCLUDED
#define PROTOBUF_rt_5ftps_5fcontourslices_2eproto__INCLUDED

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
        void  protobuf_AddDesc_rt_5ftps_5fcontourslices_2eproto();
        void protobuf_AssignDesc_rt_5ftps_5fcontourslices_2eproto();
        void protobuf_ShutdownFile_rt_5ftps_5fcontourslices_2eproto();

        class RT_TPS_CONTOUR_SLICES;

        // ===================================================================

        class RT_TPS_CONTOUR_SLICES : public ::google::protobuf::Message {
        public:
            RT_TPS_CONTOUR_SLICES();
            virtual ~RT_TPS_CONTOUR_SLICES();

            RT_TPS_CONTOUR_SLICES(const RT_TPS_CONTOUR_SLICES& from);

            inline RT_TPS_CONTOUR_SLICES& operator=(const RT_TPS_CONTOUR_SLICES& from) {
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
            static const RT_TPS_CONTOUR_SLICES& default_instance();

            void Swap(RT_TPS_CONTOUR_SLICES* other);

            // implements Message ----------------------------------------------

            RT_TPS_CONTOUR_SLICES* New() const;
            void CopyFrom(const ::google::protobuf::Message& from);
            void MergeFrom(const ::google::protobuf::Message& from);
            void CopyFrom(const RT_TPS_CONTOUR_SLICES& from);
            void MergeFrom(const RT_TPS_CONTOUR_SLICES& from);
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

            // optional int32 first_slice = 1;
            inline bool has_first_slice() const;
            inline void clear_first_slice();
            static const int kFirstSliceFieldNumber = 1;
            inline ::google::protobuf::int32 first_slice() const;
            inline void set_first_slice(::google::protobuf::int32 value);

            // optional int32 second_slice = 2;
            inline bool has_second_slice() const;
            inline void clear_second_slice();
            static const int kSecondSliceFieldNumber = 2;
            inline ::google::protobuf::int32 second_slice() const;
            inline void set_second_slice(::google::protobuf::int32 value);

            // required string voi_uid = 3;
            inline bool has_voi_uid() const;
            inline void clear_voi_uid();
            static const int kVoiUidFieldNumber = 3;
            inline const ::std::string& voi_uid() const;
            inline void set_voi_uid(const ::std::string& value);
            inline void set_voi_uid(const char* value);
            inline void set_voi_uid(const char* value, size_t size);
            inline ::std::string* mutable_voi_uid();
            inline ::std::string* release_voi_uid();

            // optional int32 edit_mode = 4;
            inline bool has_edit_mode() const;
            inline void clear_edit_mode();
            static const int kEditModeFieldNumber = 4;
            inline ::google::protobuf::int32 edit_mode() const;
            inline void set_edit_mode(::google::protobuf::int32 value);

            // optional bool isReplace = 5;
            inline bool has_isreplace() const;
            inline void clear_isreplace();
            static const int kIsReplaceFieldNumber = 5;
            inline bool isreplace() const;
            inline void set_isreplace(bool value);

            // optional int32 windowType = 6;
            inline bool has_windowtype() const;
            inline void clear_windowtype();
            static const int kWindowTypeFieldNumber = 6;
            inline ::google::protobuf::int32 windowtype() const;
            inline void set_windowtype(::google::protobuf::int32 value);

            // optional string windowUid = 7;
            inline bool has_windowuid() const;
            inline void clear_windowuid();
            static const int kWindowUidFieldNumber = 7;
            inline const ::std::string& windowuid() const;
            inline void set_windowuid(const ::std::string& value);
            inline void set_windowuid(const char* value);
            inline void set_windowuid(const char* value, size_t size);
            inline ::std::string* mutable_windowuid();
            inline ::std::string* release_windowuid();

            // optional int32 sub_edit_mode = 8;
            inline bool has_sub_edit_mode() const;
            inline void clear_sub_edit_mode();
            static const int kSubEditModeFieldNumber = 8;
            inline ::google::protobuf::int32 sub_edit_mode() const;
            inline void set_sub_edit_mode(::google::protobuf::int32 value);

            // @@protoc_insertion_point(class_scope:tps.proto.RT_TPS_CONTOUR_SLICES)
        private:
            inline void set_has_first_slice();
            inline void clear_has_first_slice();
            inline void set_has_second_slice();
            inline void clear_has_second_slice();
            inline void set_has_voi_uid();
            inline void clear_has_voi_uid();
            inline void set_has_edit_mode();
            inline void clear_has_edit_mode();
            inline void set_has_isreplace();
            inline void clear_has_isreplace();
            inline void set_has_windowtype();
            inline void clear_has_windowtype();
            inline void set_has_windowuid();
            inline void clear_has_windowuid();
            inline void set_has_sub_edit_mode();
            inline void clear_has_sub_edit_mode();

            ::google::protobuf::UnknownFieldSet _unknown_fields_;

            ::google::protobuf::int32 first_slice_;
            ::google::protobuf::int32 second_slice_;
            ::std::string* voi_uid_;
            ::google::protobuf::int32 edit_mode_;
            bool isreplace_;
            ::std::string* windowuid_;
            ::google::protobuf::int32 windowtype_;
            ::google::protobuf::int32 sub_edit_mode_;

            mutable int _cached_size_;
            ::google::protobuf::uint32 _has_bits_[(8 + 31) / 32];

            friend void  protobuf_AddDesc_rt_5ftps_5fcontourslices_2eproto();
            friend void protobuf_AssignDesc_rt_5ftps_5fcontourslices_2eproto();
            friend void protobuf_ShutdownFile_rt_5ftps_5fcontourslices_2eproto();

            void InitAsDefaultInstance();
            static RT_TPS_CONTOUR_SLICES* default_instance_;
        };
        // ===================================================================


        // ===================================================================

        // RT_TPS_CONTOUR_SLICES

        // optional int32 first_slice = 1;
        inline bool RT_TPS_CONTOUR_SLICES::has_first_slice() const {
            return (_has_bits_[0] & 0x00000001u) != 0;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_has_first_slice() {
            _has_bits_[0] |= 0x00000001u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_has_first_slice() {
            _has_bits_[0] &= ~0x00000001u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_first_slice() {
            first_slice_ = 0;
            clear_has_first_slice();
        }
        inline ::google::protobuf::int32 RT_TPS_CONTOUR_SLICES::first_slice() const {
            return first_slice_;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_first_slice(::google::protobuf::int32 value) {
            set_has_first_slice();
            first_slice_ = value;
        }

        // optional int32 second_slice = 2;
        inline bool RT_TPS_CONTOUR_SLICES::has_second_slice() const {
            return (_has_bits_[0] & 0x00000002u) != 0;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_has_second_slice() {
            _has_bits_[0] |= 0x00000002u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_has_second_slice() {
            _has_bits_[0] &= ~0x00000002u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_second_slice() {
            second_slice_ = 0;
            clear_has_second_slice();
        }
        inline ::google::protobuf::int32 RT_TPS_CONTOUR_SLICES::second_slice() const {
            return second_slice_;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_second_slice(::google::protobuf::int32 value) {
            set_has_second_slice();
            second_slice_ = value;
        }

        // required string voi_uid = 3;
        inline bool RT_TPS_CONTOUR_SLICES::has_voi_uid() const {
            return (_has_bits_[0] & 0x00000004u) != 0;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_has_voi_uid() {
            _has_bits_[0] |= 0x00000004u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_has_voi_uid() {
            _has_bits_[0] &= ~0x00000004u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_voi_uid() {
            if (voi_uid_ != &::google::protobuf::internal::kEmptyString) {
                voi_uid_->clear();
            }
            clear_has_voi_uid();
        }
        inline const ::std::string& RT_TPS_CONTOUR_SLICES::voi_uid() const {
            return *voi_uid_;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_voi_uid(const ::std::string& value) {
            set_has_voi_uid();
            if (voi_uid_ == &::google::protobuf::internal::kEmptyString) {
                voi_uid_ = new ::std::string;
            }
            voi_uid_->assign(value);
        }
        inline void RT_TPS_CONTOUR_SLICES::set_voi_uid(const char* value) {
            set_has_voi_uid();
            if (voi_uid_ == &::google::protobuf::internal::kEmptyString) {
                voi_uid_ = new ::std::string;
            }
            voi_uid_->assign(value);
        }
        inline void RT_TPS_CONTOUR_SLICES::set_voi_uid(const char* value, size_t size) {
            set_has_voi_uid();
            if (voi_uid_ == &::google::protobuf::internal::kEmptyString) {
                voi_uid_ = new ::std::string;
            }
            voi_uid_->assign(reinterpret_cast<const char*>(value), size);
        }
        inline ::std::string* RT_TPS_CONTOUR_SLICES::mutable_voi_uid() {
            set_has_voi_uid();
            if (voi_uid_ == &::google::protobuf::internal::kEmptyString) {
                voi_uid_ = new ::std::string;
            }
            return voi_uid_;
        }
        inline ::std::string* RT_TPS_CONTOUR_SLICES::release_voi_uid() {
            clear_has_voi_uid();
            if (voi_uid_ == &::google::protobuf::internal::kEmptyString) {
                return NULL;
            } else {
                ::std::string* temp = voi_uid_;
                voi_uid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
                return temp;
            }
        }

        // optional int32 edit_mode = 4;
        inline bool RT_TPS_CONTOUR_SLICES::has_edit_mode() const {
            return (_has_bits_[0] & 0x00000008u) != 0;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_has_edit_mode() {
            _has_bits_[0] |= 0x00000008u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_has_edit_mode() {
            _has_bits_[0] &= ~0x00000008u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_edit_mode() {
            edit_mode_ = 0;
            clear_has_edit_mode();
        }
        inline ::google::protobuf::int32 RT_TPS_CONTOUR_SLICES::edit_mode() const {
            return edit_mode_;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_edit_mode(::google::protobuf::int32 value) {
            set_has_edit_mode();
            edit_mode_ = value;
        }

        // optional bool isReplace = 5;
        inline bool RT_TPS_CONTOUR_SLICES::has_isreplace() const {
            return (_has_bits_[0] & 0x00000010u) != 0;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_has_isreplace() {
            _has_bits_[0] |= 0x00000010u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_has_isreplace() {
            _has_bits_[0] &= ~0x00000010u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_isreplace() {
            isreplace_ = false;
            clear_has_isreplace();
        }
        inline bool RT_TPS_CONTOUR_SLICES::isreplace() const {
            return isreplace_;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_isreplace(bool value) {
            set_has_isreplace();
            isreplace_ = value;
        }

        // optional int32 windowType = 6;
        inline bool RT_TPS_CONTOUR_SLICES::has_windowtype() const {
            return (_has_bits_[0] & 0x00000020u) != 0;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_has_windowtype() {
            _has_bits_[0] |= 0x00000020u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_has_windowtype() {
            _has_bits_[0] &= ~0x00000020u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_windowtype() {
            windowtype_ = 0;
            clear_has_windowtype();
        }
        inline ::google::protobuf::int32 RT_TPS_CONTOUR_SLICES::windowtype() const {
            return windowtype_;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_windowtype(::google::protobuf::int32 value) {
            set_has_windowtype();
            windowtype_ = value;
        }

        // optional string windowUid = 7;
        inline bool RT_TPS_CONTOUR_SLICES::has_windowuid() const {
            return (_has_bits_[0] & 0x00000040u) != 0;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_has_windowuid() {
            _has_bits_[0] |= 0x00000040u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_has_windowuid() {
            _has_bits_[0] &= ~0x00000040u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_windowuid() {
            if (windowuid_ != &::google::protobuf::internal::kEmptyString) {
                windowuid_->clear();
            }
            clear_has_windowuid();
        }
        inline const ::std::string& RT_TPS_CONTOUR_SLICES::windowuid() const {
            return *windowuid_;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_windowuid(const ::std::string& value) {
            set_has_windowuid();
            if (windowuid_ == &::google::protobuf::internal::kEmptyString) {
                windowuid_ = new ::std::string;
            }
            windowuid_->assign(value);
        }
        inline void RT_TPS_CONTOUR_SLICES::set_windowuid(const char* value) {
            set_has_windowuid();
            if (windowuid_ == &::google::protobuf::internal::kEmptyString) {
                windowuid_ = new ::std::string;
            }
            windowuid_->assign(value);
        }
        inline void RT_TPS_CONTOUR_SLICES::set_windowuid(const char* value, size_t size) {
            set_has_windowuid();
            if (windowuid_ == &::google::protobuf::internal::kEmptyString) {
                windowuid_ = new ::std::string;
            }
            windowuid_->assign(reinterpret_cast<const char*>(value), size);
        }
        inline ::std::string* RT_TPS_CONTOUR_SLICES::mutable_windowuid() {
            set_has_windowuid();
            if (windowuid_ == &::google::protobuf::internal::kEmptyString) {
                windowuid_ = new ::std::string;
            }
            return windowuid_;
        }
        inline ::std::string* RT_TPS_CONTOUR_SLICES::release_windowuid() {
            clear_has_windowuid();
            if (windowuid_ == &::google::protobuf::internal::kEmptyString) {
                return NULL;
            } else {
                ::std::string* temp = windowuid_;
                windowuid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
                return temp;
            }
        }

        // optional int32 sub_edit_mode = 8;
        inline bool RT_TPS_CONTOUR_SLICES::has_sub_edit_mode() const {
            return (_has_bits_[0] & 0x00000080u) != 0;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_has_sub_edit_mode() {
            _has_bits_[0] |= 0x00000080u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_has_sub_edit_mode() {
            _has_bits_[0] &= ~0x00000080u;
        }
        inline void RT_TPS_CONTOUR_SLICES::clear_sub_edit_mode() {
            sub_edit_mode_ = 0;
            clear_has_sub_edit_mode();
        }
        inline ::google::protobuf::int32 RT_TPS_CONTOUR_SLICES::sub_edit_mode() const {
            return sub_edit_mode_;
        }
        inline void RT_TPS_CONTOUR_SLICES::set_sub_edit_mode(::google::protobuf::int32 value) {
            set_has_sub_edit_mode();
            sub_edit_mode_ = value;
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

#endif  // PROTOBUF_rt_5ftps_5fcontourslices_2eproto__INCLUDED
