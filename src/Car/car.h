#pragma once
#include <SFML/Graphics.hpp>
#include "CarShape.h"
#include "RaySensorHandler.h"
#include "../AI/NeuralNetwork.h"
#include <vector>
#include "../Interfaces/IRenderable.h"

class Car : public IRenderable
{
private:
    float x, y;
    float velocity;
    float rotation;
    float acceleration;
    float steering;
    CarShape carShape;

    // Car physics constants
    float maxSpeed;
    float maxReverseSpeed;
    float deceleration;
    float rotationSpeed;

    // Ray sensor handler for AI
    RaySensorHandler raySensorHandler;

    // Neural network brain for AI
    NeuralNetwork brain;

public:
    Car(float startX, float startY, float carWidth = 20.0f, float carHeight = 10.0f);

    void draw(sf::RenderWindow &window) const override;
    void update(float deltaTime);
    void handleInput();
    void handleCollision(const std::vector<sf::Vector2f> &innerEdgePoints, const std::vector<sf::Vector2f> &outerEdgePoints);

    // Getters for position and angle
    float getX() const { return x; }
    float getY() const { return y; }
    float getAngle() const { return rotation; }

    // Setters for position
    void setPosition(float newX, float newY);

    // Ray sensor methods for AI
    void updateRaySensors(const std::vector<sf::Vector2f> &innerEdgePoints,
                          const std::vector<sf::Vector2f> &outerEdgePoints);
    std::vector<float> getRayDistances() const;
    void drawRaySensors(sf::RenderWindow &window) const;

    // AI control methods
    void setAIInputs(float steering, float acceleration);
    sf::Vector2f getVelocity() const { return sf::Vector2f(velocity * std::cos(rotation * 3.14159f / 180.0f),
                                                           velocity * std::sin(rotation * 3.14159f / 180.0f)); }
    float getRotation() const { return rotation; }
    float getSpeed() const { return velocity; }

    // Reset car to start position
    void resetPosition();

    // Get global bounds for collision detection
    sf::FloatRect getGlobalBounds() const;

    // Neural network brain access
    const NeuralNetwork &getBrain() const { return brain; }
    NeuralNetwork &getBrain() { return brain; }
};
