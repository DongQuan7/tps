////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_voieditingcommandbase.h
/// 
///  \brief class TpsVOIEditingRedoUndoCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/21
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_VOIEDITINGREDOUNDOCMDBASE_H_
#define TPS_FW_VOIEDITINGREDOUNDOCMDBASE_H_

#include "tps_fw_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include <memory>
#include "RtTpsFramework/tps_fw_active_responser_interface.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsVOIEditingCache;

/// \class TpsVOIEditingRedoUndoCmd
/// 
/// \brief *****
class TPS_FW_DECLSPEC TpsVOIRedoUndoCmdBase : public TpsRedoUndoCommand {
public:
    //the constructor and destructor
    TpsVOIRedoUndoCmdBase(const std::string& uid) : mVoiUid(uid), mResponser(nullptr){}
    virtual ~TpsVOIRedoUndoCmdBase() {}

    inline std::string GetVoiUid() const { return mVoiUid; }
    inline void SetResponser(const std::shared_ptr<VOIActiveResponser>& responser) 
    { mResponser = responser; }
protected:
    std::string                         mVoiUid;
    std::shared_ptr<VOIActiveResponser> mResponser;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsVOIRedoUndoCmdBase);
};

class TPS_FW_DECLSPEC TpsVOIRedoUndoCacheCmd : public TpsVOIRedoUndoCmdBase {
public:
    //the constructor and destructor
    TpsVOIRedoUndoCacheCmd(const std::string& uid);
    virtual ~TpsVOIRedoUndoCacheCmd();

    void SetPreState(std::shared_ptr<TpsVOIEditingCache> state);
    void SetPostState(std::shared_ptr<TpsVOIEditingCache> state);
    std::shared_ptr<TpsVOIEditingCache> GetPreState() const;
    std::shared_ptr<TpsVOIEditingCache> GetPostState() const;
    
    virtual bool Ready() const;
protected:
    virtual int PreExecute();
    virtual int PostExecute();

    std::shared_ptr<TpsVOIEditingCache> mPreState;
    std::shared_ptr<TpsVOIEditingCache> mPostState;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsVOIRedoUndoCacheCmd);
};
TPS_END_NAMESPACE  // end namespace tps

#endif