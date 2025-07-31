#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

// Forward declarations
class Background;
class Track;
class Car;
class TimerLogic;
class TimerRenderer;
class CheckpointHandler;
class CheckpointUIRenderer;
class UIManager;
class NetworkRenderHandler;

class Game
{
private:
    std::unique_ptr<sf::RenderWindow> window;
    std::unique_ptr<Background> background;
    std::unique_ptr<Track> track;
    std::unique_ptr<Car> car;
    std::unique_ptr<TimerLogic> timerLogic;
    std::unique_ptr<TimerRenderer> timerRenderer;
    std::unique_ptr<CheckpointHandler> checkpointHandler;
    std::unique_ptr<CheckpointUIRenderer> checkpointUIRenderer;
    std::unique_ptr<UIManager> uiManager;

    // Performance monitoring
    sf::Clock performanceClock;
    sf::Clock fpsClock;
    float deltaTime;
    float fps;
    int frameCount;
    sf::Font performanceFont;
    sf::Text* fpsText;
    bool performanceFontLoaded;
    
    // Network visualization
    std::unique_ptr<NetworkRenderHandler> networkRenderHandler;

    // Game state
    sf::Vector2f previousCarPosition;
    sf::Clock deltaClock;

public:
    Game(unsigned int width, unsigned int height);
    ~Game();

    void run();
    void handleEvents();
    void update();
    void render();
    void reset();
    void startTimer();
    void stopTimer();
    bool isRunning() const;
    bool isLapCompleted() const;

private:
    void updatePerformanceStats();
    void drawPerformanceStats();
}; 