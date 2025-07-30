#include "TimerLogic.h"
#include <sstream>
#include <iomanip>

TimerLogic::TimerLogic()
    : running(false), started(false), elapsedTime(0.0f), timeString("00:00.000")
{
}

void TimerLogic::start()
{
    if (!started)
    {
        started = true;
        running = true;
        clock.restart();
    }
}

void TimerLogic::stop()
{
    running = false;
}

void TimerLogic::reset()
{
    running = false;
    started = false;
    elapsedTime = 0.0f;
    clock.restart();
    timeString = "00:00.000";
}

void TimerLogic::update()
{
    if (running)
    {
        float newElapsedTime = clock.getElapsedTime().asSeconds();
        
        // Only update if time has actually changed (reduces string operations)
        if (newElapsedTime != elapsedTime)
        {
            elapsedTime = newElapsedTime;
            timeString = formatTime(elapsedTime);
        }
    }
}

bool TimerLogic::isRunning() const
{
    return running;
}

bool TimerLogic::hasStarted() const
{
    return started;
}

float TimerLogic::getElapsedTime() const
{
    return elapsedTime;
}

std::string TimerLogic::getTimeString() const
{
    return timeString;
}

std::string TimerLogic::formatTime(float timeInSeconds) const
{
    int minutes = static_cast<int>(timeInSeconds) / 60;
    int seconds = static_cast<int>(timeInSeconds) % 60;
    int milliseconds = static_cast<int>((timeInSeconds - static_cast<int>(timeInSeconds)) * 1000);

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds << "."
        << std::setfill('0') << std::setw(3) << milliseconds;

    return oss.str();
} 