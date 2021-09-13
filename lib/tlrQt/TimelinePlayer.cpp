// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021 Darby Johnston
// All rights reserved.

#include <tlrQt/TimelinePlayer.h>

#include <tlrCore/Math.h>

namespace tlr
{
    namespace qt
    {
        struct TimelinePlayer::Private
        {
            std::shared_ptr<timeline::TimelinePlayer> timelinePlayer;

            std::shared_ptr<observer::ValueObserver<float> > speedObserver;
            std::shared_ptr<observer::ValueObserver<timeline::Playback> > playbackObserver;
            std::shared_ptr<observer::ValueObserver<timeline::Loop> > loopObserver;
            std::shared_ptr<observer::ValueObserver<otime::RationalTime> > currentTimeObserver;
            std::shared_ptr<observer::ValueObserver<otime::TimeRange> > inOutRangeObserver;
            std::shared_ptr<observer::ValueObserver<uint16_t> > videoLayerObserver;
            std::shared_ptr<observer::ValueObserver<timeline::Frame> > frameObserver;
            std::shared_ptr<observer::ListObserver<otime::TimeRange> > cachedFramesObserver;
        };

        TimelinePlayer::TimelinePlayer(
            const file::Path& path,
            const std::shared_ptr<core::Context>& context,
            QObject* parent) :
            QObject(parent),
            _p(new Private)
        {
            TLR_PRIVATE_P();

            p.timelinePlayer = timeline::TimelinePlayer::create(path, context);

            p.speedObserver = observer::ValueObserver<float>::create(
                p.timelinePlayer->observeSpeed(),
                [this](float value)
                {
                    Q_EMIT speedChanged(value);
                });

            p.playbackObserver = observer::ValueObserver<timeline::Playback>::create(
                p.timelinePlayer->observePlayback(),
                [this](timeline::Playback value)
                {
                    Q_EMIT playbackChanged(value);
                });

            p.loopObserver = observer::ValueObserver<timeline::Loop>::create(
                p.timelinePlayer->observeLoop(),
                [this](timeline::Loop value)
                {
                    Q_EMIT loopChanged(value);
                });

            p.currentTimeObserver = observer::ValueObserver<otime::RationalTime>::create(
                p.timelinePlayer->observeCurrentTime(),
                [this](const otime::RationalTime& value)
                {
                    Q_EMIT currentTimeChanged(value);
                });

            p.inOutRangeObserver = observer::ValueObserver<otime::TimeRange>::create(
                p.timelinePlayer->observeInOutRange(),
                [this](const otime::TimeRange value)
                {
                    Q_EMIT inOutRangeChanged(value);
                });

            p.videoLayerObserver = observer::ValueObserver<uint16_t>::create(
                p.timelinePlayer->observeVideoLayer(),
                [this](uint16_t value)
                {
                    Q_EMIT videoLayerChanged(value);
                });

            p.frameObserver = observer::ValueObserver<timeline::Frame>::create(
                p.timelinePlayer->observeFrame(),
                [this](const timeline::Frame& value)
                {
                    Q_EMIT frameChanged(value);
                });

            p.cachedFramesObserver = observer::ListObserver<otime::TimeRange>::create(
                p.timelinePlayer->observeCachedFrames(),
                [this](const std::vector<otime::TimeRange>& value)
                {
                    Q_EMIT cachedFramesChanged(value);
                });

            startTimer(playerTimerInterval, Qt::PreciseTimer);
        }

        TimelinePlayer::~TimelinePlayer()
        {}
        
        const std::weak_ptr<core::Context>& TimelinePlayer::context() const
        {
            return _p->timelinePlayer->getContext();
        }

        const otio::SerializableObject::Retainer<otio::Timeline>& TimelinePlayer::timeline() const
        {
            return _p->timelinePlayer->getTimeline();
        }

        const file::Path& TimelinePlayer::path() const
        {
            return _p->timelinePlayer->getPath();
        }

        const otime::RationalTime& TimelinePlayer::globalStartTime() const
        {
            return _p->timelinePlayer->getGlobalStartTime();
        }

        const otime::RationalTime& TimelinePlayer::duration() const
        {
            return _p->timelinePlayer->getDuration();
        }

        const std::vector<imaging::Info>& TimelinePlayer::videoInfo() const
        {
            return _p->timelinePlayer->getVideoInfo();
        }

       float TimelinePlayer::defaultSpeed() const
        {
            return _p->timelinePlayer->getDefaultSpeed();
        }

        float TimelinePlayer::speed() const
        {
            return _p->timelinePlayer->observeSpeed()->get();
        }

        timeline::Playback TimelinePlayer::playback() const
        {
            return _p->timelinePlayer->observePlayback()->get();
        }

        timeline::Loop TimelinePlayer::loop() const
        {
            return _p->timelinePlayer->observeLoop()->get();
        }

        const otime::RationalTime& TimelinePlayer::currentTime() const
        {
            return _p->timelinePlayer->observeCurrentTime()->get();
        }

        const otime::TimeRange& TimelinePlayer::inOutRange() const
        {
            return _p->timelinePlayer->observeInOutRange()->get();
        }

        int TimelinePlayer::videoLayer() const
        {
            return _p->timelinePlayer->observeVideoLayer()->get();
        }

        const timeline::Frame& TimelinePlayer::frame() const
        {
            return _p->timelinePlayer->observeFrame()->get();
        }

        int TimelinePlayer::frameCacheReadAhead()
        {
            return _p->timelinePlayer->getFrameCacheReadAhead();
        }

        int TimelinePlayer::frameCacheReadBehind()
        {
            return _p->timelinePlayer->getFrameCacheReadBehind();
        }

        const std::vector<otime::TimeRange>& TimelinePlayer::cachedFrames() const
        {
            return _p->timelinePlayer->observeCachedFrames()->get();
        }

        int TimelinePlayer::requestCount() const
        {
            return _p->timelinePlayer->getRequestCount();
        }

        int TimelinePlayer::requestTimeout() const
        {
            return _p->timelinePlayer->getRequestTimeout().count();
        }

        void TimelinePlayer::setSpeed(float value)
        {
            _p->timelinePlayer->setSpeed(value);
        }

        void TimelinePlayer::setPlayback(timeline::Playback value)
        {
            _p->timelinePlayer->setPlayback(value);
        }

        void TimelinePlayer::stop()
        {
            _p->timelinePlayer->setPlayback(timeline::Playback::Stop);
        }

        void TimelinePlayer::forward()
        {
            _p->timelinePlayer->setPlayback(timeline::Playback::Forward);
        }

        void TimelinePlayer::reverse()
        {
            _p->timelinePlayer->setPlayback(timeline::Playback::Reverse);
        }

        void TimelinePlayer::togglePlayback()
        {
            _p->timelinePlayer->setPlayback(
                timeline::Playback::Stop == _p->timelinePlayer->observePlayback()->get() ?
                timeline::Playback::Forward :
                timeline::Playback::Stop);
        }

        void TimelinePlayer::setLoop(timeline::Loop value)
        {
            _p->timelinePlayer->setLoop(value);
        }

        void TimelinePlayer::seek(const otime::RationalTime& value)
        {
            _p->timelinePlayer->seek(value);
        }

        void TimelinePlayer::timeAction(timeline::TimeAction value)
        {
            _p->timelinePlayer->timeAction(value);
        }

        void TimelinePlayer::start()
        {
            _p->timelinePlayer->start();
        }

        void TimelinePlayer::end()
        {
            _p->timelinePlayer->end();
        }

        void TimelinePlayer::framePrev()
        {
            _p->timelinePlayer->framePrev();
        }

        void TimelinePlayer::frameNext()
        {
            _p->timelinePlayer->frameNext();
        }

        void TimelinePlayer::setInOutRange(const otime::TimeRange& value)
        {
            _p->timelinePlayer->setInOutRange(value);
        }

        void TimelinePlayer::setInPoint()
        {
            _p->timelinePlayer->setInPoint();
        }

        void TimelinePlayer::resetInPoint()
        {
            _p->timelinePlayer->resetInPoint();
        }

        void TimelinePlayer::setOutPoint()
        {
            _p->timelinePlayer->setOutPoint();
        }

        void TimelinePlayer::resetOutPoint()
        {
            _p->timelinePlayer->resetOutPoint();
        }

        void TimelinePlayer::setVideoLayer(int value)
        {
            _p->timelinePlayer->setVideoLayer(math::clamp(value, 0, static_cast<int>(std::numeric_limits<uint16_t>::max())));
        }

        void TimelinePlayer::setFrameCacheReadAhead(int value)
        {
            _p->timelinePlayer->setFrameCacheReadAhead(std::max(0, value));
        }

        void TimelinePlayer::setFrameCacheReadBehind(int value)
        {
            _p->timelinePlayer->setFrameCacheReadBehind(std::max(0, value));
        }

        void TimelinePlayer::setRequestCount(int value)
        {
            _p->timelinePlayer->setRequestCount(std::max(0, value));
        }

        void TimelinePlayer::setRequestTimeout(int value)
        {
            _p->timelinePlayer->setRequestTimeout(std::chrono::milliseconds(std::max(0, value)));
        }

        void TimelinePlayer::timerEvent(QTimerEvent*)
        {
            _p->timelinePlayer->tick();
        }
    }
}

