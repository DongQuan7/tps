#ifndef TPS_RL_EASYPLANDRRGRAPHICOBJECT_H_
#define TPS_RL_EASYPLANDRRGRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework\tps_fw_common_enums.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
//#include "RtTpsRenderLibrary/tps_rl_drrrenderstrategy.h"
#include "RtTpsFramework\tps_fw_graphicobjectbase.h"
#include "Mcsf3DArithmetic\mcsf_3d_matrix3x3.h"
namespace Viewer3D = Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC EasyPlanDrrGraphicObejct : public TpsGraphicObjectBase {
public:
    EasyPlanDrrGraphicObejct(void);
    virtual ~EasyPlanDrrGraphicObejct(void);
    virtual bool Initialize();
    virtual bool Finalize();
    void SetImageBuffer(char* buffer,int width,int height,
        unsigned long bufferLen,DATATYPE dataType);
    char *GetImageBuffer(){return mImage;};
    unsigned long GetImageBufferLength(){return mBufferLen;};
    int GetImageWidth(){return mWidth;};
    int GetImageHight(){return mHeight;};
    DATATYPE GetDataType(){return mDataType;};
    /////////////////////////////////////////////////////////////////
    ///  \brief SetDrrModel2ImageModel
    ///
    ///  \param[in]  Mcsf::Matrix3f & matrix3fModel2W
    ///              
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    bool SetDrrModel2ImageModel(const Mcsf::Matrix3f & drrModel2ImageModel);
    /////////////////////////////////////////////////////////////////
    ///  \brief GetDrrModel2ImageModel
    ///
    ///  \param[in]  
    ///              
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    bool  GetDrrModel2ImageModel(Mcsf::Matrix3f* drrModel2ImageModel);
    /////////////////////////////////////////////////////////////////
    ///  \brief SetInitialDrrModel2ImageModel
    ///
    ///  \param[in]  Mcsf::Matrix3f & matrix3fModel2W
    ///              
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    bool SetInitialDrrModel2ImageModel(const Mcsf::Matrix3f & drrModel2ImageModel);
    /////////////////////////////////////////////////////////////////
    ///  \brief GetInitialDrrModel2ImageModel
    ///
    ///  \param[in]  Mcsf::Matrix3f & matrix3fModel2W
    ///              
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    bool GetInitialDrrModel2ImageModel(Mcsf::Matrix3f* drrModel2ImageModel);
    /////////////////////////////////////////////////////////////////
    ///  \brief ResetDrrModel2ImageModel
    ///
    ///  \param[in]  Mcsf::Matrix3f & matrix3fModel2W
    ///              
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    bool ResetDrrModel2ImageModel();
    bool SetWindowingWinWidthAndWinCenter(int winWidth, int winCenter);
    bool GetWindowingWinWidthAndWinCenter(int* winWidth,int* winCenter);
    void SetZoomFactor(float zoomFactor);
    float GetZoomFactor();
    bool GetIsInversed();
    void SetIsInversed(bool isInversed);
    bool GetIsOpposedImage();
    void SetIsOpposedImage(bool isOpposedImage);
    bool SetMlcTransformMatrix(const Mcsf::Matrix3f & transformMatrix);
	bool GetMlcTransformMatrix(Mcsf::Matrix3f* transformMatrix);
	void ResetMlcTransformMatrix();
    void SetImageCaptureTableAngle(double imageCaptureTableAngle);
    double GetImageCaptureTableAngle();
    void SetImageTableAngle(double tableAngle);
    double GetImageTableAngle();
    void SetIsocenterPosInImageModel(const Mcsf::Point3f &isocenterPosInImageModel);
    Mcsf::Point3f GetIsocenterPosInImageModel();
    void SetBeamUID(const std::string beamUID);
    const std::string GetBeamUID();
private:
    char* mImage;
    int mWidth;
    int mHeight;
    unsigned long mBufferLen;
    DATATYPE mDataType;
    int mWinWidth;
    int mWinCenter;
    Mcsf::Matrix3f mDrrModel2ImageModelMatrix3f;
    Mcsf::Matrix3f mInitialDrrModel2ImageModelMatrix3f;
	Mcsf::Matrix3f mTransformMatrix3f;
    float mZoomFactor;
    bool mIsInversed;
    bool mIsOpposedImage;
    double mImageCaptureTableAngle;
    double mCurrentImageTableAngle;
    Mcsf::Point3f mIsocenterPosInImageModel;
    std::string mBeamUID;
    //Viewer3D::Vector3D mTranslation;
};
TPS_END_NAMESPACE  // end namespace tps

#endif
