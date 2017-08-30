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

//ĿǰSSD��ʱ�������ݿ⣬load����ȫ����һ�¡�InterfaceRequrement44844��ʱ��ʵ�֡�
//��Ҫ��һ����һ��SSD�Ƿ�Ҫ�����ڵ�����Dicom�ļ���
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

    //ȡimage���õ�MatrxiP2V
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
    mIsFirstSSD = false; // ����һ��SSD��Ϊbeam��SSD
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
        if (mIsBeamSSD) //�����ֻ����beam SSDʱ��ֱ��returnȡ��һ�μ�������
        {
            mIsBeamSSD = false;
            return TPS_ER_SUCCESS;
        }
        //���SSD��ֵ���³���ǰ����ʾ����С��Чֵ1mm���������ݿ�j
        //Ϊ�˱�֤���ݿ��ǰ����ʾ��ֵʼ��һ�£������ж�ssd�ı仯ֵ
        //֮ǰҪ��ssdתΪint��������ǰ��0.1cm����Чֵ��
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
    // ȷ������segments��ssd���ɹ����º���д�뵽�ڴ棬����
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
    //�����󽻵�ķ�����
    //1,����skin��mesh�����������Σ�������㡣
    //2,����skin��subvolume���󽻵㡣��ʵ��Ϊ����2.
    //�����arcbeam��ֻ��startangle��Ӧ��ssd
    Mcsf::Matrix4f matB2V;
    matB2V = matP2V * matB2P;
    //��Դ�㣬isocenter�������תΪVolume����ϵ
    Mcsf::Point3f pointSource(0.0, 0.0, 0.0), pointIsocenter(0.0, 0.0, -1000.0);
    pointSource = matB2V.Transform(pointSource);
    pointIsocenter = matB2V.Transform(pointIsocenter);
    //��ֱ��pointSource-pointIsocenter��Volume�Ľ��㡣
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
    //ȡ��һ��ֱ��pointSource-pointIsocenter���棬�õ������contour
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

    //����ֱ����polygon�Ľ��㡣�õ���pointSource����Ľ���
    int firstIndex = 0, secondIndex = 0;
    Point3D intersection, minIntersection;
    double distance = 1e10;
    bool hasInter = false;
    //�ӳ�4��SAD���룬ȷ���ܴ���Ƥ��
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

    //���û�н��㣬�򷵻�
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

    //��pointSource��pointIsocenter��minIntersection��תΪpatient����ϵ��
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

// Ϊ�˵õ�IsoCenter��Volume����ϵ�µ�ȡֵ��Χ
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
    //��֤����p1!=p2��p3!=p4 
    if (p1 == p2 || p3 == p4) {
        return -1; //����-1����������һ���߶���β�غϣ����ܹ����߶� 
    }

    //Ϊ�������㣬��֤���߶ε������ǰ���յ��ں� 
    if (AbovePoint(p1, p2)) {
        swap(p1, p2);
    }
    if (AbovePoint(p3, p4)) {
        swap(p3, p4);
    }
    //�ж����߶��Ƿ���ȫ�غ� 
    if (p1 == p3 && p2 == p4) {
        return 6;
    }
    //������߶ι��ɵ����� 
    Point3D v1(p2.x - p1.x, p2.y - p1.y, p1.z), v2(p4.x - p3.x, p4.y - p3.y, p4.z);
    //�������������ƽ��ʱ���Ϊ0 
    float Corss = CrossProduct(v1, v2);
    //�������غ� 
    if (p1 == p3) {
        Int = p1;
        //����غ��ҹ���(ƽ��)����5����ƽ�����ڶ˵㣬����3 
        return (Equal(Corss, 0) ? 5 : 3);
    }
    //����յ��غ� 
    if (p2 == p4) {
        Int = p2;
        //�յ��غ��ҹ���(ƽ��)����5����ƽ�����ڶ˵㣬����3 
        return (Equal(Corss, 0) ? 5 : 3);
    }
    //������߶���β���� 
    if (p1 == p4) {
        Int = p1;
        return 3;
    }
    if (p2 == p3) {
        Int = p2;
        return 3;
    }//���������жϣ���β�����ص���������ų��� 
    //���߶ΰ���������������߶�1�����ϴ������߶ν��� 
    if (AbovePoint(p1, p3)) {
        swap(p1, p3);
        swap(p2, p4);
        //����ԭ�ȼ��������������� 
        swap(v1, v2);
        Corss = CrossProduct(v1, v2);
    }
    //�������߶�ƽ�е���� 
    if (Equal(Corss, 0)) {
        //������v1(p1, p2)��vs(p1,p3)��������ж��Ƿ��� 
        Point3D vs(p3.x - p1.x, p3.y - p1.y, p3.z);
        //���Ϊ0����ƽ���߶ι��ߣ������ж��Ƿ����غϲ��� 
        if (Equal(CrossProduct(v1, vs), 0)) {
            //ǰһ���ߵ��յ���ں�һ���ߵ���㣬���ж������غ� 
            if (AbovePoint(p2, p3)) {
                Int = p3;
                return 4; //����ֵ4�����߶β����غ� 
            }
        }//�����㲻���ߣ���������ƽ���߶αز����ߡ� 
        //�����߻��ߵ����غϵ�ƽ���߾��޽��� 
        return 0;
    } //����Ϊ��ƽ�е�������Ƚ��п����ų����� 
    //x���������򣬿�ֱ�ӱȽϡ�y����Ҫ�������߶ε�������Сֵ 
    float ymax1 = p1.y, ymin1 = p2.y, ymax2 = p3.y, ymin2 = p4.y;
    if (ymax1 < ymin1) {
        swap(ymax1, ymin1);
    }
    if (ymax2 < ymin2) {
        swap(ymax2, ymin2);
    }
    //��������߶�Ϊ�Խ��ߵľ��β��ཻ�����޽��� 
    if (p1.x > p4.x || p2.x < p3.x || ymax1 < ymin2 || ymin1 > ymax2) {
        return 0;
    }//������п������� 
    Point3D vs1(p1.x - p3.x, p1.y - p3.y, p1.z), vs2(p2.x - p3.x, p2.y - p3.y, p2.z);
    Point3D vt1(p3.x - p1.x, p3.y - p1.y, p3.z), vt2(p4.x - p1.x, p4.y - p1.y, p4.z);
    float s1v2, s2v2, t1v1, t2v1;
    //�����������ж��������� 
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
    } //δ�������ϣ����ж��Ƿ��ཻ 
    if (s1v2 * s2v2 > 0 || t1v1 * t2v1 > 0) {
        return 0;
    } //����Ϊ�ཻ��������㷨����ĵ� 
    //�����������ʽ������������ 
    float ConA = p1.x * v1.y - p1.y * v1.x;
    float ConB = p3.x * v2.y - p3.y * v2.x;
    //��������ʽD1��D2��ֵ������ϵ������ʽ��ֵ���õ��������� 
    Int.x = (ConB * v1.x - ConA * v2.x) / Corss;
    Int.y = (ConB * v1.y - ConA * v2.y) / Corss;
    //��������1 
    return 1;

}

TPS_END_NAMESPACE