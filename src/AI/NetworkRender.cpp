#include "NetworkRender.h"
#include <iostream>
#include <sstream>
#include <iomanip>

NetworkRender::NetworkRender()
    : nodeRadius(12.0f), connectionWidth(2.0f), nodeSpacing(35.0f), layerSpacing(80.0f),
      inputNodeColor(100, 150, 255), hiddenNodeColor(255, 200, 100), outputNodeColor(100, 255, 150),
      connectionColor(200, 200, 200), position(0, 0), size(400, 300)
{
}

void NetworkRender::setNetwork(const NeuralNetwork &network)
{
    // Clear existing visuals
    nodes.clear();
    connections.clear();

    // Create new visuals
    createNodeVisuals(network);
    createConnectionVisuals(network);

    // Layout everything
    layoutNodes();
    layoutConnections();

    // Update text
    updateValues(network);
}

void NetworkRender::updateValues(const NeuralNetwork &network)
{
    // Update connection colors based on weights
    for (auto &conn : connections)
    {
        conn.shape.setFillColor(getWeightColor(conn.weight));
    }
}

void NetworkRender::createNodeVisuals(const NeuralNetwork &network)
{
    // Get actual network structure from the neural network
    int numInputs = network.getNumInputs();
    int numOutputs = network.getNumOutputs();
    int numHidden = network.getNumHidden();

    // Create input nodes
    for (int i = 0; i < numInputs; ++i)
    {
        NodeVisual node;
        node.shape.setRadius(nodeRadius);
        node.shape.setFillColor(inputNodeColor);
        node.shape.setOutlineColor(sf::Color::Black);
        node.shape.setOutlineThickness(2.0f);

        node.nodeId = i;
        node.isInput = true;
        node.isOutput = false;
        node.isHidden = false;

        nodes.push_back(node);
    }

    // Create hidden nodes (if any exist)
    // Note: We'll need to add a getter for hidden nodes in NeuralNetwork
    // For now, we'll calculate based on total nodes minus inputs and outputs
    // int totalNodes = numInputs + numOutputs; // This will be updated when we add hidden node getter
    // int numHidden = totalNodes - numInputs - numOutputs;

    for (int i = 0; i < numHidden; ++i)
    {
        NodeVisual node;
        node.shape.setRadius(nodeRadius);
        node.shape.setFillColor(hiddenNodeColor);
        node.shape.setOutlineColor(sf::Color::Black);
        node.shape.setOutlineThickness(2.0f);

        node.nodeId = numInputs + i; // Start after input nodes
        node.isInput = false;
        node.isOutput = false;
        node.isHidden = true;

        nodes.push_back(node);
    }

    // Create output nodes
    for (int i = 0; i < numOutputs; ++i)
    {
        NodeVisual node;
        node.shape.setRadius(nodeRadius);
        node.shape.setFillColor(outputNodeColor);
        node.shape.setOutlineColor(sf::Color::Black);
        node.shape.setOutlineThickness(2.0f);

        node.nodeId = numInputs + numHidden + i; // Start after input and hidden nodes
        node.isInput = false;
        node.isOutput = true;
        node.isHidden = false;

        nodes.push_back(node);
    }
}

void NetworkRender::createConnectionVisuals(const NeuralNetwork &network)
{
    // We need to add getters to NeuralNetwork to access actual connections
    // For now, we'll create a simple visualization based on the network structure
    // This will be updated when we add connection getters to NeuralNetwork
    
    int numInputs = network.getNumInputs();
    int numHidden = network.getNumHidden();
    int numOutputs = network.getNumOutputs();
    
    // Create connections from inputs to hidden nodes (if hidden nodes exist)
    if (numHidden > 0)
    {
        for (int input = 0; input < numInputs; ++input)
        {
            for (int hidden = 0; hidden < numHidden; ++hidden)
            {
                ConnectionVisual conn;
                conn.shape.setSize(sf::Vector2f(connectionWidth, 0)); // Will be set in layoutConnections
                conn.shape.setFillColor(connectionColor);

                conn.fromNode = input;
                conn.toNode = numInputs + hidden;
                conn.weight = 0.1; // Default weight

                connections.push_back(conn);
            }
        }

        // Create connections from hidden nodes to outputs
        for (int hidden = 0; hidden < numHidden; ++hidden)
        {
            for (int output = 0; output < numOutputs; ++output)
            {
                ConnectionVisual conn;
                conn.shape.setSize(sf::Vector2f(connectionWidth, 0)); // Will be set in layoutConnections
                conn.shape.setFillColor(connectionColor);

                conn.fromNode = numInputs + hidden;
                conn.toNode = numInputs + numHidden + output;
                conn.weight = 0.1; // Default weight

                connections.push_back(conn);
            }
        }
    }
    else
    {
        // Direct connections from inputs to outputs (no hidden layer)
        for (int input = 0; input < numInputs; ++input)
        {
            for (int output = 0; output < numOutputs; ++output)
            {
                ConnectionVisual conn;
                conn.shape.setSize(sf::Vector2f(connectionWidth, 0)); // Will be set in layoutConnections
                conn.shape.setFillColor(connectionColor);

                conn.fromNode = input;
                conn.toNode = numInputs + output;
                conn.weight = 0.1; // Default weight

                connections.push_back(conn);
            }
        }
    }
}

void NetworkRender::layoutNodes()
{
    float currentX = position.x + 30.0f; // Start with some margin
    
    int numInputs = 0, numHidden = 0, numOutputs = 0;
    
    // Count nodes by type
    for (const auto& node : nodes)
    {
        if (node.isInput) numInputs++;
        else if (node.isHidden) numHidden++;
        else if (node.isOutput) numOutputs++;
    }

    // Calculate total height for each layer to center vertically
    float totalInputHeight = (numInputs > 1) ? (numInputs - 1) * nodeSpacing : 0;
    float totalHiddenHeight = (numHidden > 1) ? (numHidden - 1) * nodeSpacing : 0;
    float totalOutputHeight = (numOutputs > 1) ? (numOutputs - 1) * nodeSpacing : 0;

    // Layout input nodes (vertical column, centered)
    float inputY = position.y + (size.y - totalInputHeight) / 2.0f;
    int inputIndex = 0;
    for (auto& node : nodes)
    {
        if (node.isInput)
        {
            node.shape.setPosition(sf::Vector2f(currentX, inputY + inputIndex * nodeSpacing));
            inputIndex++;
        }
    }

    // Layout hidden nodes (vertical column, centered) - if any exist
    if (numHidden > 0)
    {
        currentX += layerSpacing;
        float hiddenY = position.y + (size.y - totalHiddenHeight) / 2.0f;
        int hiddenIndex = 0;
        for (auto& node : nodes)
        {
            if (node.isHidden)
            {
                node.shape.setPosition(sf::Vector2f(currentX, hiddenY + hiddenIndex * nodeSpacing));
                hiddenIndex++;
            }
        }
    }

    // Layout output nodes (vertical column, centered)
    currentX += layerSpacing;
    float outputY = position.y + (size.y - totalOutputHeight) / 2.0f;
    int outputIndex = 0;
    for (auto& node : nodes)
    {
        if (node.isOutput)
        {
            node.shape.setPosition(sf::Vector2f(currentX, outputY + outputIndex * nodeSpacing));
            outputIndex++;
        }
    }
}

void NetworkRender::layoutConnections()
{
    for (auto &conn : connections)
    {
        // Find the positions of the connected nodes
        sf::Vector2f fromPos(0, 0);
        sf::Vector2f toPos(0, 0);

        for (const auto &node : nodes)
        {
            if (node.nodeId == conn.fromNode)
            {
                fromPos = node.shape.getPosition() + sf::Vector2f(nodeRadius, nodeRadius);
            }
            if (node.nodeId == conn.toNode)
            {
                toPos = node.shape.getPosition() + sf::Vector2f(nodeRadius, nodeRadius);
            }
        }

        // Calculate connection properties
        sf::Vector2f direction = toPos - fromPos;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159f;

        // Set connection shape
        conn.shape.setSize(sf::Vector2f(length, connectionWidth));
        conn.shape.setPosition(fromPos);
        conn.shape.setRotation(sf::degrees(angle));
    }
}

sf::Color NetworkRender::getWeightColor(double weight) const
{
    // Color based on weight: red for negative, green for positive, intensity based on magnitude
    float intensity = std::min(1.0f, static_cast<float>(std::abs(weight)));

    if (weight < 0)
    {
        return sf::Color(255, 0, 0, static_cast<unsigned char>(intensity * 255));
    }
    else
    {
        return sf::Color(0, 255, 0, static_cast<unsigned char>(intensity * 255));
    }
}

void NetworkRender::setPosition(float x, float y)
{
    position = sf::Vector2f(x, y);
    layoutNodes();
    layoutConnections();
}

void NetworkRender::setSize(float width, float height)
{
    size = sf::Vector2f(width, height);
}

void NetworkRender::setNodeRadius(float radius)
{
    nodeRadius = radius;
    for (auto &node : nodes)
    {
        node.shape.setRadius(radius);
    }
    layoutNodes();
    layoutConnections();
}

void NetworkRender::setConnectionWidth(float width)
{
    connectionWidth = width;
    for (auto &conn : connections)
    {
        conn.shape.setSize(sf::Vector2f(conn.shape.getSize().x, width));
    }
}

void NetworkRender::setNodeSpacing(float spacing)
{
    nodeSpacing = spacing;
    layoutNodes();
    layoutConnections();
}

void NetworkRender::setLayerSpacing(float spacing)
{
    layerSpacing = spacing;
    layoutNodes();
    layoutConnections();
}

void NetworkRender::setInputNodeColor(const sf::Color &color)
{
    inputNodeColor = color;
    for (auto &node : nodes)
    {
        if (node.isInput)
        {
            node.shape.setFillColor(color);
        }
    }
}

void NetworkRender::setHiddenNodeColor(const sf::Color &color)
{
    hiddenNodeColor = color;
    for (auto &node : nodes)
    {
        if (node.isHidden)
        {
            node.shape.setFillColor(color);
        }
    }
}

void NetworkRender::setOutputNodeColor(const sf::Color &color)
{
    outputNodeColor = color;
    for (auto &node : nodes)
    {
        if (node.isOutput)
        {
            node.shape.setFillColor(color);
        }
    }
}

void NetworkRender::setConnectionColor(const sf::Color &color)
{
    connectionColor = color;
    for (auto &conn : connections)
    {
        conn.shape.setFillColor(color);
    }
}

void NetworkRender::draw(sf::RenderWindow &window) const
{
    // Draw connections first (behind nodes)
    for (const auto &conn : connections)
    {
        window.draw(conn.shape);
    }

    // Draw nodes
    for (const auto &node : nodes)
    {
        window.draw(node.shape);
    }
}