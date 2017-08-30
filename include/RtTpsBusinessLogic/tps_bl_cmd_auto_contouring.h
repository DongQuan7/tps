//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file    tps_cca_autocontouring_cmd.h
///  \brief   Auto Contouring command
///
///  \version 1.0
///  \date    Jan. 8, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_AUTO_CONTOURING_H_
#define TPS_BL_CMD_AUTO_CONTOURING_H_

#include <vector>
#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

namespace Mcsf 
{
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE;
struct CertifiedAutoVois;
struct CertifiedVOI;
class TpsImage3DEntity;
class RtImage3DHeader;
class TpsRenderProxy;
class ProgressActiveResponser;
class TpsProgress;

static const RtDbDef::BiologyRole BIOLOGY_ROLE_ARRAY[] = 
{
    RtDbDef::BiologyRole_Skin                      , 
    RtDbDef::BiologyRole_Skin                      ,
    RtDbDef::BiologyRole_Lung                      ,
    RtDbDef::BiologyRole_Lung                      ,
    RtDbDef::BiologyRole_Lung                      ,
    RtDbDef::BiologyRole_Cord                      ,
    RtDbDef::BiologyRole_Heart                     ,
    RtDbDef::BiologyRole_Liver                     ,
    RtDbDef::BiologyRole_Spleen                    ,
    RtDbDef::BiologyRole_Kidney                     ,
    RtDbDef::BiologyRole_Kidney                     ,
    RtDbDef::BiologyRole_Kidney                     ,
    RtDbDef::BiologyRole_Parotid                   ,
    RtDbDef::BiologyRole_Parotid                   ,
    RtDbDef::BiologyRole_Parotid                   ,
    RtDbDef::BiologyRole_Brainstem                 ,
    RtDbDef::BiologyRole_TBD                       ,
    RtDbDef::BiologyRole_Eye                       ,
    RtDbDef::BiologyRole_Eye                       ,
    RtDbDef::BiologyRole_Eye                       ,
    RtDbDef::BiologyRole_Bone                      ,
    RtDbDef::BiologyRole_Brain                     ,
    RtDbDef::BiologyRole_Rectum                    ,
    RtDbDef::BiologyRole_Prostate                  ,
    RtDbDef::BiologyRole_Bone                      ,
    RtDbDef::BiologyRole_Pancreas                  ,//NOT delivered 
    RtDbDef::BiologyRole_Bladder                   ,//NOT delivered
    RtDbDef::BiologyRole_Unknown                    , //NOT delivered
    RtDbDef::BiologyRole_Unknown
};

static const RtDbDef::PairedType IS_PAIRED_ARRAY[] = 
{
    RtDbDef::PairedType_No                     , 
    RtDbDef::PairedType_No                     ,
    RtDbDef::PairedType_Yes                    , 
    RtDbDef::PairedType_Left                   ,
    RtDbDef::PairedType_Right                  ,
    RtDbDef::PairedType_No                     ,
    RtDbDef::PairedType_No                     ,
    RtDbDef::PairedType_No                     ,
    RtDbDef::PairedType_No                     ,
    RtDbDef::PairedType_Yes                    ,
    RtDbDef::PairedType_Left                   ,
    RtDbDef::PairedType_Right                  ,
    RtDbDef::PairedType_Yes                    ,
    RtDbDef::PairedType_Left                   ,
    RtDbDef::PairedType_Right                  ,
    RtDbDef::PairedType_No                     ,
    RtDbDef::PairedType_No                     ,
    RtDbDef::PairedType_Yes                    ,
    RtDbDef::PairedType_Left                   ,
    RtDbDef::PairedType_Right                  ,
    RtDbDef::PairedType_Yes                    ,
    RtDbDef::PairedType_No                     ,
    RtDbDef::PairedType_No                     ,
    RtDbDef::PairedType_No                     ,
    RtDbDef::PairedType_Yes                    ,
    RtDbDef::PairedType_No                     ,//NOT delivered 
    RtDbDef::PairedType_No                     ,//NOT delivered
    RtDbDef::PairedType_Unknown                ,//NOT delivered
    RtDbDef::PairedType_Unknown
};

class TPS_BL_DECLSPEC AutoCoutouringCmd : public TpsCommand
{
public:
    AutoCoutouringCmd(CertifiedAutoVois &);
    virtual ~AutoCoutouringCmd(void);
    
    static TPS_ORGAN_TYPE ConvertName2OrganTypeEnum(const std::string& strVal);
    void SetProgressResponser(const std::shared_ptr<ProgressActiveResponser>& responser);
private:
    bool ProcessOneROI_i(CertifiedVOI& certfiedVOI,
        TpsImage3DEntity* image3D, 
        const Mcsf::Matrix4f& matP2V);

    int DeleteVoi_i(const std::string& sVoiUid);

protected:
    virtual int Execute();

private:
    CertifiedAutoVois&                          mCertifiedAutoVois;
    TpsProgress*                                mProgress;

    std::shared_ptr<ProgressActiveResponser>    mProgressResponser;
};



TPS_END_NAMESPACE

#endif //TPS_CCA_AUTOCONTOURING_CMD_H_
