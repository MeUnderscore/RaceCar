#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "../BezierCurve/BezierCurve.h"

class BezierShape
{
private:
    std::vector<sf::RectangleShape> segments;
    sf::Vector2f startPoint;
    sf::Vector2f controlPoint;
    sf::Vector2f endPoint;
    float width;
    int numSegments;
    sf::Color color;

public:
    BezierShape(sf::Vector2f start, sf::Vector2f control, sf::Vector2f end, float w, int segments = 40, sf::Color c = sf::Color(32, 32, 32));

    // Draw the shape
    void draw(sf::RenderWindow &window) const;

    // Draw black circles at the edges of each rectangle
    void drawEdgeCircles(sf::RenderWindow &window) const;

    // Get the tangent at the start of the curve
    sf::Vector2f getStartTangent() const;

    // Get the tangent at the end of the curve
    sf::Vector2f getEndTangent() const;

    // Get the start point of the curve
    sf::Vector2f getStartPoint() const;

    // Get the end point of the curve
    sf::Vector2f getEndPoint() const;

    // Get the control point of the curve
    sf::Vector2f getControlPoint() const;

    // Get edge points of the curve
    std::vector<sf::Vector2f> getEdgePoints() const;

    // Get inner and outer edge points separately
    std::vector<sf::Vector2f> getInnerEdgePoints() const;
    std::vector<sf::Vector2f> getOuterEdgePoints() const;

private:
    // Generate the segments from the Bezier curve
    void generateSegments();
};