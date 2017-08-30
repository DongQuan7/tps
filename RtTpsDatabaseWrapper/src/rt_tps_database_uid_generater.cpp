//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_uid_generater.cpp
///  \brief     UidGenerater
///
///  \version 1.0
///  \date    Aug 11,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"

#include <sstream> 
#include <iostream> 
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/thread/thread.hpp"
#ifdef _WIN32
#include <winsock2.h> 
#include <iphlpapi.h> 
#include <process.h>
#pragma comment(lib, "IPHLPAPI.lib")
#else
#include <net/if.h>  
#include <sys/ioctl.h>
#include <unistd.h>
#endif

RT_TPS_DATABASE_BEGIN_NAMESPACE;

const std::string csUIHOID = "1.2.156.112605";
static std::string g_sLastTimestamp = "";
static int g_iTimestampNo = 1;

const std::string TrimLeftZero(const std::string sStr) 
{
    std::string sRetStr = sStr;
    while ((1 < sRetStr.length()) && ('0' == sRetStr.at(0))) {
        sRetStr = sRetStr.substr(1, (sRetStr.length()-1));
    }
    return sRetStr;
}

static void GetMacPid(std::string& sMac, std::string&sPid)
{
    //Get MAC Address
    sMac = "0";
    sPid = "0";
#ifdef _WIN32  // For Windows
    std::vector<boost::uint8_t> buf; 
    DWORD bufLen = 0; 
    GetAdaptersAddresses(0, 0, 0, 0, &bufLen); 
    if (bufLen) { 
        buf.resize(bufLen, 0); 
        IP_ADAPTER_ADDRESSES * ptr = 
            reinterpret_cast<IP_ADAPTER_ADDRESSES*>(&buf[0]); 
        DWORD err = GetAdaptersAddresses(0, 0, 0, ptr, &bufLen); 
        if (err == NO_ERROR) { 
            if (nullptr != ptr) { 
                //the MAC address format should be FF-FF-FF-FF-FF-FF
                if (6 == ptr->PhysicalAddressLength) { 
                    std::vector<boost::uint8_t> macbuf;
                    macbuf.resize(ptr->PhysicalAddressLength, 0);
                    memcpy(&macbuf[0], ptr->PhysicalAddress, ptr->PhysicalAddressLength);
                    unsigned long long ulMacAddr = 0;
                    for (int i=0; i<6; ++i) {
                        ulMacAddr <<= 8;
                        ulMacAddr |= (unsigned long long)(macbuf.at(i));
                    }
                    sMac = boost::lexical_cast<std::string>(ulMacAddr);
                } 
            } 
        } 
    } 

    sPid = boost::lexical_cast<std::string>(_getpid());
#else  // For Linux
    register int fd = 0;
    register int intrface = 0;
    const int iMaxInterface = 8;
    struct ifconf ifc;
    struct ifreq buf[iMaxInterface];

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)(&buf[0]);
        if (!ioctl(fd, SIOCGIFCONF, (char *)(&ifc))) {
            intrface = ifc.ifc_len / sizeof (struct ifreq);
            for (int iIntCunt = 0; iIntCunt < intrface; iIntCunt++) {
                /*Get HW ADDRESS of the net card */
                if (!(ioctl(fd, SIOCGIFHWADDR, (char*)(&buf[iIntCunt])))) {
                    if (strncmp(buf[iIntCunt].ifr_name, "eth", 3)) {
                        continue;
                    }

                    //std::cout << "ifr_name:" << buf[iIntCunt].ifr_name <<endl;
                    unsigned long ulMacAddr = 0;
                    for (int i=0; i<6; ++i) {
                        ulMacAddr <<= 8;
                        ulMacAddr |= (unsigned char)(buf[iIntCunt].ifr_hwaddr.sa_data[i]);
                    }

                    sMac = boost::lexical_cast<std::string>(ulMacAddr);

                    if (ulMacAddr > 0) {
                        break;
                    }
                }
            }
        } 
        close (fd);
    }

    sPid = boost::lexical_cast<std::string>(getpid());
#endif
    if (0 == sMac.length()) {
        sMac = "0";
    }

    if (0 == sPid.length()) {
        sPid = "0";
    }
}

std::string RtUidGenerater::GeneraterUid(void)
{
    std::string sUID = "";
    std::string sPID = "0";
    std::string sTimestamp = "0";
    std::string sCount = "1";
    std::string sType = "0";
    std::string sSuffix = "";

    try {
        //******* Part 1: OID *******
        sUID += csUIHOID;

        //******* Part 2: Mac *******
        std::string sMac, sPid;
        GetMacPid(sMac, sPid);
        sUID += "." + sMac;

        //******* Part 3: Timestamp *******
        const boost::posix_time::ptime ptimeUTCTime = boost::posix_time::microsec_clock::universal_time();
        const boost::gregorian::date dtDateInstance = ptimeUTCTime.date();
        const boost::posix_time::time_duration dtTimeInstance = ptimeUTCTime.time_of_day();

        // DA(Date) format: YYYYMMDD
        std::string sDate = boost::gregorian::to_iso_string(dtDateInstance);
        // TM(Time) format: HHMMSS.FFFFFF
        std::string sTime = boost::posix_time::to_iso_string(dtTimeInstance);
        std::string sTsHHMMSS = "000000";
        if (sTime.length() >= 6) {
            sTsHHMMSS = sTime.substr(0,6);
        }
        // sTimestamp format: YYYYMMDDHHMMSS
        sTimestamp = sDate + sTsHHMMSS;
        sUID += "." + sTimestamp;

        //******* Part 4: Type *******
            sType = "0";
        sUID += "." + sType;

        //******* Part 5: Process ID *******
        sPID = sPid;
        if (sPID.length() > 5) {
            sPID = TrimLeftZero(sPID.substr((sPID.length()-5), 5));
        }
        sUID += "." + sPID;

        //******* Part 6: Count *******
        // Add No for same timestamp situation
        if (sTimestamp == g_sLastTimestamp) {
            g_iTimestampNo++;
        }
        else {
            g_sLastTimestamp = sTimestamp;
            g_iTimestampNo = 1;
        }
        if (g_iTimestampNo > 99999) {
            g_iTimestampNo = 1;
        }
        sCount = boost::lexical_cast<std::string>(g_iTimestampNo);
        sUID += "." + sCount;

        if (!sSuffix.empty() 
            && (sSuffix != "0")
            && (sSuffix.length() <= 4)) {
            sUID += "." + sSuffix;
        }

        //validity check (UID length<=64, contain only number and ".")
        if (sUID.length() <= 64) {
            for (int i=0; i<sUID.size(); ++i) {
                if ((!std::isdigit(sUID[i])) && ('.' != sUID[i])) {
                    TPS_LOG_DEV_ERROR<<"UID is invalid while UID="<<sUID;
                    sUID = "";
                    break;
                }
            }
        }
        else {
            TPS_LOG_DEV_ERROR<<("UID length is too length while UID=" + sUID + "!");
            sUID = "";
        }
    }
    catch (const std::exception &e) {
        TPS_LOG_DEV_ERROR<<("UIDManager created empty UID as " + std::string(e.what()) +  " while UID=" + sUID + "!");
        sUID = "";
    }
    if (sUID.empty()) {
        TPS_LOG_DEV_ERROR<<("UIDManager created empty UID!");
    }    
    return sUID;
}

RT_TPS_DATABASE_END_NAMESPACE;
