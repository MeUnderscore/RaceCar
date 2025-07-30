#include "Button.h"
#include <iostream>

Button::Button(const std::string &label, float x, float y, float width, float height)
    : label(label), isHovered(false), isPressed(false),
      baseColor(100, 100, 100), hoverColor(120, 120, 120), pressedColor(80, 80, 80)
{
    std::cout << "Creating button: " << label << std::endl;

    // Set up the button shape
    shape.setSize(sf::Vector2f(width, height));
    shape.setPosition(sf::Vector2f(x, y));
    shape.setFillColor(baseColor);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(2.0f);

    std::cout << "Button constructor completed for: " << label << std::endl;
}

void Button::handleEvent(const sf::Event &event, const sf::Vector2f &mousePos)
{
    if (auto mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button == sf::Mouse::Button::Left && shape.getGlobalBounds().contains(mousePos))
        {
            isPressed = true;
        }
    }
    else if (auto mouseReleased = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (mouseReleased->button == sf::Mouse::Button::Left && isPressed && shape.getGlobalBounds().contains(mousePos))
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
        shape.setFillColor(pressedColor);
    }
    else if (isHovered)
    {
        shape.setFillColor(hoverColor);
    }
    else
    {
        shape.setFillColor(baseColor);
    }
}

void Button::draw(sf::RenderWindow &window) const
{
    window.draw(shape);
}

void Button::setPosition(float x, float y)
{
    shape.setPosition(sf::Vector2f(x, y));
}

void Button::setSize(float width, float height)
{
    shape.setSize(sf::Vector2f(width, height));
}

void Button::setEnabled(bool enabled)
{
    if (enabled)
    {
        shape.setFillColor(baseColor);
    }
    else
    {
        shape.setFillColor(sf::Color(60, 60, 60));
    }
}

void Button::setColors(const sf::Color &base, const sf::Color &hover, const sf::Color &pressed)
{
    baseColor = base;
    hoverColor = hover;
    pressedColor = pressed;

    // Update current color if not disabled
    if (shape.getFillColor() != sf::Color(60, 60, 60))
    {
        if (isPressed)
        {
            shape.setFillColor(pressedColor);
        }
        else if (isHovered)
        {
            shape.setFillColor(hoverColor);
        }
        else
        {
            shape.setFillColor(baseColor);
        }
    }
}