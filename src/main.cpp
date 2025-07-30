#include <SFML/Graphics.hpp>
#include <iostream>
#include "Game/Game.h"

using namespace sf;
using namespace std;

int main()
{
    unsigned int windowWidth = 2560;
    unsigned int windowHeight = 1440;

    try
    {
        // Create and run the game
        Game game(windowWidth, windowHeight);
        game.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}