#include <SFML/Graphics.hpp>
#include <iostream>
#include "Background/Background.h"
#include "Track/track.h"
#include "Car/Car.h"

using namespace sf;
using namespace std;

int main()
{

    unsigned int windowWidth = 2560;
    unsigned int windowHeight = 1440;

    RenderWindow *window = new RenderWindow(VideoMode({windowWidth, windowHeight}), "Race Car");

    window->setFramerateLimit(60);

    // Create background
    Background background(windowWidth, windowHeight, 128);

    // Create track
    Track track(windowWidth, windowHeight);

    // Create car at the track's start position (checkered flag)
    sf::Vector2f startPos = track.getStartPosition();
    Car car(startPos.x, startPos.y);

    while (window->isOpen())
    {

        while (const optional event = window->pollEvent())
        {

            if (event->is<Event::Closed>())
            {
                window->close();
            }
            else if (const auto *keyPressed = event->getIf<Event::KeyPressed>())
            {
                if (keyPressed->scancode == Keyboard::Scancode::Escape)
                {
                    window->close();
                }
            }
            else if (const auto *resized = event->getIf<Event::Resized>())
            {
                // Update window dimensions and background when resized
                window->clear();
                windowWidth = resized->size.x;
                windowHeight = resized->size.y;
                background.setWindowSize(windowWidth, windowHeight);
                track.setWindowSize(windowWidth, windowHeight);

                // Update the view to match the new window size
                sf::View view = window->getView();
                view.setSize({static_cast<float>(windowWidth), static_cast<float>(windowHeight)});
                view.setCenter({static_cast<float>(windowWidth) / 2.0f, static_cast<float>(windowHeight) / 2.0f});
                window->setView(view);
            }
        }

        // Draw background (this also clears the window)
        background.draw(*window);

        // Draw track
        track.draw(*window);

        // Draw checkered flag at start line
        track.drawCheckeredFlag(*window);

        // Handle car input and update
        car.handleInput();
        car.update(0.016f); // Assuming 60 FPS

        // Draw car
        car.draw(*window);

        window->display();
    }

    delete window;
    return 0;
}