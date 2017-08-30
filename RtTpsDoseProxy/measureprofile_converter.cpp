//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file beam_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "measureprofile_converter.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_measuredprofile.h"
#include "alg\RtAlgDataLayer\rt_alg_dl_object_measuredprofile.h"
#include "boost\lexical_cast.hpp"

TPS_BEGIN_NAMESPACE



bool MeasureProfileConverter::ConvertToALG(const RtMeasuredprofile* source, std::shared_ptr<RTFWK::RTFWKDLDOMeasuredProfile>& target)
{
    target.reset(new RTFWK::RTFWKDLDOMeasuredProfile(false));
    target->SetCommissionedUnitUID(source->get_commissionedunituid());
    target->SetWedgeAngle(source->get_wedgeangle());
    target->SetViewStatus(source->get_viewstatus());
    target->SetViewDisplay(source->get_viewdisplay());
    target->SetUID(source->get_uid());
    target->SetAccessoryUID(source->get_accessoryuid());
    target->SetOffsetY(source->get_offsety());
    target->SetOffsetX(source->get_offsetx());
    target->SetDepth(source->get_depth());
    target->SetCurveType((RTFWK::CURVETYPE)source->get_curvetype());
    
    //RTFWK::FieldSize fieldSize =  target->GetFieldSize();
    RTFWK::FieldSize tempField;
    tempField.m_fFieldSizeX = source->get_fieldsizex();
    tempField.m_fFieldSizeY = source->get_fieldsizey();
    target->SetFieldSize(tempField);
    target->SetFieldType((RTFWK::FIELDTYPE)source->get_fieldtype());


    //Measurepointdoselist;
    std::string strMeaPointDoseList = source->get_meapointdoselist();
    if (strMeaPointDoseList != ""){
        std::vector<std::string> vecValue;
        (void)boost::split(vecValue, strMeaPointDoseList, boost::is_any_of("|"));
        int ilen = static_cast<int>(vecValue.size());

        std::vector<RTFWK::PointDose*> vMeaPointDoseList;
        for (int i = 0; i < ilen; i++){
            std::vector<std::string> strValue;
            (void)boost::split(strValue, vecValue[i], boost::is_any_of(","));

            RTFWK::PointDose* pointDose = new RTFWK::PointDose();
            if (strValue.size() == 4)
            {
                pointDose->m_fDose = static_cast<float>(atof(strValue[0].c_str()));
                pointDose->m_fX = static_cast<float>(atof(strValue[1].c_str()));
                pointDose->m_fY = static_cast<float>(atof(strValue[2].c_str()));
                pointDose->m_fZ = static_cast<float>(atof(strValue[3].c_str()));
            }
            vMeaPointDoseList.push_back(pointDose);
        }
        target->SetMeaPointDoseList(vMeaPointDoseList);
        DEL_PTRVECTOR(vMeaPointDoseList);
    }

    //calpointdoselist;
    std::string strCalPointDoseList = source->get_calcpointdoselist();
    if (strCalPointDoseList != ""){
        std::vector<std::string> vecValue;
        (void)boost::split(vecValue, strCalPointDoseList, boost::is_any_of("|"));
        int ilen = static_cast<int>(vecValue.size());

        std::vector<RTFWK::PointDose*> vCalPointDoseList;
        for (int i = 0; i < ilen; i++){
            std::vector<std::string> strValue;
            (void)boost::split(strValue, vecValue[i], boost::is_any_of(","));

            RTFWK::PointDose* pointDose = new RTFWK::PointDose();
            if (strValue.size() == 4)
            {
                pointDose->m_fDose = static_cast<float>(atof(strValue[0].c_str()));
                pointDose->m_fX = static_cast<float>(atof(strValue[1].c_str()));
                pointDose->m_fY = static_cast<float>(atof(strValue[2].c_str()));
                pointDose->m_fZ = static_cast<float>(atof(strValue[3].c_str()));
            }

            vCalPointDoseList.push_back(pointDose);
        }
        target->SetCalcPointDoseList(vCalPointDoseList);
        DEL_PTRVECTOR(vCalPointDoseList);
    }

    //curveAnalyse
    std::string strCurveResultList = source->get_curveresultanalyselist();
    if (strCurveResultList != "")
    {
        std::vector<std::string> vecValue;
        (void)boost::split(vecValue, strCurveResultList, boost::is_any_of("|"));
        int ilen = static_cast<int>(vecValue.size());

        std::vector<RTFWK::RTFWKDLDOCurveResultAnalyse*> vCurveResultList;
        for (int i = 0; i < ilen; i++){
            std::vector<std::string> strValue;
            (void)boost::split(strValue, vecValue[i], boost::is_any_of(","));

            RTFWK::RTFWKDLDOCurveResultAnalyse* curveResult = new RTFWK::RTFWKDLDOCurveResultAnalyse();
            if (strValue.size() == 5)
            {
                curveResult->m_sName = strValue[0];
                curveResult->m_fAverageError = static_cast<float>(atof(strValue[1].c_str()));
				curveResult->m_fMaxError = static_cast<float>(atof(strValue[2].c_str()));
                curveResult->m_fErrorLimit = static_cast<float>(atof(strValue[3].c_str()));
                curveResult->m_fFailurePoits = static_cast<float>(atof(strValue[4].c_str()));
            }

            vCurveResultList.push_back(curveResult);
        }
        target->SetCurveResultAnalyse(vCurveResultList);
        DEL_PTRVECTOR(vCurveResultList);
    }
    return true;
}

bool MeasureProfileConverter::ConvertToALGList(std::vector<RtMeasuredprofile*> source, std::vector<shared_ptr<RTFWK::RTFWKDLDOMeasuredProfile>>& target)
{
    target.clear();
    for (int i = 0; i < source.size(); i++){
        std::shared_ptr<RTFWK::RTFWKDLDOMeasuredProfile> algMeasureProfile;
        ConvertToALG(source[i], algMeasureProfile);
        target.push_back(algMeasureProfile);
    }
    return true;
}

bool MeasureProfileConverter::ConverBackFromALGList(std::vector<RTFWK::RTFWKDLDOMeasuredProfile*> source, std::vector<shared_ptr<RtMeasuredprofile>>&  target)
{
    for (int i = 0; i < source.size(); i++){
        shared_ptr<RtMeasuredprofile> measureprofile;
        ConverBackFromALG(source[i], measureprofile);
        target.push_back(measureprofile);
    }
    return true;
}

bool MeasureProfileConverter::ConverBackFromALG(const RTFWK::RTFWKDLDOMeasuredProfile* source, shared_ptr<RtMeasuredprofile>& target)
{
    target.reset(new RtMeasuredprofile());
    target->set_commissionedunituid(source->GetCommissionedUnitUID());
    target->set_wedgeangle(source->GetWedgeAngle());
    target->set_viewstatus(source->GetViewStatus());
    target->set_viewdisplay(source->GetViewDisplay());
    target->set_uid(source->GetUID());
    target->set_accessoryuid(source->GetAccessoryUID());
    target->set_offsetx(source->GetOffsetX());
    target->set_offsety(source->GetOffsetY());
    target->set_curvetype((int)source->GetCurveType());
    target->set_depth(source->GetDepth());
    RTFWK::FieldSize tempField;
    tempField = source->GetFieldSize();

    target->set_fieldsizex(tempField.m_fFieldSizeX);
    target->set_fieldsizey(tempField.m_fFieldSizeY);
    target->set_fieldtype((int)source->GetFieldType());

    std::vector<PointDose*> vMeaPointDoseList, vCalPointDoseList;
    std::vector<RTFWK::PointDose*> tempMeaPointDoseList = source->GetMeaPointDoseList();
    std::vector<RTFWK::PointDose*> tempCalPointDoseList = source->GetCalcPointDoseList();
    std::vector<RTFWK::RTFWKDLDOCurveResultAnalyse*> tempCurveResultList = source->GetCurveResultAnalyse();

    std::string str;
    for (auto it = tempMeaPointDoseList.begin(); it != tempMeaPointDoseList.end(); ++it){
        if (str != ""){
            str = str + "|";
        }
        str = str + boost::lexical_cast<std::string>((*it)->m_fDose) + ",";
        str = str + boost::lexical_cast<std::string>((*it)->m_fX) + ",";
        str = str + boost::lexical_cast<std::string>((*it)->m_fY) + ",";
        str = str + boost::lexical_cast<std::string>((*it)->m_fZ);
    }
    target->set_meapointdoselist(str);

    str = "";
    for (auto it = tempCalPointDoseList.begin(); it != tempCalPointDoseList.end(); ++it){
        if (str != ""){
            str = str + "|";
        }
        str = str + boost::lexical_cast<std::string>((*it)->m_fDose) + ",";
        str = str + boost::lexical_cast<std::string>((*it)->m_fX) + ",";
        str = str + boost::lexical_cast<std::string>((*it)->m_fY) + ",";
        str = str + boost::lexical_cast<std::string>((*it)->m_fZ);
    }
    target->set_calcpointdoselist(str);


    str = "";
    for (auto it = tempCurveResultList.begin(); it != tempCurveResultList.end(); ++it){
        if (str != ""){
            str = str + "|";
        }
        str = str + boost::lexical_cast<std::string>((*it)->m_sName) + ",";
		str = str + boost::lexical_cast<std::string>((*it)->m_fAverageError)+ ",";
        str = str + boost::lexical_cast<std::string>((*it)->m_fMaxError) + ",";
        str = str + boost::lexical_cast<std::string>((*it)->m_fErrorLimit) + ",";
		str = str + boost::lexical_cast<std::string>((*it)->m_fFailurePoits);
    }
    target->set_curveresultanalyselist(str);
    return true;
}

TPS_END_NAMESPACE