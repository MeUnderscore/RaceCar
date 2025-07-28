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

void Car::draw(sf::RenderWindow &window)
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        // Accelerate forward
        velocity += acceleration * 0.016f;
        if (velocity > maxSpeed)
            velocity = maxSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        // Reverse/brake
        velocity -= acceleration * 0.016f;
        if (velocity < -maxReverseSpeed)
            velocity = -maxReverseSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        // Rotate left (only when moving)
        if (std::abs(velocity) > 10.0f)
        { // Small threshold to prevent turning when nearly stopped
            if (velocity > 0)
            {
                angle -= rotationSpeed * 0.016f; // Normal steering when moving forward
            }
            else
            {
                angle += rotationSpeed * 0.016f; // Reversed steering when reversing
            }
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        // Rotate right (only when moving)
        if (std::abs(velocity) > 10.0f)
        { // Small threshold to prevent turning when nearly stopped
            if (velocity > 0)
            {
                angle += rotationSpeed * 0.016f; // Normal steering when moving forward
            }
            else
            {
                angle -= rotationSpeed * 0.016f; // Reversed steering when reversing
            }
        }
    }

    // Natural deceleration when no keys are pressed
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        if (velocity > 0)
        {
            velocity -= deceleration * 0.5f * 0.016f; // Slow down forward motion
            if (velocity < 0)
                velocity = 0;
        }
        else if (velocity < 0)
        {
            velocity += deceleration * 0.5f * 0.016f; // Slow down reverse motion
            if (velocity > 0)
                velocity = 0;
        }
    }
}

void Car::setPosition(float newX, float newY)
{
    x = newX;
    y = newY;
    carShape.setPosition({x, y});
}

void Car::handleCollision(const std::vector<sf::Vector2f> &edgePoints)
{
    // Get car bounds for collision detection
    sf::FloatRect carBounds = carShape.getGlobalBounds();

    // Check collision with each edge point
    for (const auto &edgePoint : edgePoints)
    {
        // Create a small circle around the edge point for collision detection
        float collisionRadius = 3.0f; // Radius of collision detection around edge points

        // Calculate distance between car center and edge point
        float dx = x - edgePoint.x;
        float dy = y - edgePoint.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // Check if car is within collision radius of edge point
        if (distance < collisionRadius + carShape.getSize().x / 2.0f)
        {
            // Collision detected! Handle bounce and velocity reduction

            // Calculate collision normal (direction from edge point to car)
            sf::Vector2f normal(dx / distance, dy / distance);

            // Bounce the car away from the edge
            float bounceDistance = collisionRadius + carShape.getSize().x / 2.0f - distance + 1.0f;
            x += normal.x * bounceDistance;
            y += normal.y * bounceDistance;

            // Reduce velocity by 90% and reverse direction slightly
            velocity *= 0.1f; // 90% reduction

            // Add a small bounce effect by reversing velocity component along collision normal
            float velocityMagnitude = std::abs(velocity);
            float velocityAngle = std::atan2(std::sin(angle * 3.14159f / 180.0f), std::cos(angle * 3.14159f / 180.0f));

            // Calculate new angle with bounce effect
            float normalAngle = std::atan2(normal.y, normal.x);
            float newAngle = 2.0f * normalAngle - velocityAngle;

            // Set new angle and velocity
            angle = newAngle * 180.0f / 3.14159f;
            velocity = -velocityMagnitude * 0.5f; // Reverse and reduce velocity

            // Update car shape position
            carShape.setPosition({x, y});
            carShape.setRotation(sf::degrees(angle));

            // Only handle one collision per frame to prevent multiple bounces
            return;
        }
    }
}