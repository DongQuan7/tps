#ifndef TPS_RU_DEFS_H_
#define TPS_RU_DEFS_H_

#include "tps_defs.h"
//
TPS_BEGIN_NAMESPACE

#ifdef TPS_RU_EXPORT
#define TPS_RU_DECLSPEC    TPS_EXPORT_CLS
#else
#define TPS_RU_DECLSPEC    TPS_IMPORT_CLS
#endif


TPS_END_NAMESPACE

#endif //TPS_RU_DEFS_H_
