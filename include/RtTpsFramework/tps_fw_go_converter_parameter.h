#ifndef TPS_FW_GO_CONVERTER_PARAMETER_H_
#define TPS_FW_GO_CONVERTER_PARAMETER_H_

#include "tps_fw_defs.h"
#include <map>

TPS_BEGIN_NAMESPACE  // begin namespace tps

class ModelWarehouse;
class ITpsDataRepository;
class TpsGraphicObjectBase;
typedef std::shared_ptr<TpsGraphicObjectBase> GOPtr;
typedef std::map<DATA_SOURCE_ID, DATA_SOURCE_UID> DataSourceMap;
typedef std::map<GO_TYPE, GOPtr> GOList;

struct GOConverterParameter
{
    DataSourceMap data_source_map;
    int section;
    std::string obj_uid;
    //to be extended
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_FW_GO_CONVERTER_PARAMETER_H_