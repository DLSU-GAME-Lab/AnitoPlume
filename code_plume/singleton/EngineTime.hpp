#pragma once
#include <iostream>
#include <chrono>
#include <ctime>

class Window;

class EngineTime
{
public:
	static EngineTime* getInstance();
	static void initialize();
	static void destroy();

	double getTime() const;
	double getDeltaTime() const;
	double getUnscaledTime() const;
	double getUnscaledDeltaTime() const;
	void setTimeScale(double timeScale);

private:
	EngineTime();
	~EngineTime();
	EngineTime(EngineTime const&) {};
	EngineTime& operator=(EngineTime const&) {};

	static EngineTime* sharedInstance;

	std::chrono::system_clock::time_point lastTime;
	std::chrono::system_clock::time_point currentTime;

	double time = 0.0;
	double deltaTime = 0.0;
	double unscaledTime = 0.0;
	double unscaledDeltaTime = 0.0;
	double timeScale = 1.0;

	void logFrame();

	friend class AnitoPlume;
};

