#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Timer
{
private:
    sf::Font font;
    sf::Text *timerText;
    sf::Clock clock;
    bool isRunning;
    bool hasStarted;
    float elapsedTime;      // Time in seconds
    std::string timeString; // Store the formatted time string

public:
    Timer();
    ~Timer();

    // Start the timer (called when W is pressed for the first time)
    void start();

    // Reset the timer (called when R is pressed or car hits checkpoint)
    void reset();

    // Update the timer (called each frame)
    void update();

    // Draw the timer (called each frame) - simplified for now
    void draw(sf::RenderWindow &window);

    // Get the formatted time string
    std::string getTimeString() const;

    // Check if timer has been started
    bool getHasStarted() const;

    // Get current elapsed time in seconds
    float getElapsedTime() const;

    // Format time as MM:SS.mmm
    std::string formatTime(float timeInSeconds) const;

private:
    // Draw a simple visual representation of the time
    void drawTimeVisualization(sf::RenderWindow &window, const std::string &timeStr);
};