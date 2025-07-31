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
    // Get node information from the network
    // For now, we'll create a simple visualization with default values
    // In the future, we'll need to add getters to NeuralNetwork to access node data

    // Create input nodes (18 inputs: 16 ray sensors + speed + rotation)
    for (int i = 0; i < 18; ++i)
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

    // Create hidden nodes (assuming 4 hidden nodes)
    for (int i = 0; i < 4; ++i)
    {
        NodeVisual node;
        node.shape.setRadius(nodeRadius);
        node.shape.setFillColor(hiddenNodeColor);
        node.shape.setOutlineColor(sf::Color::Black);
        node.shape.setOutlineThickness(2.0f);

        node.nodeId = 18 + i; // Start after input nodes
        node.isInput = false;
        node.isOutput = false;
        node.isHidden = true;

        nodes.push_back(node);
    }

    // Create output nodes (assuming 2 outputs for steering and acceleration)
    for (int i = 0; i < 2; ++i)
    {
        NodeVisual node;
        node.shape.setRadius(nodeRadius);
        node.shape.setFillColor(outputNodeColor);
        node.shape.setOutlineColor(sf::Color::Black);
        node.shape.setOutlineThickness(2.0f);

        node.nodeId = 22 + i; // Start after hidden nodes
        node.isInput = false;
        node.isOutput = true;
        node.isHidden = false;

        nodes.push_back(node);
    }
}

void NetworkRender::createConnectionVisuals(const NeuralNetwork &network)
{
    // Create connections from inputs to hidden nodes
    for (int input = 0; input < 18; ++input)
    {
        for (int hidden = 0; hidden < 4; ++hidden)
        {
            ConnectionVisual conn;
            conn.shape.setSize(sf::Vector2f(connectionWidth, 0)); // Will be set in layoutConnections
            conn.shape.setFillColor(connectionColor);

            conn.fromNode = input;
            conn.toNode = 18 + hidden;
            conn.weight = 0.1; // Default weight

            connections.push_back(conn);
        }
    }

    // Create connections from hidden nodes to outputs
    for (int hidden = 0; hidden < 4; ++hidden)
    {
        for (int output = 0; output < 2; ++output)
        {
            ConnectionVisual conn;
            conn.shape.setSize(sf::Vector2f(connectionWidth, 0)); // Will be set in layoutConnections
            conn.shape.setFillColor(connectionColor);

            conn.fromNode = 18 + hidden;
            conn.toNode = 22 + output;
            conn.weight = 0.1; // Default weight

            connections.push_back(conn);
        }
    }
}

void NetworkRender::layoutNodes()
{
    float currentX = position.x + 30.0f; // Start with some margin

    // Calculate total height for each layer to center vertically
    float totalInputHeight = 17 * nodeSpacing; // 18 nodes = 17 gaps
    float totalHiddenHeight = 3 * nodeSpacing; // 4 nodes = 3 gaps
    float totalOutputHeight = 1 * nodeSpacing; // 2 nodes = 1 gap

    // Layout input nodes (vertical column, centered)
    float inputY = position.y + (size.y - totalInputHeight) / 2.0f;
    for (int i = 0; i < 18; ++i)
    {
        if (i < nodes.size())
        {
            nodes[i].shape.setPosition(sf::Vector2f(currentX, inputY + i * nodeSpacing));
        }
    }

    // Layout hidden nodes (vertical column, centered)
    currentX += layerSpacing;
    float hiddenY = position.y + (size.y - totalHiddenHeight) / 2.0f;
    for (int i = 0; i < 4; ++i)
    {
        if (18 + i < nodes.size())
        {
            nodes[18 + i].shape.setPosition(sf::Vector2f(currentX, hiddenY + i * nodeSpacing));
        }
    }

    // Layout output nodes (vertical column, centered)
    currentX += layerSpacing;
    float outputY = position.y + (size.y - totalOutputHeight) / 2.0f;
    for (int i = 0; i < 2; ++i)
    {
        if (22 + i < nodes.size())
        {
            nodes[22 + i].shape.setPosition(sf::Vector2f(currentX, outputY + i * nodeSpacing));
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