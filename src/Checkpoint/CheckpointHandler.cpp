#include "CheckpointHandler.h"
#include <iostream>

CheckpointHandler::CheckpointHandler()
    : totalCheckpoints(0), hitCheckpoints(0), lapCompleted(false)
{
}

void CheckpointHandler::initializeCheckpoints(const std::vector<SegmentData> &segmentData)
{
    checkpoints.clear();
    finalCheckpoint.reset();
    totalCheckpoints = 0;
    hitCheckpoints = 0;
    lapCompleted = false;

    for (size_t i = 0; i < segmentData.size(); ++i)
    {
        const auto &segment = segmentData[i];

        // Calculate the four corners of the rectangle based on position, size, and rotation
        float cosRot = std::cos(segment.rotation * 3.14159f / 180.0f);
        float sinRot = std::sin(segment.rotation * 3.14159f / 180.0f);

        // Double the width for better collision detection at high speeds
        float doubledWidth = segment.size.x * 8.0f;

        std::vector<sf::Vector2f> corners;

        // Top-left corner
        corners.push_back(segment.position + sf::Vector2f(
                                                 -doubledWidth / 2.0f * cosRot - segment.size.y / 2.0f * sinRot,
                                                 -doubledWidth / 2.0f * sinRot + segment.size.y / 2.0f * cosRot));

        // Top-right corner
        corners.push_back(segment.position + sf::Vector2f(
                                                 doubledWidth / 2.0f * cosRot - segment.size.y / 2.0f * sinRot,
                                                 doubledWidth / 2.0f * sinRot + segment.size.y / 2.0f * cosRot));

        // Bottom-right corner
        corners.push_back(segment.position + sf::Vector2f(
                                                 doubledWidth / 2.0f * cosRot + segment.size.y / 2.0f * sinRot,
                                                 doubledWidth / 2.0f * sinRot - segment.size.y / 2.0f * cosRot));

        // Bottom-left corner
        corners.push_back(segment.position + sf::Vector2f(
                                                 -doubledWidth / 2.0f * cosRot + segment.size.y / 2.0f * sinRot,
                                                 -doubledWidth / 2.0f * sinRot - segment.size.y / 2.0f * cosRot));

        checkpoints.push_back(std::make_unique<Checkpoint>(corners, static_cast<int>(i)));
        totalCheckpoints++;

        // Create final checkpoint at the same position as the first checkpoint
        if (i == 0)
        {
            finalCheckpoint = std::make_unique<Checkpoint>(corners, -1); // Use -1 to indicate final checkpoint
        }
    }
}

CheckpointHandler::~CheckpointHandler()
{
}

void CheckpointHandler::checkCarPositionWithLine(const sf::Vector2f &previousPosition, const sf::Vector2f &currentPosition)
{
    // Debug output removed

    // Calculate distance moved to determine sampling frequency
    float distanceMoved = std::sqrt(
        (currentPosition.x - previousPosition.x) * (currentPosition.x - previousPosition.x) +
        (currentPosition.y - previousPosition.y) * (currentPosition.y - previousPosition.y));

    // Sample multiple points along the movement path for fast-moving cars
    int numSamples = std::max(1, static_cast<int>(distanceMoved / 10.0f)); // Sample every 10 pixels
    numSamples = std::min(numSamples, 10);                                 // Cap at 10 samples to avoid performance issues

    bool checkpointHit = false;
    int checkRange = 3; // Check up to 3 checkpoints ahead

    // Check each sample point along the movement path
    for (int sample = 0; sample <= numSamples && !checkpointHit; ++sample)
    {
        // Calculate sample position along the movement path
        float t = (numSamples > 0) ? static_cast<float>(sample) / static_cast<float>(numSamples) : 0.0f;
        sf::Vector2f samplePosition = previousPosition + t * (currentPosition - previousPosition);

        // Check the next few checkpoints in sequence
        for (int i = 0; i < checkRange && (hitCheckpoints + i) < checkpoints.size(); ++i)
        {
            auto &checkpoint = checkpoints[hitCheckpoints + i];

            if (!checkpoint->getIsHit() && checkpoint->isPointInside(samplePosition))
            {
                checkpoint->markAsHit();
                hitCheckpoints += i + 1;
                checkpointHit = true;
                break;
            }
        }
    }

    // If no checkpoint was hit with sampling, try the original line intersection method
    if (!checkpointHit)
    {
        for (int i = 0; i < checkRange && (hitCheckpoints + i) < checkpoints.size(); ++i)
        {
            auto &checkpoint = checkpoints[hitCheckpoints + i];

            if (!checkpoint->getIsHit() &&
                (checkpoint->isLineIntersecting(previousPosition, currentPosition) ||
                 checkpoint->isPointInside(currentPosition)))
            {
                checkpoint->markAsHit();
                hitCheckpoints += i + 1;
                checkpointHit = true;
                break;
            }
        }
    }

    // If no checkpoint was hit in the forward range, check if we missed any previous checkpoints
    if (!checkpointHit && hitCheckpoints > 0)
    {
        // Check a few checkpoints behind in case we missed one
        for (int i = 1; i <= 2 && (hitCheckpoints - i) >= 0; ++i)
        {
            auto &checkpoint = checkpoints[hitCheckpoints - i];

            if (!checkpoint->getIsHit() &&
                (checkpoint->isLineIntersecting(previousPosition, currentPosition) ||
                 checkpoint->isPointInside(currentPosition)))
            {
                checkpoint->markAsHit();
                break;
            }
        }
    }

    // Check final checkpoint only if all regular checkpoints are hit
    if (hitCheckpoints == totalCheckpoints && finalCheckpoint && !lapCompleted)
    {
        if (finalCheckpoint->isLineIntersecting(previousPosition, currentPosition) ||
            finalCheckpoint->isPointInside(currentPosition))
        {
            finalCheckpoint->markAsHit();
            lapCompleted = true;
        }
    }
}

int CheckpointHandler::getHitCheckpoints() const
{
    return hitCheckpoints;
}

int CheckpointHandler::getTotalCheckpoints() const
{
    return totalCheckpoints;
}

bool CheckpointHandler::isLapCompleted() const
{
    return lapCompleted;
}

void CheckpointHandler::resetAllCheckpoints()
{
    for (auto &checkpoint : checkpoints)
    {
        checkpoint->reset();
    }
    if (finalCheckpoint)
    {
        finalCheckpoint->reset();
    }
    hitCheckpoints = 0;
    lapCompleted = false;
}

void CheckpointHandler::drawCheckpoints(sf::RenderWindow &window) const
{
    // Only draw the next checkpoint (much faster than drawing all)
    if (hitCheckpoints < checkpoints.size())
    {
        checkpoints[hitCheckpoints]->draw(window);
    }

    // Draw final checkpoint only when all regular checkpoints are hit
    if (finalCheckpoint && hitCheckpoints == totalCheckpoints)
    {
        finalCheckpoint->draw(window);
    }
}

const std::vector<std::unique_ptr<Checkpoint>> &CheckpointHandler::getCheckpoints() const
{
    return checkpoints;
}

const std::unique_ptr<Checkpoint> &CheckpointHandler::getFinalCheckpoint() const
{
    return finalCheckpoint;
}