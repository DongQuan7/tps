//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      tps_fw_error.cpp
///  \brief     tps error class
///
///  \version 1.0
///  \date    Oct. 21,2016
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_error.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE;   // begin namespace tps

struct st_error
{
    const char *name;
    int         code;
    const char *text;
};

static st_error global_error_names[] =
{
#include "tps_error_name.h"
    { 0, 0, 0 }
};

TpsError::TpsError(int err, const std::string &message)
    :_message(message), _error(err)
{

}

TpsError::~TpsError()
{

}

TpsError::TpsError(const TpsError& err):
_error(err.Error()),
    _message(err.What())
{ 

}

const TpsError& TpsError::operator = (const TpsError& err)
{
    if (this != &err)
    {
        this->_error = err.Error();
        this->_message = err.What();
    }
    return *this;
}

int TpsError::Error() const 
{
    return _error; 
}

TpsError::operator bool () const
{
    return 0 != _error;
}

const char* TpsError::What() const
{
    return _message.c_str();
}

const char* TpsError::ErrorName() const
{
    st_error *tmp_error = & global_error_names[0];
    while (tmp_error->name != nullptr)
    {
        if (tmp_error->code == _error)
        {
            return tmp_error->name;
        }
        ++tmp_error;
    }
    return "";
}

void TpsError::SetError(int err, const std::string &message)
{
    _error = err;
    _message = message;
}

void TpsError::SetMessage(const std::string &message)
{
    _message = message;
}

bool TpsError::GetErrorMessage(int code, const char **name_ptr, const char **msg_ptr)
{
    st_error *tmp_error = & global_error_names[0];

    while (tmp_error->name != NULL)
    {
        if (tmp_error->code == code)
        {
            *name_ptr= tmp_error->name;
            *msg_ptr= tmp_error->text;
            return true;
        }
        tmp_error++;
    }

    return false;
}

const char* TpsError::GetErrorMessage(int code)
{
    st_error tmp;
    if (this->GetErrorMessage(code, &tmp.name, &tmp.text))
    {
        return tmp.name;
    }
    return "";
}

TPS_END_NAMESPACE  // end namespace tps
