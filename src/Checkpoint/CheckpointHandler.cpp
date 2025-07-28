#include "CheckpointHandler.h"
#include <iostream>
#include <sstream>

CheckpointHandler::CheckpointHandler()
    : totalCheckpoints(0), hitCheckpoints(0), checkpointText(nullptr)
{
    // Load font for checkpoint counter
    if (!font.openFromFile("../../Fonts/ARIAL.TTF"))
    {
        std::cout << "Failed to load font for checkpoint counter!" << std::endl;
    }
    else
    {
        // Initialize checkpoint text
        checkpointText = new sf::Text(font, "Checkpoints: 0/0", 20);
        checkpointText->setFillColor(sf::Color::White);
        checkpointText->setOutlineColor(sf::Color::Black);
        checkpointText->setOutlineThickness(1.0f);
    }
}

void CheckpointHandler::initializeCheckpoints(const std::vector<std::vector<sf::Vector2f>> &segmentCorners)
{
    checkpoints.clear();
    totalCheckpoints = 0;
    hitCheckpoints = 0;

    for (size_t i = 0; i < segmentCorners.size(); ++i)
    {
        const auto &corners = segmentCorners[i];
        if (corners.size() == 4)
        {
            checkpoints.push_back(std::make_unique<Checkpoint>(corners, static_cast<int>(i)));
            totalCheckpoints++;
        }
    }

    updateCheckpointText();
    std::cout << "Initialized " << totalCheckpoints << " checkpoints" << std::endl;
}

CheckpointHandler::~CheckpointHandler()
{
    if (checkpointText)
    {
        delete checkpointText;
    }
}

void CheckpointHandler::checkCarPosition(const sf::Vector2f &carPosition)
{
    for (auto &checkpoint : checkpoints)
    {
        if (!checkpoint->getIsHit() && checkpoint->isPointInside(carPosition))
        {
            checkpoint->markAsHit();
            hitCheckpoints++;
            updateCheckpointText();
            std::cout << "Checkpoint " << checkpoint->getCheckpointNumber() << " hit! ("
                      << hitCheckpoints << "/" << totalCheckpoints << ")" << std::endl;
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

void CheckpointHandler::resetAllCheckpoints()
{
    for (auto &checkpoint : checkpoints)
    {
        checkpoint->reset();
    }
    hitCheckpoints = 0;
    updateCheckpointText();
    std::cout << "All checkpoints reset" << std::endl;
}

void CheckpointHandler::drawCheckpoints(sf::RenderWindow &window)
{
    for (const auto &checkpoint : checkpoints)
    {
        checkpoint->draw(window);
    }
}

void CheckpointHandler::drawCheckpointCounter(sf::RenderWindow &window)
{
    if (checkpointText)
    {
        // Position the text in the top right corner
        sf::Vector2u windowSize = window.getSize();
        checkpointText->setPosition(sf::Vector2f(
            windowSize.x - 200.0f,
            20.0f));

        window.draw(*checkpointText);
    }
}

void CheckpointHandler::updateCheckpointText()
{
    if (checkpointText)
    {
        std::ostringstream oss;
        oss << "Checkpoints: " << hitCheckpoints << "/" << totalCheckpoints;
        checkpointText->setString(oss.str());
    }
}