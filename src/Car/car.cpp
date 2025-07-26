#include "Car.h"

Car::Car(float startX, float startY, float carWidth, float carHeight)
    : x(startX), y(startY), angle(0.0f), velocity(0.0f), maxSpeed(500.0f), 
      maxReverseSpeed(150.0f), acceleration(500.0f), deceleration(300.0f), rotationSpeed(180.0f)
{
    // Create the car shape
    carShape.setSize({carWidth, carHeight});
    carShape.setFillColor(sf::Color::Red);
    carShape.setPosition({x, y});
    carShape.setOrigin({carWidth / 2.0f, carHeight / 2.0f}); // Center the origin
    carShape.setRotation(sf::degrees(angle));
}

void Car::draw(sf::RenderWindow& window)
{
    window.draw(carShape);
}

void Car::update(float deltaTime)
{
    // Update car position based on velocity and angle
    float radians = angle * 3.14159f / 180.0f;
    x += velocity * std::cos(radians) * deltaTime;
    y += velocity * std::sin(radians) * deltaTime;
    
    // Update car shape position and rotation
    carShape.setPosition({x, y});
    carShape.setRotation(sf::degrees(angle));
}

void Car::handleInput()
{
    // Handle WASD input for car physics
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        // Accelerate forward
        velocity += acceleration * 0.016f;
        if (velocity > maxSpeed) velocity = maxSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        // Reverse/brake
        velocity -= acceleration * 0.016f;
        if (velocity < -maxReverseSpeed) velocity = -maxReverseSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        // Rotate left (only when moving)
        if (std::abs(velocity) > 10.0f) { // Small threshold to prevent turning when nearly stopped
            if (velocity > 0) {
                angle -= rotationSpeed * 0.016f; // Normal steering when moving forward
            } else {
                angle += rotationSpeed * 0.016f; // Reversed steering when reversing
            }
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        // Rotate right (only when moving)
        if (std::abs(velocity) > 10.0f) { // Small threshold to prevent turning when nearly stopped
            if (velocity > 0) {
                angle += rotationSpeed * 0.016f; // Normal steering when moving forward
            } else {
                angle -= rotationSpeed * 0.016f; // Reversed steering when reversing
            }
        }
    }
    
    // Natural deceleration when no keys are pressed
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        if (velocity > 0) {
            velocity -= deceleration * 0.5f * 0.016f; // Slow down forward motion
            if (velocity < 0) velocity = 0;
        } else if (velocity < 0) {
            velocity += deceleration * 0.5f * 0.016f; // Slow down reverse motion
            if (velocity > 0) velocity = 0;
        }
    }
}

void Car::setPosition(float newX, float newY)
{
    x = newX;
    y = newY;
    carShape.setPosition({x, y});
}
