////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_editchessboardooperation.h
/// 
///  \brief class EditChessBoard declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/29
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EDITCHESSBOARDOOPERATION_H_
#define TPS_RL_EDITCHESSBOARDOOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class MPRGraphicObject;
class FusionChessGraphicObject;
class FusionGraphicObject;
typedef std::shared_ptr<FusionChessGraphicObject> ChessGoPtr;
typedef std::shared_ptr<MPRGraphicObject> MprGoPtr;
typedef std::shared_ptr<FusionGraphicObject> FusionGoPtr;

/// \class EditChessBoardOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC EditChessBoardOperation
 : public OperationBase{
public:
    //the constructor and destructor
    EditChessBoardOperation(double x, double y, int actionType, 
        DISPLAY_SIZE displaySize);
    virtual ~EditChessBoardOperation();

    virtual bool ModifyGraphicObject() const;

    inline void SetChessGOKey(const std::string& goKey) { mChessGoKey = goKey; }
    inline void SetFusionGOKey(const std::string& goKey) { mFusionGoKey = goKey; }
    inline void SetMprGOKey(const std::string& goKey) { mMprKey = goKey; }
private:
    HIT_TEST_NUM HitTest_i(FusionGoPtr fusionGo, ChessGoPtr chess, MprGoPtr mpr) const;
    void MouseOver_i(FusionGoPtr fusionGo, ChessGoPtr chess, MprGoPtr mpr) const;
    void BeginEdit_i(FusionGoPtr fusionGo, ChessGoPtr chess, MprGoPtr mpr) const;
    void EditChess_i(ChessGoPtr chess, MprGoPtr mpr) const;
    void EndEdit_i(ChessGoPtr chess) const;
private:
    double                  mX;
    double                  mY;
    int                     mMouseActionType;
    DISPLAY_SIZE            mDisplaySize;

    std::string             mChessGoKey;
    std::string             mFusionGoKey;
    std::string             mMprKey;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(EditChessBoardOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif