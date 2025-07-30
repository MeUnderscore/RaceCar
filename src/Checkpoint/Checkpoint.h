#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "../Interfaces/ICollidable.h"
#include "../Interfaces/IRenderable.h"

class Checkpoint : public ICollidable, public IRenderable
{
private:
    std::vector<sf::Vector2f> corners; // 4 corners of the checkpoint rectangle
    bool isHit;
    int checkpointNumber;

public:
    Checkpoint(const std::vector<sf::Vector2f> &cornerPositions, int number);

    // ICollidable interface implementation
    bool isPointInside(const sf::Vector2f &point) const override;
    sf::FloatRect getBounds() const override;
    
    // Line segment intersection for fast-moving cars
    bool isLineIntersecting(const sf::Vector2f &start, const sf::Vector2f &end) const;

private:
    // Helper method for line segment intersection
    bool lineSegmentsIntersect(const sf::Vector2f &a1, const sf::Vector2f &a2, 
                              const sf::Vector2f &b1, const sf::Vector2f &b2) const;

    // IRenderable interface implementation
    void draw(sf::RenderWindow &window) const override;

    // Checkpoint-specific methods
    bool getIsHit() const;
    void markAsHit();
    int getCheckpointNumber() const;
    sf::Vector2f getCenter() const;
    void reset();
};