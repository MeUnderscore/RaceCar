#include "NeuralNetwork.h"
#include <algorithm>
#include <cmath>
#include <random>

NeuralNetwork::NeuralNetwork()
{
}

void NeuralNetwork::initializeSimple(int numInputs, int numOutputs, int numHidden)
{
    nodes.clear();
    connections.clear();
    inputNodes.clear();
    outputNodes.clear();
    hiddenNodes.clear();

    // Create input nodes
    for (int i = 0; i < numInputs; ++i)
    {
        int nodeId = i;
        addNode(nodeId);
        inputNodes.push_back(nodeId);
    }

    // Create hidden nodes
    for (int i = 0; i < numHidden; ++i)
    {
        int nodeId = numInputs + i;
        addNode(nodeId);
        hiddenNodes.push_back(nodeId);
    }

    // Create output nodes
    for (int i = 0; i < numOutputs; ++i)
    {
        int nodeId = numInputs + numHidden + i;
        addNode(nodeId);
        outputNodes.push_back(nodeId);
    }

    // Create connections
    if (numHidden == 0)
    {
        // Direct connections from inputs to outputs
        for (int input : inputNodes)
        {
            for (int output : outputNodes)
            {
                addConnection(input, output);
            }
        }
    }
    else
    {
        // Connections from inputs to hidden
        for (int input : inputNodes)
        {
            for (int hidden : hiddenNodes)
            {
                addConnection(input, hidden);
            }
        }

        // Connections from hidden to outputs
        for (int hidden : hiddenNodes)
        {
            for (int output : outputNodes)
            {
                addConnection(hidden, output);
            }
        }
    }
}

std::vector<double> NeuralNetwork::process(const std::vector<double> &inputs)
{
    // Reset node values
    for (auto &node : nodes)
    {
        node.value = 0.0;
    }

    // Set input values
    for (size_t i = 0; i < inputs.size() && i < inputNodes.size(); ++i)
    {
        nodes[inputNodes[i]].value = inputs[i];
    }

    // Process hidden nodes
    for (int hiddenId : hiddenNodes)
    {
        double sum = nodes[hiddenId].bias;
        for (const auto &conn : connections)
        {
            if (conn.toNode == hiddenId && conn.enabled)
            {
                sum += nodes[conn.fromNode].value * conn.weight;
            }
        }
        nodes[hiddenId].value = std::tanh(sum); // Activation function
    }

    // Process output nodes
    std::vector<double> outputs;
    for (int outputId : outputNodes)
    {
        double sum = nodes[outputId].bias;
        for (const auto &conn : connections)
        {
            if (conn.toNode == outputId && conn.enabled)
            {
                sum += nodes[conn.fromNode].value * conn.weight;
            }
        }
        outputs.push_back(std::tanh(sum)); // Activation function
    }

    return outputs;
}

void NeuralNetwork::mutate()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);

    double mutationChance = dis(gen);

    if (mutationChance < 0.8)
        mutateWeights();
    if (mutationChance < 0.1)
        mutateBias();
    if (mutationChance < 0.05)
        mutateAddConnection();
    if (mutationChance < 0.03)
        mutateAddNode();
}

void NeuralNetwork::addNode(int nodeId, double bias)
{
    Node node;
    node.id = nodeId;
    node.value = 0.0;
    node.bias = bias;
    nodes.push_back(node);
}

void NeuralNetwork::addConnection(int fromNode, int toNode, double weight, bool enabled)
{
    static int innovationCounter = 0;

    Connection conn;
    conn.fromNode = fromNode;
    conn.toNode = toNode;
    conn.weight = weight;
    conn.enabled = enabled;
    conn.innovationNumber = innovationCounter++;

    connections.push_back(conn);
}

void NeuralNetwork::mutateWeights()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(-0.1, 0.1);

    for (auto &conn : connections)
    {
        if (dis(gen) < 0.1) // 10% chance to mutate each weight
        {
            conn.weight += dis(gen);
        }
    }
}

void NeuralNetwork::mutateBias()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(-0.1, 0.1);

    for (auto &node : nodes)
    {
        if (dis(gen) < 0.1) // 10% chance to mutate each bias
        {
            node.bias += dis(gen);
        }
    }
}

void NeuralNetwork::mutateAddConnection()
{
    // Simple implementation - add a random connection
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> weightDis(-1.0, 1.0);

    if (nodes.size() < 2)
        return;

    std::uniform_int_distribution<> nodeDis(0, nodes.size() - 1);
    int fromNode = nodeDis(gen);
    int toNode = nodeDis(gen);

    // Don't connect a node to itself
    if (fromNode != toNode)
    {
        addConnection(fromNode, toNode, weightDis(gen));
    }
}

void NeuralNetwork::mutateAddNode()
{
    // Simple implementation - add a hidden node
    if (connections.empty())
        return;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> connDis(0, connections.size() - 1);

    int connIndex = connDis(gen);
    Connection &conn = connections[connIndex];

    // Disable the original connection
    conn.enabled = false;

    // Add new hidden node
    int newNodeId = nodes.size();
    addNode(newNodeId);
    hiddenNodes.push_back(newNodeId);

    // Add connections to and from the new node
    addConnection(conn.fromNode, newNodeId, 1.0);
    addConnection(newNodeId, conn.toNode, conn.weight);
}

NeuralNetwork NeuralNetwork::crossover(const NeuralNetwork &other) const
{
    NeuralNetwork child;
    // Simple crossover - inherit structure from this network
    child = *this;

    // Crossover weights and biases
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);

    for (size_t i = 0; i < child.connections.size() && i < other.connections.size(); ++i)
    {
        if (dis(gen) < 0.5)
        {
            child.connections[i].weight = other.connections[i].weight;
        }
    }

    for (size_t i = 0; i < child.nodes.size() && i < other.nodes.size(); ++i)
    {
        if (dis(gen) < 0.5)
        {
            child.nodes[i].bias = other.nodes[i].bias;
        }
    }

    return child;
}