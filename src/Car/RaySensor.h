#pragma once
#include <SFML/Graphics.hpp>

class RaySensor
{
private:
    sf::RectangleShape shape;
    float angle; // Angle relative to car's forward direction
    float currentLength;
    float maxLength;

public:
    RaySensor(float angle);

    // Getters
    float getAngle() const { return angle; }
    float getLength() const { return currentLength; }
    float getMaxLength() const { return maxLength; }

    // Set position and rotation based on car
    void setPosition(const sf::Vector2f &carPosition, float carRotation);

    // Change color (for collision feedback)
    void setColor(const sf::Color &color);

    // Set ray length dynamically
    void setLength(float length);

    // Get ray end point for collision detection
    sf::Vector2f getEndPoint(const sf::Vector2f &carPosition, float carRotation) const;

    // Drawing
    void draw(sf::RenderWindow &window) const;
};