////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_deleteoperation.h
/// 
///  \brief delete 
/// 
///  \version 1.0
/// 
///  \date    2014/3/07
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DELETEOPERATION_H_
#define TPS_RL_DELETEOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsDeleteVOIOperation
    : public OperationBase {
public:
    TpsDeleteVOIOperation(const std::string &imageUid, const std::string &voiUid);

    ~TpsDeleteVOIOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string   mVoiUid;
    std::string   mImageUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDeleteVOIOperation);
};

class TPS_RL_DECLSPEC TpsDeletePOIOperation
    : public OperationBase {
public:
    TpsDeletePOIOperation( const std::string& strSeriesUid, const std::string &strPoiUid);

    ~TpsDeletePOIOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string   m_strPoiUid;
    std::string   m_strSeriesUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDeletePOIOperation);
};

class TPS_RL_DECLSPEC TpsDeleteMaxPointOperation
    : public OperationBase {
public:
    TpsDeleteMaxPointOperation(const std::string& strPlanUid, const std::string& strPoiUid);

    ~TpsDeleteMaxPointOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string   m_strPoiUid;
    std::string   m_strPlanUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDeleteMaxPointOperation);
};

class TPS_RL_DECLSPEC TpsDeleteBeamOperation
    : public OperationBase {
public:
    TpsDeleteBeamOperation(const std::string &normGroupUid, const std::string &beamUid);

    ~TpsDeleteBeamOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string   mBeamUid;
    std::string   mNormGroupUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDeleteBeamOperation);
};

class TPS_RL_DECLSPEC TpsDeleteNormGroupOperation
    : public OperationBase {
public:
    TpsDeleteNormGroupOperation(const std::string &planUid, const std::string &ngUid);

    ~TpsDeleteNormGroupOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string   mPlanUid;
    std::string   mNormGroupUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDeleteNormGroupOperation);
};

TPS_END_NAMESPACE

#endif
