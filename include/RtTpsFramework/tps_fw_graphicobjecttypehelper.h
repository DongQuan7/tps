////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_graphicobjecttypehelper.h
/// 
///  \brief class GOTypeHelper declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/09/03
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_GRAPHICOBJECTTYPEHELPER_H_
#define TPS_FW_GRAPHICOBJECTTYPEHELPER_H_

#include "RtTpsFramework/tps_fw_defs.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

enum GOTypeComponent{
    MAIN,
    FIRST_POSTFIX,
    SECOND_POSTFIX,
};

///******************Graphic object type defination***********************
///*Graphic object type is a unsigned integer number(32 bits) which has 
///*three components defined as GOTypeCompent enum. The "MAIN" component is 
///*a unsigned short number located in the high 16 bits of the whole type.
///*The other components are unsigned char(8 bits) located in the rest bits respectively.
///*The distribution of the type in memory is: MAIN-FIRST_POSTFIX-SECOND_POSTFIX.
class TPS_FW_DECLSPEC GOTypeHelper{
public:

    //compose graphic object type according specified three components
    static GO_TYPE ComposeAsGOType(MAIN_COMPONENT main, 
        FIRST_POSTFIX_COMPONENT firstPostfix = EMPTY_COMPONENT, 
        SECOND_POSTFIX_COMPONENT secondPostfix = EMPTY_COMPONENT);

    //get main component 
    static MAIN_COMPONENT GetMainComponent(GO_TYPE goType);

    //get first postfix component
    static FIRST_POSTFIX_COMPONENT GetFirstPostfixComponent(GO_TYPE goType);

    //get second postfix component
    static SECOND_POSTFIX_COMPONENT GetSecondPostfixComponent(GO_TYPE goType);

    //get go type string
    static std::string ToString(GO_TYPE goType);

    //convert back
    static GO_TYPE ToGoType(const std::string& strType);
};

TPS_END_NAMESPACE  // end namespace tps

#endif