#ifndef tps_rl_3d_window_pan_zoom_rotate_operation_h__
#define tps_rl_3d_window_pan_zoom_rotate_operation_h__

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE
/// \class _3dWindowPanOperation
class TPS_RL_DECLSPEC _3dWindowPanOperation
    : public OperationBase{
public:
    //the constructor and destructor
    /// \brief constructor
    /// \param[in] startx   start x position in display cache cs
    /// \param[in] starty   start y position in display cache cs
    /// \param[in] stopx    stop x position in display cache cs
    /// \param[in] stopy    stop y position in display cache cs
    _3dWindowPanOperation(const std::string& imageUid, DISPLAY_SIZE displaySize,
        float startx, float starty, float stopx, float stopy);
    ~_3dWindowPanOperation();

    virtual bool ModifyGraphicObject() const;

private:
    float           mStartX;
    float           mStartY;
    float           mStopX;
    float           mStopY;
    DISPLAY_SIZE    mDisplaySize;
    std::string     mImageUid;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(_3dWindowPanOperation);
};

TPS_END_NAMESPACE
#endif // tps_rl_3d_window_pan_zoom_rotate_operation_h__
