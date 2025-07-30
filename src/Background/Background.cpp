#include "Background.h"

Background::Background(unsigned int width, unsigned int height, unsigned int grid)
    : windowWidth(width), windowHeight(height), gridSize(grid)
{
}

void Background::draw(sf::RenderWindow& window) const
{
    // Clear the window with a dark gray color
    window.clear(sf::Color(64, 64, 64));

    // Draw a grid pattern
    sf::RectangleShape line;
    line.setFillColor(sf::Color(96, 96, 96));

    // Draw vertical lines
    for (unsigned int x = 0; x <= windowWidth; x += gridSize)
    {
        line.setSize(sf::Vector2f(1, windowHeight));
        line.setPosition(sf::Vector2f(x, 0));
        window.draw(line);
    }

    // Draw horizontal lines
    for (unsigned int y = 0; y <= windowHeight; y += gridSize)
    {
        line.setSize(sf::Vector2f(windowWidth, 1));
        line.setPosition(sf::Vector2f(0, y));
        window.draw(line);
    }
}

void Background::setWindowSize(unsigned int width, unsigned int height)
{
    windowWidth = width;
    windowHeight = height;
}