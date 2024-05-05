#pragma once

class cHiResTimer
{
public:
	cHiResTimer(unsigned int numberOfSamples);

	double GetFrameTime(void);

	void SetMaxDelaTime(double newMaxDeltaTime);

private:
	double mLastTime;
	double mMaxDeltaTime;

	static const double MAX_DELTA_TIME_DEFAULT;

	unsigned int mNextBufferIndex = 0;

	std::vector<double> deltaTimeList;

	cHiResTimer();
};