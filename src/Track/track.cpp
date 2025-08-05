#include "track.h"
#include <fstream>
#include <iostream>

Track::Track(unsigned int width, unsigned int height)
    : windowWidth(width), windowHeight(height), trackWidth(100.0f)
{
    // Add the first curve
    addCurve(sf::Vector2f(350.0f, 230.0f), sf::Vector2f(600.0f, 200.0f), sf::Vector2f(700.0f, 350.0f), trackWidth, 400);

    // Add the second curve
    addSmoothCurve(sf::Vector2f(1240.0f, 225.0f), trackWidth, 600);

    // Add the third curve
    addSmoothCurve(sf::Vector2f(1650.0f, 290.0f), trackWidth, 400);

    // Add the fourth curve
    addSmoothCurve(sf::Vector2f(2000.0f, 150.0f), trackWidth, 300);

    // Add the fifth curve
    addSmoothCurve(sf::Vector2f(2400.0f, 350.0f), trackWidth, 500);

    // Add the sixth curve
    addSmoothCurve(sf::Vector2f(2300.0f, 650.0f), trackWidth, 300);

    // Add the seventh curve
    addSmoothCurve(sf::Vector2f(2300.0f, 900.0f), trackWidth, 200);

    // Add the eighth curve
    addSmoothCurve(sf::Vector2f(2350.0f, 1150.0f), trackWidth, 200);

    // Add the ninth curve
    addSmoothCurve(sf::Vector2f(1800.0f, 1050.0f), trackWidth, 700);

    // Add the tenth curve
    addSmoothCurve(sf::Vector2f(1100.0f, 1000.0f), trackWidth, 400);

    // Add the eleventh curve
    addSmoothCurve(sf::Vector2f(500.0f, 1150.0f), trackWidth, 400);

    // Add the twelfth curve
    addSmoothCurve(sf::Vector2f(350.0f, 850.0f), trackWidth, 300);

    // Add the thirteenth curve
    addSmoothCurve(sf::Vector2f(150.0f, 550.0f), trackWidth, 200);

    // Add the fourteenth curve
    addSmoothCurve(sf::Vector2f(180.0f, 300.0f), trackWidth, 200);

    // Add the fifteenth curve that connects back to the first curve (creating a complete loop)
    addLoopClosingCurve(trackWidth, 200);
}

void Track::draw(sf::RenderWindow &window) const
{
    // Draw all track shapes
    for (const auto &shape : trackShapes)
    {
        shape.draw(window);
    }
}

void Track::setWindowSize(unsigned int width, unsigned int height)
{
    windowWidth = width;
    windowHeight = height;
    // Note: You might want to regenerate track segments here if they need to scale with window
}

void Track::createCurvedSegment(sf::Vector2f start, sf::Vector2f control, sf::Vector2f end, float width, int numSegments)
{
    // Create a new BezierShape and add it to the track
    BezierShape shape(start, control, end, width, numSegments, sf::Color(32, 32, 32));
    trackShapes.push_back(shape);
}

void Track::addCurve(sf::Vector2f start, sf::Vector2f control, sf::Vector2f end, float width, int numSegments)
{
    // Add a curve to the track list
    createCurvedSegment(start, control, end, width, numSegments);
}

void Track::addSmoothCurve(sf::Vector2f end, float width, int numSegments)
{
    if (trackShapes.empty())
    {
        // If no previous curve, just create a straight line
        addCurve(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(end.x / 2.0f, end.y / 2.0f), end, width, numSegments);
        return;
    }

    // Get the last curve's end point and tangent
    const BezierShape &lastCurve = trackShapes.back();
    sf::Vector2f start = lastCurve.getEndPoint();
    sf::Vector2f endTangent = lastCurve.getEndTangent();

    // Calculate the control point to ensure smooth connection
    // The control point should be positioned along the end tangent direction
    float distance = std::sqrt((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y));
    sf::Vector2f control = start + endTangent * (distance * 0.5f);

    // Add the smooth curve
    addCurve(start, control, end, width, numSegments);
}

void Track::addLoopClosingCurve(float width, int numSegments)
{
    if (trackShapes.empty())
    {
        return; // No curves to connect
    }

    // Get the first curve's start point and tangent
    const BezierShape &firstCurve = trackShapes.front();
    sf::Vector2f end = firstCurve.getStartPoint();
    sf::Vector2f startTangent = firstCurve.getStartTangent();

    // Get the last curve's end point and tangent
    const BezierShape &lastCurve = trackShapes.back();
    sf::Vector2f start = lastCurve.getEndPoint();
    sf::Vector2f endTangent = lastCurve.getEndTangent();

    // Calculate the control point to ensure smooth connection
    // The control point needs to create tangents that match at both ends
    float distance = std::sqrt((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y));

    // Calculate control point that ensures:
    // 1. Start tangent matches the last curve's end tangent
    // 2. End tangent matches the first curve's start tangent
    float controlDistance = distance * 0.25f;

    // Position control point to create the desired tangents
    // This creates a curve where:
    // - At t=0: tangent matches endTangent (last curve's end)
    // - At t=1: tangent matches startTangent (first curve's start)
    sf::Vector2f control = start + endTangent * controlDistance;

    // Create the closing curve with extended start to ensure overlap
    sf::Vector2f extendedStart = start - endTangent * (width * 0.05f);

    // Add the smooth curve with extended start to eliminate gaps between curves
    addCurve(extendedStart, control, end, width, numSegments);
}

sf::Vector2f Track::getStartPosition() const
{
    // Return the specific spawn position
    return sf::Vector2f(350.0f, 220.0f);
}

float Track::getStartRotation() const
{
    if (trackShapes.empty())
        return 0.0f; // Default rotation if no track shapes
    
    // Get the tangent direction at the start of the first curve
    sf::Vector2f startTangent = trackShapes.front().getStartTangent();
    
    // Convert tangent to rotation angle in degrees
    float angle = std::atan2(startTangent.y, startTangent.x) * 180.0f / 3.14159f;
    return angle;
}

sf::FloatRect Track::getTrackBounds() const
{
    if (trackShapes.empty())
        return sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    
    // Calculate bounds by finding min/max coordinates of all track shapes
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    
    for (const auto& shape : trackShapes)
    {
        // Get edge points from the shape
        auto edgePoints = shape.getEdgePoints();
        for (const auto& point : edgePoints)
        {
            minX = std::min(minX, point.x);
            minY = std::min(minY, point.y);
            maxX = std::max(maxX, point.x);
            maxY = std::max(maxY, point.y);
        }
    }
    
    // Add some padding around the track
    float padding = 50.0f;
    return sf::FloatRect(sf::Vector2f(minX - padding, minY - padding), 
                        sf::Vector2f((maxX - minX) + 2 * padding, (maxY - minY) + 2 * padding));
}

void Track::drawCheckeredFlag(sf::RenderWindow &window) const
{
    if (trackShapes.empty())
        return;

    // Get the start position and tangent of the first curve
    sf::Vector2f startPos = trackShapes.front().getStartPoint();
    sf::Vector2f startTangent = trackShapes.front().getStartTangent();

    // Calculate perpendicular direction for the flag width
    sf::Vector2f perpendicular(-startTangent.y, startTangent.x);

    // Create checkered flag pattern - only 2 squares wide in track direction
    float squareSize = 20.0f;     // Size of each checkered square (factor of 100)
    float flagWidth = trackWidth; // Full track width

    // The startPos is now properly at the center of the track
    sf::Vector2f trackCenter = startPos;

    // Draw squares in a grid pattern aligned with the track direction
    // Only 2 squares wide in the track direction, centered on startPos
    for (float along = -squareSize / 2.0f; along < squareSize * 1.5f; along += squareSize)
    {
        for (float across = -flagWidth / 2; across < flagWidth / 2; across += squareSize)
        {
            sf::RectangleShape square;
            square.setSize({squareSize, squareSize});

            // Calculate position using tangent and perpendicular vectors
            sf::Vector2f pos = trackCenter + startTangent * along + perpendicular * across;
            square.setPosition({pos.x, pos.y});

            // Calculate rotation angle from tangent
            float angle = std::atan2(startTangent.y, startTangent.x) * 180.0f / 3.14159f;
            square.setRotation(sf::degrees(angle));

            // Alternate black and white squares based on grid position
            int alongIndex = (int)((along + squareSize / 2.0f) / squareSize);
            int acrossIndex = (int)((across + flagWidth / 2) / squareSize);
            bool isBlack = (alongIndex + acrossIndex) % 2 == 0;
            square.setFillColor(isBlack ? sf::Color::Black : sf::Color::White);

            window.draw(square);
        }
    }
}

sf::FloatRect Track::getCheckeredFlagBounds() const
{
    if (trackShapes.empty())
        return sf::FloatRect(sf::Vector2f(350.0f, 220.0f), sf::Vector2f(40.0f, trackWidth));

    // Get the start position and tangent of the first curve
    sf::Vector2f startPos = trackShapes.front().getStartPoint();
    sf::Vector2f startTangent = trackShapes.front().getStartTangent();

    // Calculate perpendicular direction for the flag width
    sf::Vector2f perpendicular(-startTangent.y, startTangent.x);

    // Create bounds for the checkered flag area
    float squareSize = 20.0f;
    float flagWidth = trackWidth;

    // Calculate the four corners of the checkered flag area
    sf::Vector2f topLeft = startPos + startTangent * (-squareSize / 2.0f) + perpendicular * (-flagWidth / 2.0f);
    sf::Vector2f topRight = startPos + startTangent * (-squareSize / 2.0f) + perpendicular * (flagWidth / 2.0f);
    sf::Vector2f bottomLeft = startPos + startTangent * (squareSize * 1.5f) + perpendicular * (-flagWidth / 2.0f);
    sf::Vector2f bottomRight = startPos + startTangent * (squareSize * 1.5f) + perpendicular * (flagWidth / 2.0f);

    // Calculate the bounding rectangle
    float minX = std::min(std::min(topLeft.x, topRight.x), std::min(bottomLeft.x, bottomRight.x));
    float maxX = std::max(std::max(topLeft.x, topRight.x), std::max(bottomLeft.x, bottomRight.x));
    float minY = std::min(std::min(topLeft.y, topRight.y), std::min(bottomLeft.y, bottomRight.y));
    float maxY = std::max(std::max(topLeft.y, topRight.y), std::max(bottomLeft.y, bottomRight.y));

    return sf::FloatRect(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));
}

// void Track::drawBezierPoints(sf::RenderWindow &window)

void Track::drawTrackEdges(sf::RenderWindow &window) const
{
    // Draw edge circles for each BezierShape
    for (const auto &shape : trackShapes)
    {
        shape.drawEdgeCircles(window);
    }
}

std::vector<sf::Vector2f> Track::getAllEdgePoints() const
{
    std::vector<sf::Vector2f> allEdgePoints;

    // Collect edge points from all track shapes
    for (const auto &shape : trackShapes)
    {
        std::vector<sf::Vector2f> shapeEdgePoints = shape.getEdgePoints();
        allEdgePoints.insert(allEdgePoints.end(), shapeEdgePoints.begin(), shapeEdgePoints.end());
    }

    return allEdgePoints;
}

std::vector<sf::Vector2f> Track::getInnerEdgePoints() const
{
    std::vector<sf::Vector2f> allInnerEdgePoints;

    // Collect inner edge points from all track shapes
    for (const auto &shape : trackShapes)
    {
        std::vector<sf::Vector2f> shapeInnerEdgePoints = shape.getInnerEdgePoints();
        allInnerEdgePoints.insert(allInnerEdgePoints.end(), shapeInnerEdgePoints.begin(), shapeInnerEdgePoints.end());
    }

    return allInnerEdgePoints;
}

std::vector<sf::Vector2f> Track::getOuterEdgePoints() const
{
    std::vector<sf::Vector2f> allOuterEdgePoints;

    // Collect outer edge points from all track shapes
    for (const auto &shape : trackShapes)
    {
        std::vector<sf::Vector2f> shapeOuterEdgePoints = shape.getOuterEdgePoints();
        allOuterEdgePoints.insert(allOuterEdgePoints.end(), shapeOuterEdgePoints.begin(), shapeOuterEdgePoints.end());
    }

    return allOuterEdgePoints;
}

std::vector<SegmentData> Track::getCheckpointSegments() const
{
    std::vector<SegmentData> allCheckpointSegments;

    // Collect checkpoint segments from all track shapes
    for (const auto &shape : trackShapes)
    {
        const auto &shapeSegments = shape.getCheckpointSegments();
        allCheckpointSegments.insert(allCheckpointSegments.end(), shapeSegments.begin(), shapeSegments.end());
    }

    return allCheckpointSegments;
}

void Track::saveCheckpointSegmentsToBinaryFile(const std::string &filename) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        // Failed to open binary file silently
        return;
    }

    // Write number of track shapes
    size_t numShapes = trackShapes.size();
    file.write(reinterpret_cast<const char *>(&numShapes), sizeof(numShapes));

    // Write segments from each track shape
    for (const auto &shape : trackShapes)
    {
        const auto &checkpointSegments = shape.getCheckpointSegments();
        size_t numSegments = checkpointSegments.size();
        file.write(reinterpret_cast<const char *>(&numSegments), sizeof(numSegments));

        for (const auto &segment : checkpointSegments)
        {
            file.write(reinterpret_cast<const char *>(&segment.position.x), sizeof(float));
            file.write(reinterpret_cast<const char *>(&segment.position.y), sizeof(float));
            file.write(reinterpret_cast<const char *>(&segment.size.x), sizeof(float));
            file.write(reinterpret_cast<const char *>(&segment.size.y), sizeof(float));
            file.write(reinterpret_cast<const char *>(&segment.rotation), sizeof(float));
            file.write(reinterpret_cast<const char *>(&segment.segmentIndex), sizeof(int));
        }
    }

    file.close();
    std::cout << "Checkpoint segments saved to binary file: " << filename << std::endl;
}
