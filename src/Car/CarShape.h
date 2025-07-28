#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class CarShape : public sf::Drawable
{
private:
    std::vector<sf::RectangleShape> bodyParts;
    sf::Vector2f position;
    float angle; // Rotation angle in degrees

public:
    CarShape(float carWidth = 20.0f, float carHeight = 10.0f);

    // Set the position of the entire car
    void setPosition(const sf::Vector2f &pos);

    // Set the rotation of the entire car
    void setRotation(float rotationAngle);

    // Get the current position
    sf::Vector2f getPosition() const;

    // Get the current rotation
    float getRotation() const;

    // Get the size of the car (for collision detection)
    sf::Vector2f getSize() const;

    // Get the global bounds for collision detection
    sf::FloatRect getGlobalBounds() const;

private:
    // Initialize the car body parts
    void initializeBodyParts(float width, float height);

    // Update the positions of all body parts
    void updateBodyParts();

    // Override draw method from sf::Drawable
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};