#include "CheckpointUIRenderer.h"
#include "CheckpointHandler.h"
#include <iostream>
#include <sstream>

CheckpointUIRenderer::CheckpointUIRenderer(const CheckpointHandler &handler)
    : checkpointHandler(handler), checkpointText(nullptr)
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

CheckpointUIRenderer::~CheckpointUIRenderer()
{
    if (checkpointText)
    {
        delete checkpointText;
    }
}

void CheckpointUIRenderer::draw(sf::RenderWindow &window) const
{
    if (checkpointText)
    {
        try
        {
            // Position the text in the top right corner
            sf::Vector2u windowSize = window.getSize();
            checkpointText->setPosition(sf::Vector2f(
                windowSize.x - 200.0f,
                20.0f));

            window.draw(*checkpointText);
        }
        catch (...)
        {
            // If drawing text fails, just skip it
        }
    }
}

void CheckpointUIRenderer::updateText()
{
    if (checkpointText)
    {
        std::ostringstream oss;
        oss << "Checkpoints: " << checkpointHandler.getHitCheckpoints() << "/" << checkpointHandler.getTotalCheckpoints();
        checkpointText->setString(oss.str());
    }
}