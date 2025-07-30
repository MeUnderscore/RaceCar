#include "TimerRenderer.h"

TimerRenderer::TimerRenderer(TimerLogic& timer)
    : timerLogic(timer), timerText(nullptr)
{
    // Load the font from the Fonts directory
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
}

TimerRenderer::~TimerRenderer()
{
    if (timerText)
    {
        delete timerText;
    }
}

void TimerRenderer::draw(sf::RenderWindow& window) const
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
        // Update the text with current time
        timerText->setString(timerLogic.getTimeString());
        
        // Position the text in the center of the background
        timerText->setPosition(sf::Vector2f(
            (window.getSize().x - 100) / 2.0f,
            30.0f));
        window.draw(*timerText);
    }
    else
    {
        // Fallback to colored rectangles if text is not available
        drawTimeVisualization(window, timerLogic.getTimeString());
    }
}

void TimerRenderer::drawTimeVisualization(sf::RenderWindow& window, const std::string& timeStr) const
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