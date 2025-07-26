#include "Background.h"

Background::Background(unsigned int width, unsigned int height, unsigned int grid)
    : windowWidth(width), windowHeight(height), gridSize(grid)
{
    // No need for RenderTexture, we'll draw directly
}

void Background::draw(sf::RenderWindow &window)
{
    // Clear with dark grey background
    window.clear(sf::Color(64, 64, 64)); // Dark grey
    
    

    // Draw the grid pattern
    sf::RectangleShape line;
    line.setFillColor(sf::Color(76, 76, 76)); // Light grey

    // Draw vertical lines (make them thicker)
    for (unsigned int x = 0; x <= windowWidth; x += gridSize)
    {
        line.setSize(sf::Vector2f(2.0f, static_cast<float>(windowHeight))); // 2 pixels thick
        line.setPosition(sf::Vector2f(static_cast<float>(x), 0.0f));
        window.draw(line);
    }

    // Draw horizontal lines (make them thicker)
    for (unsigned int y = 0; y <= windowHeight; y += gridSize)
    {
        line.setSize(sf::Vector2f(static_cast<float>(windowWidth), 2.0f)); // 2 pixels thick
        line.setPosition(sf::Vector2f(0.0f, static_cast<float>(y)));
        window.draw(line);
    }

    // Draw Edges
    sf::RectangleShape edge;
    edge.setFillColor(sf::Color(0, 0, 0)); // Black

    // Top Edge
    edge.setSize(sf::Vector2f(windowWidth, 4.0f));
    edge.setPosition(sf::Vector2f(0.0f, 0.0f));
    window.draw(edge);

    // Left Edge
    edge.setSize(sf::Vector2f(4.0f, windowHeight));
    edge.setPosition(sf::Vector2f(0.0f, 0.0f));
    window.draw(edge);

    // Bottom Edge
    edge.setSize(sf::Vector2f(windowWidth, 4.0f));
    edge.setPosition(sf::Vector2f(0.0f, windowHeight - 4.0f));
    window.draw(edge);

    // Right Edge
    edge.setSize(sf::Vector2f(4.0f, windowHeight));
    edge.setPosition(sf::Vector2f(windowWidth - 4.0f, 0.0f));
    window.draw(edge);
    
    
}

void Background::setWindowSize(unsigned int width, unsigned int height)
{
    windowWidth = width;
    windowHeight = height;
}