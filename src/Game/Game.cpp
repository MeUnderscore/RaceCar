#include "Game.h"
#include <iostream>

Game::Game(unsigned int width, unsigned int height)
    : deltaTime(0.016f), previousCarPosition(350.0f, 230.0f) // Default to 60 FPS
{
    // Create window
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode({width, height}), "Race Car");
    window->setFramerateLimit(60);
    window->setVerticalSyncEnabled(true);

    // Create game objects
    background = std::make_unique<Background>(width, height, 128);
    track = std::make_unique<Track>(width, height);
    car = std::make_unique<Car>(350.0f, 230.0f);

    // Create timer system
    timerLogic = std::make_unique<TimerLogic>();
    timerRenderer = std::make_unique<TimerRenderer>(*timerLogic);

    // Create checkpoint system
    checkpointHandler = std::make_unique<CheckpointHandler>();
    checkpointHandler->initializeCheckpoints(track->getCheckpointSegments());
    checkpointUIRenderer = std::make_unique<CheckpointUIRenderer>(*checkpointHandler);
    
    // Create UI system
    uiManager = std::make_unique<UIManager>();
    uiManager->initialize();
    
    // Set up button callbacks
    uiManager->setStartCallback([this]() { 
        std::cout << "Start button clicked!" << std::endl;
        // TODO: Start AI learning process
    });
    
    uiManager->setStopCallback([this]() { 
        std::cout << "Stop button clicked!" << std::endl;
        // TODO: Stop AI learning process
    });
    
    uiManager->setSaveCallback([this]() { 
        std::cout << "Save button clicked!" << std::endl;
        // TODO: Save AI training data
    });
}

Game::~Game()
{
}

void Game::run()
{
    while (isRunning())
    {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents()
{
    while (const auto event = window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window->close();
        }
        else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
            {
                window->close();
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::R)
            {
                reset();
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::W)
            {
                startTimer();
            }
        }
        else if (const auto *resized = event->getIf<sf::Event::Resized>())
        {
            // Update window dimensions and game objects when resized
            window->clear();
            unsigned int newWidth = resized->size.x;
            unsigned int newHeight = resized->size.y;

            background->setWindowSize(newWidth, newHeight);
            track->setWindowSize(newWidth, newHeight);

            // Update the view to match the new window size
            sf::View view = window->getView();
            view.setSize({static_cast<float>(newWidth), static_cast<float>(newHeight)});
            view.setCenter({static_cast<float>(newWidth) / 2.0f, static_cast<float>(newHeight) / 2.0f});
            window->setView(view);
        }
        
        // Handle UI events
        sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
        uiManager->handleEvent(*event, mousePos);
    }
}

void Game::update()
{
    // Calculate delta time for smooth movement
    deltaTime = deltaClock.restart().asSeconds();

    // Store previous car position for collision detection
    previousCarPosition = sf::Vector2f(car->getX(), car->getY());

    // Handle car input and update
    car->handleInput();
    car->update(deltaTime);

    // Get track edge points for ray sensor updates
    std::vector<sf::Vector2f> innerEdgePoints = track->getInnerEdgePoints();
    std::vector<sf::Vector2f> outerEdgePoints = track->getOuterEdgePoints();

    // Update ray sensors for AI
    car->updateRaySensors(innerEdgePoints, outerEdgePoints);

    // Handle collision detection
    car->handleCollision(innerEdgePoints, outerEdgePoints);

    // Check checkpoint collisions with line segment (prevents tunneling)
    sf::Vector2f currentCarPosition(car->getX(), car->getY());
    checkpointHandler->checkCarPositionWithLine(previousCarPosition, currentCarPosition);

    // Check if lap is completed and stop timer
    if (checkpointHandler->isLapCompleted() && timerLogic->hasStarted())
    {
        stopTimer();
    }

    // Update timer
    timerLogic->update();

    // Update UI
    checkpointUIRenderer->updateText();
    
    // Update UI manager
    sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    uiManager->update(mousePos);
}

void Game::render()
{
    window->clear();

    // Draw background
    background->draw(*window);

    // Draw track
    track->draw(*window);
    track->drawCheckeredFlag(*window);
    track->drawTrackEdges(*window);

    // Draw car
    car->draw(*window);

    // Draw ray sensors for debugging
    car->drawRaySensors(*window);

    // Draw checkpoints
    checkpointHandler->drawCheckpoints(*window);

    // Draw UI
    timerRenderer->draw(*window);
    checkpointUIRenderer->draw(*window);
    
    // Draw UI manager (buttons)
    uiManager->draw(*window);

    window->display();
}

void Game::reset()
{
    car->resetPosition();
    timerLogic->reset();
    checkpointHandler->resetAllCheckpoints();
    checkpointUIRenderer->updateText();
}

void Game::startTimer()
{
    timerLogic->start();
}

void Game::stopTimer()
{
    timerLogic->stop();
}

bool Game::isRunning() const
{
    return window->isOpen();
}

bool Game::isLapCompleted() const
{
    return checkpointHandler->isLapCompleted();
}