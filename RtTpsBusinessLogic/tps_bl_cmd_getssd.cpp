#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_getssd.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
//ZHENGHE
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfMedViewer3DArithmetic/vector3d.h"

#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_mask.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"

#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsROIUtility/tps_ru_roi_shape_converter.h"
#include "RtTpsFramework/tps_fw_arithmetic.h"
#include "RtTpsFramework/tps_fw_contour.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

GetSSDCmd::GetSSDCmd(const std::string& beamUid, const std::string& isocenterUid, const std::string& skinUid, bool isBeamSSD) :
mBeamUid(beamUid), mIsocenterUid(isocenterUid), mSkinUid(skinUid), mIsBeamSSD(isBeamSSD)
{

}

GetSSDCmd::~GetSSDCmd()
{

}

//目前SSD暂时不存数据库，load上来全部算一下。InterfaceRequrement44844暂时不实现。
//需要问一下张一戈SSD是否要保存在导出的Dicom文件中
int GetSSDCmd::Execute()
{
    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mBeamUid);
    if (beam == nullptr) return TPS_ER_FAILURE;
    auto iec = mDataRepository->GetBeamManager()->GetBeamCS(mBeamUid);
    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
    if (segments.size() == 0) return TPS_ER_FAILURE;
    std::vector<float> segmentStartSSDs(segments.size());
    std::vector<float> segmentEndSSDs(segments.size());

    ROIEntity *voiEntity = mDataRepository->GetVoiManager()->GetROI(mSkinUid);
    SignedDistanceField* sdf = voiEntity->GetSDF();

    //取image，得到MatrxiP2V
    //get image 3D;
    std::string imageUID;
    imageUID = voiEntity->GetSeriesUid();
    if (imageUID.empty())
    {
        TPS_LOG_DEV_ERROR << "failed to get image uid";
        return TPS_ER_FAILURE;
    }
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(imageUID, &image))
    {
        TPS_LOG_DEV_ERROR << "failed to get image 3D";
        return TPS_ER_FAILURE;
    }

    Mcsf::Matrix4f matV2P;
    if (!image->GetVolumeToPatientMatrix(matV2P))
    {
        TPS_LOG_DEV_ERROR << "Failed to get volume to patient matrix of image:";
        return TPS_ER_FAILURE;
    }
    Mcsf::Matrix4f matP2V = matV2P.Inverse();
    RtImage3DHeader* header = image->GetRtSeries()->get_header();
    size_t dim[3] = { 1 };
    if (nullptr != header)
    {
        dim[0] = header->m_iXDim;
        dim[1] = header->m_iYDim;
        dim[2] = header->m_iSliceCount;
    }
    float segmentSsd;
    mIsFirstSSD = false; // 将第一个SSD作为beam的SSD
    for (int i = 0; i < segments.size(); i++)
    {
        if (i == 0)
        {
            mIsFirstSSD = true;
        }
        Mcsf::Matrix4f matB2P;
        iec->RotateGantry(segments[i]->get_startgantryangle());
        iec->RotateCollimator(segments[i]->get_startcollimatorangle());
        iec->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matB2P);
        if (ComputeSSD(sdf, matB2P, matP2V, matV2P, dim, segmentSsd) != TPS_ER_SUCCESS)
        {
            return TPS_ER_FAILURE;
        }
        mIsFirstSSD = false;
        if (mIsBeamSSD) //如果是只计算beam SSD时，直接return取第一次计算结果。
        {
            mIsBeamSSD = false;
            return TPS_ER_SUCCESS;
        }
        //如果SSD数值更新超过前端显示的最小有效值1mm，更新数据库j
        //为了保证数据库和前端显示的值始终一致，这里判断ssd的变化值
        //之前要将ssd转为int，即等于前端0.1cm的有效值。
        if (abs((int)segments[i]->get_startssd() - (int)segmentSsd) >= 1.0)
        {
            segmentStartSSDs[i] = segmentSsd;
        }
        iec->RotateGantry(segments[i]->get_arclength() + segments[i]->get_startgantryangle());
        iec->RotateCollimator(segments[i]->get_endcollimatorangle());
        iec->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matB2P);
        if (ComputeSSD(sdf, matB2P, matP2V, matV2P, dim, segmentSsd) != TPS_ER_SUCCESS)
        {
            return TPS_ER_FAILURE;
        }
        if (abs((int)segments[i]->get_endssd() - (int)segmentSsd) >= 1.0)
        {
            segmentEndSSDs[i] = segmentSsd;
        }
    }
    // 确保所有segments的ssd都成功更新后，再写入到内存，保存
    for (int i = 0; i < segments.size(); i++)
    {
        segments[i]->set_startssd(segmentStartSSDs[i]);
        segments[i]->set_endssd(segmentEndSSDs[i]);
    }
    beam->set_ssd(mSSD);
    mSaveObjects->UpdateBeamSegments(segments);
    return TPS_ER_SUCCESS;
}

int GetSSDCmd::ComputeSSD(SignedDistanceField* sdf,  Mcsf::Matrix4f matB2P, Mcsf::Matrix4f matP2V, 
    Mcsf::Matrix4f matV2P, size_t dim[3], float& ssd)
{
    //两种求交点的方法，
    //1,生成skin的mesh，遍历三角形，求出交点。
    //2,利用skin的subvolume，求交点。现实现为方法2.
    //如果是arcbeam，只求startangle对应的ssd
    Mcsf::Matrix4f matB2V;
    matB2V = matP2V * matB2P;
    //将源点，isocenter点的坐标转为Volume坐标系
    Mcsf::Point3f pointSource(0.0, 0.0, 0.0), pointIsocenter(0.0, 0.0, -1000.0);
    pointSource = matB2V.Transform(pointSource);
    pointIsocenter = matB2V.Transform(pointIsocenter);
    //求直线pointSource-pointIsocenter与Volume的交点。
    double minDis = 1e10, maxDis = -1.0, dis;
    Mcsf::Point3f minP, maxP;
    std::vector<Mcsf::Point3f> inters = GetIntersectionsOfLineAndVolume(pointSource, pointIsocenter, dim[0] - 1, dim[1] - 1, dim[2] - 1);
    if (inters.size() < 2)
    {
        ssd = 1e10;
        if (mIsBeamSSD || mIsFirstSSD)
        {
            mSSD = 1e10;
            mRangeMin = -1;
            mRangeMax = -1;
        }
        return TPS_ER_SUCCESS;
    }
    for (int i = 0; i < inters.size(); ++i)
    {
        dis = Mcsf::Vector3f(pointSource - inters[i]).Magnitude();
        if (minDis > dis)
        {
            minDis = dis;
            minP = inters[i];
        }
        if (maxDis < dis)
        {
            maxDis = dis;
            maxP = inters[i];
        }
    }
    minP = matV2P.Transform(minP);
    maxP = matV2P.Transform(maxP);

    pointSource = matV2P.Transform(pointSource);
    pointIsocenter = matV2P.Transform(pointIsocenter);
    //取任一过直线pointSource-pointIsocenter的面，得到该面的contour
    Mcsf::Point3f thirdPoint, forthPoint;
    if (abs(pointIsocenter.GetZ() - pointSource.GetZ()) > 1e-3)
    {
        thirdPoint.SetX(pointIsocenter.GetX());
        thirdPoint.SetY(pointIsocenter.GetY() + 10.0f);
        thirdPoint.SetZ(10.0 * (pointSource.GetY() - pointIsocenter.GetY()) / (pointIsocenter.GetZ() - pointSource.GetZ()) + pointIsocenter.GetZ());
    }
    else if (abs(pointIsocenter.GetY() - pointSource.GetY()) > 1e-3)
    {
        thirdPoint.SetX(pointIsocenter.GetX());
        thirdPoint.SetZ(pointIsocenter.GetZ() + 10.0f);
        thirdPoint.SetY(10.0 * (pointSource.GetZ() - pointIsocenter.GetZ()) / (pointIsocenter.GetY() - pointSource.GetY()) + pointIsocenter.GetY());
    }
    else
    {
        thirdPoint.SetY(pointIsocenter.GetY());
        thirdPoint.SetZ(pointIsocenter.GetZ() + 10.0f);
        thirdPoint.SetX(10.0 * (pointSource.GetZ() - pointIsocenter.GetZ()) / (pointIsocenter.GetX() - pointSource.GetX()) + pointIsocenter.GetX());
    }
    forthPoint.SetX(thirdPoint.GetX() + pointSource.GetX() - pointIsocenter.GetX());
    forthPoint.SetY(thirdPoint.GetY() + pointSource.GetY() - pointIsocenter.GetY());
    forthPoint.SetZ(thirdPoint.GetZ() + pointSource.GetZ() - pointIsocenter.GetZ());

    PT3 v[4] = 
    {
        TpsArithmeticConverter::ConvertToPoint3D(pointSource),
        TpsArithmeticConverter::ConvertToPoint3D(pointIsocenter),
        TpsArithmeticConverter::ConvertToPoint3D(thirdPoint),
        TpsArithmeticConverter::ConvertToPoint3D(forthPoint)
    };
    PLANE plane(v[0], v[3], v[1]);
    std::vector<Contour> contours;
    ROIShapeConverter::SliceSDFToContours_CPU(*sdf, plane, contours);

    //计算直线与polygon的交点。得到离pointSource最近的交点
    int firstIndex = 0, secondIndex = 0;
    Point3D intersection, minIntersection;
    double distance = 1e10;
    bool hasInter = false;
    //延长4个SAD距离，确保能穿过皮肤
    v[1] += 4 * (v[1] - v[0]);
    for (int i = 0; i < contours.size(); ++i)
    {
        int vc = contours[i].pt.size();
        if(vc < 3) continue;
        for (int j = 0; j < vc; ++j)
        {
            firstIndex = j;
            secondIndex = firstIndex + 1;
            if (secondIndex == contours[i].pt.size()) secondIndex = 0;
            v[2] = contours[i].pt[j];
            v[3] = contours[i].pt[(j+1)%vc];
            if (Intersection(v[0], v[1], v[2], v[3], intersection) <= 0) continue;
            if ((intersection - v[0]).Magnitude() < distance)
            {
                distance = (intersection - v[0]).Magnitude();
                minIntersection = intersection;
                hasInter = true;
            }
        }
    }

    //如果没有交点，则返回
    pointSource = matB2P.Transform(Mcsf::Point3f(0.0, 0.0, 0.0));
    if (!hasInter)
    {
        ssd = 1e10;
        if (mIsBeamSSD  || mIsFirstSSD)
        {
            mOriginalX = pointSource.GetX();
            mOriginalY = pointSource.GetY();
            mOriginalZ = pointSource.GetZ();
            mRangeMax = 1e10;
            mRangeMin = 1e10;
            mSSD = 1e10;
        }
        return TPS_ER_SUCCESS;
    }

    //将pointSource，pointIsocenter，minIntersection点转为patient坐标系。
    Mcsf::Point3f minIntersectionF = TpsArithmeticConverter::ConvertToPoint3f(minIntersection);

    ssd = Mcsf::Vector3f(pointSource - minIntersectionF).Magnitude();
    if (mIsBeamSSD  || mIsFirstSSD)
    {
        mSSD = ssd;
        mRangeMax = 1000 + Mcsf::Vector3f(minIntersectionF - minP).Magnitude();
        mRangeMin = 500 > (1000 - Mcsf::Vector3f(minIntersectionF - maxP).Magnitude()) ? 500.0 : (1000 - Mcsf::Vector3f(minIntersectionF - maxP).Magnitude());
        mOriginalX = pointSource.GetX();
        mOriginalY = pointSource.GetY();
        mOriginalZ = pointSource.GetZ();
    }
    return TPS_ER_SUCCESS;
}

int GetSSDCmd::GetSSD(double& ssd, double& rangeMin, double& rangeMax, double& ox, double& oy, double&oz)
{
    ssd = mSSD;
    rangeMin = mRangeMin;
    rangeMax = mRangeMax;
    ox = mOriginalX;
    oy = mOriginalY;
    oz = mOriginalZ;
    return TPS_ER_SUCCESS;
}

// 为了得到IsoCenter在Volume坐标系下的取值范围
std::vector<Mcsf::Point3f> GetSSDCmd::GetIntersectionsOfLineAndVolume(
    const Mcsf::Point3f& p1, const Mcsf::Point3f& p2, double dimX, double dimY, double dimZ)
{
    Mcsf::Point3f intersection;
    std::vector<Mcsf::Point3f> intersections;
    double ss = 0.0;
    if (abs(p2.GetX() - p1.GetX()) > 1e-5)
    {
        //Plane X = 0.0
        intersection.SetX(0.0);
        ss = p2.GetY() - p2.GetX() * (p2.GetY() - p1.GetY()) / (p2.GetX() - p1.GetX());
        intersection.SetY(ss);
        if (ss < dimY - 0.0 && ss >= 0.0)
        {
            ss = p2.GetZ() - p2.GetX() * (p2.GetZ() - p1.GetZ()) / (p2.GetX() - p1.GetX());
            intersection.SetZ(ss);
            if (ss < dimZ - 0.0 && ss >= 0.0)
            {
                intersections.push_back(intersection);
            }
        }
        //Plane X = dimX - 0.0
        intersection.SetX(dimX - 0.0);
        ss = p2.GetY() - (p2.GetX() - dimX + 0.0) * (p2.GetY() - p1.GetY()) / (p2.GetX() - p1.GetX());
        intersection.SetY(ss);
        if (ss < dimY - 0.0 && ss >= 0.0)
        {
            ss = p2.GetZ() - (p2.GetX() - dimX + 0.0) * (p2.GetZ() - p1.GetZ()) / (p2.GetX() - p1.GetX());
            intersection.SetZ(ss);
            if (ss < dimZ - 0.0 && ss >= 0.0)
            {
                intersections.push_back(intersection);
            }
        }
    }
    if (abs(p2.GetY() - p1.GetY()) > 1e-5)
    {
        //Plane Y = 0.0
        intersection.SetY(0.0);
        ss = p2.GetX() - p2.GetY() * (p2.GetX() - p1.GetX()) / (p2.GetY() - p1.GetY());
        intersection.SetX(ss);
        if (ss < dimX - 0.0 && ss >= 0.0)
        {
            ss = p2.GetZ() - p2.GetY() * (p2.GetZ() - p1.GetZ()) / (p2.GetY() - p1.GetY());
            intersection.SetZ(ss);
            if (ss < dimZ - 0.0 && ss >= 0.0)
            {
                intersections.push_back(intersection);
            }
        }
        //Plane Y = dimY - 0.0
        intersection.SetY(dimY - 0.0);
        ss = p2.GetX() - (p2.GetY() - dimY + 0.0) * (p2.GetX() - p1.GetX()) / (p2.GetY() - p1.GetY());
        intersection.SetX(ss);
        if (ss < dimX - 0.0 && ss >= 0.0)
        {
            ss = p2.GetZ() - (p2.GetY() - dimY + 0.0) * (p2.GetZ() - p1.GetZ()) / (p2.GetY() - p1.GetY());
            intersection.SetZ(ss);
            if (ss < dimZ - 0.0 && ss >= 0.0)
            {
                intersections.push_back(intersection);
            }
        }
    }
    if (abs(p2.GetZ() - p1.GetZ()) > 1e-5)
    {
        //Plane Z = 0.0
        intersection.SetZ(0.0);
        ss = p2.GetY() - p2.GetZ() * (p2.GetY() - p1.GetY()) / (p2.GetZ() - p1.GetZ());
        intersection.SetY(ss);
        if (ss < dimY - 0.0 && ss >= 0.0)
        {
            ss = p2.GetX() - p2.GetZ() * (p2.GetX() - p1.GetX()) / (p2.GetZ() - p1.GetZ());
            intersection.SetX(ss);
            if (ss < dimX - 0.0 && ss >= 0.0)
            {
                intersections.push_back(intersection);
            }
        }
        //Plane Z = dimZ - 0.0
        intersection.SetZ(dimZ - 0.0);
        ss = p2.GetY() - (p2.GetZ() - dimZ + 0.0) * (p2.GetY() - p1.GetY()) / (p2.GetZ() - p1.GetZ());
        intersection.SetY(ss);
        if (ss < dimY - 0.0 && ss >= 0.0)
        {
            ss = p2.GetX() - (p2.GetZ() - dimZ + 0.0) * (p2.GetX() - p1.GetX()) / (p2.GetZ() - p1.GetZ());
            intersection.SetX(ss);
            if (ss < dimX - 0.0 && ss >= 0.0)
            {
                intersections.push_back(intersection);
            }
        }
    }
    return intersections;
}

int GetSSDCmd::Intersection(Point3D p1, Point3D p2, Point3D p3, Point3D p4, Point3D& Int){
    //保证参数p1!=p2，p3!=p4 
    if (p1 == p2 || p3 == p4) {
        return -1; //返回-1代表至少有一条线段首尾重合，不能构成线段 
    }

    //为方便运算，保证各线段的起点在前，终点在后。 
    if (AbovePoint(p1, p2)) {
        swap(p1, p2);
    }
    if (AbovePoint(p3, p4)) {
        swap(p3, p4);
    }
    //判定两线段是否完全重合 
    if (p1 == p3 && p2 == p4) {
        return 6;
    }
    //求出两线段构成的向量 
    Point3D v1(p2.x - p1.x, p2.y - p1.y, p1.z), v2(p4.x - p3.x, p4.y - p3.y, p4.z);
    //求两向量外积，平行时外积为0 
    float Corss = CrossProduct(v1, v2);
    //如果起点重合 
    if (p1 == p3) {
        Int = p1;
        //起点重合且共线(平行)返回5；不平行则交于端点，返回3 
        return (Equal(Corss, 0) ? 5 : 3);
    }
    //如果终点重合 
    if (p2 == p4) {
        Int = p2;
        //终点重合且共线(平行)返回5；不平行则交于端点，返回3 
        return (Equal(Corss, 0) ? 5 : 3);
    }
    //如果两线端首尾相连 
    if (p1 == p4) {
        Int = p1;
        return 3;
    }
    if (p2 == p3) {
        Int = p2;
        return 3;
    }//经过以上判断，首尾点相重的情况都被排除了 
    //将线段按起点坐标排序。若线段1的起点较大，则将两线段交换 
    if (AbovePoint(p1, p3)) {
        swap(p1, p3);
        swap(p2, p4);
        //更新原先计算的向量及其外积 
        swap(v1, v2);
        Corss = CrossProduct(v1, v2);
    }
    //处理两线段平行的情况 
    if (Equal(Corss, 0)) {
        //做向量v1(p1, p2)和vs(p1,p3)的外积，判定是否共线 
        Point3D vs(p3.x - p1.x, p3.y - p1.y, p3.z);
        //外积为0则两平行线段共线，下面判定是否有重合部分 
        if (Equal(CrossProduct(v1, vs), 0)) {
            //前一条线的终点大于后一条线的起点，则判定存在重合 
            if (AbovePoint(p2, p3)) {
                Int = p3;
                return 4; //返回值4代表线段部分重合 
            }
        }//若三点不共线，则这两条平行线段必不共线。 
        //不共线或共线但无重合的平行线均无交点 
        return 0;
    } //以下为不平行的情况，先进行快速排斥试验 
    //x坐标已有序，可直接比较。y坐标要先求两线段的最大和最小值 
    float ymax1 = p1.y, ymin1 = p2.y, ymax2 = p3.y, ymin2 = p4.y;
    if (ymax1 < ymin1) {
        swap(ymax1, ymin1);
    }
    if (ymax2 < ymin2) {
        swap(ymax2, ymin2);
    }
    //如果以两线段为对角线的矩形不相交，则无交点 
    if (p1.x > p4.x || p2.x < p3.x || ymax1 < ymin2 || ymin1 > ymax2) {
        return 0;
    }//下面进行跨立试验 
    Point3D vs1(p1.x - p3.x, p1.y - p3.y, p1.z), vs2(p2.x - p3.x, p2.y - p3.y, p2.z);
    Point3D vt1(p3.x - p1.x, p3.y - p1.y, p3.z), vt2(p4.x - p1.x, p4.y - p1.y, p4.z);
    float s1v2, s2v2, t1v1, t2v1;
    //根据外积结果判定否交于线上 
    if (Equal(s1v2 = CrossProduct(vs1, v2), 0) && AbovePoint(p4, p1) && AbovePoint(p1, p3)) {
        Int = p1;
        return 2;
    }
    if (Equal(s2v2 = CrossProduct(vs2, v2), 0) && AbovePoint(p4, p2) && AbovePoint(p2, p3)) {
        Int = p2;
        return 2;
    }
    if (Equal(t1v1 = CrossProduct(vt1, v1), 0) && AbovePoint(p2, p3) && AbovePoint(p3, p1)) {
        Int = p3;
        return 2;
    }
    if (Equal(t2v1 = CrossProduct(vt2, v1), 0) && AbovePoint(p2, p4) && AbovePoint(p4, p1)) {
        Int = p4;
        return 2;
    } //未交于线上，则判定是否相交 
    if (s1v2 * s2v2 > 0 || t1v1 * t2v1 > 0) {
        return 0;
    } //以下为相交的情况，算法详见文档 
    //计算二阶行列式的两个常数项 
    float ConA = p1.x * v1.y - p1.y * v1.x;
    float ConB = p3.x * v2.y - p3.y * v2.x;
    //计算行列式D1和D2的值，除以系数行列式的值，得到交点坐标 
    Int.x = (ConB * v1.x - ConA * v2.x) / Corss;
    Int.y = (ConB * v1.y - ConA * v2.y) / Corss;
    //正交返回1 
    return 1;

}

TPS_END_NAMESPACE