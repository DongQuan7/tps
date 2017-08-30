


#ifndef TPS_RC_MPRRENDERCONTROLLER_AXIAL_H_
#define TPS_RC_MPRRENDERCONTROLLER_AXIAL_H_

#include "tps_rc_common_mpr_render_controller.h"

TPS_BEGIN_NAMESPACE
class TpsSetupPtCrossHairStrategy;
class TPS_RC_DECLSPEC TpsMprRenderControllerAxial :
    public TpsCommonMPRRenderController {
public:
    TpsMprRenderControllerAxial(WINDOW_TYPE type);
    virtual ~TpsMprRenderControllerAxial();

protected:

    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image)
    // \return  bool  true if succeeded
    // false if no data source has been set or other error occurs
    virtual bool SetupScene_i();

    virtual bool ResetImage_i();

    /// clear graphic objects from ModelWarehouse
    virtual void ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    virtual void RemoveDataSource_i(DATA_SOURCE_ID id);
private:
    std::shared_ptr<TpsSetupPtCrossHairStrategy>     mSetupCrosshairStrategy;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsMprRenderControllerAxial);
};

TPS_END_NAMESPACE



#endif