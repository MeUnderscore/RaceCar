#pragma once
#include <SFML/Graphics.hpp>

// Interface for objects that can be rendered
class IRenderable
{
public:
    virtual ~IRenderable() = default;
    virtual void draw(sf::RenderWindow& window) const = 0;
}; 