#include "RaySensor.h"
#include <cmath>

RaySensor::RaySensor(float angle)
    : angle(angle), currentLength(200.0f), maxLength(200.0f)
{
    // Create a thin rectangle for the ray with initial length of 200
    shape.setSize(sf::Vector2f(currentLength, 2.0f)); // 200 pixels long, 2 pixels wide
    shape.setOrigin(sf::Vector2f(0.0f, 1.0f));        // Origin at left center
    shape.setFillColor(sf::Color::Cyan);
}

void RaySensor::setPosition(const sf::Vector2f &carPosition, float carRotation)
{
    // Calculate the ray's absolute angle
    float absoluteAngle = carRotation + angle;

    // Set position and rotation
    shape.setPosition(carPosition);
    shape.setRotation(sf::degrees(absoluteAngle));
}

void RaySensor::setColor(const sf::Color &color)
{
    shape.setFillColor(color);
}

void RaySensor::setLength(float length)
{
    currentLength = std::min(length, maxLength);
    shape.setSize(sf::Vector2f(currentLength, 2.0f));
}

sf::Vector2f RaySensor::getEndPoint(const sf::Vector2f &carPosition, float carRotation) const
{
    // Calculate the ray's absolute angle
    float absoluteAngle = carRotation + angle;
    float radians = absoluteAngle * 3.14159f / 180.0f;

    // Calculate end point using current length
    float endX = carPosition.x + std::cos(radians) * currentLength;
    float endY = carPosition.y + std::sin(radians) * currentLength;

    return sf::Vector2f(endX, endY);
}

void RaySensor::draw(sf::RenderWindow &window) const
{
    window.draw(shape);
}