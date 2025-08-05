#include "Game.h"
#include "../Background/Background.h"
#include "../Track/track.h"
#include "../Car/Car.h"
#include "../Timer/TimerLogic.h"
#include "../Timer/TimerRenderer.h"
#include "../Checkpoint/CheckpointHandler.h"
#include "../Checkpoint/CheckpointUIRenderer.h"
#include "../UI/UIManager.h"
#include "../AI/NetworkRenderHandler.h"
#include "../AI/Population.h"
#include "../AI/AIController.h"
#include <iostream>
#include <unordered_map> // Added for checkCarStuck
#include <iomanip>       // Added for std::fixed and std::setprecision

Game::Game(unsigned int width, unsigned int height)
    : deltaTime(0.016f), fps(60.0f), frameCount(0), performanceFontLoaded(false),
      fpsText(nullptr), // Default to 60 FPS
      aiLearningEnabled(false), aiLearningPaused(false), generationTime(0.0f),
      maxGenerationTime(20.0f), currentGeneration(0), bestFitnessGeneration(0)
{
    // Create window
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode({width, height}), "Race Car - AI Learning Simulation");
    window->setFramerateLimit(60);
    window->setVerticalSyncEnabled(true);

    // Create game objects
    background = std::make_unique<Background>(width, height, 128);
    track = std::make_unique<Track>(width, height);

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
    uiManager->setStartCallback([this]()
                                {
                                    std::cout << "Start button clicked!" << std::endl;
                                    startAILearning(); });

    uiManager->setStopCallback([this]()
                               {
                                   std::cout << "Pause button clicked!" << std::endl;
                                   pauseAILearning(); });

    uiManager->setSaveCallback([this]()
                               {
                                   std::cout << "Save button clicked!" << std::endl;
                                   saveAITrainingData(); });

    // Initialize AI population
    const int numInputs = 10;      // 8 ray sensors + speed + rotation
    const int numOutputs = 2;      // steering + acceleration
    const int numHidden = 0;       // Start with 0 hidden nodes (basic structure)
    const int populationSize = 25; // Reduced from 50 to 25 for better performance

    aiPopulation = std::make_unique<Population>(populationSize, numInputs, numOutputs, numHidden);

    // Set up checkpoint handler for multiple cars
    checkpointHandler->setMaxCars(populationSize);

    createAICars();

    // Initialize performance monitoring
    if (performanceFont.openFromFile("../../Fonts/ARIAL.TTF"))
    {
        performanceFontLoaded = true;
        fpsText = new sf::Text(performanceFont, "", 12);
        fpsText->setFillColor(sf::Color::Yellow);
        fpsText->setOutlineColor(sf::Color::Black);
        fpsText->setOutlineThickness(1.0f);
    }

    // Initialize network visualization
    networkRenderHandler = std::make_unique<NetworkRenderHandler>();
    networkRenderHandler->setDisplayPosition(20.0f, 800.0f);
    networkRenderHandler->setDisplaySize(400.0f, 300.0f);
    networkRenderHandler->setVisible(true); // Start visible for testing
}

Game::~Game()
{
    if (fpsText)
    {
        delete fpsText;
    }
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
    // Calculate delta time
    deltaTime = deltaClock.restart().asSeconds();

    // Update performance statistics
    updatePerformanceStats();

    // Update timer
    timerLogic->update();

    // Update UI
    checkpointUIRenderer->updateText();

    // Update UI manager
    sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    uiManager->update(mousePos);

    // Update AI cars if AI learning is enabled
    if (aiLearningEnabled)
    {
        updateAICars(deltaTime);
        updateNetworkVisualization();
    }
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

    // Draw AI cars if AI learning is enabled
    if (aiLearningEnabled)
    {
        for (const auto &aiCar : aiCars)
        {
            aiCar->draw(*window);
            aiCar->drawRaySensors(*window);
        }
    }

    // Draw checkpoints
    checkpointHandler->drawCheckpoints(*window);

    // Draw UI
    timerRenderer->draw(*window);
    checkpointUIRenderer->draw(*window);

    // Draw UI manager (buttons)
    uiManager->draw(*window);

    // Draw performance stats
    drawPerformanceStats();

    // Draw AI stats
    drawAIStats();

    // Draw network visualization
    if (networkRenderHandler)
    {
        networkRenderHandler->draw(*window);
    }

    window->display();
}

void Game::reset()
{
    timerLogic->reset();
    checkpointHandler->resetAllCheckpoints();
    checkpointUIRenderer->updateText();
    if (aiLearningEnabled)
    {
        stopAILearning();
        resetAICars();
    }
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

void Game::updatePerformanceStats()
{
    frameCount++;

    // Update FPS every second
    if (fpsClock.getElapsedTime().asSeconds() >= 1.0f)
    {
        fps = static_cast<float>(frameCount) / fpsClock.getElapsedTime().asSeconds();
        frameCount = 0;
        fpsClock.restart();
    }
}

void Game::drawPerformanceStats()
{
    if (!performanceFontLoaded || !fpsText)
        return;

    try
    {
        // Position in top-right corner, moved down to avoid AI stats overlap
        float windowWidth = static_cast<float>(window->getSize().x);
        fpsText->setPosition(sf::Vector2f(windowWidth - 350.0f, 200.0f));

        // Update text content
        std::string fpsString = "FPS: " + std::to_string(static_cast<int>(fps));
        std::string frameTimeString = "Frame: " + std::to_string(static_cast<int>(deltaTime * 1000.0f)) + "ms";

        fpsText->setString(fpsString + "\n" + frameTimeString);

        window->draw(*fpsText);
    }
    catch (...)
    {
        // Silently fail if text rendering fails
    }
}

// AI Learning Methods

void Game::initializeAIPopulation()
{
    // Initialize NEAT population with 50 cars, 19 inputs (16 rays + speed + rotation + checkpoint progress), 2 outputs (steering, acceleration)
    const int populationSize = 50;
    const int numInputs = 19; // 16 ray sensors + speed + rotation + checkpoint progress
    const int numOutputs = 2; // steering, acceleration
    const int numHidden = 8;  // hidden layer size

    aiPopulation = std::make_unique<Population>(populationSize, numInputs, numOutputs, numHidden);

    // Set up checkpoint handler for multiple cars
    checkpointHandler->setMaxCars(populationSize);

    createAICars();
}

void Game::createAICars()
{
    aiCars.clear();

    // Get the starting position from the track
    sf::Vector2f startPos = track->getStartPosition();
    float startRotation = track->getStartRotation();

    // Create cars for each AI controller
    const auto &controllers = aiPopulation->getControllers();
    for (size_t i = 0; i < controllers.size(); ++i)
    {
        auto car = std::make_unique<Car>(startPos.x, startPos.y);
        car->setPosition(startPos.x, startPos.y);
        aiCars.push_back(std::move(car));

        // Reset the controller with the car's position
        controllers[i]->reset(startPos, startRotation);
    }
}

void Game::startAILearning()
{
    aiLearningEnabled = true;
    aiLearningPaused = false;
    currentGeneration = aiPopulation->getGeneration();
    generationTime = 0.0f;

    // Reset checkpoints for new generation
    checkpointHandler->resetAllCarProgress();

    std::cout << "=== AI LEARNING SIMULATION STARTED ===" << std::endl;
    std::cout << "Generation: " << currentGeneration << " (20s limit per generation)" << std::endl;
    std::cout << "Population Size: " << aiCars.size() << " cars" << std::endl;
    std::cout << "Press the buttons to control the simulation." << std::endl;
}

void Game::pauseAILearning()
{
    aiLearningPaused = !aiLearningPaused;
    std::cout << "AI Learning " << (aiLearningPaused ? "PAUSED" : "RESUMED") << std::endl;
}

void Game::stopAILearning()
{
    aiLearningEnabled = false;
    aiLearningPaused = false;
    std::cout << "=== AI LEARNING SIMULATION STOPPED ===" << std::endl;
}

void Game::saveAITrainingData()
{
    // TODO: Implement saving the best neural network to file
    std::cout << "AI training data saved!" << std::endl;
}

void Game::loadAITrainingData()
{
    // TODO: Implement loading neural network from file
    std::cout << "AI training data loaded!" << std::endl;
}

void Game::evolvePopulation()
{
    if (!aiPopulation)
        return;

    // Evolve the population
    aiPopulation->evolve();
    currentGeneration = aiPopulation->getGeneration();

    // Reset all cars for the new generation
    resetAICars();

    // Update best fitness tracking
    double currentBestFitness = aiPopulation->getBestFitness();
    if (currentBestFitness > 0) // Only update if we have valid fitness
    {
        bestFitnessGeneration = currentGeneration;
    }

    std::cout << "=== GENERATION " << currentGeneration << " COMPLETED ===" << std::endl;
    std::cout << "Best Fitness: " << currentBestFitness << std::endl;
    std::cout << "Generation Time: " << std::fixed << std::setprecision(1) << generationTime << "s" << std::endl;
    std::cout << "Species Count: " << aiPopulation->getSpeciesCount() << std::endl;
    std::cout << "Starting next generation..." << std::endl;
}

void Game::resetAICars()
{
    if (!aiPopulation)
        return;

    sf::Vector2f startPos = track->getStartPosition();
    float startRotation = track->getStartRotation();

    const auto &controllers = aiPopulation->getControllers();
    for (size_t i = 0; i < aiCars.size() && i < controllers.size(); ++i)
    {
        aiCars[i]->resetPosition();
        aiCars[i]->setPosition(startPos.x, startPos.y);
        controllers[i]->reset(startPos, startRotation);
    }

    generationTime = 0.0f;
}

void Game::updateAICars(float deltaTime)
{
    if (!aiPopulation || !aiLearningEnabled || aiLearningPaused)
        return;

    const auto &controllers = aiPopulation->getControllers();

    for (size_t i = 0; i < aiCars.size() && i < controllers.size(); ++i)
    {
        auto &car = aiCars[i];
        auto &controller = controllers[i];

        // Store previous position for checkpoint detection
        sf::Vector2f previousPosition = sf::Vector2f(car->getX(), car->getY());

        // Update car physics
        car->update(deltaTime);

        // Update ray sensors
        car->updateRaySensors(track->getInnerEdgePoints(), track->getOuterEdgePoints());

        // Handle collisions with track edges (bounce back instead of instant kill)
        car->handleCollision(track->getInnerEdgePoints(), track->getOuterEdgePoints());

        // Check checkpoint progress
        sf::Vector2f currentPosition = sf::Vector2f(car->getX(), car->getY());
        checkpointHandler->checkCarPositionWithLine(i, previousPosition, currentPosition);

        // Update checkpoint count for this controller
        int currentCheckpoints = checkpointHandler->getHitCheckpoints(i);
        controller->setCheckpointsHit(currentCheckpoints);

        // Get sensor data
        std::vector<float> rayDistances = car->getRayDistances();

        // Control the car with AI
        controller->controlCar(*car, rayDistances);

        // Update fitness
        controller->updateFitness(deltaTime);

        // Debug output for first few cars
        if (i < 3 && controller->isCarAlive())
        {
            static int debugCounter = 0;
            debugCounter++;
            if (debugCounter % 60 == 0) // Every 60 frames (1 second at 60 FPS)
            {
                std::cout << "Car " << i << " - Fitness: " << controller->getFitness()
                          << ", Checkpoints: " << currentCheckpoints
                          << ", Time Alive: " << controller->getTimeAlive() << std::endl;
            }
        }

        // Check if car is stuck (but don't check for crashes again since we already did)
        if (checkCarStuck(*car))
        {
            controller->kill();
        }
    }

    // Update generation timer
    generationTime += deltaTime;

    // Check if generation should end
    if (generationTime >= maxGenerationTime || allAICarsFinished())
    {
        evolvePopulation();
    }
}

bool Game::allAICarsFinished() const
{
    if (!aiPopulation)
        return true;

    const auto &controllers = aiPopulation->getControllers();
    for (const auto &controller : controllers)
    {
        if (controller->isCarAlive())
            return false;
    }
    return true;
}

bool Game::checkCarStuck(const Car &car) const
{
    // Check if car hasn't moved much in the last few seconds
    // This is a simple implementation - could be improved
    static std::unordered_map<const Car *, sf::Vector2f> lastPositions;
    static std::unordered_map<const Car *, float> stuckTimers;
    static int frameCounter = 0;

    // Only check every 10 frames for better performance
    frameCounter++;
    if (frameCounter % 10 != 0)
    {
        return false;
    }

    sf::Vector2f currentPos(car.getX(), car.getY());

    // Initialize if first time seeing this car
    if (lastPositions.find(&car) == lastPositions.end())
    {
        lastPositions[&car] = currentPos;
        stuckTimers[&car] = 0.0f;
        return false;
    }

    sf::Vector2f lastPos = lastPositions[&car];
    float dx = currentPos.x - lastPos.x;
    float dy = currentPos.y - lastPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < 5.0f) // Less than 5 pixels movement
    {
        stuckTimers[&car] += this->deltaTime;
        if (stuckTimers[&car] > 3.0f) // Stuck for 3 seconds
        {
            stuckTimers[&car] = 0.0f;
            lastPositions[&car] = currentPos;
            return true;
        }
    }
    else
    {
        stuckTimers[&car] = 0.0f;
    }

    lastPositions[&car] = currentPos;
    return false;
}

bool Game::checkCarCrashed(const Car &car) const
{
    // Get car position
    sf::Vector2f carPos(car.getX(), car.getY());

    // Get track edge points
    const auto &innerEdgePoints = this->track->getInnerEdgePoints();
    const auto &outerEdgePoints = this->track->getOuterEdgePoints();

    // Check if car is too close to any track edge point
    const float collisionRadius = 15.0f; // Collision radius around track edges

    // Check inner edge collision - only check every few points for performance
    for (size_t i = 0; i < innerEdgePoints.size(); i += 3) // Check every 3rd point
    {
        const auto &edgePoint = innerEdgePoints[i];
        float dx = carPos.x - edgePoint.x;
        float dy = carPos.y - edgePoint.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance < collisionRadius)
        {
            return true; // Car hit inner edge
        }
    }

    // Check outer edge collision - only check every few points for performance
    for (size_t i = 0; i < outerEdgePoints.size(); i += 3) // Check every 3rd point
    {
        const auto &edgePoint = outerEdgePoints[i];
        float dx = carPos.x - edgePoint.x;
        float dy = carPos.y - edgePoint.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance < collisionRadius)
        {
            return true; // Car hit outer edge
        }
    }

    // Also check if car is completely outside track bounds
    // Note: Track bounds check removed due to SFML API compatibility issues
    // The edge collision detection above should be sufficient for detecting crashes

    return false;
}

void Game::drawAIStats()
{
    if (!performanceFontLoaded || !fpsText || !aiPopulation)
        return;

    try
    {
        // Create AI stats text
        sf::Text aiStatsText(performanceFont, "", 16);
        aiStatsText.setFillColor(sf::Color::Green);
        aiStatsText.setOutlineColor(sf::Color::Black);
        aiStatsText.setOutlineThickness(1.0f);
        aiStatsText.setPosition(sf::Vector2f(10.0f, 10.0f));

        std::string aiStats = "=== AI LEARNING SIMULATION ===\n";
        aiStats += "Status: " + std::string(aiLearningEnabled ? "ACTIVE" : "STOPPED") + "\n";
        aiStats += "Paused: " + std::string(aiLearningPaused ? "YES" : "NO") + "\n";
        aiStats += "Generation: " + std::to_string(currentGeneration) + "\n";
        aiStats += "Best Fitness: " + std::to_string(static_cast<int>(aiPopulation->getBestFitness())) + "\n";
        aiStats += "Species Count: " + std::to_string(aiPopulation->getSpeciesCount()) + "\n";
        aiStats += "Generation Time: " + std::to_string(static_cast<int>(generationTime)) + "/20s\n";
        aiStats += "Active Cars: " + std::to_string(aiCars.size()) + "\n";

        aiStatsText.setString(aiStats);
        window->draw(aiStatsText);
    }
    catch (...)
    {
        // Silently fail if text rendering fails
    }
}

void Game::updateNetworkVisualization()
{
    if (!networkRenderHandler || !aiPopulation)
        return;

    // Show the best performing network
    const auto &controllers = aiPopulation->getControllers();
    if (!controllers.empty())
    {
        // Find the best controller
        auto bestController = controllers[0];
        double bestFitness = bestController->getFitness();

        for (const auto &controller : controllers)
        {
            if (controller->getFitness() > bestFitness)
            {
                bestFitness = controller->getFitness();
                bestController = controller;
            }
        }

        // Update network visualization with the best brain
        networkRenderHandler->setNetwork(&bestController->getBrain());
    }
}