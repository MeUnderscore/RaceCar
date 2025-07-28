#include <SFML/Graphics.hpp>
#include <iostream>
#include "Background/Background.h"
#include "Track/track.h"
#include "Car/Car.h"
#include "Timer/Timer.h"
#include "Checkpoint/CheckpointHandler.h"

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

    // Save track segment corners to file
    track.saveTrackSegmentCornersToFile("track_segments.txt");

    // Create car at the track's start position (checkered flag)
    Car car(350.0f, 230.0f);

    // Create timer
    Timer timer;

    // Create checkpoint handler
    CheckpointHandler checkpointHandler;
    checkpointHandler.initializeCheckpoints(track.getTrackSegmentCorners());

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
                else if (keyPressed->scancode == Keyboard::Scancode::R)
                {
                    // Reset car to start position
                    car.resetPosition();
                    // Reset timer
                    timer.reset();
                    // Reset checkpoints
                    checkpointHandler.resetAllCheckpoints();
                }
                else if (keyPressed->scancode == Keyboard::Scancode::W)
                {
                    // Start timer when W is pressed for the first time
                    timer.start();
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

        // Draw black circles at track edges
        track.drawTrackEdges(*window);

        // Handle car input and update
        car.handleInput();
        car.update(0.016f); // Assuming 60 FPS

        // Handle collision detection
        std::vector<sf::Vector2f> innerEdgePoints = track.getInnerEdgePoints();
        std::vector<sf::Vector2f> outerEdgePoints = track.getOuterEdgePoints();
        car.handleCollision(innerEdgePoints, outerEdgePoints);

        // Check checkpoint collisions
        checkpointHandler.checkCarPosition(sf::Vector2f(car.getX(), car.getY()));

        // Draw car
        car.draw(*window);

        // Update and draw timer
        timer.update();
        timer.draw(*window);

        // Draw checkpoints and checkpoint counter
        checkpointHandler.drawCheckpoints(*window);
        checkpointHandler.drawCheckpointCounter(*window);

        window->display();
    }

    delete window;
    return 0;
}