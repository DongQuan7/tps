//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_fw_voistructure.cpp
///  \brief  VOI and VOICollection structure for beam template
///  \
///  class  
///  \version 1.0
///  \date  2014/12/05
///  \
//////////////////////////////////////////////////////////////////////////

#include"StdAfx.h"
#include "RtTpsFramework/tps_fw_voistructure.h"

//TMS


TPS_BEGIN_NAMESPACE

SimpleVOI::SimpleVOI()
{

}

SimpleVOI::SimpleVOI(const std::string& name, int planningrole, int subrole)
    : mName(name), mPlanningRole(planningrole), mSubrole(subrole)
{

}

SimpleVOI::SimpleVOI(const SimpleVOI& voi){
    mName   = voi.mName;
    for(int i = 0; i < 4; ++i){
        mColor[i] = voi.mColor[i];
    }
    mPlanningRole       = voi.mPlanningRole;
    mSubrole          = voi.mSubrole;
}

VOITemplate::VOITemplate()
{

}

VOITemplate::VOITemplate(const std::string& name, const std::string& description) 
    : mTemplateName(name), mTemplateDescription(description)
{

}

TPS_END_NAMESPACE