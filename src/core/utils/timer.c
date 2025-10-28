#pragma once

#include "../types.c"
#include "../raylib.h"

typedef struct Timer
{
	f32 m_time;
	bool m_started : 1;
	bool m_finished : 1;
	bool m_paused : 1;
} Timer;

Timer TimerInit(const f32 _duration)
{
	Timer timer;
	timer.m_time = _duration;
	timer.m_finished = false;
	timer.m_paused = false;
	timer.m_started = false;

	return timer;
}

void TimerStart(Timer* _timer)
{
	_timer->m_started = true;
}

bool TimerIsStarted(Timer* _timer)
{
	return _timer->m_started;
}

bool TimerIsFinished(Timer* _timer)
{
	return _timer->m_finished;
}

bool TimerIsPaused(Timer* _timer)
{
	return _timer->m_paused;
}

void TimerAddDuration(Timer* _timer, const f32 _duration)
{
	_timer->m_time += _duration;
	_timer->m_finished = false;
}

void TimerUpdate(Timer* _timer, const f32 _dt)
{
	if (_timer->m_started == false)
		return;

	if (_timer->m_paused)
		return;

	if (_timer->m_finished)
		return;

	_timer->m_time -= _dt;

	if (_timer->m_time < 0.f)
	{
		_timer->m_time = 0.f;
		_timer->m_finished = true;
	}
}