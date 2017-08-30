//////////////////////////////////////////////////////////////////
/////  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
/////  All rights reserved.
///// 
/////  \author  GONG Maoliang maoliang.gong@united-imaging.com
///// 
/////  \file tps_rl_voigraphicobejctcollectionconverter.cpp
///// 
/////  \brief class TpsVOIGOCollectionConverter implementation 
///// 
/////  \version 1.0
///// 
/////  \date    2014/01/20
//////////////////////////////////////////////////////////////////
//
#include "StdAfx.h"
//#include "RtTpsRenderLibrary/tps_rl_contoursgocollectionconverter.h"
//
//#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
//#include "RtTpsFramework/tps_fw_image3dentity.h"
//#include "RtTpsDataAccess/tps_da_idatarepository.h"
//#include "RtTpsRenderLibrary/tps_rl_contoursgraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_contoursgraphicobjectcollection.h"
//#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
//#include "RtTpsDataAccess/tps_da_tablemanager.h"
//#include "RtTpsFramework/tps_fw_modelwarehouse.h"
//#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
//#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
//
//using namespace Mcsf::MedViewer3D;
//TPS_BEGIN_NAMESPACE   // begin namespace tps
//
//TpsContoursGOCollectionConverter::TpsContoursGOCollectionConverter(
//    GO_TYPE goType)
//    : TpsGOConverterBase(goType){
//
//}
//
//TpsContoursGOCollectionConverter::~TpsContoursGOCollectionConverter(){
//
//}
//
//void TpsContoursGOCollectionConverter::SetModelWarehouse(ModelWarehouse* modelWarehouse){
//    mModelWarehouse = modelWarehouse;
//}
//
//void TpsContoursGOCollectionConverter::SetDataRepository(ITpsDataRepository* dataRepository){
//    mDataRepository = dataRepository;
//}
//
//GraphicObjectPtr TpsContoursGOCollectionConverter::Convert(const DataSourceMap& dataSourceMap)
//{
//    auto it = dataSourceMap.find(IMAGE);
//    if (it == dataSourceMap.end()) return nullptr;
//    return TpsGOConverterBase::Convert(it->second);
//}
//
//bool TpsContoursGOCollectionConverter::Update(const DataSourceMap& dataSourceMap, GraphicObjectPtr go)
//{
//    auto it = dataSourceMap.find(IMAGE);
//    if (it == dataSourceMap.end()) return false;
//    return TpsGOConverterBase::Update(it->second, go);
//}
//
//GraphicObjectPtr TpsContoursGOCollectionConverter::CreateGraphicObject_i(){
//    return std::shared_ptr<ContoursGraphicObjectCollection>(new ContoursGraphicObjectCollection());
//}
//
//bool TpsContoursGOCollectionConverter::PrepareSourceData_i(const std::string& uid){
//    using namespace Mcsf::MedViewer3D;
//    auto go = mModelWarehouse->GetModelObject(
//        uid + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME));
//    mVolume = std::dynamic_pointer_cast<GraphicObjVolume>(go);
//    if (mVolume == nullptr){
//        TPS_LOG_DEV_ERROR<<"The volume GO of image 3d: "<<uid<<" does not exist.";
//        return false;
//    }
//    return true;
//}
//
//bool TpsContoursGOCollectionConverter::Update_i(GraphicObjectPtr go){
//    auto contoursGOCollection = std::dynamic_pointer_cast<ContoursGraphicObjectCollection>(go);
//    std::map<std::string, TPSTable*> tableMap;
//    mDataRepository->GetTableManager()->GetAllTables(tableMap);
//    Mcsf::MedViewer3D::Point3D point;
//    std::vector<Mcsf::MedViewer3D::Point3D> points;
//    for (auto iter = tableMap.begin(); iter != tableMap.end(); iter++)
//    {
//        auto contoursGo = contoursGOCollection->GetContoursGo(iter->first);
//        if(contoursGo == nullptr)
//        {
//            contoursGo = std::shared_ptr<ContoursGraphicObject>(new ContoursGraphicObject());
//            contoursGo->SetUid(iter->first);
//        }
//        size_t pDim[3];
//        mVolume->GetDimension(pDim);
//        Matrix4x4 matrixp2w = mVolume->GetWorld2Patient().Inverse();
//		size_t dim[3];
//		mVolume->GetDimension(dim);
//		// ZK: 勾勒时Table的Z值映射到加载序列时Table的Z值
//		Matrix4x4 modelToVolume = Mcsf::MedViewer3D::Matrix4x4(dim[0], 0, 0, 0, 
//			0, dim[1], 0, 0, 0, 0, dim[2], 0, -0.5, -0.5, -0.5, 1);
//
//		Matrix4x4 matvolume2world = mVolume->GetModel2World() * modelToVolume.Inverse();
//        contoursGo->InitializeContoursGo(pDim[0], pDim[1], iter->second->GetSliceNum());
//        std::map<int, std::vector<RtTablecontour*> > tableContoursMap = iter->second->GetRtTableContourListMap();
//        for(auto tciter = tableContoursMap.begin(); tciter != tableContoursMap.end(); ++tciter)
//        {
//            for (auto rtiter = tciter->second.begin(); rtiter != tciter->second.end(); ++rtiter)
//            {
//                auto contour = (*rtiter)->get_points();
//                points.clear();
//                for (auto citer = contour.begin(); citer != contour.end(); ++citer)
//                {
//                    point.x = citer->x;
//                    point.y = citer->y;
//                    point.z = citer->z;
//                    point = matrixp2w.Transform(point);
//					point.z = matvolume2world.Transform(Point3D(0,0,tciter->first)).z;
//                    points.push_back(point);
//                }
//                contoursGo->AddContour(points, tciter->first);
//            }
//        }
//        contoursGOCollection->AddContoursGo(contoursGo);
//    }
//    return true;
//}
//
//TPS_END_NAMESPACE  // end namespace tps