#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Checkpoint.h"

class CheckpointHandler
{
private:
    std::vector<std::unique_ptr<Checkpoint>> checkpoints;
    sf::Font font;
    sf::Text *checkpointText;
    int totalCheckpoints;
    int hitCheckpoints;

public:
    CheckpointHandler();
    ~CheckpointHandler();

    // Initialize checkpoints from track segment corners
    void initializeCheckpoints(const std::vector<std::vector<sf::Vector2f>> &segmentCorners);

    // Check if car position hits any checkpoint
    void checkCarPosition(const sf::Vector2f &carPosition);

    // Get the number of hit checkpoints
    int getHitCheckpoints() const;

    // Get the total number of checkpoints
    int getTotalCheckpoints() const;

    // Reset all checkpoints
    void resetAllCheckpoints();

    // Draw all checkpoints
    void drawCheckpoints(sf::RenderWindow &window);

    // Draw the checkpoint counter in the top right
    void drawCheckpointCounter(sf::RenderWindow &window);

    // Update the checkpoint counter text
    void updateCheckpointText();
};