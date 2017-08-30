//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_rl_beamtemplategraphicobject.h 
///  \brief  
///  \
///  class  TpsBeamTemplateGraphicObject
///  \version 1.0
///  \date  2014/11/21
///  \
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_BEAMTEMPLATE_GRAPHICOBJECT_H
#define TPS_RL_BEAMTEMPLATE_GRAPHICOBJECT_H

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsAlgorithmProxy/tps_pl_collimator.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"
#include <vector>

TPS_BEGIN_NAMESPACE

//typedef enum{
//    APERTURE,
//    MASK,
//}BUFF_TYPE;
//
//typedef enum{
//    MLC_IDLE,
//    MLC_SELECTING,
//    MLC_DRAGGING,
//    MLC_FINISHED
//}SELECTION_MODE;

struct tagMLC{
    const static int              MAX_LEAF_NUM = 60;
    plunc::LEAF_POSITION_STC      m_pLeaf[MAX_LEAF_NUM];
    int                             m_Leaf_num;

    int                             m_nSelectedLeaf;
    int                             m_nSelectedRightEdge;   // 0: left edge is selected, 1: right edge

    //SELECTION_MODE                 m_nSelectionMode;       // 0: idle 1: selecting 2: dragging 3: finished
    double                          m_dPickX;               // mouse position to pick the MLC (screen cs)
    double                          m_dPickY;
    double                          m_dTargetX;             // the target mouse position when dragging to edit MLC (screen cs)
    double                          m_dTargetY;

    float                           m_fColorR;
    float                           m_fColorG;
    float                           m_fColorB;
    float                           m_fSelectedColorR;
    float                           m_fSelectedColorG;
    float                           m_fSelectedColorB;
};

class TpsSimpleBeamGraphicObject;

class TPS_RL_DECLSPEC TpsBeamTemplateGraphicObject : public TpsGraphicObjectBase{
public:
    TpsBeamTemplateGraphicObject();
    virtual ~TpsBeamTemplateGraphicObject();

    ///////////////////////////////////////////////////////////////////////////////////////////
    //\brief virtual methods derived from GraphicObiBase;
    virtual bool Initialize();
    virtual bool Finalize();
    virtual void SetDirty(bool bFlag = false);

    ///////////////////////////////////////////////////////////////////////////////////////////
    //void AddBeamGo(const std::string& suid,std::shared_ptr<TpsSimpleBeamGraphicObject> beamgo);
    void AddBeamGo(std::shared_ptr<TpsSimpleBeamGraphicObject> simpleBeamGo);
    void RemoveAll();
    //std::shared_ptr<TpsSimpleBeamGraphicObject> SelectBeam(const std::string &);
    //void SetSelectedBeam(std::string beamUid);
    void GetBeamGoCollection(std::vector<std::shared_ptr<TpsSimpleBeamGraphicObject>>&) const;
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    void InitJawPosition();
    void InitLeafPosition();
    bool GetDefaultSegmentOutLine(std::vector<Viewer3D::Point3D> *ptList);


private:
    ///////////////////////////////////////////////////////////////////////////////////////////
    tagMLC                                      mtagMLC;
    float                                       mJawPosition[4];
    std::vector<Viewer3D::Point3D>           mSegmentOutLine;

    //std::map<std::string,std::shared_ptr<TpsSimpleBeamGraphicObject>> mBeamGoMap;
    std::vector<std::shared_ptr<TpsSimpleBeamGraphicObject>> mSimpleBeamGoCollection;
    std::shared_ptr<TpsSimpleBeamGraphicObject> mSelectedBeamGo;

    TPS_DISALLOW_COPY_AND_ASSIGN(TpsBeamTemplateGraphicObject);
};
TPS_END_NAMESPACE

#endif
