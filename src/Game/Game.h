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
class Population;
class AIController;

class Game
{
private:
    std::unique_ptr<sf::RenderWindow> window;
    std::unique_ptr<Background> background;
    std::unique_ptr<Track> track;
    std::unique_ptr<TimerLogic> timerLogic;
    std::unique_ptr<TimerRenderer> timerRenderer;
    std::unique_ptr<CheckpointHandler> checkpointHandler;
    std::unique_ptr<CheckpointUIRenderer> checkpointUIRenderer;
    std::unique_ptr<UIManager> uiManager;

    // AI Population management
    std::unique_ptr<Population> aiPopulation;
    std::vector<std::unique_ptr<Car>> aiCars;
    bool aiLearningEnabled;
    bool aiLearningPaused;
    float generationTime;
    float maxGenerationTime;
    int currentGeneration;
    int bestFitnessGeneration;

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

    // AI Learning methods
    void startAILearning();
    void pauseAILearning();
    void stopAILearning();
    void saveAITrainingData();
    void loadAITrainingData();
    void evolvePopulation();
    void resetAICars();
    void updateAICars(float deltaTime);
    bool allAICarsFinished() const;
    bool checkCarStuck(const Car& car) const;
    bool checkCarCrashed(const Car& car) const;

private:
    void updatePerformanceStats();
    void drawPerformanceStats();
    void drawAIStats();
    void initializeAIPopulation();
    void createAICars();
    void updateNetworkVisualization();
}; 