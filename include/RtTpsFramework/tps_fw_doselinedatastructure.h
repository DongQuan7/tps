////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  liu linqi  linqi.liu@united-imaging.com
/// 
///  \file tps_fw_doselinedatastructure.h
/// 
///  \brief class DoseLineInfo declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/09/01
////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


#ifndef TPS_FW_DOSELINE_DATASTRUCTURE_H_
#define TPS_FW_DOSELINE_DATASTRUCTURE_H_


//TPS
#include "tps_fw_defs.h"
#include "tps_fw_common_enums.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>

TPS_BEGIN_NAMESPACE

 //TPS_DA_DECLSPEC
class TPS_FW_DECLSPEC DoseLineTemplateItem {
public :
    DoseLineTemplateItem();

    DoseLineTemplateItem(const DoseLineTemplateItem& doseLine);

    friend class boost::serialization::access;
    template<class Archive>
    void serialize( Archive &ar,const unsigned int version) {
        version;
        ar & mDosePercent;
        for(int i = 0 ; i < 4;i++) {
            ar & mColor[i];
        }
    }

    float GetDosePercent() const ;
    void SetDosePercent(const float& fDosePercent) ;

    const float* GetColor() const ;
    void SetColor(const float fColor[4]) ;

private:
    float mDosePercent;
    float mColor[4];
};


class TPS_FW_DECLSPEC DoseLineTemplate{
public:

    DoseLineTemplate();
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar,const unsigned int version) {
        version;
        ar & mDoseLineTemplateUID;
        ar & mDoseLineTemplateName;
        ar & mDoseLineTemplateDescription;
        ar & mDisplayMode;
        ar & mHundredPercentDefinition;
        ar & mSpecifiedDose;
        ar & mDoseLineTemplateItemList;
    }

    std::string GetDoseLineTemplateUID() const;
    void SetDoseLineTemplateUID(std::string groupUid);

    std::string GetDoseLineTemplateName() const;
    void SetDoseLineTemplateName(std::string groupName);

    std::string GetDoseLineTemplateDescription() const;
    void SetDoseLineTemplateDescription(std::string description);

    int GetDoseLineTemplateDisplayMode() const;
    void SetDoseLineTemplateDisplayMode(int displayMode);

    int GetDoseLineTemplateHundredPercentDefinition() const;
    void SetDoseLineTemplateHundredPercentDefinition(int hundredPercentDefinition);

    double GetDoseLineTemplateSpecifiedDose() const;
    void SetDoseLineTemplateSpecifiedDose(double specifiedDose);

    void AddDoseLineTemplateItem(DoseLineTemplateItem &doseLineInfo);
    void ClearDoseLineTemplateItemList();
    void GetDoseLineTemplateItemList(std::list<DoseLineTemplateItem>& doseLineList);

private :
    std::string mDoseLineTemplateUID;
    std::string mDoseLineTemplateName;
    std::string mDoseLineTemplateDescription;
    int mDisplayMode;
    int mHundredPercentDefinition;
    double mSpecifiedDose;
    std::list<DoseLineTemplateItem> mDoseLineTemplateItemList;
};

TPS_END_NAMESPACE
#endif  //TPS_FW_DOSELINE_DATASTRUCTURE_H_