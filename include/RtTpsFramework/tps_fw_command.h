//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_command.h
///  \brief   define command of internal for outside
///
///  \version 1.0
///  \date    Oct. 9, 2013
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_COMMAND_H
#define TPS_COMMAND_H

#include "tps_fw_defs.h"
#include "tps_fw_error.h"

TPS_BEGIN_NAMESPACE

class ITpsDataRepository;
class TpsRenderProxy;
class IRtDatabaseWrapper;
class TpsSaveObjects;
class TpsLoadObjects;
class TpsDoseProxy;
class DoseCalculationImpl;

class TPS_FW_DECLSPEC TpsCommand
{
public:
    TpsCommand();
    virtual ~TpsCommand(void);

    virtual int RunCommand();

    virtual void SetDataRepository(ITpsDataRepository* dataRepos) {mDataRepository=dataRepos;}
    virtual void SetRenderProxy(TpsRenderProxy* proxy) {mRenderProxy = proxy;}
    virtual void SetDatabaseWrapper(IRtDatabaseWrapper* databaseWrapper){ mDatabaseWrapper = databaseWrapper;}

    virtual void SetDoseProxy(TpsDoseProxy* doseProxy) { mDoseProxy = doseProxy; }

    virtual void SetSaveObjects(TpsSaveObjects* saveObjects){ mSaveObjects = saveObjects; }
    virtual void SetLoadObjects(TpsLoadObjects* loadObjects){ mLoadObjects = loadObjects; }
    virtual void SetDoseCalculator(DoseCalculationImpl* doseCalculator) {mDoseCalculator = doseCalculator;}
    virtual TpsError& GetLastError(){ return mLastError; }
protected:
    virtual int PreExecute();
    virtual int Execute();
    virtual int PostExecute();

    ITpsDataRepository*         mDataRepository;
    IRtDatabaseWrapper*         mDatabaseWrapper;
    TpsSaveObjects*             mSaveObjects;
    TpsLoadObjects*             mLoadObjects;
    TpsRenderProxy*             mRenderProxy;
    DoseCalculationImpl*        mDoseCalculator;
    TpsError                    mLastError;
    TpsDoseProxy*               mDoseProxy;
};

class TPS_FW_DECLSPEC TpsRedoUndoCommand : public TpsCommand
{
public:
    TpsRedoUndoCommand();
    virtual ~TpsRedoUndoCommand(void);

    virtual int Undo();
    virtual int Redo();
    virtual bool Ready() const;

protected:
    virtual int PostExecute();
    virtual TpsRedoUndoCommand* Clone() = 0;
};
TPS_END_NAMESPACE

#endif //TPS_COMMAND_H
