#include "StdAfx.h"
//#include "RtTpsRenderLibrary/tps_rl_contoursgraphicobjectcollection.h"
//
//TPS_BEGIN_NAMESPACE  // begin namespace tps
//
//ContoursGraphicObjectCollection::ContoursGraphicObjectCollection()
//{
//
//}
//
//ContoursGraphicObjectCollection::~ContoursGraphicObjectCollection(){
//    mContoursGoCollection.clear();
//}
//
//void ContoursGraphicObjectCollection::AddContoursGo(TpsContoursGraphicObjectPtr go) {
//    mContoursGoCollection.push_back(go);
//}
//void ContoursGraphicObjectCollection::RemoveContoursGo(const std::string& uid) {
//    for (auto iter = mContoursGoCollection.begin(); iter != mContoursGoCollection.end(); ++iter)
//    {
//        if((*iter)->GetUid() == uid)
//        {
//            mContoursGoCollection.erase(iter);
//            break;
//        }
//    }
//}
//TpsContoursGraphicObjectPtr ContoursGraphicObjectCollection::GetContoursGo(const std::string& uid) {
//    for (auto iter = mContoursGoCollection.begin(); iter != mContoursGoCollection.end(); ++iter)
//    {
//        if((*iter)->GetUid() == uid)
//        {
//            return (*iter);
//        }
//    }
//    return nullptr;
//}
//
//std::vector<TpsContoursGraphicObjectPtr> ContoursGraphicObjectCollection::GetAllContoursGo() {
//    return mContoursGoCollection;
//}
//
//void ContoursGraphicObjectCollection::SetConotursGoVisible(const std::string& uid) {
//    for (auto iter = mContoursGoCollection.begin(); iter != mContoursGoCollection.end(); ++iter)
//    {
//        if((*iter)->GetUid() == uid)
//        {
//            (*iter)->SetIsVisible(true);
//        }
//        else
//        {
//            (*iter)->SetIsVisible(false);
//        }
//    }
//}
//
//
//
//TPS_END_NAMESPACE  // end namespace tps