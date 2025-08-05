#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "Checkpoint.h"
#include "../BezierShape/BezierShape.h" // For SegmentData

// Forward declaration
class CheckpointUIRenderer;

class CheckpointHandler
{
private:
    std::vector<std::unique_ptr<Checkpoint>> checkpoints;
    std::unique_ptr<Checkpoint> finalCheckpoint; // Final checkpoint at the start/finish line
    int totalCheckpoints;
    int hitCheckpoints;
    bool lapCompleted;

    // Multi-car support
    struct CarProgress
    {
        int hitCheckpoints;
        bool lapCompleted;
        sf::Vector2f lastPosition;
        
        CarProgress() : hitCheckpoints(0), lapCompleted(false), lastPosition(0.0f, 0.0f) {}
    };
    
    std::vector<CarProgress> carProgress;
    int maxCars;

public:
    CheckpointHandler();
    ~CheckpointHandler();

    // Initialize checkpoints from segment data
    void initializeCheckpoints(const std::vector<SegmentData> &segmentData);

    // Check if car position hits any checkpoint
    void checkCarPosition(const sf::Vector2f &carPosition);
    
    // Check collision with line segment (for fast-moving cars)
    void checkCarPositionWithLine(const sf::Vector2f &previousPosition, const sf::Vector2f &currentPosition);

    // Get the number of hit checkpoints
    int getHitCheckpoints() const;

    // Get the total number of checkpoints
    int getTotalCheckpoints() const;

    // Check if lap is completed
    bool isLapCompleted() const;

    // Reset all checkpoints
    void resetAllCheckpoints();

    // Draw all checkpoints
    void drawCheckpoints(sf::RenderWindow &window) const;

    // Get all checkpoints for external rendering
    const std::vector<std::unique_ptr<Checkpoint>>& getCheckpoints() const;
    const std::unique_ptr<Checkpoint>& getFinalCheckpoint() const;

    // Multi-car support methods
    void setMaxCars(int maxCars);
    void checkCarPosition(int carId, const sf::Vector2f &carPosition);
    void checkCarPositionWithLine(int carId, const sf::Vector2f &previousPosition, const sf::Vector2f &currentPosition);
    int getHitCheckpoints(int carId) const;
    bool isLapCompleted(int carId) const;
    float getProgressForCar(int carId) const;
    void resetCarProgress(int carId);
    void resetAllCarProgress();
};