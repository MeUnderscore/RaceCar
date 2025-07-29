#include "Car.h"

Car::Car(float startX, float startY, float carWidth, float carHeight)
    : carShape(carWidth, carHeight), x(startX), y(startY), angle(0.0f), velocity(0.0f), maxSpeed(500.0f),
      maxReverseSpeed(150.0f), acceleration(500.0f), deceleration(300.0f), rotationSpeed(180.0f)
{
    // Set the initial position and rotation
    carShape.setPosition({x, y});
    carShape.setRotation(angle);
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
    carShape.setRotation(angle);
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

void Car::resetPosition()
{
    // Reset to start position (checkered flag position)
    x = 350.0f;
    y = 230.0f;
    angle = 0.0f;
    velocity = 0.0f;

    // Update car shape
    carShape.setPosition({x, y});
    carShape.setRotation(angle);
}

sf::FloatRect Car::getGlobalBounds() const
{
    return carShape.getGlobalBounds();
}

void Car::handleCollision(const std::vector<sf::Vector2f> &innerEdgePoints, const std::vector<sf::Vector2f> &outerEdgePoints)
{
    // Check collision with inner edge points
    for (size_t i = 0; i < innerEdgePoints.size(); ++i)
    {
        const auto &edgePoint = innerEdgePoints[i];

        // Calculate distance between car center and edge point
        float dx = x - edgePoint.x;
        float dy = y - edgePoint.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // Check if car is within collision radius of edge point
        if (distance < 5.0f + carShape.getSize().x / 2.0f)
        {
            // Find adjacent points to calculate tangent
            sf::Vector2f prevPoint, nextPoint;

            if (i > 0 && i < innerEdgePoints.size() - 1)
            {
                // Middle point - use points on both sides
                prevPoint = innerEdgePoints[i - 1];
                nextPoint = innerEdgePoints[i + 1];
            }
            else if (i == 0)
            {
                // First point - use next point and extrapolate backwards
                nextPoint = innerEdgePoints[i + 1];
                prevPoint = edgePoint - (nextPoint - edgePoint);
            }
            else
            {
                // Last point - use previous point and extrapolate forwards
                prevPoint = innerEdgePoints[i - 1];
                nextPoint = edgePoint + (edgePoint - prevPoint);
            }

            // Calculate tangent vector (direction of the curve at this point)
            sf::Vector2f tangent = nextPoint - prevPoint;
            float tangentLength = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
            if (tangentLength > 0)
            {
                tangent /= tangentLength; // Normalize
            }

            // Calculate surface normal (perpendicular to tangent)
            // For inner edge, normal should point outward (away from track center)
            sf::Vector2f normal(-tangent.y, tangent.x);

            // Ensure normal points outward from track (toward car)
            // Since we're on the inner edge, the car should be outside the track
            // So the normal should point from the edge toward the car
            sf::Vector2f toCar = sf::Vector2f(dx, dy);
            float normalDotProduct = toCar.x * normal.x + toCar.y * normal.y;
            if (normalDotProduct < 0)
            {
                normal = -normal; // Flip normal to point toward car
            }

            // Bounce the car away from the edge
            float bounceDistance = 5.0f + carShape.getSize().x / 2.0f - distance + 1.0f;
            x += normal.x * bounceDistance;
            y += normal.y * bounceDistance;

            // Calculate reflection using vector math (more robust)
            float velocityAngle = angle * 3.14159f / 180.0f;

            // Create velocity vector
            sf::Vector2f velocityVector(std::cos(velocityAngle), std::sin(velocityAngle));

            // Calculate angle of incidence (angle between velocity and surface normal)
            float innerVelocityDotProduct = velocityVector.x * normal.x + velocityVector.y * normal.y;
            float angleOfIncidence = std::acos(std::abs(innerVelocityDotProduct));

            // Calculate reflection using the reflection formula: R = V - 2(V·N)N
            sf::Vector2f reflection = velocityVector - 2.0f * innerVelocityDotProduct * normal;

            // Calculate new angle from reflection vector
            float newVelocityAngle = std::atan2(reflection.y, reflection.x);
            float newAngle = newVelocityAngle * 180.0f / 3.14159f;

            // Check if the rotation would be too large (more than 90 degrees)
            float angleDifference = std::abs(newAngle - angle);
            if (angleDifference > 180.0f)
            {
                angleDifference = 360.0f - angleDifference; // Handle angle wrapping
            }

            // Only apply rotation if it's less than 90 degrees
            if (angleDifference < 45.0f)
            {
                angle = newAngle;
            }
            else
            {
                // If rotation would be too large, reverse the velocity instead
                velocity = -velocity * 0.5f; // Reverse and reduce velocity by 50%
            }

            // Calculate velocity reduction based on angle of incidence
            float velocityReductionFactor = 0.0f + ((angleOfIncidence / (3.14159f / 2.0f)) * 0.9f) * .3f;
            velocity *= velocityReductionFactor;

            // Update car shape
            carShape.setPosition({x, y});
            carShape.setRotation(angle);

            return; // Only handle one collision per frame
        }
    }

    // Check collision with outer edge points (similar logic)
    for (size_t i = 0; i < outerEdgePoints.size(); ++i)
    {
        const auto &edgePoint = outerEdgePoints[i];

        float dx = x - edgePoint.x;
        float dy = y - edgePoint.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance < 5.0f + carShape.getSize().x / 2.0f)
        {
            // Find adjacent points to calculate tangent
            sf::Vector2f prevPoint, nextPoint;

            if (i > 0 && i < outerEdgePoints.size() - 1)
            {
                prevPoint = outerEdgePoints[i - 1];
                nextPoint = outerEdgePoints[i + 1];
            }
            else if (i == 0)
            {
                nextPoint = outerEdgePoints[i + 1];
                prevPoint = edgePoint - (nextPoint - edgePoint);
            }
            else
            {
                prevPoint = outerEdgePoints[i - 1];
                nextPoint = edgePoint + (edgePoint - prevPoint);
            }

            // Calculate tangent vector
            sf::Vector2f tangent = nextPoint - prevPoint;
            float tangentLength = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
            if (tangentLength > 0)
            {
                tangent /= tangentLength;
            }

            // Calculate surface normal (perpendicular to tangent)
            // For outer edge, normal should point inward (toward track center)
            sf::Vector2f normal(tangent.y, -tangent.x);

            // Ensure normal points inward toward track (away from car)
            // Since we're on the outer edge, the car should be outside the track
            // So the normal should point from the edge toward the track center (away from car)
            sf::Vector2f toCar = sf::Vector2f(dx, dy);
            float outerNormalDotProduct = toCar.x * normal.x + toCar.y * normal.y;
            if (outerNormalDotProduct > 0)
            {
                normal = -normal; // Flip normal to point away from car
            }

            // Bounce the car away from the edge
            float bounceDistance = 5.0f + carShape.getSize().x / 2.0f - distance + 1.0f;
            x += normal.x * bounceDistance;
            y += normal.y * bounceDistance;

            // Calculate reflection using vector math (more robust)
            float velocityAngle = angle * 3.14159f / 180.0f;

            // Create velocity vector
            sf::Vector2f velocityVector(std::cos(velocityAngle), std::sin(velocityAngle));

            // Calculate angle of incidence (angle between velocity and surface normal)
            float outerVelocityDotProduct = velocityVector.x * normal.x + velocityVector.y * normal.y;
            float angleOfIncidence = std::acos(std::abs(outerVelocityDotProduct));

            // Calculate reflection using the reflection formula: R = V - 2(V·N)N
            sf::Vector2f reflection = velocityVector - 2.0f * outerVelocityDotProduct * normal;

            // Calculate new angle from reflection vector
            float newVelocityAngle = std::atan2(reflection.y, reflection.x);
            float newAngle = newVelocityAngle * 180.0f / 3.14159f;

            // Check if the rotation would be too large (more than 90 degrees)
            float angleDifference = std::abs(newAngle - angle);
            if (angleDifference > 180.0f)
            {
                angleDifference = 360.0f - angleDifference; // Handle angle wrapping
            }

            // Only apply rotation if it's less than 90 degrees
            if (angleDifference < 90.0f)
            {
                angle = newAngle;
            }
            else
            {
                // If rotation would be too large, reverse the velocity instead
                velocity = -velocity * 0.5f; // Reverse and reduce velocity by 50%
            }

            // Calculate velocity reduction based on angle of incidence
            float velocityReductionFactor = 1.0f - (angleOfIncidence / (3.14159f / 2.0f)) * 0.9f;
            velocity *= velocityReductionFactor;

            // Update car shape
            carShape.setPosition({x, y});
            carShape.setRotation(angle);

            return; // Only handle one collision per frame
        }
    }
}