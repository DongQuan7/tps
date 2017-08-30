////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  Liang Xiang  xiang.liang@united-imaging.com
/// 
///  \file tps_rl_drrgocollection.h
/// 
///  \brief class EasyPlanDrrGraphicObejctCollection declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/7/12
////////////////////////////////////////////////////////////////


#ifndef TPS_RL_DRRGOCOLLECTION_H
#define TPS_RL_DRRGOCOLLECTION_H

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

namespace Viewer3D = Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

class EasyPlanDrrGraphicObejct;
class TPS_RL_DECLSPEC EasyPlanDrrGraphicObjectCollection : public TpsGraphicObjectBase 
{
public:
    //the constructor and destructor
    EasyPlanDrrGraphicObjectCollection();
    virtual ~EasyPlanDrrGraphicObjectCollection();

    //////////////////////////////////////////////////////////////////////////
    //\brief virtual methods derived from GraphicObiBase;
    virtual bool Initialize();
    virtual bool Finalize();

    //////////////////////////////////////////////////////////////////////////
    // \brief add a drr to go collection;
    // \param[in] suid  the uid of drr go;
    // \param[in] drrgo the drr go pointer;
    void AddDrr(const std::string& suid,std::shared_ptr<EasyPlanDrrGraphicObejct> drrgo);

    //////////////////////////////////////////////////////////////////////////
    // \brief delete a drr from go collection
    // \param[in] the uid of the drr to be deleted;
    bool DeleteDrr(const std::string &);

    //////////////////////////////////////////////////////////////////////////
    // \brief remove all the drr go from go collection
    void RemoveAll();

    //////////////////////////////////////////////////////////////////////////
    // \brief get a drr from go collection
    // \param[in] the uid of the drr to be selected;
    // \return the drr go pointer of the selected drr;
    std::shared_ptr<EasyPlanDrrGraphicObejct> GetDrr(const std::string &) ;

    //////////////////////////////////////////////////////////////////////////
    // \brief get the all the drr go instance from go collection
    // \return the map of the drr go;
    std::map<std::string,std::shared_ptr<EasyPlanDrrGraphicObejct>> GetDrrMap() const;

    virtual bool IsDirty() const;

    //////////////////////////////////////////////////////////////////////////
    // \brief set dirty flag
    virtual void SetDirty(bool bFlag);

private:
    std::map<std::string,std::shared_ptr<EasyPlanDrrGraphicObejct>> mDrrGoMap;

};
TPS_END_NAMESPACE

#endif