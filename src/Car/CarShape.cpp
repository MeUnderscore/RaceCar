#include "CarShape.h"
#include <cmath>

CarShape::CarShape(float carWidth, float carHeight)
    : position(0.0f, 0.0f), angle(0.0f)
{
    initializeBodyParts(carWidth, carHeight);
}

void CarShape::setPosition(const sf::Vector2f &pos)
{
    position = pos;
    updateBodyParts();
}

void CarShape::setRotation(float rotationAngle)
{
    angle = rotationAngle;
    updateBodyParts();
}

sf::Vector2f CarShape::getPosition() const
{
    return position;
}

float CarShape::getRotation() const
{
    return angle;
}

sf::Vector2f CarShape::getSize() const
{
    // Return the overall size of the car (main body)
    if (!bodyParts.empty())
    {
        return bodyParts[0].getSize(); // Main body size
    }
    return sf::Vector2f(20.0f, 10.0f); // Default size
}

void CarShape::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    for (const auto &part : bodyParts)
    {
        target.draw(part, states);
    }
}

sf::FloatRect CarShape::getGlobalBounds() const
{
    if (bodyParts.empty())
    {
        return sf::FloatRect(sf::Vector2f(position.x, position.y), sf::Vector2f(20.0f, 10.0f));
    }

    // For simplicity, just return the main body bounds
    // This is sufficient for collision detection
    return bodyParts[0].getGlobalBounds();
}

void CarShape::initializeBodyParts(float width, float height)
{
    bodyParts.clear();

    // Main body (largest rectangle)
    sf::RectangleShape mainBody;
    mainBody.setSize({width, height});
    mainBody.setFillColor(sf::Color::Red);
    mainBody.setOrigin({width / 2.0f, height / 2.0f});
    bodyParts.push_back(mainBody);

    // Hood (front part)
    sf::RectangleShape hood;
    hood.setSize({width * 0.4f, height * 0.8f});
    hood.setFillColor(sf::Color(200, 0, 0)); // Darker red
    hood.setOrigin({width * 0.4f / 2.0f, height * 0.8f / 2.0f});
    bodyParts.push_back(hood);

    // Trunk (back part)
    sf::RectangleShape trunk;
    trunk.setSize({width * 0.3f, height * 0.7f});
    trunk.setFillColor(sf::Color(180, 0, 0)); // Even darker red
    trunk.setOrigin({width * 0.3f / 2.0f, height * 0.7f / 2.0f});
    bodyParts.push_back(trunk);

    // Windows (top part)
    sf::RectangleShape windows;
    windows.setSize({width * 0.6f, height * 0.4f});
    windows.setFillColor(sf::Color(100, 150, 255)); // Blue tint
    windows.setOrigin({width * 0.6f / 2.0f, height * 0.4f / 2.0f});
    bodyParts.push_back(windows);

    // Wheels (4 wheels)
    sf::RectangleShape wheel;
    wheel.setSize({width * 0.15f, height * 0.3f});
    wheel.setFillColor(sf::Color::Black);
    wheel.setOrigin({width * 0.15f / 2.0f, height * 0.3f / 2.0f});

    // Front left wheel
    bodyParts.push_back(wheel);
    // Front right wheel
    bodyParts.push_back(wheel);
    // Back left wheel
    bodyParts.push_back(wheel);
    // Back right wheel
    bodyParts.push_back(wheel);

    // Headlights
    sf::RectangleShape headlight;
    headlight.setSize({width * 0.1f, height * 0.2f});
    headlight.setFillColor(sf::Color::White);
    headlight.setOrigin({width * 0.1f / 2.0f, height * 0.2f / 2.0f});

    // Left headlight
    bodyParts.push_back(headlight);
    // Right headlight
    bodyParts.push_back(headlight);

    updateBodyParts();
}

void CarShape::updateBodyParts()
{
    if (bodyParts.empty())
        return;

    float radians = angle * 3.14159f / 180.0f;
    float cosRot = std::cos(radians);
    float sinRot = std::sin(radians);

    // Main body (center)
    bodyParts[0].setPosition(position);
    bodyParts[0].setRotation(sf::degrees(angle));

    // Hood (front)
    float hoodX = position.x + (cosRot * 0.3f * bodyParts[0].getSize().x);
    float hoodY = position.y + (sinRot * 0.3f * bodyParts[0].getSize().x);
    bodyParts[1].setPosition({hoodX, hoodY});
    bodyParts[1].setRotation(sf::degrees(angle));

    // Trunk (back)
    float trunkX = position.x - (cosRot * 0.25f * bodyParts[0].getSize().x);
    float trunkY = position.y - (sinRot * 0.25f * bodyParts[0].getSize().x);
    bodyParts[2].setPosition({trunkX, trunkY});
    bodyParts[2].setRotation(sf::degrees(angle));

    // Windows (top)
    float windowsX = position.x + (cosRot * 0.1f * bodyParts[0].getSize().x);
    float windowsY = position.y + (sinRot * 0.1f * bodyParts[0].getSize().x);
    bodyParts[3].setPosition({windowsX, windowsY});
    bodyParts[3].setRotation(sf::degrees(angle));

    // Wheels
    float wheelOffsetX = bodyParts[0].getSize().x * 0.35f;
    float wheelOffsetY = bodyParts[0].getSize().y * 0.4f;

    // Front left wheel
    float flX = position.x + (cosRot * wheelOffsetX) - (sinRot * wheelOffsetY);
    float flY = position.y + (sinRot * wheelOffsetX) + (cosRot * wheelOffsetY);
    bodyParts[4].setPosition({flX, flY});
    bodyParts[4].setRotation(sf::degrees(angle));

    // Front right wheel
    float frX = position.x + (cosRot * wheelOffsetX) + (sinRot * wheelOffsetY);
    float frY = position.y + (sinRot * wheelOffsetX) - (cosRot * wheelOffsetY);
    bodyParts[5].setPosition({frX, frY});
    bodyParts[5].setRotation(sf::degrees(angle));

    // Back left wheel
    float blX = position.x - (cosRot * wheelOffsetX) - (sinRot * wheelOffsetY);
    float blY = position.y - (sinRot * wheelOffsetX) + (cosRot * wheelOffsetY);
    bodyParts[6].setPosition({blX, blY});
    bodyParts[6].setRotation(sf::degrees(angle));

    // Back right wheel
    float brX = position.x - (cosRot * wheelOffsetX) + (sinRot * wheelOffsetY);
    float brY = position.y - (sinRot * wheelOffsetX) - (cosRot * wheelOffsetY);
    bodyParts[7].setPosition({brX, brY});
    bodyParts[7].setRotation(sf::degrees(angle));

    // Headlights
    float headlightOffsetX = bodyParts[0].getSize().x * 0.45f;
    float headlightOffsetY = bodyParts[0].getSize().y * 0.25f;

    // Left headlight
    float hlX = position.x + (cosRot * headlightOffsetX) - (sinRot * headlightOffsetY);
    float hlY = position.y + (sinRot * headlightOffsetX) + (cosRot * headlightOffsetY);
    bodyParts[8].setPosition({hlX, hlY});
    bodyParts[8].setRotation(sf::degrees(angle));

    // Right headlight
    float hrX = position.x + (cosRot * headlightOffsetX) + (sinRot * headlightOffsetY);
    float hrY = position.y + (sinRot * headlightOffsetX) - (cosRot * headlightOffsetY);
    bodyParts[9].setPosition({hrX, hrY});
    bodyParts[9].setRotation(sf::degrees(angle));
}