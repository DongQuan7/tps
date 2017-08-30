////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_beamgocollection.h
/// 
///  \brief class TpsBeamGraphicObjectCollection declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/11
////////////////////////////////////////////////////////////////


#ifndef TPS_RL_BEAMGOCOLLECTION_H
#define TPS_RL_BEAMGOCOLLECTION_H
#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

namespace Viewer3D = Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE
class TpsBeamGraphicObject;
class TPS_RL_DECLSPEC TpsBeamGraphicObjectCollection : public TpsGraphicObjectBase {
public:
    //the constructor and destructor
    TpsBeamGraphicObjectCollection();
    virtual ~TpsBeamGraphicObjectCollection();

    //////////////////////////////////////////////////////////////////////////
    //\brief virtual methods derived from GraphicObiBase;
    virtual bool Initialize();
    virtual bool Finalize();

    //////////////////////////////////////////////////////////////////////////
    // \brief add a beam to go collection;
    // \param[in] suid  the uid of beam go;
    // \param[in] beamgo the beam go pointer;
    void AddBeam(const std::string& suid,std::shared_ptr<TpsBeamGraphicObject> beamgo);

    //////////////////////////////////////////////////////////////////////////
    // \brief delete a beam from go collection
    // \param[in] the uid of the beam to be deleted;
    bool DeleteBeam(const std::string &);

    //////////////////////////////////////////////////////////////////////////
    // \brief remove all the beam go from go collection
    void RemoveAll();

    //////////////////////////////////////////////////////////////////////////
    // \brief select a beam from go collection
    // \param[in] the uid of the beam to be selected;
    // \return the beam go pointer of the selected beam;
    std::shared_ptr<TpsBeamGraphicObject> SelectBeam(const std::string &) ;

    //////////////////////////////////////////////////////////////////////////
    // \brief get the all the beam go instance from go collection
    // \return the map of the beam go;
    std::map<std::string,std::shared_ptr<TpsBeamGraphicObject>> GetBeamMap() const;

    //////////////////////////////////////////////////////////////////////////
    // \ brief get active beam  for BEV;
    std::shared_ptr<TpsBeamGraphicObject> GetActiveBeamGo() const;

    virtual bool IsDirty() const;
    
    //////////////////////////////////////////////////////////////////////////
    // \brief set dirty flag
    virtual void SetDirty(bool bFlag);

    void SetActiveBeam(std::string beamUid);

    inline void SetIsJoined(bool isJoined) { mIsJoined = isJoined; }
    inline bool IsJoined() const { return mIsJoined; }

    inline void SetIsElectronNormalGroup(bool isEletronNG) { mIsElectronNG = isEletronNG; }
    inline bool IsElectronNormalGroup() const { return mIsElectronNG; }

    inline void SetIsApproved(bool isApproved) { mIsApproved = isApproved; }
    inline bool IsApproved() const { return mIsApproved; }

    TpsBeamGraphicObjectCollection& operator=(const TpsBeamGraphicObjectCollection& src)
    {
        this->mBeamGoMap = src.mBeamGoMap;
        return *this;
    }

    inline double GetSAD() const { return mSAD; }
    inline void SetSAD(double sad) { mSAD = sad; }
    const double* GetFieldLimits() const;
    void SetFieldLimits(const double* limits);
private:
    std::map<std::string,std::shared_ptr<TpsBeamGraphicObject>> mBeamGoMap;
    bool mIsJoined;
    bool mIsElectronNG;
    bool mIsApproved;
    
    std::string mActiveBeamUID;
    double mSAD;
    //low x, up x, low y, up y
    double mFieldLimits[4];

    //std::shared_ptr<TpsBeamGraphicObject>                       mActiveBeam;
    //TPS_DISALLOW_COPY_AND_ASSIGN(TpsBeamGraphicObjectCollection);
};
TPS_END_NAMESPACE


#endif
