//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      tps_fw_error.h
///  \brief     tps error class
///
///  \version 1.0
///  \date    Oct. 21,2016
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef  TPS_FW_ERROR_H_
#define  TPS_FW_ERROR_H_

#include "tps_fw_defs.h"

TPS_BEGIN_NAMESPACE;

class TPS_FW_DECLSPEC TpsError
{
public:
    TpsError(int err = 0, const std::string &message = "");

    virtual ~TpsError();

    int Error() const;

    operator bool () const;

    TpsError(const TpsError& err);
    const TpsError& operator = (const TpsError& err);

    const char* What() const;

    const char* ErrorName() const;

    void SetError(int err = 0, const std::string &message = "");

    void SetMessage(const std::string &message = "");
    /**
    Lookup an error by code in the global_error_names array.
    @param code the code to lookup
    @param [out] name_ptr the error name, when found
    @param [out] msg_ptr the error text, when found
    @return true when found, otherwise false
    */
    bool GetErrorMessage(int code, const char **name_ptr, const char **msg_ptr);

    const char* GetErrorMessage(int code);

private:
    std::string _message;
    int _error;
};
TPS_END_NAMESPACE;

#endif //TPS_FW_ERROR_H_
