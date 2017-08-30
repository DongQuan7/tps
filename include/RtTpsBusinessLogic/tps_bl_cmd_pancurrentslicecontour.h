////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_cca_pan_slice_contour_cmd.h
/// 
///  \brief class PanSliceContourCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/09/16
////////////////////////////////////////////////////////////////

#ifndef TPS_CCA_PAN_SLICE_CONTOUR_CMD_H_
#define TPS_CCA_PAN_SLICE_CONTOUR_CMD_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedmovevoi.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

struct CertifiedVOI;
/// \class PanSliceContourCmd
/// 
/// \brief pan contour of specified voi in one slice
class TPS_BL_DECLSPEC PanSliceContourCmd : public TpsVOIRedoUndoCacheCmd{
public:
    //the constructor and destructor
    PanSliceContourCmd(const CertifiedMoveVOI &);
    virtual ~PanSliceContourCmd();

    virtual int Undo();
    virtual int Redo();
protected:
    virtual int Execute();
    virtual TpsRedoUndoCommand* Clone();
private:
    bool UpdateVOI_i(const std::string &voiUID);

private:
    CertifiedMoveVOI    mCertMoveVOI;
    CertifiedVOI*       mCertVOI;
    
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(PanSliceContourCmd);
};

TPS_END_NAMESPACE  // end namespace tps

#endif