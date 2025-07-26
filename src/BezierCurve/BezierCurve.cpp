#include "BezierCurve.h"
#include <cmath>

BezierCurve::BezierCurve(sf::Vector2f start, sf::Vector2f control, sf::Vector2f end)
    : p0(start), p1(control), p2(end)
{
}

sf::Vector2f BezierCurve::getPoint(float t)
{
    // Quadratic Bezier curve formula: B(t) = (1-t)²P₀ + 2(1-t)tP₁ + t²P₂
    float oneMinusT = 1.0f - t;
    float oneMinusTSquared = oneMinusT * oneMinusT;
    float tSquared = t * t;

    sf::Vector2f point;
    point.x = oneMinusTSquared * p0.x + 2.0f * oneMinusT * t * p1.x + tSquared * p2.x;
    point.y = oneMinusTSquared * p0.y + 2.0f * oneMinusT * t * p1.y + tSquared * p2.y;

    return point;
}

sf::Vector2f BezierCurve::getTangent(float t)
{
    // Derivative of quadratic Bezier curve: B'(t) = 2(1-t)(P₁-P₀) + 2t(P₂-P₁)
    float oneMinusT = 1.0f - t;

    sf::Vector2f tangent;
    tangent.x = 2.0f * oneMinusT * (p1.x - p0.x) + 2.0f * t * (p2.x - p1.x);
    tangent.y = 2.0f * oneMinusT * (p1.y - p0.y) + 2.0f * t * (p2.y - p1.y);

    // Normalize the tangent vector
    float length = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
    if (length > 0.0f)
    {
        tangent.x /= length;
        tangent.y /= length;
    }

    return tangent;
}

void BezierCurve::generatePoints(int numSegments)
{
    curvePoints.clear();
    tangents.clear();

    for (int i = 0; i <= numSegments; i++)
    {
        float t = static_cast<float>(i) / static_cast<float>(numSegments);
        curvePoints.push_back(getPoint(t));
        tangents.push_back(getTangent(t));
    }
}

const std::vector<sf::Vector2f> &BezierCurve::getPoints() const
{
    return curvePoints;
}

const std::vector<sf::Vector2f> &BezierCurve::getTangents() const
{
    return tangents;
}