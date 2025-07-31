#pragma once
#include "NetworkRender.h"
#include "NeuralNetwork.h"
#include <memory>

class NetworkRenderHandler
{
private:
    std::unique_ptr<NetworkRender> networkRender;
    const NeuralNetwork* currentNetwork;
    bool isVisible;
    
    // Display settings
    float displayX;
    float displayY;
    float displayWidth;
    float displayHeight;

public:
    NetworkRenderHandler();
    
    // Set the network to visualize
    void setNetwork(const NeuralNetwork* network);
    
    // Update the visualization
    void update();
    
    // Draw the network
    void draw(sf::RenderWindow& window) const;
    
    // Visibility control
    void setVisible(bool visible) { isVisible = visible; }
    bool getVisible() const { return isVisible; }
    
    // Display settings
    void setDisplayPosition(float x, float y);
    void setDisplaySize(float width, float height);
    
    // Get current network
    const NeuralNetwork* getCurrentNetwork() const { return currentNetwork; }
    
    // Toggle visibility
    void toggleVisibility() { isVisible = !isVisible; }
}; 