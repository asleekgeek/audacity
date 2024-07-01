#include "playcursorcontroller.h"

using namespace au::projectscene;
using namespace muse::actions;

//! TODO Make better name
constexpr double INSURE_VISIBLE_GAP_PX(16);
constexpr double AUTO_SHIFT_PERCENT(0.75);

PlayCursorController::PlayCursorController(QObject* parent)
    : QObject(parent)
{
}

au::context::IPlaybackStatePtr PlayCursorController::playbackState() const
{
    return globalContext()->playbackState();
}

void PlayCursorController::init()
{
    playbackState()->playbackPositionChanged().onReceive(this, [this](audio::secs_t secs) {
        updatePositionX(secs);
    });
}

void PlayCursorController::seekToX(double x)
{
    double secs = m_context->positionToTime(x);
    dispatcher()->dispatch("playback_seek", ActionData::make_arg1<double>(secs));
}

void PlayCursorController::insureVisible(audio::secs_t pos)
{
    // move to play cursor position
    if (pos < m_context->frameStartTime()) {
        m_context->moveToFrameTime(pos);
    } else {
        // auto shift
        double endGapPx = m_context->zoom() * (m_context->frameEndTime() - pos);
        if (endGapPx < INSURE_VISIBLE_GAP_PX) {
            double frameTime = m_context->frameEndTime() - m_context->frameStartTime();
            m_context->shiftFrameTime(frameTime * AUTO_SHIFT_PERCENT);
        }
    }
}

void PlayCursorController::updatePositionX(audio::secs_t secs)
{
    insureVisible(secs);

    m_positionX = m_context->timeToPosition(secs);
    emit positionXChanged();
}

void PlayCursorController::onFrameTimeChanged()
{
    m_positionX = m_context->timeToPosition(playbackState()->playbackPosition());
    emit positionXChanged();
}

double PlayCursorController::positionX() const
{
    return m_positionX;
}

TimelineContext* PlayCursorController::timelineContext() const
{
    return m_context;
}

void PlayCursorController::setTimelineContext(TimelineContext* newContext)
{
    if (m_context == newContext) {
        return;
    }

    if (m_context) {
        disconnect(m_context, nullptr, this, nullptr);
    }

    m_context = newContext;

    if (m_context) {
        connect(m_context, &TimelineContext::frameTimeChanged, this, &PlayCursorController::onFrameTimeChanged);
    }

    emit timelineContextChanged();
}
