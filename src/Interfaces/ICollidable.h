#pragma once
#include <SFML/Graphics.hpp>

// Interface for objects that can be collided with
class ICollidable
{
public:
    virtual ~ICollidable() = default;
    virtual bool isPointInside(const sf::Vector2f& point) const = 0;
    virtual sf::FloatRect getBounds() const = 0;
}; 