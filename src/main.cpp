#include <SFML/Graphics.hpp>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 600), "RaceCar Game");
    window.setFramerateLimit(60);

    // Create a simple shape to test rendering
    sf::CircleShape shape(50.f);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(375, 275); // Center of the window

    // Game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window
        window.clear(sf::Color::Black);
        
        // Draw the shape
        window.draw(shape);
        
        // Display what was drawn
        window.display();
    }

    return 0;
} 