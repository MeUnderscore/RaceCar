#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "../Interfaces/IRenderable.h"
#include "../Interfaces/ITimer.h"
#include "../Timer/TimerLogic.h"
#include "../Timer/TimerRenderer.h"
#include "../Checkpoint/CheckpointHandler.h"
#include "../Checkpoint/CheckpointUIRenderer.h"
#include "../Car/Car.h"
#include "../Track/track.h"
#include "../Background/Background.h"

// Game class that manages the overall game state
class Game
{
private:
    // Game objects
    std::unique_ptr<sf::RenderWindow> window;
    std::unique_ptr<Background> background;
    std::unique_ptr<Track> track;
    std::unique_ptr<Car> car;
    
    // Timer system (separated logic and rendering)
    std::unique_ptr<TimerLogic> timerLogic;
    std::unique_ptr<TimerRenderer> timerRenderer;
    
    // Checkpoint system
    std::unique_ptr<CheckpointHandler> checkpointHandler;
    std::unique_ptr<CheckpointUIRenderer> checkpointUIRenderer;
    
    // Performance optimization
    sf::Clock deltaClock;
    float deltaTime;
    sf::Vector2f previousCarPosition;

public:
    Game(unsigned int width, unsigned int height);
    ~Game();
    
    // Game loop
    void run();
    
    // Game state management
    void handleEvents();
    void update();
    void render();
    
    // Game control
    void reset();
    void startTimer();
    void stopTimer();
    
    // Getters
    bool isRunning() const;
    bool isLapCompleted() const;
}; 