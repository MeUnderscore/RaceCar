#include "track.h"

Track::Track(unsigned int width, unsigned int height)
    : windowWidth(width), windowHeight(height), trackWidth(100.0f)
{
    // Add the first curve
    addCurve(sf::Vector2f(350.0f, 220.0f), sf::Vector2f(600.0f, 200.0f), sf::Vector2f(700.0f, 400.0f), trackWidth, 1000);

    // Add the second curve
    addSmoothCurve(sf::Vector2f(1240.0f, 275.0f), trackWidth, 1000);

    // Add the third curve
    addSmoothCurve(sf::Vector2f(1650.0f, 440.0f), trackWidth, 1000);

    // Add the fourth curve
    addSmoothCurve(sf::Vector2f(2000.0f, 200.0f), trackWidth, 1000);

    // Add the fifth curve
    addSmoothCurve(sf::Vector2f(2400.0f, 400.0f), trackWidth, 1000);

    // Add the sixth curve
    addSmoothCurve(sf::Vector2f(2300.0f, 700.0f), trackWidth, 1000);

    // Add the seventh curve
    addSmoothCurve(sf::Vector2f(2300.0f, 950.0f), trackWidth, 1000);

    // Add the eighth curve
    addSmoothCurve(sf::Vector2f(2350.0f, 1200.0f), trackWidth, 1000);

    // Add the ninth curve
    addSmoothCurve(sf::Vector2f(1800.0f, 1100.0f), trackWidth, 1000);

    // Add the tenth curve
    addSmoothCurve(sf::Vector2f(1100.0f, 1050.0f), trackWidth, 1000);

    // Add the eleventh curve
    addSmoothCurve(sf::Vector2f(500.0f, 1200.0f), trackWidth, 1000);

    // Add the twelfth curve
    addSmoothCurve(sf::Vector2f(350.0f, 900.0f), trackWidth, 1000);

    // Add the thirteenth curve
    addSmoothCurve(sf::Vector2f(150.0f, 600.0f), trackWidth, 1000);

    // Add the fourteenth curve
    addSmoothCurve(sf::Vector2f(170.0f, 370.0f), trackWidth, 1000);

    // Add the thirteenth curve that connects back to the first curve (creating a complete loop)
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
    // Use the first curve's start tangent to ensure proper alignment
    float distance = std::sqrt((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y));
    float controlDistance = distance * 0.3f;
    sf::Vector2f control = end - startTangent * controlDistance;

    // Create the closing curve with extended start to ensure overlap
    sf::Vector2f extendedStart = start - endTangent * (width * 0.05f);

    // Add the smooth curve with extended start to eliminate gaps between curves
    addCurve(extendedStart, control, end, width, numSegments);
}

sf::Vector2f Track::getStartPosition() const
{
    // Return the position at the start of the first curve (checkered flag position)
    // This is the start point of the first BezierShape
    if (!trackShapes.empty())
    {
        return trackShapes.front().getStartPoint();
    }
    return sf::Vector2f(400.0f, 300.0f); // Default position if no curves exist
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

    // Create checkered flag pattern across the track
    float flagWidth = trackWidth;
    float squareSize = 15.0f; // Size of each checkered square

    for (float offset = -flagWidth / 2; offset <= flagWidth / 2; offset += squareSize)
    {
        for (float along = -20.0f; along <= 20.0f; along += squareSize)
        {
            sf::RectangleShape square;
            square.setSize({squareSize, squareSize});

            // Calculate position
            sf::Vector2f pos = startPos + perpendicular * offset + startTangent * along;
            square.setPosition({pos.x - squareSize / 2, pos.y - squareSize / 2});

            // Alternate black and white squares
            bool isBlack = ((int)(offset / squareSize) + (int)(along / squareSize)) % 2 == 0;
            square.setFillColor(isBlack ? sf::Color::Black : sf::Color::White);

            window.draw(square);
        }
    }
}
