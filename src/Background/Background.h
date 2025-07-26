#pragma once
#include <SFML/Graphics.hpp>

class Background {
private:
    unsigned int windowWidth;
    unsigned int windowHeight;
    unsigned int gridSize;

public:
    Background(unsigned int width, unsigned int height, unsigned int grid = 32);
    void draw(sf::RenderWindow& window);
    void setWindowSize(unsigned int width, unsigned int height);
};