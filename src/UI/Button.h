#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

class Button
{
private:
    sf::RectangleShape shape;
    sf::Text text;
    std::string label;
    std::function<void()> onClickCallback;
    bool isHovered;
    bool isPressed;

public:
    Button(const std::string& label, float x, float y, float width, float height);
    
    // Set the callback function for when button is clicked
    void setOnClick(std::function<void()> callback) { onClickCallback = callback; }
    
    // Handle input events
    void handleEvent(const sf::Event& event, const sf::Vector2f& mousePos);
    
    // Update button state
    void update(const sf::Vector2f& mousePos);
    
    // Draw the button
    void draw(sf::RenderWindow& window) const;
    
    // Getters
    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
    const std::string& getLabel() const { return label; }
    
    // Setters
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setEnabled(bool enabled);
}; 