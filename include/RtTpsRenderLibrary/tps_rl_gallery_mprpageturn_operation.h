//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_mprpageturn_operation.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_MPRPAGETURN_OPERATION_H_
#define TPS_RL_GALLERY_MPRPAGETURN_OPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"

namespace Mcsf{
    namespace MedViewer3D{
        class ClsGraphicObjectMPR;
        class GraphicObjVolume;
    }
}

//namespace RTFWK{
//    class RTFWKDLDOImage3D;
//}


TPS_BEGIN_NAMESPACE
//class TpsImage3DEntity;
//class TpsTextOverlayGraphicObject;
//
///// \class TpsGalleryMPRPageTurnOperation
///// 
///// \brief this operation calls ModelWarehouse notify to fire the rendering
//class TPS_RL_DECLSPEC TpsGalleryMPRPageTurnOperation
//    : public OperationBase{
//
//        typedef Mcsf::MedViewer3D::ClsGraphicObjectMPR  McsfMprGo;
//        typedef Mcsf::MedViewer3D::GraphicObjVolume     McsfVolumeGo;
//        typedef std::shared_ptr<McsfMprGo>              McsfMprGoPtr;
//        typedef std::shared_ptr<McsfVolumeGo>           McsfVolumeGoPtr;
//
//public:
//    //the constructor and destructor
//    TpsGalleryMPRPageTurnOperation(int turnStep);
//    ~TpsGalleryMPRPageTurnOperation();
//
//    /// \brief Set GraphicObject keys
//    /// \param[in] sVolumeKey       volume
//    /// \param[in] sMPRKey          MPR
//    /// \return void 
//    void SetGraphicObjectKeys(const std::string& sVolumeKey, 
//        const std::string& sMPRKey,
//        const std::string &mprCornerInfoKey,
//        const std::string& sMeasureKey,
//        const std::string& sTextKey);
//
//    virtual bool ModifyGraphicObject() const;
//
//
//
//private:
//    bool ConvertSlicePosAndThicknessToImageText_i(std::string sliceposValKey, std::string slicetotalValKey, std::string thicknessValKey, 
//        double slicepos, size_t slicetotal, double thickness,
//        std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const;
//
//    void UpdatePosInPatientImageText_i(const std::string& posKey, double mousePosInPat, 
//        std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const;
//
//private:
//    int                                             mTurnStep;
//    std::string                                     mVolumeKey;
//    std::string                                     mMPRKey;
//    std::string                                     mMprCornerInfoKey;
//    std::string                                     mMeasureKey;
//    std::string                                     mTextKey;
//    std::shared_ptr<TpsTextOverlayGraphicObject>    mCornerTextInfoGraphicObject;
//
//private:
//    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMPRPageTurnOperation);
//};

class TPS_RL_DECLSPEC TpsGalleryMPRPageTurnOperation : public OperationBase
{
public:
    //the constructor and destructor
    TpsGalleryMPRPageTurnOperation(
        int turnStep, const std::string& strImageUID, 
        FIRST_POSTFIX_COMPONENT ucSectionType, 
        //SECOND_POSTFIX_COMPONENT ucSectionExtendType = EMPTY_COMPONENT, 
        bool bIsCyclic = false);

    ~TpsGalleryMPRPageTurnOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string              mImageUID;
    FIRST_POSTFIX_COMPONENT  mSectionType;
    //SECOND_POSTFIX_COMPONENT mSectionExtendType;
    int                      mTurnStep;
    bool                     mIsCyclic;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMPRPageTurnOperation);
};

TPS_END_NAMESPACE

#endif
