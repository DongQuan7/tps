////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_da_displaystatemanager.h
/// 
///  \brief class DisplayStateManager declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/01/21
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_DISPLAYSTATEMANAGER_H_
#define TPS_DA_DISPLAYSTATEMANAGER_H_

#include "tps_da_defs.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class DisplayStateManager
/// 
/// \brief this is a temporary data manager for display state. 
///        the data is not from database, it's all secondary or temporary.
class TPS_DA_DECLSPEC TpsDisplayStateManager : public TpsDataManagerBase 
{
public:
    TpsDisplayStateManager();
    ~TpsDisplayStateManager();

    void SetBevViewType(BEV_VIEW_TYPE type);
    BEV_VIEW_TYPE GetBevViewType() const;

	void SetBevDisplayType(BEV_DISPLAY_TYPE type);
	BEV_DISPLAY_TYPE GetBevDisplayType() const;

	void SetDoselineRenderModeType(DOSELINE_RENDERMODE type);
	DOSELINE_RENDERMODE GetDoselineRenderModeType() const;

    void SetBevEditTarget(BEV_EDIT_TARGET target);
    BEV_EDIT_TARGET GetBevEditTarget() const;

    bool GetMoveVoiFlag() const;
    void SetMoveVoiFlag(bool flag);

	void SetDoselineTransparency(float type);
	float GetDoselineTransparency() const;

	void SetDoselineTransparency3D(float type);
	float GetDoselineTransparency3D() const;

	void SetMprSection(int flag);
	int GetMprSection()const;

	void SetMPRTransparency3D(float type);
	float GetMPRTransparency3D() const;

private:
    BEV_VIEW_TYPE                mBEVViewType;
	BEV_DISPLAY_TYPE             mBEVDisplayType;
    BEV_EDIT_TARGET              mBevEditTarget;
    bool                         mMoveVOIChanged;
	DOSELINE_RENDERMODE          mDoselineRenderMode;
	float                        mDoselineTransparency;
	float                        mDoselineTransparency3D;
	int                          mMprSection;
	float                        mMPRTransparency3D;
};

TPS_END_NAMESPACE  // end namespace tps

#endif
