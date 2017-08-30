////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_active_response_interface.h
/// 
///  \brief class IActiveResponser declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/12
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_ACTIVE_RESPONSER_INTERFACE_H_
#define TPS_FW_ACTIVE_RESPONSER_INTERFACE_H_

#include "tps_fw_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class IActiveResponseObject
/// 
/// \brief *****
class IActiveResponser 
{
public:
    //the constructor and destructor
    IActiveResponser() : mHandlePtr(nullptr) {}
    virtual ~IActiveResponser() {}

    virtual void SetHandle(void* handle) = 0;
protected:
    virtual void FreeHandle_i() = 0;
    void* mHandlePtr;
};

class ISimpleGuiResponser : public IActiveResponser
{
public:
    virtual void Response(const char* pInfo, int size) = 0;
};

//voi active responser interface
struct CertifiedVOI;
class VOIActiveResponser : public IActiveResponser
{
public:
    virtual void Response(const CertifiedVOI& certVoi) = 0;
};

//redo-undo active responser interface
class RedoUndoActiveResponser : public IActiveResponser
{
public:
    virtual void Response(unsigned int undoSize, unsigned int redoSize) = 0;
};

//window update active responser interface
class WindowActiveResponser : public IActiveResponser
{
public:
    virtual void Response(int width, int height, void* buffer) = 0;
};

//optimizer active responser interface
namespace proto
{
    class RT_TPS_OptOutput;
}
class OptActiveResponser : public IActiveResponser
{
public:
    virtual void Response(const proto::RT_TPS_OptOutput& optOutput) = 0;
};

//beam active responser interface
class RtBeam;
class BeamActiveResponser : public IActiveResponser
{
public:
    virtual void Response(RtBeam* beam) = 0;
};

//progress active responser interface
class ProgressActiveResponser : public IActiveResponser
{
public:
    virtual void Response(double value, bool isToCancel, const std::string& message) = 0;
};

//prepare active responser interface
class PrepareActiveResponser : public IActiveResponser
{
public:
    virtual void Response(int totalFileNumber,int errortype) = 0;
};
TPS_END_NAMESPACE  // end namespace tps

#endif