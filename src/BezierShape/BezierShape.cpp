#include "BezierShape.h"
#include <cmath>
#include <fstream>  // Added for file operations
#include <iostream> // Added for console output

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

void BezierShape::drawEdgeCircles(sf::RenderWindow &window) const
{
    // Create a small black circle
    sf::CircleShape circle;
    circle.setRadius(5.0f);
    circle.setFillColor(sf::Color::Black);
    circle.setOrigin({5.0f, 5.0f}); // Center the origin

    // Draw circles at each end of every 5th rectangle
    for (size_t i = 0; i < segments.size(); ++i)
    {
        if (i % 5 != 0)
            continue;

        const auto &segment = segments[i];

        // Get the rectangle's position and size
        sf::Vector2f pos = segment.getPosition();
        sf::Vector2f size = segment.getSize();
        float rotation = segment.getRotation().asRadians();

        // Calculate the four corners of the rectangle
        float cosRot = std::cos(rotation);
        float sinRot = std::sin(rotation);

        // Top-left corner
        sf::Vector2f topLeft = pos + sf::Vector2f(
                                         -size.x / 2.0f * cosRot - size.y / 2.0f * sinRot,
                                         -size.x / 2.0f * sinRot + size.y / 2.0f * cosRot);

        // Top-right corner
        sf::Vector2f topRight = pos + sf::Vector2f(
                                          size.x / 2.0f * cosRot - size.y / 2.0f * sinRot,
                                          size.x / 2.0f * sinRot + size.y / 2.0f * cosRot);

        // Bottom-left corner
        sf::Vector2f bottomLeft = pos + sf::Vector2f(
                                            -size.x / 2.0f * cosRot + size.y / 2.0f * sinRot,
                                            -size.x / 2.0f * sinRot - size.y / 2.0f * cosRot);

        // Bottom-right corner
        sf::Vector2f bottomRight = pos + sf::Vector2f(
                                             size.x / 2.0f * cosRot + size.y / 2.0f * sinRot,
                                             size.x / 2.0f * sinRot - size.y / 2.0f * cosRot);

        // Draw circles at the four corners
        circle.setPosition(topLeft);
        window.draw(circle);
        circle.setPosition(topRight);
        window.draw(circle);
        circle.setPosition(bottomLeft);
        window.draw(circle);
        circle.setPosition(bottomRight);
        window.draw(circle);
    }
}

std::vector<sf::Vector2f> BezierShape::getEdgePoints() const
{
    std::vector<sf::Vector2f> edgePoints;
    for (const auto &segment : segments)
    {
        // Get the rectangle's position and size
        sf::Vector2f pos = segment.getPosition();
        sf::Vector2f size = segment.getSize();
        float rotation = segment.getRotation().asRadians();

        // Calculate the four corners of the rectangle
        float cosRot = std::cos(rotation);
        float sinRot = std::sin(rotation);

        // Top-left corner
        sf::Vector2f topLeft = pos + sf::Vector2f(
                                         -size.x / 2.0f * cosRot - size.y / 2.0f * sinRot,
                                         -size.x / 2.0f * sinRot + size.y / 2.0f * cosRot);

        // Top-right corner
        sf::Vector2f topRight = pos + sf::Vector2f(
                                          size.x / 2.0f * cosRot - size.y / 2.0f * sinRot,
                                          size.x / 2.0f * sinRot + size.y / 2.0f * cosRot);

        // Bottom-left corner
        sf::Vector2f bottomLeft = pos + sf::Vector2f(
                                            -size.x / 2.0f * cosRot + size.y / 2.0f * sinRot,
                                            -size.x / 2.0f * sinRot - size.y / 2.0f * cosRot);

        // Bottom-right corner
        sf::Vector2f bottomRight = pos + sf::Vector2f(
                                             size.x / 2.0f * cosRot + size.y / 2.0f * sinRot,
                                             size.x / 2.0f * sinRot - size.y / 2.0f * cosRot);

        edgePoints.push_back(topLeft);
        edgePoints.push_back(topRight);
        edgePoints.push_back(bottomLeft);
        edgePoints.push_back(bottomRight);
    }
    return edgePoints;
}

std::vector<sf::Vector2f> BezierShape::getInnerEdgePoints() const
{
    std::vector<sf::Vector2f> innerEdgePoints;
    for (const auto &segment : segments)
    {
        // Get the rectangle's position and size
        sf::Vector2f pos = segment.getPosition();
        sf::Vector2f size = segment.getSize();
        float rotation = segment.getRotation().asRadians();

        // Calculate the four corners of the rectangle
        float cosRot = std::cos(rotation);
        float sinRot = std::sin(rotation);

        // For inner edge, we need to determine which corners are "inside" the track
        // This depends on the curve direction, but for now we'll use the left side
        // Top-left corner (inner edge)
        sf::Vector2f topLeft = pos + sf::Vector2f(
                                         -size.x / 2.0f * cosRot - size.y / 2.0f * sinRot,
                                         -size.x / 2.0f * sinRot + size.y / 2.0f * cosRot);

        // Bottom-left corner (inner edge)
        sf::Vector2f bottomLeft = pos + sf::Vector2f(
                                            -size.x / 2.0f * cosRot + size.y / 2.0f * sinRot,
                                            -size.x / 2.0f * sinRot - size.y / 2.0f * cosRot);

        innerEdgePoints.push_back(topLeft);
        innerEdgePoints.push_back(bottomLeft);
    }
    return innerEdgePoints;
}

std::vector<sf::Vector2f> BezierShape::getOuterEdgePoints() const
{
    std::vector<sf::Vector2f> outerEdgePoints;
    for (const auto &segment : segments)
    {
        // Get the rectangle's position and size
        sf::Vector2f pos = segment.getPosition();
        sf::Vector2f size = segment.getSize();
        float rotation = segment.getRotation().asRadians();

        // Calculate the four corners of the rectangle
        float cosRot = std::cos(rotation);
        float sinRot = std::sin(rotation);

        // For outer edge, we need to determine which corners are "outside" the track
        // This depends on the curve direction, but for now we'll use the right side
        // Top-right corner (outer edge)
        sf::Vector2f topRight = pos + sf::Vector2f(
                                          size.x / 2.0f * cosRot - size.y / 2.0f * sinRot,
                                          size.x / 2.0f * sinRot + size.y / 2.0f * cosRot);

        // Bottom-right corner (outer edge)
        sf::Vector2f bottomRight = pos + sf::Vector2f(
                                             size.x / 2.0f * cosRot + size.y / 2.0f * sinRot,
                                             size.x / 2.0f * sinRot - size.y / 2.0f * cosRot);

        outerEdgePoints.push_back(topRight);
        outerEdgePoints.push_back(bottomRight);
    }
    return outerEdgePoints;
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

sf::Vector2f BezierShape::getControlPoint() const
{
    return controlPoint;
}

void BezierShape::generateSegments()
{
    segments.clear();
    checkpointSegments.clear();

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
        segment.setPosition({segmentCenter.x, segmentCenter.y});

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

    // Record 20 evenly spaced segments for checkpoints (including first and last)
    int totalSegments = static_cast<int>(segments.size());
    int checkpointCount = 20;

    for (int i = 0; i < checkpointCount; i++)
    {
        int segmentIndex;
        if (i == 0)
        {
            // First segment
            segmentIndex = 0;
        }
        else if (i == checkpointCount - 1)
        {
            // Last segment
            segmentIndex = totalSegments - 1;
        }
        else
        {
            // Evenly spaced segments in between
            segmentIndex = (i * (totalSegments - 1)) / (checkpointCount - 1);
        }

        if (segmentIndex < totalSegments)
        {
            const auto &segment = segments[segmentIndex];

            SegmentData data;
            data.position = segment.getPosition();
            data.size = segment.getSize();
            data.rotation = segment.getRotation().asDegrees();
            data.segmentIndex = segmentIndex;

            checkpointSegments.push_back(data);
        }
    }
}

const std::vector<SegmentData> &BezierShape::getCheckpointSegments() const
{
    return checkpointSegments;
}

void BezierShape::saveCheckpointSegmentsToBinaryFile(const std::string &filename) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cout << "Failed to open binary file: " << filename << std::endl;
        return;
    }

    // Write number of segments
    size_t numSegments = checkpointSegments.size();
    file.write(reinterpret_cast<const char *>(&numSegments), sizeof(numSegments));

    // Write each segment
    for (const auto &segment : checkpointSegments)
    {
        file.write(reinterpret_cast<const char *>(&segment.position.x), sizeof(float));
        file.write(reinterpret_cast<const char *>(&segment.position.y), sizeof(float));
        file.write(reinterpret_cast<const char *>(&segment.size.x), sizeof(float));
        file.write(reinterpret_cast<const char *>(&segment.size.y), sizeof(float));
        file.write(reinterpret_cast<const char *>(&segment.rotation), sizeof(float));
        file.write(reinterpret_cast<const char *>(&segment.segmentIndex), sizeof(int));
    }

    file.close();
    std::cout << "Checkpoint segments saved to binary file: " << filename << std::endl;
}