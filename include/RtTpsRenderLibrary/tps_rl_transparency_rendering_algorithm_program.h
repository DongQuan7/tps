////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_transparency_rendering_algorithm_program.h
/// 
///  \brief class TRALGProgram declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/03/01
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_TRANSPARENCY_RENDERING_ALGORITHM_PROGRAM_H_
#define TPS_RL_TRANSPARENCY_RENDERING_ALGORITHM_PROGRAM_H_

#include "tps_rl_defs.h"
#include <vector>
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TRALGProgram : public GLProgram
{
public:
    TRALGProgram();
    virtual ~TRALGProgram();

    void AttachShadingShaders(
        const std::vector<const char*>& vss, 
        const std::vector<const char*>& fss);

    TPS_DISALLOW_COPY_AND_ASSIGN(TRALGProgram);
};

TPS_END_NAMESPACE
#endif // TPS_RL_TRANSPARENCY_RENDERING_ALGORITHM_PROGRAM_H_
