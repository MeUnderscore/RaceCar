#pragma once
#include "../Interfaces/ITimer.h"
#include <SFML/System.hpp>

// Timer logic class - handles only timing calculations
class TimerLogic : public ITimer
{
private:
    sf::Clock clock;
    bool running;
    bool started;
    float elapsedTime;
    std::string timeString;

public:
    TimerLogic();
    
    // ITimer interface implementation
    void start() override;
    void stop() override;
    void reset() override;
    void update() override;
    bool isRunning() const override;
    bool hasStarted() const override;
    float getElapsedTime() const override;
    std::string getTimeString() const override;

private:
    std::string formatTime(float timeInSeconds) const;
}; 