#include "Checkpoint.h"
#include <iostream>

Checkpoint::Checkpoint(const std::vector<sf::Vector2f> &cornerPositions, int number)
    : corners(cornerPositions), isHit(false), checkpointNumber(number)
{
    if (corners.size() != 4)
    {
        std::cout << "Warning: Checkpoint " << number << " must have exactly 4 corners!" << std::endl;
    }
}

bool Checkpoint::isPointInside(const sf::Vector2f &point) const
{
    if (corners.size() != 4)
        return false;

    // Simple bounding box check for now
    // Calculate the bounding box of the checkpoint
    float minX = std::min({corners[0].x, corners[1].x, corners[2].x, corners[3].x});
    float maxX = std::max({corners[0].x, corners[1].x, corners[2].x, corners[3].x});
    float minY = std::min({corners[0].y, corners[1].y, corners[2].y, corners[3].y});
    float maxY = std::max({corners[0].y, corners[1].y, corners[2].y, corners[3].y});

    return (point.x >= minX && point.x <= maxX && point.y >= minY && point.y <= maxY);
}

bool Checkpoint::getIsHit() const
{
    return isHit;
}

void Checkpoint::markAsHit()
{
    isHit = true;
}

int Checkpoint::getCheckpointNumber() const
{
    return checkpointNumber;
}

sf::Vector2f Checkpoint::getCenter() const
{
    if (corners.size() != 4)
        return sf::Vector2f(0, 0);

    // Calculate the center as the average of all corners
    sf::Vector2f center(0, 0);
    for (const auto &corner : corners)
    {
        center += corner;
    }
    center /= static_cast<float>(corners.size());

    return center;
}

void Checkpoint::draw(sf::RenderWindow &window) const
{
    if (corners.size() != 4)
        return;

    // Draw the checkpoint as a rectangle
    sf::RectangleShape rect;

    // Calculate bounding box
    float minX = std::min({corners[0].x, corners[1].x, corners[2].x, corners[3].x});
    float maxX = std::max({corners[0].x, corners[1].x, corners[2].x, corners[3].x});
    float minY = std::min({corners[0].y, corners[1].y, corners[2].y, corners[3].y});
    float maxY = std::max({corners[0].y, corners[1].y, corners[2].y, corners[3].y});

    rect.setSize(sf::Vector2f(maxX - minX, maxY - minY));
    rect.setPosition(sf::Vector2f(minX, minY));

    // Color based on hit status
    if (isHit)
    {
        rect.setFillColor(sf::Color(0, 255, 0, 100)); // Green with transparency
        rect.setOutlineColor(sf::Color::Green);
    }
    else
    {
        rect.setFillColor(sf::Color(255, 255, 0, 100)); // Yellow with transparency
        rect.setOutlineColor(sf::Color::Yellow);
    }

    rect.setOutlineThickness(2.0f);
    window.draw(rect);
}

void Checkpoint::reset()
{
    isHit = false;
}