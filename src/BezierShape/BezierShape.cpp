#include "BezierShape.h"
#include <cmath>

BezierShape::BezierShape(sf::Vector2f start, sf::Vector2f control, sf::Vector2f end, float w, int segments, sf::Color c)
    : startPoint(start), controlPoint(control), endPoint(end), width(w), numSegments(segments), color(c)
{
    generateSegments();
}

void BezierShape::draw(sf::RenderWindow &window) const
{
    for (const auto &segment : segments)
    {
        window.draw(segment);
    }
}

sf::Vector2f BezierShape::getStartTangent() const
{
    // Create a temporary BezierCurve to get the tangent at t=0
    BezierCurve curve(startPoint, controlPoint, endPoint);
    return curve.getTangent(0.0f);
}

sf::Vector2f BezierShape::getEndTangent() const
{
    // Create a temporary BezierCurve to get the tangent at t=1
    BezierCurve curve(startPoint, controlPoint, endPoint);
    return curve.getTangent(1.0f);
}

sf::Vector2f BezierShape::getStartPoint() const
{
    return startPoint;
}

sf::Vector2f BezierShape::getEndPoint() const
{
    return endPoint;
}

void BezierShape::generateSegments()
{
    segments.clear();

    // Create Bezier curve
    BezierCurve curve(startPoint, controlPoint, endPoint);
    curve.generatePoints(numSegments);

    const std::vector<sf::Vector2f> &points = curve.getPoints();
    const std::vector<sf::Vector2f> &tangents = curve.getTangents();

    // Create rectangle segments along the curve with overlap
    for (size_t i = 0; i < points.size() - 1; i++)
    {
        sf::RectangleShape segment;

        // Calculate segment length (distance between consecutive points)
        sf::Vector2f currentPoint = points[i];
        sf::Vector2f nextPoint = points[i + 1];
        float segmentLength = std::sqrt(
            (nextPoint.x - currentPoint.x) * (nextPoint.x - currentPoint.x) +
            (nextPoint.y - currentPoint.y) * (nextPoint.y - currentPoint.y));

        // Set segment size
        segment.setSize({segmentLength * 1.8f, width});

        // Calculate perpendicular direction for width
        sf::Vector2f tangent = tangents[i];
        sf::Vector2f perpendicular(-tangent.y, tangent.x); // Rotate 90 degrees

        // Calculate segment position (center of the segment)
        sf::Vector2f segmentCenter = currentPoint;
        segment.setPosition({segmentCenter.x - segmentLength / 2.0f, segmentCenter.y - width / 2.0f});

        // Calculate rotation angle
        float angle = std::atan2(tangent.y, tangent.x) * 180.0f / 3.14159f;
        segment.setRotation(sf::degrees(angle));

        // Set segment appearance
        // Make first and last rectangles red, others use the normal color
        if (i == 0 || i == points.size() - 2)
        {
            segment.setFillColor(color);
        }
        else
        {
            segment.setFillColor(color);
        }
        segment.setOrigin({segmentLength / 2.0f, width / 2.0f}); // Set origin to center for proper rotation

        segments.push_back(segment);
    }
}