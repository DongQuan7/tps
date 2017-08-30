//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_fw_voistructure.h
///  \brief  VOI and VOICollection structure for voi template
///  \
///  class  
///  \version 1.0
///  \date  2014/12/05
///  \
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_FW_VOI_STRUCTURE_H_
#define TPS_FW_VOI_STRUCTURE_H_

//TPS
#include "tps_fw_defs.h"
#include "tps_fw_common_enums.h"

//#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/serialization/string.hpp>
//#include <boost/serialization/base_object.hpp>
//#include <boost/serialization/utility.hpp>
//#include <boost/serialization/list.hpp>
//#include <boost/serialization/assume_abstract.hpp>
#include <vector>

namespace boost{
    namespace serialization{
        class access;
    }
}

TPS_BEGIN_NAMESPACE

class TPS_FW_DECLSPEC SimpleVOI{
public :

    SimpleVOI();
    SimpleVOI(const std::string& name, int planningrole, int subrole);
    SimpleVOI(const SimpleVOI& voi);
    
    friend class boost::serialization::access;
    template<class Archive>
    void serialize( Archive &ar,const unsigned int version){
        version;
        ar & mName;
        ar & mColor;
        ar & mPlanningRole;
        ar & mSubrole;
    }


public:
    std::string     mName;
    float           mColor[4];
    int             mPlanningRole;
    int             mSubrole;

};


class TPS_FW_DECLSPEC VOITemplate{
public:
    VOITemplate();
    VOITemplate(const std::string& name, const std::string& description);


    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar,const unsigned int version){
        version;
        ar & mTemplateUid;
        ar & mTemplateName;
        ar & mTemplateDescription;
        ar & mVOICollection;
    }

public:
    std::string mTemplateUid;
    std::string mTemplateName;
    std::string mTemplateDescription;
    std::vector<SimpleVOI> mVOICollection;
};

TPS_END_NAMESPACE
#endif