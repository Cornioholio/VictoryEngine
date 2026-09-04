#pragma once
#include "Core/Common/CoreMinimal.h"
class VICTORY_API Timer
{
public:
	Timer();

	float TotalTime() const; // Total execution time (seconds)
	float DeltaTime() const; // Time elapsed between frames

	void Reset(); // Call before game loop
	void Start(); // Call when unpausing game
	void Stop(); // Call when pausing game
	void Tick(); // Call once per frame

private:
	double secondsPerCount_;
	double deltaTime_;

	LARGE_INTEGER baseTime_;
	LARGE_INTEGER pausedTime_;
	LARGE_INTEGER stopTime_;
	LARGE_INTEGER prevTime_;
	LARGE_INTEGER currTime_;

	bool stopped_;

};


