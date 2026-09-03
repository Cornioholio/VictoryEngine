#include "Timer.h"

Timer::Timer() : secondsPerCount_(0.0), deltaTime_(-1.0), stopped_(false) 
{
	LARGE_INTEGER countsPerSec;
	QueryPerformanceFrequency(&countsPerSec);
	secondsPerCount_ = 1.0 / static_cast<double>(countsPerSec.QuadPart);

	baseTime_.QuadPart = 0;
	pausedTime_.QuadPart = 0;
	stopTime_.QuadPart = 0;
	prevTime_.QuadPart = 0;
	currTime_.QuadPart = 0;
}

float Timer::TotalTime() const 
{
	if(stopped_) 
	{
		return static_cast<float>(((stopTime_.QuadPart - pausedTime_.QuadPart) - baseTime_.QuadPart) * secondsPerCount_);
	} else 
	{
		return static_cast<float>(((currTime_.QuadPart - pausedTime_.QuadPart) - baseTime_.QuadPart) * secondsPerCount_);
	}
}

float Timer::DeltaTime() const
{
	return static_cast<float>(deltaTime_);
}

void Timer::Reset() 
{
	LARGE_INTEGER currTime;
	QueryPerformanceCounter(&currTime);

	baseTime_ = currTime;
	stopTime_ = currTime;
	prevTime_ = currTime;
	stopped_ = false;
}

void Timer::Start() 
{
	LARGE_INTEGER startTime;
	QueryPerformanceCounter(&startTime);

	if(!stopped_) 
	{
		pausedTime_.QuadPart += (startTime.QuadPart - stopTime_.QuadPart);
	}
}

void Timer::Stop() 
{
	if(!stopped_) 
	{
		LARGE_INTEGER currTime;
		QueryPerformanceCounter(&currTime);

		stopTime_ = currTime;
		stopped_ = true;
	}
}

void Timer::Tick() 
{
	if(stopped_) 
	{
		deltaTime_ = 0.0;
		return;
	}

	LARGE_INTEGER currTime;
	QueryPerformanceCounter(&currTime);
	currTime_ = currTime;

	// Calculate frame dt
	deltaTime_ = (currTime_.QuadPart - prevTime_.QuadPart) * secondsPerCount_;
	prevTime_ = currTime_;

	// Prevent negative values
	if(deltaTime_ < 0.0) 
	{
		deltaTime_ = 0.0;
	}
}