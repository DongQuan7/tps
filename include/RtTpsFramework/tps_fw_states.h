////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_da_states.h
/// 
///  \brief class TPS states declaration 
/// 
///  \version 1.0
/// 
///  \date    Sep.2,2014
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_STATES_H_
#define TPS_DA_STATES_H_

#include "tps_fw_defs.h"

//#include "boost/thread/mutex.hpp"

namespace boost
{
    class mutex;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps


/// \class TpsStates
/// 
/// \brief *****
class TPS_FW_DECLSPEC TpsStates{
public:
    //the constructor and destructor
    TpsStates();
    ~TpsStates(void);

    bool IsFluenceMapMode() const;
    void SetFluenceMapMode(bool flag);

    void Lock();
    void Unlock();

private:

    boost::mutex                *mMutex;
    bool                        mIsFluenceMapMode;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsStates);

};

TPS_END_NAMESPACE  // end namespace tps

#endif
