//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_client_protocol.h
///  \brief     rt tps client protocol
///
///
///  \version 1.0
///  \date   Apr. 5, 2017
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_CLIENT_PROTOCOL_H_
#define RT_TPS_DATABASE_CLIENT_PROTOCOL_H_

#include <string>

namespace tps
{
        enum protocol_method
        {
            TCP_GET = 0,
            TCP_POST = 1,
            TCP_DELETE = 2,
        };

        /// A protocol received from a client.
        /// method 0:TCP_GET 1:TCP_POST 2:TCP_DELETE
        struct protocol
        {
            protocol::protocol()
            {
                this->method                = TCP_GET;
                this->filename              = "";
                this->separator             = "|";
                this->filepath_maxlength    = 260;
            }
            //0:TCP_GET 1:TCP_POST 2:TCP_DELETE
            int         method;
            std::string separator;
            int         filepath_maxlength;
            //!!should never include '*'
            std::string filename;
        };
} // namespace tps

#endif // RT_TPS_DATABASE_CLIENT_PROTOCOL_H_