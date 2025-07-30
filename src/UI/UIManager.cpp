#include "UIManager.h"
#include <iostream>

UIManager::UIManager() : fontLoaded(false)
{
}

void UIManager::initialize()
{
    std::cout << "UIManager::initialize() started" << std::endl;
    
    // Load font
    if (!fontLoaded)
    {
        if (font.openFromFile("../../Fonts/ARIAL.TTF"))
        {
            fontLoaded = true;
            std::cout << "UIManager font loaded successfully" << std::endl;
        }
        else
        {
            std::cout << "UIManager font loading failed" << std::endl;
            // Font loading failed silently
        }
    }

    // Create buttons
    buttons.clear();
    buttonLabels.clear();
    std::cout << "Creating buttons..." << std::endl;

    // Start button (Green)
    std::cout << "Creating Start button..." << std::endl;
    buttons.emplace_back("", 10, 10, 80, 30); // Empty label since we'll use separate text
    std::cout << "Start button created, setting colors..." << std::endl;
    buttons[0].setColors(sf::Color(50, 150, 50), sf::Color(70, 170, 70), sf::Color(30, 130, 30));

    // Pause button (Red)
    std::cout << "Creating Pause button..." << std::endl;
    buttons.emplace_back("", 100, 10, 80, 30); // Empty label since we'll use separate text
    std::cout << "Pause button created, setting colors..." << std::endl;
    buttons[1].setColors(sf::Color(150, 50, 50), sf::Color(170, 70, 70), sf::Color(130, 30, 30));

    // Save button (Blue)
    std::cout << "Creating Save button..." << std::endl;
    buttons.emplace_back("", 190, 10, 80, 30); // Empty label since we'll use separate text
    std::cout << "Save button created, setting colors..." << std::endl;
    buttons[2].setColors(sf::Color(50, 50, 150), sf::Color(70, 70, 170), sf::Color(30, 30, 130));

    // Create button labels if font is loaded
    if (fontLoaded)
    {
        // Start label
        sf::Text startLabel(font, "Start", 14);
        startLabel.setFillColor(sf::Color::White);
        startLabel.setOutlineColor(sf::Color::Black);
        startLabel.setOutlineThickness(1.0f);
        startLabel.setPosition(sf::Vector2f(25, 18)); // Position over the Start button
        buttonLabels.push_back(startLabel);

        // Pause label
        sf::Text pauseLabel(font, "Pause", 14);
        pauseLabel.setFillColor(sf::Color::White);
        pauseLabel.setOutlineColor(sf::Color::Black);
        pauseLabel.setOutlineThickness(1.0f);
        pauseLabel.setPosition(sf::Vector2f(115, 18)); // Position over the Pause button
        buttonLabels.push_back(pauseLabel);

        // Save label
        sf::Text saveLabel(font, "Save", 14);
        saveLabel.setFillColor(sf::Color::White);
        saveLabel.setOutlineColor(sf::Color::Black);
        saveLabel.setOutlineThickness(1.0f);
        saveLabel.setPosition(sf::Vector2f(205, 18)); // Position over the Save button
        buttonLabels.push_back(saveLabel);
        
        std::cout << "Button labels created successfully" << std::endl;
    }
    else
    {
        std::cout << "No font loaded, buttons will have no labels" << std::endl;
    }

    // Initially disable Pause and Save buttons
    setStopEnabled(false);
    setSaveEnabled(false);
    
    std::cout << "UIManager::initialize() completed" << std::endl;
}

void UIManager::handleEvent(const sf::Event &event, const sf::Vector2f &mousePos)
{
    for (auto &button : buttons)
    {
        button.handleEvent(event, mousePos);
    }
}

void UIManager::update(const sf::Vector2f &mousePos)
{
    for (auto &button : buttons)
    {
        button.update(mousePos);
    }
}

void UIManager::draw(sf::RenderWindow &window) const
{
    // Draw buttons
    for (const auto &button : buttons)
    {
        button.draw(window);
    }
    
    // Draw button labels
    for (const auto &label : buttonLabels)
    {
        window.draw(label);
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