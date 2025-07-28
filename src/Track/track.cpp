#include "track.h"
#include <fstream>
#include <iostream>

Track::Track(unsigned int width, unsigned int height)
    : windowWidth(width), windowHeight(height), trackWidth(100.0f)
{
    // Add the first curve
    addCurve(sf::Vector2f(350.0f, 230.0f), sf::Vector2f(600.0f, 200.0f), sf::Vector2f(700.0f, 350.0f), trackWidth, 1000);

    // Add the second curve
    addSmoothCurve(sf::Vector2f(1240.0f, 225.0f), trackWidth, 1000);

    // Add the third curve
    addSmoothCurve(sf::Vector2f(1650.0f, 290.0f), trackWidth, 1000);

    // Add the fourth curve
    addSmoothCurve(sf::Vector2f(2000.0f, 150.0f), trackWidth, 1000);

    // Add the fifth curve
    addSmoothCurve(sf::Vector2f(2400.0f, 350.0f), trackWidth, 1000);

    // Add the sixth curve
    addSmoothCurve(sf::Vector2f(2300.0f, 650.0f), trackWidth, 1000);

    // Add the seventh curve
    addSmoothCurve(sf::Vector2f(2300.0f, 900.0f), trackWidth, 1000);

    // Add the eighth curve
    addSmoothCurve(sf::Vector2f(2350.0f, 1150.0f), trackWidth, 1000);

    // Add the ninth curve
    addSmoothCurve(sf::Vector2f(1800.0f, 1050.0f), trackWidth, 1000);

    // Add the tenth curve
    addSmoothCurve(sf::Vector2f(1100.0f, 1000.0f), trackWidth, 1000);

    // Add the eleventh curve
    addSmoothCurve(sf::Vector2f(500.0f, 1150.0f), trackWidth, 1000);

    // Add the twelfth curve
    addSmoothCurve(sf::Vector2f(350.0f, 850.0f), trackWidth, 1000);

    // Add the thirteenth curve
    addSmoothCurve(sf::Vector2f(150.0f, 550.0f), trackWidth, 1000);

    // Add the fourteenth curve
    addSmoothCurve(sf::Vector2f(180.0f, 300.0f), trackWidth, 1000);

    // Add the fifteenth curve that connects back to the first curve (creating a complete loop)
    addLoopClosingCurve(trackWidth, 1000);
}

void Track::draw(sf::RenderWindow &window)
{
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

void Track::drawCheckeredFlag(sf::RenderWindow &window)
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
    float minX = std::min({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x});
    float maxX = std::max({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x});
    float minY = std::min({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y});
    float maxY = std::max({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y});

    return sf::FloatRect(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));
}

std::vector<std::vector<sf::Vector2f>> Track::getTrackSegmentCorners() const
{
    std::vector<std::vector<sf::Vector2f>> allSegmentCorners;

    for (const auto &shape : trackShapes)
    {
        // Get the edge points for this shape
        std::vector<sf::Vector2f> innerPoints = shape.getInnerEdgePoints();
        std::vector<sf::Vector2f> outerPoints = shape.getOuterEdgePoints();

        // Calculate how many points to skip to get 20 evenly spaced rectangles
        int totalPoints = std::min(innerPoints.size(), outerPoints.size());
        int skipAmount = std::max(1, totalPoints / 20);

        for (int i = 0; i < totalPoints && i < 20 * skipAmount; i += skipAmount)
        {
            if (i < innerPoints.size() && i < outerPoints.size())
            {
                // Get the current segment points
                sf::Vector2f innerPoint = innerPoints[i];
                sf::Vector2f outerPoint = outerPoints[i];

                // Calculate the 4 corners of the rectangle
                // We need to get the next point to calculate the rectangle width
                sf::Vector2f nextInnerPoint, nextOuterPoint;

                if (i + skipAmount < innerPoints.size() && i + skipAmount < outerPoints.size())
                {
                    nextInnerPoint = innerPoints[i + skipAmount];
                    nextOuterPoint = outerPoints[i + skipAmount];
                }
                else
                {
                    // Use the last point if we're at the end
                    nextInnerPoint = innerPoints.back();
                    nextOuterPoint = outerPoints.back();
                }

                // Calculate the 4 corners of the rectangle segment
                std::vector<sf::Vector2f> segmentCorners;
                segmentCorners.push_back(innerPoint);     // Top-left
                segmentCorners.push_back(outerPoint);     // Top-right
                segmentCorners.push_back(nextInnerPoint); // Bottom-left
                segmentCorners.push_back(nextOuterPoint); // Bottom-right

                allSegmentCorners.push_back(segmentCorners);
            }
        }
    }

    return allSegmentCorners;
}

void Track::saveTrackSegmentCornersToFile(const std::string &filename) const
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::vector<std::vector<sf::Vector2f>> segmentCorners = getTrackSegmentCorners();

    file << "Track Segment Corners" << std::endl;
    file << "Format: Segment_Index Corner_Index X Y" << std::endl;
    file << "========================" << std::endl;

    for (size_t segmentIndex = 0; segmentIndex < segmentCorners.size(); ++segmentIndex)
    {
        const auto &corners = segmentCorners[segmentIndex];
        file << "Segment " << segmentIndex << ":" << std::endl;

        for (size_t cornerIndex = 0; cornerIndex < corners.size(); ++cornerIndex)
        {
            const auto &corner = corners[cornerIndex];
            file << "  Corner " << cornerIndex << ": (" << corner.x << ", " << corner.y << ")" << std::endl;
        }
        file << std::endl;
    }

    file.close();
    std::cout << "Track segment corners saved to: " << filename << std::endl;
}

// void Track::drawBezierPoints(sf::RenderWindow &window)
// {
//     // Create a small red circle shape
//     sf::CircleShape circle;
//     circle.setRadius(1.0f);
//     circle.setFillColor(sf::Color::Red);
//     circle.setOrigin({1.0f, 1.0f}); // Center the origin

//     // Go through each BezierShape and draw points from its curve
//     for (const auto &shape : trackShapes)
//     {
//         // Create a temporary BezierCurve to get the points
//         BezierCurve curve(shape.getStartPoint(),
//                           shape.getControlPoint(),
//                           shape.getEndPoint());

//         // Generate points for this curve
//         curve.generatePoints(1000); // Use fewer points for visibility

//         // Draw a circle at each point
//         const std::vector<sf::Vector2f> &points = curve.getPoints();
//         for (const auto &point : points)
//         {
//             circle.setPosition(point);
//             window.draw(circle);
//         }
//     }
// }

void Track::drawTrackEdges(sf::RenderWindow &window)
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
