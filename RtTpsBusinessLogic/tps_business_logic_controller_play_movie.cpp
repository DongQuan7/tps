//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song  song.li@united-imaging.com
///
///  \file    tps_cca_contouring_controllerapp_voi_advanced.cpp
///  \brief   Contouring Controller Application
///
///  \version 1.0
///  \date    Feb. 3, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include"stdafx.h"
#include "RtTpsBusinessLogic/tps_business_logic_controller.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_volume4d_change_current.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_volume_change_slice.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_gallery_volume_change_slice.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

#include <boost/asio.hpp>                                           // for movie
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"     // for AXIAL

#include "RtTpsCore/tps_core_simple_gui_responser.h"

TPS_BEGIN_NAMESPACE

int TpsBLController::PlaySeries(const std::string& strSeriesUid, const std::string& strPlanUid, PLAY_STATE playState, PLAY_SPEED playSpeed, PLAY_MODE playMode,
                                std::shared_ptr<SimpleGuiResponser> spResponser,bool isCyclic) {

    {
        boost::mutex::scoped_lock lock(*m_mutex); 
        m_strMovieSeriesUid = strSeriesUid;
        m_strMoviePlanUid = strPlanUid;
        mPlayState = playState;
        mPlaySpeed = playSpeed;
        mPlayMode = playMode;
        mIsCyclic = isCyclic;
        if (mPlayMode == FOURD)
            m_spResponser4d = spResponser;
        else 
            m_spResponser3d = spResponser;
    }

    if (m_thread == nullptr)
    {
        m_thread = new boost::thread(boost::bind(&TpsBLController::PlayerThreadCallback, this));
    }

    return TPS_ER_SUCCESS;
}

void TpsBLController::PlayerThreadCallback() 
{
    boost::asio::io_service io;
    boost::asio::deadline_timer timer(io, boost::posix_time::seconds(0));
    typedef std::function<void(const boost::system::error_code&)> timer_callback;
    timer_callback timerCallback = [&](const boost::system::error_code& err) 
    {
        err;
        boost::mutex::scoped_lock lock(*m_mutex);

        if (mPlayMode == THREED) {
            Play3DSeries(mPlayState);
        }
        else if (mPlayMode == FOURD) {
            Play4DSeries(mPlayState);
        }

        int millisec = 0;
        switch (mPlaySpeed) {
        case SLOW:
            millisec = 1000;
            break;
        case NORMAL:
            millisec = 500;
            break;
        case FAST:
            millisec = 200;
            break;
        }
        //在pause或stop时，会出现此线程一直在走空流程。如果要解决此问题，需要把timer和io.run的线程分开。
        timer.expires_at(timer.expires_at() + boost::posix_time::millisec(millisec));
        timer.async_wait(timerCallback);
    };

    timer.async_wait(timerCallback);

    io.run();
}

void TpsBLController::Play3DSeries(PLAY_STATE playState) {

    switch (playState) {
    case PLAY:
    case RESUME:
        {
        VolumeChangeSliceCmd volumeChangeSliceCmd(m_strMovieSeriesUid, m_strMoviePlanUid, (int)AXIAL, 1, m_spResponser3d, mIsCyclic); // 1 = next slice
            //volumeChangeSliceCmd.RunCommand();
            ProcessCommand(&volumeChangeSliceCmd);
            break;
        }
    case PAUSE:
        break;
    case STOP:
        break;
    }
}

void TpsBLController::Play4DSeries(PLAY_STATE playState) {

    switch (playState) {
    case PLAY:
    case RESUME:
        {
            Volume4dChangeCurrentCmd volume4dCmd(m_strMovieSeriesUid, 0, m_spResponser4d, mIsCyclic);   // 0 = play
            //volume4dCmd.RunCommand();
            ProcessCommand(&volume4dCmd);
            break;
        }
    case PAUSE:
        break;
    case STOP:
        {
            Volume4dChangeCurrentCmd volume4dCmd(m_strMovieSeriesUid, 1, m_spResponser4d, mIsCyclic);   // 1 = stop
            //volume4dCmd.RunCommand();
            ProcessCommand(&volume4dCmd);
            break;
        }
    }
}

void TpsBLController::SetMainPhase(std::string strGroupUid, std::string strSeriesUid)
{
    TpsImageDataManager* pImageManager = mDataRepository->GetImageDataManager();
    if (pImageManager == nullptr)
        return;
    pImageManager->SetMainPhase(strGroupUid, strSeriesUid);
}

void TpsBLController::SetSeriesGroup(std::string strGroupUid, std::vector<std::string> vecSeriesUid)
{
    TpsImageDataManager* pImageManager = mDataRepository->GetImageDataManager();
    if (pImageManager == nullptr)
        return;

    pImageManager->SetSeriesGroup(strGroupUid, vecSeriesUid);
}

int TpsBLController::PlaySeriesForGallery(const std::string& strSeriesUid, PLAY_STATE playState, PLAY_SPEED playSpeed, PLAY_MODE playMode,
    std::shared_ptr<SimpleGuiResponser> spResponser) {

        {
            boost::mutex::scoped_lock lock(*m_galleryMutex); 
            m_strGalleryMovieSeriesUid = strSeriesUid;
            mGalleryPlayState = playState;
            mGalleryPlaySpeed = playSpeed;
            mGalleryPlayMode = playMode;

            if (mGalleryPlayMode == FOURD)
                m_spGalleryResponser4d = spResponser;
            else 
                m_spGalleryResponser3d = spResponser;
        }

        if (m_galleryThread == nullptr)
        {
            m_galleryThread = new boost::thread(boost::bind(&TpsBLController::GalleryPlayerThreadCallback, this));
        }

        return TPS_ER_SUCCESS;
}

void TpsBLController::GalleryPlayerThreadCallback() 
{
    boost::asio::io_service io;
    boost::asio::deadline_timer timer(io, boost::posix_time::seconds(0));
    typedef std::function<void(const boost::system::error_code&)> timer_callback;
    timer_callback timerCallback = [&](const boost::system::error_code& err) 
    {
        err;
        boost::mutex::scoped_lock lock(*m_galleryMutex);

        if (mGalleryPlayMode == THREED) {
            Play3DSeriesForGallery(mGalleryPlayState);
        }
        else if (mGalleryPlayMode == FOURD) {
            Play4DSeriesForGallery(mGalleryPlayState);
        }

        int millisec = 0;
        switch (mGalleryPlaySpeed) {
        case SLOW:
            millisec = 1000;
            break;
        case NORMAL:
            millisec = 500;
            break;
        case FAST:
            millisec = 200;
            break;
        }
        //在pause或stop时，会出现此线程一直在走空流程。如果要解决此问题，需要把timer和io.run的线程分开。
        timer.expires_at(timer.expires_at() + boost::posix_time::millisec(millisec));
        timer.async_wait(timerCallback);
    };

    timer.async_wait(timerCallback);

    io.run();
}

void TpsBLController::Play3DSeriesForGallery(PLAY_STATE playState) {

    switch (playState) {
    case PLAY:
    case RESUME:
        {
            GalleryVolumeChangeSliceCmd volumeChangeSliceCmd(m_strGalleryMovieSeriesUid, (int)AXIAL, 1, m_spGalleryResponser3d); // 1 = next slice
            //volumeChangeSliceCmd.RunCommand();
            ProcessCommand(&volumeChangeSliceCmd);
            break;
        }
    case PAUSE:
        break;
    case STOP:
        break;
    }
}

void TpsBLController::Play4DSeriesForGallery(PLAY_STATE playState) {

    switch (playState) {
    case PLAY:
    case RESUME:
        {
            //Volume4dChangeCurrentCmd volume4dCmd(m_strMovieSeriesUid, 0, m_spResponser4d);   // 0 = play
            ////volume4dCmd.RunCommand();
            //ProcessCommand(&volume4dCmd);
            //break;
        }
    case PAUSE:
        break;
    case STOP:
        {
            //Volume4dChangeCurrentCmd volume4dCmd(m_strMovieSeriesUid, 1, m_spResponser4d);   // 1 = stop
            ////volume4dCmd.RunCommand();
            //ProcessCommand(&volume4dCmd);
            //break;
        }
    }
}
TPS_END_NAMESPACE
