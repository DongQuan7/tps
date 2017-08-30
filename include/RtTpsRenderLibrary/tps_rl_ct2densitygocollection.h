////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  Zuo Kai  kai.zuo@united-imaging.com
/// 
///  \file tps_rl_ct2densitygocollection.h
/// 
///  \brief class ct2density go collection declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/03/08
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_CT2DENSITY_GRAPHIC_OBJECT_COLLECTION_H_
#define TPS_RL_CT2DENSITY_GRAPHIC_OBJECT_COLLECTION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsRenderLibrary/tps_rl_ct2density_graphicobject.h"
#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

    /// \class BlockGraphicObject
typedef std::shared_ptr<Ct2DensityGraphicObejct> Ct2DensityGraphicObejctPtr;;
/// 
/// \brief *****
class TPS_RL_DECLSPEC Ct2DensityGraphicObejctCollection : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    Ct2DensityGraphicObejctCollection();
    virtual ~Ct2DensityGraphicObejctCollection();

    void AddCt2DensityGo(Ct2DensityGraphicObejctPtr go);
    void RemoveCt2DensityGo(const std::string& ct2densityUid);
    Ct2DensityGraphicObejctPtr GetCt2DensityGo(const std::string& ct2densityUid);
private:
    std::vector<Ct2DensityGraphicObejctPtr>    mCt2DensityGoCollection;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(Ct2DensityGraphicObejctCollection);
};

TPS_END_NAMESPACE  // end namespace tps

#endif