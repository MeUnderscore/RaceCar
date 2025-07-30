#include "Timer.h"
#include <sstream>
#include <iomanip>
#include <iostream>

Timer::Timer()
{
    // Load the font from the Fonts directory
    // The program runs from build/Release, so we need to go up to the root directory
    if (!font.openFromFile("../../Fonts/ARIAL.TTF"))
    {
        // Fallback to colored rectangles if font loading fails
        timerText = nullptr;
    }
    else
    {
        timerText = new sf::Text(font, "00:00.000", 24);
        timerText->setFillColor(sf::Color::White);
    }
    {
        // Initialize timer text with the loaded font
        timerText = new sf::Text(font, "00:00.000", 24);
        timerText->setFillColor(sf::Color::White);
    }

    // Initialize timer state
    isRunning = false;
    hasStarted = false;
    elapsedTime = 0.0f;
    timeString = "00:00.000";
}

Timer::~Timer()
{
    if (timerText)
    {
        delete timerText;
    }
}

void Timer::start()
{
    if (!hasStarted)
    {
        hasStarted = true;
        isRunning = true;
        clock.restart();
    }
}

void Timer::stop()
{
    isRunning = false;
}

void Timer::reset()
{
    isRunning = false;
    hasStarted = false;
    elapsedTime = 0.0f;
    clock.restart();
    timeString = "00:00.000";
    if (timerText)
    {
        timerText->setString("00:00.000");
    }
}

void Timer::update()
{
    if (isRunning)
    {
        float newElapsedTime = clock.getElapsedTime().asSeconds();

        // Only update if time has actually changed (reduces string operations)
        if (newElapsedTime != elapsedTime)
        {
            elapsedTime = newElapsedTime;
            timeString = formatTime(elapsedTime);
            if (timerText)
            {
                timerText->setString(timeString);
            }
        }
    }
}

void Timer::draw(sf::RenderWindow &window)
{
    // Create a simple visual timer using rectangles
    // Draw a background rectangle for the timer
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(200, 40));
    background.setPosition(sf::Vector2f(
        (window.getSize().x - 200) / 2.0f,
        20.0f));
    background.setFillColor(sf::Color(0, 0, 0, 128)); // Semi-transparent black
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(2.0f);

    window.draw(background);

    // Draw the text if available
    if (timerText)
    {
        // Position the text in the center of the background
        timerText->setPosition(sf::Vector2f(
            (window.getSize().x - 100) / 2.0f,
            30.0f));
        window.draw(*timerText);
    }
    else
    {
        // Fallback to colored rectangles if text is not available
        drawTimeVisualization(window, timeString);
    }
}

bool Timer::getHasStarted() const
{
    return hasStarted;
}

float Timer::getElapsedTime() const
{
    return elapsedTime;
}

std::string Timer::formatTime(float timeInSeconds) const
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

std::string Timer::getTimeString() const
{
    return formatTime(elapsedTime);
}

void Timer::drawTimeVisualization(sf::RenderWindow &window, const std::string &timeStr)
{
    // Create a simple visual representation using colored rectangles
    // Each digit will be represented by a colored rectangle
    float startX = (window.getSize().x - 200) / 2.0f + 10.0f;
    float startY = 30.0f;
    float digitWidth = 15.0f;
    float digitHeight = 20.0f;
    float spacing = 5.0f;

    // Draw each character as a colored rectangle
    for (size_t i = 0; i < timeStr.length(); ++i)
    {
        char c = timeStr[i];
        sf::RectangleShape digitRect;
        digitRect.setSize(sf::Vector2f(digitWidth, digitHeight));
        digitRect.setPosition(sf::Vector2f(startX + i * (digitWidth + spacing), startY));

        // Color based on the character
        if (c == ':')
        {
            digitRect.setFillColor(sf::Color::Yellow);
        }
        else if (c == '.')
        {
            digitRect.setFillColor(sf::Color::Red);
        }
        else if (c >= '0' && c <= '9')
        {
            // Different colors for different digits
            int digit = c - '0';
            sf::Color colors[] = {
                sf::Color::Green, sf::Color::Blue, sf::Color::Cyan,
                sf::Color::Magenta, sf::Color::Yellow, sf::Color::Red,
                sf::Color::White, sf::Color::Green, sf::Color::Blue, sf::Color::Cyan};
            digitRect.setFillColor(colors[digit]);
        }
        else
        {
            digitRect.setFillColor(sf::Color::White);
        }

        window.draw(digitRect);
    }
}