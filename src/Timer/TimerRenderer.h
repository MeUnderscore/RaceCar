#pragma once
#include "../Interfaces/IRenderable.h"
#include "TimerLogic.h"
#include <SFML/Graphics.hpp>

// Timer renderer class - handles only timer rendering
class TimerRenderer : public IRenderable
{
private:
    TimerLogic& timerLogic;
    sf::Font font;
    sf::Text* timerText;

public:
    TimerRenderer(TimerLogic& timer);
    ~TimerRenderer();
    
    // IRenderable interface implementation
    void draw(sf::RenderWindow& window) const override;

private:
    void drawTimeVisualization(sf::RenderWindow& window, const std::string& timeStr) const;
}; 