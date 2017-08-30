//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_client_tcp.h
///  \brief     client using tcp
///
///  \version 1.0
///  \date    Apr.5, 2017
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "rt_tps_database_client_tcp.h"
#include "boost/asio.hpp"
#include "tps_performance.h"

// #include <string>
// #include <iostream>
// #include <fstream>

RT_TPS_DATABASE_BEGIN_NAMESPACE;

RtDatabaseClientTcp::RtDatabaseClientTcp(const std::string& host, unsigned int port):
m_sHostName(host),
    m_uiPort(port)
{
}

RtDatabaseClientTcp::~RtDatabaseClientTcp()
{

}



int RtDatabaseClientTcp::SendRequest(protocol pro, const std::string& sDataToServer, std::string& sDataFromServer)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    int iRet = -1;

    const std::string sServer = m_sHostName;
    const std::string sPort = std::to_string((long long )(m_uiPort));
    try
    {
        // Step 1. 
        //static  boost::asio::io_service stMainService;
        boost::asio::io_service stMainService;

        // Step 2. 
        boost::asio::ip::tcp::resolver stResolver(stMainService);
        boost::asio::ip::tcp::endpoint stEndpoint = *stResolver.resolve(boost::asio::ip::tcp::resolver::query(sServer, sPort)); //8731

        // Step 3.
        boost::shared_ptr<boost::asio::ip::tcp::socket>
            ptrCurSock = boost::shared_ptr<boost::asio::ip::tcp::socket>(
            new boost::asio::ip::tcp::socket(stMainService));

        // Step 4. 
        //ptrCurSock->connect(stEndpoint);
        boost::system::error_code error_code;
        ptrCurSock->connect(stEndpoint, error_code);
        if (error_code)
        {
            TPS_LOG_DEV_ERROR<<"---- Connect Failed error code:"
                << error_code.value()<<" "<< error_code.message();
            return 1;
        }
        else
        {
            TPS_LOG_DEV_INFO<< "----Connect Success => "<< ptrCurSock->local_endpoint().port();
        }

        // 
        boost::asio::streambuf stSendedBuff;
        std::ostream stSendedBuffOS(&stSendedBuff);

        std::string ssData;
        switch (pro.method)
        {
        case TCP_POST:
            {
                ssData = sDataToServer;
            }
            break;
        case TCP_GET:
            {
                ssData = sDataToServer;
            }
            break;
        case TCP_DELETE:
            {        
                ssData = sDataToServer;
            }
            break;
        default:
            TPS_LOG_DEV_ERROR<<"Wrong protocol:"<<pro.method;
            return 3;
            break;
        }

        char* maxfilename = new char[pro.filepath_maxlength + 1];
        const std::string sFileName = pro.filename;
        size_t fnSize = sFileName.size();
        for (size_t i = 0; i < fnSize; ++i)
        {
            maxfilename[i] = sFileName[i];
        }
        for (size_t i = fnSize; i < (size_t)pro.filepath_maxlength; ++i)
        {
            maxfilename[i] = '*';
        }
        maxfilename[pro.filepath_maxlength] = (char)(0);
        pro.filename.assign(maxfilename);
        delete[] maxfilename;

        // Step 5 send data to server
        stSendedBuffOS<<pro.method;
        stSendedBuffOS<<pro.separator;
        stSendedBuffOS<<pro.filename;
        stSendedBuffOS<<pro.separator;

        stSendedBuffOS<<ssData;
        size_t ullBufSize = stSendedBuff.size();

        std::cout<<"sent data size to server:"<<ullBufSize<<"\n";
        boost::asio::write(*ptrCurSock, boost::asio::buffer(&ullBufSize, sizeof(ullBufSize)), boost::asio::transfer_exactly(sizeof(ullBufSize)));

        boost::asio::streambuf sbProtocolBuffer;
        std::ostream osProtocolBuffer(&sbProtocolBuffer);

        std::cout<<"buffer size:"<<stSendedBuff.size()<<"\n";
        boost::asio::write(*ptrCurSock, stSendedBuff, boost::asio::transfer_at_least(stSendedBuff.size()));
        // sent data is removed from input sequence
        stSendedBuff.consume(stSendedBuff.size()); 

        TPS_LOG_DEV_INFO<< "**** Client Thread Sent";

        if (pro.method != TCP_DELETE)
        {
            // Step 6 receive data 
            boost::asio::streambuf sockStreamBuff;
            boost::asio::streambuf::mutable_buffers_type mutableBuffer = sockStreamBuff.prepare(sizeof(size_t));
            ptrCurSock->read_some(boost::asio::buffer(mutableBuffer)); 
            const size_t* pLen = boost::asio::buffer_cast<const size_t*>(sockStreamBuff.data()); 
            std::cout<<"buffer data size:"<<*pLen<<"\n";
            int iRecvDataSize =  (int)(*pLen);
            if (iRecvDataSize > 0)
            {
                char* pBufferRecv = new char[iRecvDataSize + 1];
                memset(pBufferRecv, 0, iRecvDataSize + 1);
                //////////////////////////////////////////////////////////////////////////
                //The following line has bug!! NOT get the right result!
                //ptrCurSock->read_some(boost::asio::buffer(pBufferRecv, iRecvDataSize));
                //////////////////////////////////////////////////////////////////////////
                size_t length = boost::asio::read(*ptrCurSock, boost::asio::buffer(pBufferRecv, iRecvDataSize));
                if (length != iRecvDataSize)
                {
                    iRet = 3;
                    TPS_LOG_DEV_ERROR<<"Get data from server error as length:"<<length<<"\t recv size:"<<iRecvDataSize;
                }
                
                else
                {
                    //TPS_LOG_DEV_INFO<< "---- Client Recv Data: \n";//<< pBufferRecv + sizeof(size_t)<< std::endl;  
                    char const * p = reinterpret_cast<char const *>(pBufferRecv);
                    std::string sTemp(p, p + iRecvDataSize);
                    //sDataFromServer.assign(pBufferRecv, iRecvDataSize);
                    sDataFromServer = sTemp;

                }
                delete[] pBufferRecv;
            }
        }

        // Step 7. waiting for server closed
        stMainService.run();

        // Step 8. shutdown Socket 
        ptrCurSock->shutdown(boost::asio::ip::tcp::socket::shutdown_both); //  shutdown_send
        ptrCurSock->close();

        TEST_PERFORMANCE_END("client_thread");
        iRet = 0;
    }
    catch (std::exception& e)
    {
        std::cout<<e.what();
        iRet = 2;
    }
    return iRet;
}
RT_TPS_DATABASE_END_NAMESPACE;

