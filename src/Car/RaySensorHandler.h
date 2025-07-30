#pragma once
#include "RaySensor.h"
#include <SFML/Graphics.hpp>
#include <vector>

class RaySensorHandler
{
private:
    std::vector<RaySensor> raySensors;
    static const int NUM_RAYS = 16;

public:
    RaySensorHandler();

    // Update ray positions based on car position and rotation
    void updateRays(const sf::Vector2f &carPosition, float carRotation);

    // Check collisions with track edges and update ray colors
    void checkCollisions(const sf::Vector2f &carPosition, float carRotation,
                         const std::vector<sf::Vector2f> &innerEdgePoints,
                         const std::vector<sf::Vector2f> &outerEdgePoints);

    // Draw all ray sensors
    void draw(sf::RenderWindow &window) const;

    // Get ray lengths for AI input
    std::vector<float> getRayLengths() const;

private:
    // Helper method to check if a point is within 5 pixels of any edge point
    bool isPointNearEdge(const sf::Vector2f &point,
                         const std::vector<sf::Vector2f> &edgePoints) const;

    // Helper method to check if a ray intersects with edge line segments
    bool checkRayIntersectsEdges(const sf::Vector2f &rayStart, const sf::Vector2f &rayEnd,
                                 const std::vector<sf::Vector2f> &edgePoints) const;

    // Helper method to check if two line segments intersect
    bool lineSegmentsIntersect(const sf::Vector2f &a1, const sf::Vector2f &a2,
                               const sf::Vector2f &b1, const sf::Vector2f &b2) const;
};