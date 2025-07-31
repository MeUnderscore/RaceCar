#pragma once
#include "NeuralNetwork.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class NetworkRender
{
private:
    struct NodeVisual
    {
        sf::CircleShape shape;
        int nodeId;
        bool isInput;
        bool isOutput;
        bool isHidden;
        
        NodeVisual() : nodeId(0), isInput(false), isOutput(false), isHidden(false) {}
    };

    struct ConnectionVisual
    {
        sf::RectangleShape shape;
        int fromNode;
        int toNode;
        double weight;
        
        ConnectionVisual() : fromNode(0), toNode(0), weight(0.0) {}
    };

    std::vector<NodeVisual> nodes;
    std::vector<ConnectionVisual> connections;
    
    // Visual properties
    float nodeRadius;
    float connectionWidth;
    float nodeSpacing;
    float layerSpacing;
    
    // Colors
    sf::Color inputNodeColor;
    sf::Color hiddenNodeColor;
    sf::Color outputNodeColor;
    sf::Color connectionColor;
    
    // Position and size
    sf::Vector2f position;
    sf::Vector2f size;

public:
    NetworkRender();
    
    // Set up the network visualization
    void setNetwork(const NeuralNetwork& network);
    
    // Update the visualization with current network values
    void updateValues(const NeuralNetwork& network);
    
    // Set visual properties
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setNodeRadius(float radius);
    void setConnectionWidth(float width);
    void setNodeSpacing(float spacing);
    void setLayerSpacing(float spacing);
    
    // Set colors
    void setInputNodeColor(const sf::Color& color);
    void setHiddenNodeColor(const sf::Color& color);
    void setOutputNodeColor(const sf::Color& color);
    void setConnectionColor(const sf::Color& color);
    
    // Draw the network
    void draw(sf::RenderWindow& window) const;
    
    // Getters
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getSize() const { return size; }

private:
    // Helper methods
    void createNodeVisuals(const NeuralNetwork& network);
    void createConnectionVisuals(const NeuralNetwork& network);
    void layoutNodes();
    void layoutConnections();
    sf::Color getWeightColor(double weight) const;
}; 