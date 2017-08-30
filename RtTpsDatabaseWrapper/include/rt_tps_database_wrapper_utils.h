//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfDatabase of Common Software Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2012
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  qiangqiang.zhou@united-imaging.com
///
///  \file        mcsf_dbwrapper.utils.h
///  \brief   database wrapper utils 
///
///  \version 1.0
///  \date    June 25, 2012
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_WRAPPER_UTILS_H_
#define RT_TPS_DATABASE_WRAPPER_UTILS_H_

#include <string>
#include <vector>
#include <map>

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

class RtCt2density;
class RtSeries;
class RtImage;


class  RtDatabaseUtils
{
public:

    // constructor
    RtDatabaseUtils();
    // destructor
    ~RtDatabaseUtils(void);

    /////////////////////////////////////////////////////////////////
    ///  \brief            Delete File
    ///
    ///  \param[in]      const std::string& sFilePath
    ///
    ///  \param[out]    None
    ///  \return            None
    ///
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    bool DeleteLocalFile(const std::string& sFilePath) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief            Delete Files
    ///
    ///  \param[in]      const std::vector<std::string>& FilePathArray
    ///
    ///  \param[out]    None
    ///  \return            None
    ///
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    void DeleteFiles(const std::vector<std::string>& FilePathArray) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief             Create folders and subfolders    
    ///                         Using Boost FileSystem 
    /// 
    ///  \param[in]     const std::string& sFolderName    
    ///                         eg. string strFilePath="E:/images/";
    ///                    
    ///  \param[out]   
    ///  \return            int 
    ///                         0:success to create a Child Folder    
    ///                         -1:fail
    ///  \pre \e        
    /////////////////////////////////////////////////////////////////
    bool CreateMultiFolder(const std::string& sFolderName) const;

    /////////////////////////////////////////////////////////////////
    ///  \brief         ReadBufferFromFileDeleteDicomFiles
    ///                 Note: should delete buffer outside
    ///  \param[in]     const std::string& sFilePath
    ///  \param[out]    char* buffer, std::size_t* size
    ///  \return        bool
    ///
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    bool ReadBufferFromFile(const std::string& sFilePath, char*& buffer, std::size_t* size) const;
    //bool ReadBufferFromFile(const std::string& sFilePath, std::string& strBuffer) const;
    //bool ReadFloatBufferFromFile(const std::string& sFilePath, std::string SplitString, std::vector<float>& vFloatValue) const;
    //bool ReadIntBufferFromFile(const std::string& sFilePath, std::string SplitString, std::vector<int>& vIntValue) const;
    bool CopyFileInBinary(const std::string& sFilePathSrc, const std::string& sFilePathDst) const;

    //bool BuildCt2densityMapFromFile(RtCt2density* ct2densityMap);
    //bool WriteCt2densityMapToFile(const RtCt2density& ct2densityMap, const std::string& folderPath);

    bool BuildImage3DFromSeries(RtSeries* pRtSeries, std::vector<RtImage*> imageList, unsigned long ulSize);

    bool WriteFileInBinary(const std::string& sFileName, const char* buffer, std::size_t size);

    template <typename T> 
    bool ReadBufferFromString(std::string sSource, std::string SplitString, std::vector<T>& vValueList) const;

private:

    bool CheckImage3D(RtSeries* pRtSeries);

    //void SetupCt2DensityMap_i(const std::string& strLongString, RtCt2density* ct2densityMap);
    //float GetDensitybyCTval_i(int ct, const std::vector<int>& vCT, std::map<int, float>& ct2densMap);
    bool WriteFile(const std::string& sFileName, const std::string& sData);
    
};

template <typename T> 
bool RtDatabaseUtils::ReadBufferFromString(
    std::string sSource, 
    std::string SplitString, 
    std::vector<T>& vValueList) const
{
    std::vector<std::string> vecValue;
    (void)boost::split(vecValue, sSource, boost::is_any_of(SplitString));

    for (auto itr = vecValue.cbegin(); itr != vecValue.cend(); ++itr)
    {
        T value = boost::lexical_cast<T>((*itr).c_str());
            vValueList.push_back(value);
    }
    return true;
}

RT_TPS_DATABASE_END_NAMESPACE

#endif  //RT_TPS_DATABASE_WRAPPER_UTILS_H_
