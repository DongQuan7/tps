#ifndef TPS_DA_CONTROLPOINTSLIST_H_
#define TPS_DA_CONTROLPOINTSLIST_H_

#include "tps_da_controlpoints.h"
#include <list>
TPS_BEGIN_NAMESPACE  // begin namespace tps


    TPS_DA_DECLSPEC class TpsControlPointsList
{
public:
    //the constructor and destructor
    TpsControlPointsList();
    ~TpsControlPointsList();

    TpsControlPoints GetControlPoints();

private:
    List<TpsControlPoints *> tpsControlPointsList;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsControlPointsList);
};

TPS_END_NAMESPACE  // end namespace tps

#endif