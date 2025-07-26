#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class BezierCurve
{
private:
    sf::Vector2f p0, p1, p2; // Control points
    std::vector<sf::Vector2f> curvePoints;
    std::vector<sf::Vector2f> tangents;

public:
    BezierCurve(sf::Vector2f start, sf::Vector2f control, sf::Vector2f end);

    // Calculate point on Bezier curve at parameter t (0.0 to 1.0)
    sf::Vector2f getPoint(float t);

    // Calculate tangent (direction) at parameter t
    sf::Vector2f getTangent(float t);

    // Generate points along the curve
    void generatePoints(int numSegments);

    // Get the generated points
    const std::vector<sf::Vector2f> &getPoints() const;

    // Get the tangents at each point
    const std::vector<sf::Vector2f> &getTangents() const;
};