////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_progress.h
/// 
///  \brief class TpsProgress declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/30
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_PROGRESS_H_
#define TPS_FW_PROGRESS_H_

#include "tps_fw_defs.h"
#include "McsfUtility/mcsf_progress_interface.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class ProgressActiveResponser;

/// \class TpsProgress
/// 
/// \brief 
class TPS_FW_DECLSPEC TpsProgress : Mcsf::IProgress{
public:
    //the constructor and destructor
    TpsProgress();
    virtual ~TpsProgress();

    /// \brief get the current message
    /// 
    /// \return current message
    virtual const std::string &Message(void) const;

    /// \brief set the current message
    /// 
    /// \param[in] ksMsg message to be set
    virtual void SetMessage(const std::string &);

    /// \brief get current progress
    /// 
    /// \return current progress
    virtual double Current(void) const;

    /// \brief set current progress
    /// 
    /// \param[in] dValue progress to be set.
    virtual void SetCurrent(double);

    /// \brief add progress on current progress
    /// 
    /// \param[in] dIncrement value to be added.
    virtual void AddCurrent(double);

    /// \brief  Get the flag which indicates whether the calculation should 
    ///         be canceled right now.
    /// 
    /// \return     true, the calculation should be canceled now.
    ///             false, the calculation should not be canceled right now.
    virtual bool GetCancelFlag(void) const;

    /// \brief  Set the flag which indicates whether the calculation should 
    ///         be canceled right now.
    /// 
    /// \param[in]  bCancelFlag   true, the calculation should be canceled now.
    ///             false, the calculation should not be canceled right now.
    void SetCancelFlag(bool bCancelFlag);

    /// \brief set weight of current process
    void SetProgressWeight(double weight);

    /// \brief set start value of current process
    void SetProgressStartValue(double value);

    /// \brief reset the start value & weight
    void Reset();

    // \brief set progress responser
    void SetProgressResponser(const std::shared_ptr<ProgressActiveResponser>& responser);
private:
    void OnProgressChanged_i() const;

    double              mProgess;
    std::string         mMessage;
    bool                mNeedCancel;
    double              mWeight;
    double              mStartVal;

    std::shared_ptr<ProgressActiveResponser> mProgressResponser;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsProgress);
};

TPS_END_NAMESPACE  // end namespace tps

#endif