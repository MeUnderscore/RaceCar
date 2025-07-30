#pragma once
#include <SFML/Graphics.hpp>
#include "CarShape.h"
#include "../Interfaces/IRenderable.h"

class Car : public IRenderable
{
private:
    CarShape carShape;
    float speed;
    float velocity;
    float maxSpeed;
    float maxReverseSpeed;
    float acceleration;
    float deceleration;
    float rotationSpeed;
    float x, y;
    float angle; // Rotation angle in degrees

public:
    Car(float startX, float startY, float carWidth = 20.0f, float carHeight = 10.0f);

    void draw(sf::RenderWindow &window) const override;
    void update(float deltaTime);
    void handleInput();
    void handleCollision(const std::vector<sf::Vector2f> &innerEdgePoints, const std::vector<sf::Vector2f> &outerEdgePoints);

    // Getters for position and angle
    float getX() const { return x; }
    float getY() const { return y; }
    float getAngle() const { return angle; }

    // Setters for position
    void setPosition(float newX, float newY);

    // Reset car to start position
    void resetPosition();

    // Get global bounds for collision detection
    sf::FloatRect getGlobalBounds() const;
};
