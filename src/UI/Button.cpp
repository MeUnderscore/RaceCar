#include "Button.h"
#include <iostream>

Button::Button(const std::string &label, float x, float y, float width, float height)
    : label(label), isHovered(false), isPressed(false)
{
    // Set up the button shape
    shape.setSize(sf::Vector2f(width, height));
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color(100, 100, 100)); // Default gray
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(2.0f);

    // Set up the text
    static sf::Font font;
    static bool fontLoaded = false;

    if (!fontLoaded)
    {
        if (font.loadFromFile("Fonts/ARIAL.TTF"))
        {
            fontLoaded = true;
        }
        else
        {
            std::cout << "Failed to load font for button" << std::endl;
        }
    }

    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::White);

    // Center the text on the button
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        x + (width - textBounds.width) / 2.0f,
        y + (height - textBounds.height) / 2.0f - 5.0f // Slight vertical adjustment
    );
}

void Button::handleEvent(const sf::Event &event, const sf::Vector2f &mousePos)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        if (shape.getGlobalBounds().contains(mousePos))
        {
            isPressed = true;
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        if (isPressed && shape.getGlobalBounds().contains(mousePos))
        {
            if (onClickCallback)
            {
                onClickCallback();
            }
        }
        isPressed = false;
    }
}

void Button::update(const sf::Vector2f &mousePos)
{
    bool wasHovered = isHovered;
    isHovered = shape.getGlobalBounds().contains(mousePos);

    // Update colors based on state
    if (isPressed)
    {
        shape.setFillColor(sf::Color(80, 80, 80)); // Darker when pressed
    }
    else if (isHovered)
    {
        shape.setFillColor(sf::Color(120, 120, 120)); // Lighter when hovered
    }
    else
    {
        shape.setFillColor(sf::Color(100, 100, 100)); // Default gray
    }
}

void Button::draw(sf::RenderWindow &window) const
{
    window.draw(shape);
    window.draw(text);
}

void Button::setPosition(float x, float y)
{
    shape.setPosition(x, y);

    // Update text position to stay centered
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        x + (shape.getSize().x - textBounds.width) / 2.0f,
        y + (shape.getSize().y - textBounds.height) / 2.0f - 5.0f);
}

void Button::setSize(float width, float height)
{
    shape.setSize(sf::Vector2f(width, height));

    // Update text position to stay centered
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        shape.getPosition().x + (width - textBounds.width) / 2.0f,
        shape.getPosition().y + (height - textBounds.height) / 2.0f - 5.0f);
}

void Button::setEnabled(bool enabled)
{
    if (enabled)
    {
        shape.setFillColor(sf::Color(100, 100, 100));
        text.setFillColor(sf::Color::White);
    }
    else
    {
        shape.setFillColor(sf::Color(60, 60, 60));
        text.setFillColor(sf::Color(150, 150, 150));
    }
}