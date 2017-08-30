//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_fw_beamstructure.h
///  \brief  beam and beamCollection structure for beam template
///  \
///  class  
///  \version 1.0
///  \date  2014/11/19
///  \
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_FW_BEAMSTRUCTURE_H_
#define TPS_FW_BEAMSTRUCTURE_H_

//TPS
#include "tps_fw_defs.h"
#include "tps_fw_common_enums.h"
#include <vector>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>


TPS_BEGIN_NAMESPACE

class TPS_FW_DECLSPEC SimpleBeam {
public :

    SimpleBeam();
    SimpleBeam(float gantrySA, float arcLength, bool isClockwise, float CouchA, float colliA);
    SimpleBeam(const SimpleBeam& beam);
    
    //void GetSimpleBeamFromTemplate(const proto::RT_TPS_SimpleBeam& smpl);

    friend class boost::serialization::access;
    template<class Archive>
    void serialize( Archive &ar,const unsigned int version) {
        version;
        ar & mGantryStartAngle;
        ar & mArcLength;
        ar & mIsClockwise;
        ar & mCollimatorAngle;
        ar & mCouchAngle;
    }

public:
    float mGantryStartAngle;
    float mArcLength;
    bool  mIsClockwise;
    float mCollimatorAngle;
    float mCouchAngle;

};


class TPS_FW_DECLSPEC BeamTemplate{
public:
    BeamTemplate();
    BeamTemplate(std::string name, std::string description);


    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar,const unsigned int version) {
        version;
        ar & mTemplateUID;
        ar & mTemplateName;
        ar & mTemplateDescription;
        ar & mBeamCollection;
    }

public:
    std::string mTemplateUID;
    std::string mTemplateName;
    std::string mTemplateDescription;
    std::vector<SimpleBeam> mBeamCollection;
};

struct TPS_FW_DECLSPEC BeamConfig{
public:
    std::string mPlanUID;
    std::string mNormGroupUID;
    std::string mISOCenterUID;
    std::string mSeriesUID;

    BeamConfig(){}
    BeamConfig(std::string planUid, std::string normUid, std::string poiUid, std::string serUid)
    : mPlanUID(planUid), mNormGroupUID(normUid), mISOCenterUID(poiUid), mSeriesUID(serUid){

    }

};

TPS_END_NAMESPACE
#endif