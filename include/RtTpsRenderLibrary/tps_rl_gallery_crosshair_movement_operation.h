//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_igallerycmdhandler.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 11, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_CROSSHAIR_MOVEMENT_OPERATION_H_
#define TPS_RL_GALLERY_CROSSHAIR_MOVEMENT_OPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_RL_DECLSPEC TpsGalleryCrosshairMovementOperation : public OperationBase{

public:
    //the constructor and destructor
    TpsGalleryCrosshairMovementOperation(const std::string& strImageUID, 
        FIRST_POSTFIX_COMPONENT ucSectionType, double xStart, double yStart, double xEnd, double yEnd, int hitMode,int crossMode);
    virtual ~TpsGalleryCrosshairMovementOperation();

    virtual bool ModifyGraphicObject() const;

    static bool GetIntersectionsOfVerticeandMpr(
        size_t* dim, const Mcsf::MedViewer3D::Point3D* pVertices,
        std::vector<Mcsf::MedViewer3D::Point3D>& intersections);

private:
    std::string     m_strImageUID;
    FIRST_POSTFIX_COMPONENT     m_ucSectionType;
    double          m_dXStart;
    double          m_dYStart;
    double          m_dXEnd;
    double          m_dYEnd;
    int             m_hitMode;
    int             m_crossMode;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryCrosshairMovementOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_CROSSHAIR_MOVEMENT_OPERATION_H_
