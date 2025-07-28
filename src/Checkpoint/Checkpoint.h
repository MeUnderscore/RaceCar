#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Checkpoint
{
private:
    std::vector<sf::Vector2f> corners; // 4 corners of the checkpoint rectangle
    bool isHit;
    int checkpointNumber;

public:
    Checkpoint(const std::vector<sf::Vector2f> &cornerPositions, int number);

    // Check if a point is inside the checkpoint
    bool isPointInside(const sf::Vector2f &point) const;

    // Check if the checkpoint has been hit
    bool getIsHit() const;

    // Mark the checkpoint as hit
    void markAsHit();

    // Get the checkpoint number
    int getCheckpointNumber() const;

    // Get the center position of the checkpoint
    sf::Vector2f getCenter() const;

    // Draw the checkpoint (for debugging)
    void draw(sf::RenderWindow &window) const;

    // Reset the checkpoint
    void reset();
};