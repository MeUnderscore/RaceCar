#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "../BezierShape/BezierShape.h"
#include "../Interfaces/IRenderable.h"

class Track : public IRenderable
{
private:
    std::vector<BezierShape> trackShapes;
    unsigned int windowWidth;
    unsigned int windowHeight;
    float trackWidth;

public:
    Track(unsigned int width, unsigned int height);
    void draw(sf::RenderWindow &window) const override;
    void setWindowSize(unsigned int width, unsigned int height);

    // Create a curved track segment using BezierShape
    void createCurvedSegment(sf::Vector2f start, sf::Vector2f control, sf::Vector2f end, float width, int numSegments = 40);

    // Add a curve to the track list
    void addCurve(sf::Vector2f start, sf::Vector2f control, sf::Vector2f end, float width, int numSegments = 40);

    // Add a curve that connects smoothly to the previous curve
    void addSmoothCurve(sf::Vector2f end, float width, int numSegments = 40);

    // Add a curve that connects back to the first curve to complete the loop
    void addLoopClosingCurve(float width, int numSegments = 40);

    // Get the start position for the car (checkered flag position)
    sf::Vector2f getStartPosition() const;

    // Draw the checkered flag at the start line
    void drawCheckeredFlag(sf::RenderWindow &window) const;

    // Draw red circles at Bezier curve points for debugging
    void drawBezierPoints(sf::RenderWindow &window) const;

    // Draw black circles at track edges
    void drawTrackEdges(sf::RenderWindow &window) const;

    // Get all edge points for collision detection
    std::vector<sf::Vector2f> getAllEdgePoints() const;

    // Get inner and outer edge points separately
    std::vector<sf::Vector2f> getInnerEdgePoints() const;
    std::vector<sf::Vector2f> getOuterEdgePoints() const;

    // Get checkered flag bounds for collision detection
    sf::FloatRect getCheckeredFlagBounds() const;

    // Get checkpoint segments data from all track shapes
    std::vector<SegmentData> getCheckpointSegments() const;

    // Save checkpoint segments to a text file
    void saveCheckpointSegmentsToFile(const std::string &filename) const;

    // Save checkpoint segments to a binary file (more efficient)
    void saveCheckpointSegmentsToBinaryFile(const std::string &filename) const;
};
