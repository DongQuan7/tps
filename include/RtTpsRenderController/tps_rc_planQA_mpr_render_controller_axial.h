


#ifndef TPS_RC_PLANQA_MPRRENDERCONTROLLER_AXIAL_H_
#define TPS_RC_PLANQA_MPRRENDERCONTROLLER_AXIAL_H_

#include "tps_rc_planQA_mpr_render_controller.h"

TPS_BEGIN_NAMESPACE
class TpsSetupPtCrossHairStrategy;
class TPS_RC_DECLSPEC TpsPlanQAMprRenderControllerAxial :
    public TpsPlanQAMPRRenderController {
public:
    TpsPlanQAMprRenderControllerAxial(WINDOW_TYPE type);
    virtual ~TpsPlanQAMprRenderControllerAxial();

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
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsPlanQAMprRenderControllerAxial);
};

TPS_END_NAMESPACE



#endif