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

    // Quick bounding box check first (much faster)
    float minX = std::min({corners[0].x, corners[1].x, corners[2].x, corners[3].x});
    float maxX = std::max({corners[0].x, corners[1].x, corners[2].x, corners[3].x});
    float minY = std::min({corners[0].y, corners[1].y, corners[2].y, corners[3].y});
    float maxY = std::max({corners[0].y, corners[1].y, corners[2].y, corners[3].y});

    // If point is outside bounding box, it's definitely not inside
    if (point.x < minX || point.x > maxX || point.y < minY || point.y > maxY)
        return false;

    // Point-in-polygon test using ray casting algorithm (only if bounding box check passes)
    bool inside = false;
    int j = corners.size() - 1;

    for (int i = 0; i < corners.size(); i++)
    {
        if (((corners[i].y > point.y) != (corners[j].y > point.y)) &&
            (point.x < (corners[j].x - corners[i].x) * (point.y - corners[i].y) / (corners[j].y - corners[i].y) + corners[i].x))
        {
            inside = !inside;
        }
        j = i;
    }

    return inside;
}

bool Checkpoint::isLineIntersecting(const sf::Vector2f &start, const sf::Vector2f &end) const
{
    if (corners.size() != 4)
        return false;

    // Quick bounding box check first
    float minX = std::min({corners[0].x, corners[1].x, corners[2].x, corners[3].x});
    float maxX = std::max({corners[0].x, corners[1].x, corners[2].x, corners[3].x});
    float minY = std::min({corners[0].y, corners[1].y, corners[2].y, corners[3].y});
    float maxY = std::max({corners[0].y, corners[1].y, corners[2].y, corners[3].y});

    // Check if line segment intersects with bounding box
    if (start.x < minX && end.x < minX) return false;
    if (start.x > maxX && end.x > maxX) return false;
    if (start.y < minY && end.y < minY) return false;
    if (start.y > maxY && end.y > maxY) return false;

    // Check if either endpoint is inside the polygon
    if (isPointInside(start) || isPointInside(end))
        return true;

    // Check if line segment intersects with any edge of the polygon
    for (size_t i = 0; i < corners.size(); ++i)
    {
        size_t j = (i + 1) % corners.size();
        if (lineSegmentsIntersect(start, end, corners[i], corners[j]))
            return true;
    }

    return false;
}

bool Checkpoint::lineSegmentsIntersect(const sf::Vector2f &a1, const sf::Vector2f &a2, 
                                      const sf::Vector2f &b1, const sf::Vector2f &b2) const
{
    // Calculate the direction vectors
    sf::Vector2f u = a2 - a1;
    sf::Vector2f v = b2 - b1;
    sf::Vector2f w = a1 - b1;

    // Calculate the cross products
    float d = u.x * v.y - u.y * v.x;

    // If d is 0, the lines are parallel
    if (std::abs(d) < 1e-10f)
        return false;

    // Calculate the intersection parameters
    float s = (v.x * w.y - v.y * w.x) / d;
    float t = (u.x * w.y - u.y * w.x) / d;

    // Check if the intersection point is on both line segments
    return s >= 0.0f && s <= 1.0f && t >= 0.0f && t <= 1.0f;
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

sf::FloatRect Checkpoint::getBounds() const
{
    if (corners.size() != 4)
        return sf::FloatRect();

    // Calculate bounding box
    float minX = std::min({corners[0].x, corners[1].x, corners[2].x, corners[3].x});
    float maxX = std::max({corners[0].x, corners[1].x, corners[2].x, corners[3].x});
    float minY = std::min({corners[0].y, corners[1].y, corners[2].y, corners[3].y});
    float maxY = std::max({corners[0].y, corners[1].y, corners[2].y, corners[3].y});

    sf::Vector2f position(minX, minY);
    sf::Vector2f size(maxX - minX, maxY - minY);
    return sf::FloatRect(position, size);
}

void Checkpoint::draw(sf::RenderWindow &window) const
{
    if (corners.size() != 4)
        return;

    // Draw the checkpoint as a convex shape using the actual corners
    sf::ConvexShape shape;
    shape.setPointCount(4);

    // Set the four corners
    for (int i = 0; i < 4; i++)
    {
        shape.setPoint(i, corners[i]);
    }

    // Color based on hit status and checkpoint type
    if (checkpointNumber == -1)
    {
        // Final checkpoint - use blue color
        if (isHit)
        {
            shape.setFillColor(sf::Color(0, 0, 255, 0)); // Blue with full transparency
            shape.setOutlineColor(sf::Color::Blue);
        }
        else
        {
            shape.setFillColor(sf::Color(0, 0, 255, 0)); // Blue with full transparency
            shape.setOutlineColor(sf::Color::Cyan);
        }
    }
    else
    {
        // Regular checkpoint
        if (isHit)
        {
            shape.setFillColor(sf::Color(0, 255, 0, 0)); // Green with full transparency
            shape.setOutlineColor(sf::Color::Green);
        }
        else
        {
            shape.setFillColor(sf::Color(255, 255, 0, 0)); // Yellow with full transparency
            shape.setOutlineColor(sf::Color::Yellow);
        }
    }

    shape.setOutlineThickness(0.0f); // No outline
    window.draw(shape);
}

void Checkpoint::reset()
{
    isHit = false;
}