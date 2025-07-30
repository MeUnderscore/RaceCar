#include "RaySensorHandler.h"
#include <iostream>
#include <cmath>

RaySensorHandler::RaySensorHandler()
{
    // Create 16 rays distributed with more density in the front (where the car is going)
    // Front: 0° to 90° and 270° to 360° (more rays)
    // Back: 90° to 270° (fewer rays)
    float angles[] = {
        0.0f,   // Forward
        15.0f,  // Forward-right
        30.0f,  // Forward-right
        45.0f,  // Forward-right
        60.0f,  // Right
        90.0f,  // Right
        120.0f, // Right-back
        150.0f, // Back-right
        180.0f, // Backward
        210.0f, // Back-Left
        240.0f, // Back-left
        270.0f, // Left-back
        300.0f, // Left
        315.0f, // Left-forward
        330.0f, // Forward-left
        345.0f  // Forward-left
    };

    for (int i = 0; i < NUM_RAYS; ++i)
    {
        raySensors.emplace_back(angles[i]);
    }

    // RaySensorHandler initialized silently
}

void RaySensorHandler::updateRays(const sf::Vector2f &carPosition, float carRotation)
{
    for (auto &ray : raySensors)
    {
        ray.setPosition(carPosition, carRotation);
    }
}

void RaySensorHandler::checkCollisions(const sf::Vector2f &carPosition, float carRotation,
                                       const std::vector<sf::Vector2f> &innerEdgePoints,
                                       const std::vector<sf::Vector2f> &outerEdgePoints)
{
    int redRays = 0;

    for (size_t i = 0; i < raySensors.size(); ++i)
    {
        auto &ray = raySensors[i];

        // Binary search to find the optimal ray length
        float minLength = 0.0f;
        float maxLength = ray.getMaxLength();
        float optimalLength = maxLength;

        // Start with maximum length and check if it collides
        ray.setLength(maxLength);
        sf::Vector2f endPoint = ray.getEndPoint(carPosition, carRotation);
        bool maxLengthCollides = checkRayIntersectsEdges(carPosition, endPoint, innerEdgePoints) ||
                                 checkRayIntersectsEdges(carPosition, endPoint, outerEdgePoints);

        if (maxLengthCollides)
        {
            // Ray collides at max length, use binary search to find the exact collision point
            const float tolerance = 1.0f; // 1 pixel tolerance

            while (maxLength - minLength > tolerance)
            {
                float midLength = (minLength + maxLength) / 2.0f;
                ray.setLength(midLength);
                endPoint = ray.getEndPoint(carPosition, carRotation);

                bool midCollides = checkRayIntersectsEdges(carPosition, endPoint, innerEdgePoints) ||
                                   checkRayIntersectsEdges(carPosition, endPoint, outerEdgePoints);

                if (midCollides)
                {
                    maxLength = midLength; // Collision found, search lower half
                }
                else
                {
                    minLength = midLength; // No collision, search upper half
                }
            }

            optimalLength = maxLength; // Use the length where collision occurs
        }
        else
        {
            // Ray doesn't collide at max length, keep it at max length
            optimalLength = maxLength;
        }

        // Set the ray to the optimal length
        ray.setLength(optimalLength);

        // Get final end point for collision check
        endPoint = ray.getEndPoint(carPosition, carRotation);
        bool intersectsInner = checkRayIntersectsEdges(carPosition, endPoint, innerEdgePoints);
        bool intersectsOuter = checkRayIntersectsEdges(carPosition, endPoint, outerEdgePoints);

        // Set color based on collision state
        if (intersectsInner || intersectsOuter)
        {
            ray.setColor(sf::Color::Red);
            redRays++;
        }
        else
        {
            ray.setColor(sf::Color::Cyan);
        }
    }

    // Debug output removed
}

bool RaySensorHandler::checkRayIntersectsEdges(const sf::Vector2f &rayStart, const sf::Vector2f &rayEnd,
                                               const std::vector<sf::Vector2f> &edgePoints) const
{
    // The edge points come in pairs (top and bottom of each segment)
    // We need to connect them properly to form the track boundary

    // For inner/outer edges, points come in pairs: [top1, bottom1, top2, bottom2, ...]
    // We need to connect: top1->top2->top3->... and bottom1->bottom2->bottom3->...

    if (edgePoints.size() < 4)
        return false; // Need at least 2 segments

    // Check intersection with top edge line (connecting all top points)
    for (size_t i = 0; i < edgePoints.size() - 2; i += 2)
    {
        if (lineSegmentsIntersect(rayStart, rayEnd, edgePoints[i], edgePoints[i + 2]))
        {
            return true;
        }
    }

    // Check intersection with bottom edge line (connecting all bottom points)
    for (size_t i = 1; i < edgePoints.size() - 2; i += 2)
    {
        if (lineSegmentsIntersect(rayStart, rayEnd, edgePoints[i], edgePoints[i + 2]))
        {
            return true;
        }
    }

    // Connect the last point back to the first point to close the track
    // Top edge: last top point to first top point
    if (edgePoints.size() >= 4)
    {
        size_t lastTopIndex = edgePoints.size() - 2; // Last top point
        size_t firstTopIndex = 0;                    // First top point
        if (lineSegmentsIntersect(rayStart, rayEnd, edgePoints[lastTopIndex], edgePoints[firstTopIndex]))
        {
            return true;
        }
    }

    // Bottom edge: last bottom point to first bottom point
    if (edgePoints.size() >= 4)
    {
        size_t lastBottomIndex = edgePoints.size() - 1; // Last bottom point
        size_t firstBottomIndex = 1;                    // First bottom point
        if (lineSegmentsIntersect(rayStart, rayEnd, edgePoints[lastBottomIndex], edgePoints[firstBottomIndex]))
        {
            return true;
        }
    }

    return false;
}

bool RaySensorHandler::lineSegmentsIntersect(const sf::Vector2f &a1, const sf::Vector2f &a2,
                                             const sf::Vector2f &b1, const sf::Vector2f &b2) const
{
    // Calculate direction vectors
    sf::Vector2f u = a2 - a1; // Ray direction
    sf::Vector2f v = b2 - b1; // Edge direction
    sf::Vector2f w = a1 - b1; // Vector from edge start to ray start

    // Calculate cross products
    float d = u.x * v.y - u.y * v.x;

    // If d is 0, the lines are parallel
    if (std::abs(d) < 1e-10f)
        return false;

    // Calculate intersection parameters
    float s = (v.x * w.y - v.y * w.x) / d; // Parameter for ray (0 to 1)
    float t = (u.x * w.y - u.y * w.x) / d; // Parameter for edge (0 to 1)

    // Check if intersection point is on both line segments
    // s must be between 0 and 1 for the ray segment
    // t must be between 0 and 1 for the edge segment
    return (s >= 0.0f && s <= 1.0f && t >= 0.0f && t <= 1.0f);
}

bool RaySensorHandler::isPointNearEdge(const sf::Vector2f &point,
                                       const std::vector<sf::Vector2f> &edgePoints) const
{
    const float collisionRadius = 5.0f; // 5 pixel radius

    for (const auto &edgePoint : edgePoints)
    {
        // Calculate distance between point and edge point
        float dx = point.x - edgePoint.x;
        float dy = point.y - edgePoint.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // If distance is less than collision radius, we have a collision
        if (distance <= collisionRadius)
        {
            return true;
        }
    }

    return false;
}

void RaySensorHandler::draw(sf::RenderWindow &window) const
{
    for (const auto &ray : raySensors)
    {
        ray.draw(window);
    }
}

std::vector<float> RaySensorHandler::getRayLengths() const
{
    std::vector<float> lengths;
    lengths.reserve(raySensors.size());

    for (const auto &ray : raySensors)
    {
        lengths.push_back(ray.getLength());
    }

    return lengths;
}