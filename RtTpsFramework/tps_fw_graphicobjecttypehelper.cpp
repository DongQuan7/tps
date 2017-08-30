////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_graphicobjecttypehelper.cpp
/// 
///  \brief class GOTypeHelper implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/09/03
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

GO_TYPE GOTypeHelper::ComposeAsGOType(MAIN_COMPONENT main, 
    FIRST_POSTFIX_COMPONENT firstPostfix/* = EMPTY_COMPONENT*/, 
    SECOND_POSTFIX_COMPONENT secondPostfix/* = EMPTY_COMPONENT*/){

    GO_TYPE type = EMPTY_COMPONENT;
    type |= (main << 16);
    type |= (firstPostfix << 8);
    type |= secondPostfix;
    return type;
}

MAIN_COMPONENT GOTypeHelper::GetMainComponent(GO_TYPE goType){
    MAIN_COMPONENT main = EMPTY_COMPONENT;
    main |= (goType >> 16);
    return main;
}

FIRST_POSTFIX_COMPONENT GOTypeHelper::GetFirstPostfixComponent(GO_TYPE goType){
    FIRST_POSTFIX_COMPONENT firstPostfix = EMPTY_COMPONENT;
    firstPostfix |= (goType >> 8);
    return firstPostfix;
}

SECOND_POSTFIX_COMPONENT GOTypeHelper::GetSecondPostfixComponent(GO_TYPE goType){
    SECOND_POSTFIX_COMPONENT secondPostfix = EMPTY_COMPONENT;
    secondPostfix |= goType;
    return secondPostfix;
}

std::string GOTypeHelper::ToString(GO_TYPE goType){

    char type[8];
    sprintf_s(type, "%x", goType);
    //_itoa_s(goType, type, 16);
    return std::string(type);
}

GO_TYPE GOTypeHelper::ToGoType(const std::string& strType){

    GO_TYPE goType = EMPTY_COMPONENT;
    sscanf_s(strType.c_str(), "%x", &goType);
    return goType;
}
TPS_END_NAMESPACE  // end namespace tps