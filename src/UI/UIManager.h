#pragma once
#include "Button.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

class UIManager
{
private:
    std::vector<Button> buttons;
    std::vector<sf::Text> buttonLabels;
    sf::Font font;
    bool fontLoaded;

public:
    UIManager();
    
    // Initialize UI elements
    void initialize();
    
    // Handle input events
    void handleEvent(const sf::Event& event, const sf::Vector2f& mousePos);
    
    // Update UI state
    void update(const sf::Vector2f& mousePos);
    
    // Draw UI elements
    void draw(sf::RenderWindow& window) const;
    
    // Button callbacks
    void setStartCallback(std::function<void()> callback);
    void setStopCallback(std::function<void()> callback);
    void setSaveCallback(std::function<void()> callback);
    
    // Enable/disable buttons
    void setStartEnabled(bool enabled);
    void setStopEnabled(bool enabled);
    void setSaveEnabled(bool enabled);
}; 