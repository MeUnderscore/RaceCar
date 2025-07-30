#pragma once
#include "../Interfaces/IRenderable.h"
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declaration
class CheckpointHandler;

// UI renderer for checkpoint counter - handles only UI rendering
class CheckpointUIRenderer : public IRenderable
{
private:
    const CheckpointHandler& checkpointHandler;
    sf::Font font;
    sf::Text* checkpointText;

public:
    CheckpointUIRenderer(const CheckpointHandler& handler);
    ~CheckpointUIRenderer();
    
    // IRenderable interface implementation
    void draw(sf::RenderWindow& window) const override;
    
    // Update the checkpoint text
    void updateText();
}; 