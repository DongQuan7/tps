//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_client.cpp
///  \brief     client to download file from server
///
///  \version 1.0
///  \date    Jan.4, 2016
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "rt_tps_database_client.h"

#include "boost/asio.hpp"
#include <algorithm>
#include "tps_performance.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

RtDatabaseClient::RtDatabaseClient(const std::string& host, unsigned int port):
m_sHostName(host),
    m_uiPort(port),
    m_sPageDownload(""),
    m_sPageUpload("")
{
}

RtDatabaseClient::~RtDatabaseClient()
{

}

void RtDatabaseClient::SetParameters(const std::string& host, unsigned int port)
{
    m_sHostName = host;
    m_uiPort = port;
}

void RtDatabaseClient::SetWebPageDownload(const std::string& page)
{
    m_sPageDownload = page;
}

void RtDatabaseClient::SetWebPageUpload(const std::string& page)
{
    m_sPageUpload = page;
}

inline int gethex(std::string hexstr)
{
    return (int)strtol(hexstr.c_str(),0,16);
}

//for http 1.1 chucked encoding
//greenbytes.de/tech/webdav/rfc7230.html#core.rules
//length := 0
//            read chunk-size, chunk-ext (if any), and CRLF
//            while (chunk-size > 0) {
//                read chunk-data and CRLF
//                    append chunk-data to decoded-body
//length := length + chunk-size
//        read chunk-size, chunk-ext (if any), and CRLF
//            }
void GetRealData(std::string& input, std::string& ss, char* output, size_t* real_size)
{
    if (nullptr == output || nullptr == real_size)
    {
        return;
    }
    ss = input.substr(0, input.find_first_of("\r"));
    size_t ss_size = ss.size();
    size_t str_transferred = 0;
    try
    { 
        //str_transferred = std::stoi(headerLine);
        str_transferred = gethex(ss);
    }
    catch(std::exception& exp)
    {
        TPS_LOG_DEV_ERROR<<"GetRealData exception"<<exp.what();
        str_transferred = 0;
    }
    if (str_transferred > 0)
    {
        //output.append(input.substr(ss.size() + 2, str_transferred));
        memcpy(output+ (*real_size), input.substr(ss.size() + 2, str_transferred).c_str(), str_transferred);
        size_t offset_size =  ss_size + 2 + str_transferred + 2;//'number\r----\r'
        input = input.substr(offset_size, input.size() - offset_size);
        *real_size += str_transferred;
        GetRealData(input, ss, output, real_size);
    }
    else
    {
        return;
    }
}

int RtDatabaseClient::DownloadFile(const std::string& path, char*& filebuffer, size_t* filesize)
{
    try
    { 

        //TEST_PERFORMANCE_INIT;
        //TEST_PERFORMANCE_BEGIN;

        std::string filepath = path;
        std::replace(filepath.begin(), filepath.end(), '\\', '/');
        //std::cout<<"new file path:\n"<<filepath<<"\n";
        using boost::asio::ip::tcp;
        boost::asio::io_service io_service;
        if(io_service.stopped())
        {
            io_service.reset();
        }

        // 
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(m_sHostName, std::to_string(long long (m_uiPort)));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator); 

        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        //NOTE HTTP/1.1 default is chucked with low performance
        // So we use HTTP/1.0
        request_stream << "POST " << m_sPageDownload << " HTTP/1.0\r\n";

        request_stream << "Host: " << m_sHostName << ":" << m_uiPort << "\r\n";
        request_stream << "Accept:*/*\r\n";
        std::string requestData = "\"" + filepath + "\"";
        request_stream << "Content-Length: " << requestData.length() << "\r\n";
        request_stream << "Content-Type: application/json\r\n";
        //request_stream << "Content-Encoding: gzip\r\n";
        request_stream << "Connection: close\r\n\r\n";
        request_stream << requestData;

        // Send the request.
        boost::asio::write(socket, request);

        // Read the response status line. The response streambuf will automatically
        // grow to accommodate the entire line. The growth may be limited by passing
        // a maximum size to the streambuf constructor.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
            //reponse_data = "Invalid response";
            TPS_LOG_DEV_ERROR<<"RtDatabaseClient::Download File Invalid response";
            return -2;
        }

        if (status_code != 200)
        {
            //reponse_data = "Response returned with status code != 200 " ;   
            TPS_LOG_DEV_ERROR<<"RtDatabaseClient::Download File response returned with status code:"<<status_code;
            return status_code;
        }

        std::string header;
        std::vector<std::string> headers;        
        while (std::getline(response_stream, header) && header != "\r")
        {
            headers.push_back(header);
        }

        // response body
        boost::system::error_code error;
        std::string headerLine;

        std::stringstream ss;
        std::vector<int> vDataSize;
        std::stringstream sResponseData;
        while (boost::asio::read(socket, response,boost::asio::transfer_at_least(1), error))
        {   
            sResponseData << &response;
        }

        size_t ulSize = sResponseData.str().size();
        if (ulSize > 0)
        {
            std::string htmlContent = sResponseData.str();

            char* pData = new char[ulSize + 1];
            //memset(pData, 0, ulSize + 1);
            memcpy(pData, htmlContent.c_str(), ulSize);
            filebuffer = pData;
            *filesize = ulSize;
            //delete[] pData;
        }

        if (error != boost::asio::error::eof)
        {
            //reponse_data = error.message();
            TPS_LOG_DEV_ERROR<<"Download File boost::asio::error::eof:"<<error.message();
            return -3;
        }
        //TEST_PERFORMANCE_END("RtDatabaseClient::DownloadFile");
        //std::cout<<"filepath:\n"<<filepath<<"\n";
    }
    catch(std::exception& e)
    {
        //reponse_data = e.what();
        TPS_LOG_DEV_ERROR<<"Download File exception:"<<e.what();
        return -4;
    }
    return 0;
}

int RtDatabaseClient::UploadFile(const char* filebuffer, size_t filesize, std::string* filepath)
{                      

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    int iRet = -1;
    try
    { 
        boost::asio::io_service io_service;
        if(io_service.stopped())
        {
            io_service.reset();
        }

        if (nullptr == filepath)
        {
            TPS_LOG_DEV_ERROR<<"filepath is null";
            return iRet;
        }

        *filepath = "";

        using boost::asio::ip::tcp;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(m_sHostName, std::to_string(long long (m_uiPort)));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator); 

        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "POST " << m_sPageUpload << " HTTP/1.1\r\n";
        request_stream << "Host: " << m_sHostName << ":" << m_uiPort << "\r\n";
        request_stream << "Accept:Application/json\r\n";
        request_stream << "Content-Length: " << filesize << "\r\n";
        request_stream << "Content-Type: Application/octet-stream\r\n";
        request_stream << "Connection: close\r\n\r\n";

        const std::string filedata(filebuffer, filesize);
        request_stream << filedata;

        //std::stringstream sstr;
        //sstr << request_stream.rdbuf();
        //std::string req = sstr.str();
        //size_t request_length = req.length() + filesize;
        //char* pRequest = new char[request_length];
        //memcpy(pRequest, req.c_str(),  req.length());
        //memcpy(pRequest + req.length(), filebuffer, filesize);

        // Send the request.
        //TODO Write to chunk to socket and async.
        //size_t iBytesWrite = boost::asio::write(socket, boost::asio::buffer(pRequest, request_length));
        size_t iBytesWrite = boost::asio::write(socket, request);
        //delete[] pRequest; 
        if (iBytesWrite < 1)
        {
            TPS_LOG_DEV_ERROR<<"boost::asio::write error byte write 0";
        }

        // Read the response status line. The response streambuf will automatically
        // grow to accommodate the entire line. The growth may be limited by passing
        // a maximum size to the streambuf constructor.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
            // reponse_data = "Invalid response";
             TPS_LOG_DEV_ERROR<<"RtDatabaseClient::DownloadFile Invalid response";
            return -2;
        }
        if (status_code != 200)
        {
            //  reponse_data = "Response returned with status code != 200 " ;
            TPS_LOG_DEV_ERROR<<"RtDatabaseClient::DownloadFile response returned with status code:"<<status_code;
            TPS_PRINTF_DEBUG("RtDatabaseClient::DownloadFile response returned with status code:%d",status_code );
            return status_code;
        }

        std::string header;
        std::vector<std::string> headers;        
        while (std::getline(response_stream, header) && header != "\r")
            headers.push_back(header);

        boost::system::error_code error;     
        std::string headerLine;
        std::stringstream ss;
        std::vector<int> vDataSize;
        std::stringstream sResponseData;
        while (boost::asio::read(socket, response,boost::asio::transfer_at_least(1), error))
        {   
            sResponseData << &response;
        }

        std::string ss1 = ss.str();

        std::string sResData("");
        size_t ulSize = sResponseData.str().size();
        //std::cout<<"ulSize size:"<<ulSize<<std::endl;

        std::string sRealData;
        if (ulSize < 1)
        {
            TPS_LOG_DEV_ERROR<<"ulsize is 0";
            iRet = - 2;
            return iRet;
        }

        std::string htmlContent = sResponseData.str();
        //GetRealData(htmlContent,sRealData);

        /* this is a way to parse a xml file
        char* pData = new char[ulSize + 1];
        size_t real_size = 0;
        memset(pData, 0, ulSize + 1);
        std::string sstemp;
        GetRealData(htmlContent,sstemp, pData, &real_size);

        std::string xml(pData);
        std::string fp_start("<FilePath>");
        std::string fp_end("</FilePath>");
        size_t found_start = xml.find(fp_start);
        size_t found_end = xml.find(fp_end);
        if (found_start != std::string::npos &&
            found_end != std::string::npos)
        {
            std::string  sfilepath = xml.substr(found_start + fp_start.length(), found_end - found_start - fp_start.length());
            *filepath = sfilepath;
            iRet = 0;
        }
        delete[] pData;
        */

        std::string fp_start("\"FilePath\":\"");
        std::string fp_end("\",\"FileType\":");
        size_t found_start = htmlContent.find(fp_start);
        size_t found_end = htmlContent.find(fp_end);
        if (found_start != std::string::npos &&
            found_end != std::string::npos)
        {
            std::string  sfilepath = htmlContent.substr(found_start + fp_start.length(), found_end - found_start - fp_start.length());
            *filepath = sfilepath;
            iRet = 0;
        }

        if (error != boost::asio::error::eof)
        {
            //    reponse_data = error.message(); 
            TPS_LOG_DEV_ERROR<<"DownloadFile boost::asio::error::eof:"<<error.message();
            return -3;
        }

        if ((*filepath).empty())
        {
            TPS_LOG_DEV_ERROR<<"file path is empty";
            iRet = -4;
        }
        
    }
    catch(std::exception& e)
    {
        //  reponse_data = e.what();
        TPS_LOG_DEV_ERROR<<"RtDatabaseClient::UploadFile exception:"<<e.what();
        std::cout<< "RtDatabaseClient::UploadFile exception:"<<e.what() <<"\n";
        iRet = -4;
    }    
    TEST_PERFORMANCE_END("RtDatabaseClient::UploadFile");
    return iRet;
}

RT_TPS_DATABASE_END_NAMESPACE;
