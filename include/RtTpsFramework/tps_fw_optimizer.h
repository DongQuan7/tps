//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_fw_optimizer.h
///  \brief  optimizer model and optimizer template information
///  \
///  class  
///  \version 1.0
///  \date  2015/2/2
///  \
//////////////////////////////////////////////////////////////////////////

#pragma once

//TPS
#include "tps_fw_defs.h"
#include "tps_fw_common_enums.h"
#include <vector>
#include "tps_fw_voistructure.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

TPS_BEGIN_NAMESPACE

class TPS_FW_DECLSPEC SimpleOptimizer{
public :

    friend class boost::serialization::access;
    template<class Archive>
    void serialize( Archive &ar,const unsigned int version) {
        version;
        ar &  mFuctionType;
        ar &  mMinMaxFlag;
        ar &  mDose;
        ar &  mVolume;
        ar &  mEud;
        ar &  mWeight;
        ar &  mIsHard;
        ar &  mTarget;
    }

public:
    int mFuctionType;
    int mMinMaxFlag;
    float mDose;
    float mVolume;
    float mEud;
    float mWeight;
    bool mIsHard;
    std::string mTarget;
};


class TPS_FW_DECLSPEC OptimizerTemplate{
public:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar,const unsigned int version) {
        version;
        ar & mTemplateUID;
        ar & mTemplateName;
        ar & mTemplateDescription;
        ar & mOptimizerCollection;
    }

public:
    std::string mTemplateUID;
    std::string mTemplateName;
    std::string mTemplateDescription;
    std::vector<SimpleOptimizer> mOptimizerCollection;
};

TPS_END_NAMESPACE
