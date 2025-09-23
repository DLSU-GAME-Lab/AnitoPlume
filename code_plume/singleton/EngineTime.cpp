#include "EngineTime.hpp"
#include "EngineTime.hpp"

EngineTime* EngineTime::sharedInstance = NULL;

EngineTime* EngineTime::getInstance()
{
    return sharedInstance;
}

void EngineTime::initialize()
{
    sharedInstance = new EngineTime();
}

void EngineTime::destroy()
{
    delete sharedInstance;
}

double EngineTime::getTime() const
{
    return this->time;
}

double EngineTime::getDeltaTime() const
{
    return this->deltaTime;
}

double EngineTime::getUnscaledTime() const
{
    return this->unscaledTime;
}

double EngineTime::getUnscaledDeltaTime() const
{
    return this->unscaledDeltaTime;
}

void EngineTime::setTimeScale(double timeScale)
{
    this->timeScale = timeScale;
}

double EngineTime::getTimeScale()
{
    return this->timeScale;
}

EngineTime::EngineTime()
{
    this->lastTime = std::chrono::system_clock::now();
}

EngineTime::~EngineTime()
{

}

void EngineTime::logFrame()
{
    this->lastTime = this->currentTime;
    this->currentTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsedSeconds = this->currentTime - this->lastTime;

    this->unscaledDeltaTime = elapsedSeconds.count();
    this->deltaTime = elapsedSeconds.count() * this->timeScale;

    this->time += this->deltaTime;
    this->unscaledTime += this->unscaledDeltaTime;

    //std::cout << "Unscaled time is " << this->unscaledTime << "\n";
    //std::cout << "Frame update finished in " << this->deltaTime << "\n";
}
