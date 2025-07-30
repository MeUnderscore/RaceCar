#pragma once
#include <string>

// Interface for timer functionality
class ITimer
{
public:
    virtual ~ITimer() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void reset() = 0;
    virtual void update() = 0;
    virtual bool isRunning() const = 0;
    virtual bool hasStarted() const = 0;
    virtual float getElapsedTime() const = 0;
    virtual std::string getTimeString() const = 0;
}; 