#include "UIManager.h"
#include <iostream>

UIManager::UIManager() : fontLoaded(false)
{
}

void UIManager::initialize()
{
    // Load font
    if (!fontLoaded)
    {
        if (font.loadFromFile("Fonts/ARIAL.TTF"))
        {
            fontLoaded = true;
        }
        else
        {
            std::cout << "Failed to load font for UI" << std::endl;
        }
    }
    
    // Create buttons
    buttons.clear();
    
    // Start button
    buttons.emplace_back("Start", 10, 10, 80, 30);
    
    // Stop button
    buttons.emplace_back("Stop", 100, 10, 80, 30);
    
    // Save button
    buttons.emplace_back("Save", 190, 10, 80, 30);
    
    // Initially disable Stop and Save buttons
    setStopEnabled(false);
    setSaveEnabled(false);
}

void UIManager::handleEvent(const sf::Event& event, const sf::Vector2f& mousePos)
{
    for (auto& button : buttons)
    {
        button.handleEvent(event, mousePos);
    }
}

void UIManager::update(const sf::Vector2f& mousePos)
{
    for (auto& button : buttons)
    {
        button.update(mousePos);
    }
}

void UIManager::draw(sf::RenderWindow& window) const
{
    for (const auto& button : buttons)
    {
        button.draw(window);
    }
}

void UIManager::setStartCallback(std::function<void()> callback)
{
    if (buttons.size() > 0)
    {
        buttons[0].setOnClick(callback);
    }
}

void UIManager::setStopCallback(std::function<void()> callback)
{
    if (buttons.size() > 1)
    {
        buttons[1].setOnClick(callback);
    }
}

void UIManager::setSaveCallback(std::function<void()> callback)
{
    if (buttons.size() > 2)
    {
        buttons[2].setOnClick(callback);
    }
}

void UIManager::setStartEnabled(bool enabled)
{
    if (buttons.size() > 0)
    {
        buttons[0].setEnabled(enabled);
    }
}

void UIManager::setStopEnabled(bool enabled)
{
    if (buttons.size() > 1)
    {
        buttons[1].setEnabled(enabled);
    }
}

void UIManager::setSaveEnabled(bool enabled)
{
    if (buttons.size() > 2)
    {
        buttons[2].setEnabled(enabled);
    }
} 