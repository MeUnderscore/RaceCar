#pragma once
#include <SFML/Graphics.hpp>
#include "../Interfaces/IRenderable.h"

class Background : public IRenderable
{
private:
    unsigned int windowWidth;
    unsigned int windowHeight;
    unsigned int gridSize;

public:
    Background(unsigned int width, unsigned int height, unsigned int grid = 32);
    void draw(sf::RenderWindow &window) const override;
    void setWindowSize(unsigned int width, unsigned int height);
};