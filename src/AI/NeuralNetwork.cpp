#include "NeuralNetwork.h"
#include "InnovationTracker.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <iostream> // Added for debug output

// Static member initialization
std::shared_ptr<InnovationTracker> NeuralNetwork::innovationTracker = nullptr;

NeuralNetwork::NeuralNetwork() : nextNodeId(0)
{
}

void NeuralNetwork::setInnovationTracker(std::shared_ptr<InnovationTracker> tracker)
{
    innovationTracker = tracker;
}

std::shared_ptr<InnovationTracker> NeuralNetwork::getInnovationTracker()
{
    return innovationTracker;
}

void NeuralNetwork::initializeSimple(int numInputs, int numOutputs, int numHidden)
{
    nodes.clear();
    connections.clear();
    inputNodes.clear();
    outputNodes.clear();
    hiddenNodes.clear();

    // Random number generator for weight initialization
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> weightDis(-1.0, 1.0);
    static std::uniform_real_distribution<> biasDis(-0.5, 0.5);
    static std::uniform_real_distribution<> probDis(0.0, 1.0);

    // Create input nodes
    for (int i = 0; i < numInputs; ++i)
    {
        int nodeId = i;
        addNode(nodeId, biasDis(gen)); // Random bias
        inputNodes.push_back(nodeId);
    }

    // Randomly add 0-3 hidden nodes for structural diversity
    int randomHiddenNodes = static_cast<int>(probDis(gen) * 4); // 0, 1, 2, or 3 hidden nodes

    // Create hidden nodes (if any)
    for (int i = 0; i < randomHiddenNodes; ++i)
    {
        int nodeId = numInputs + i;
        addNode(nodeId, biasDis(gen)); // Random bias
        hiddenNodes.push_back(nodeId);
    }

    // Create output nodes
    for (int i = 0; i < numOutputs; ++i)
    {
        int nodeId = numInputs + randomHiddenNodes + i;
        addNode(nodeId, biasDis(gen)); // Random bias
        outputNodes.push_back(nodeId);
    }

    // Create connections with structural randomness
    if (randomHiddenNodes == 0)
    {
        // Direct connections from inputs to outputs (basic structure)
        for (int input : inputNodes)
        {
            for (int output : outputNodes)
            {
                // 80% chance to add each connection
                if (probDis(gen) < 0.8)
                {
                    addConnection(input, output, weightDis(gen)); // Random weight
                }
            }
        }
    }
    else
    {
        // Connections from inputs to hidden nodes
        for (int input : inputNodes)
        {
            for (int hidden : hiddenNodes)
            {
                // 70% chance to add each connection
                if (probDis(gen) < 0.7)
                {
                    addConnection(input, hidden, weightDis(gen)); // Random weight
                }
            }
        }

        // Connections from hidden nodes to outputs
        for (int hidden : hiddenNodes)
        {
            for (int output : outputNodes)
            {
                // 80% chance to add each connection
                if (probDis(gen) < 0.8)
                {
                    addConnection(hidden, output, weightDis(gen)); // Random weight
                }
            }
        }

        // Some direct connections from inputs to outputs (skip hidden layer)
        for (int input : inputNodes)
        {
            for (int output : outputNodes)
            {
                // 30% chance to add direct connection
                if (probDis(gen) < 0.3)
                {
                    addConnection(input, output, weightDis(gen)); // Random weight
                }
            }
        }
    }

    // Set next node ID for future additions
    nextNodeId = nodes.size();
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
    Connection conn;
    conn.fromNode = fromNode;
    conn.toNode = toNode;
    conn.weight = weight;
    conn.enabled = enabled;

    // Use innovation tracker if available
    if (innovationTracker)
    {
        conn.innovationNumber = innovationTracker->getInnovationNumber(fromNode, toNode, false);
    }
    else
    {
        static int innovationCounter = 0;
        conn.innovationNumber = innovationCounter++;
    }

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

bool NeuralNetwork::connectionExists(int fromNode, int toNode) const
{
    for (const auto &conn : connections)
    {
        if (conn.fromNode == fromNode && conn.toNode == toNode)
        {
            return true;
        }
    }
    return false;
}

std::vector<int> NeuralNetwork::getConnectableNodes(int fromNode) const
{
    std::vector<int> connectable;

    // Can connect to any node that's not an input node (to avoid cycles)
    for (const auto &node : nodes)
    {
        if (node.id != fromNode &&
            std::find(inputNodes.begin(), inputNodes.end(), node.id) == inputNodes.end())
        {
            connectable.push_back(node.id);
        }
    }

    return connectable;
}

int NeuralNetwork::getRandomNode() const
{
    if (nodes.empty())
        return -1;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, nodes.size() - 1);

    return nodes[dis(gen)].id;
}

int NeuralNetwork::getRandomHiddenNode() const
{
    if (hiddenNodes.empty())
        return -1;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, hiddenNodes.size() - 1);

    return hiddenNodes[dis(gen)];
}

void NeuralNetwork::mutateAddConnection()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> weightDis(-1.0, 1.0);

    if (nodes.size() < 2)
        return;

    // Try to find a valid connection that doesn't exist
    int attempts = 0;
    const int maxAttempts = 20;

    while (attempts < maxAttempts)
    {
        int fromNode = getRandomNode();
        std::vector<int> connectable = getConnectableNodes(fromNode);

        if (!connectable.empty())
        {
            std::uniform_int_distribution<> toDis(0, connectable.size() - 1);
            int toNode = connectable[toDis(gen)];

            if (!connectionExists(fromNode, toNode))
            {
                addConnection(fromNode, toNode, weightDis(gen));
                return;
            }
        }
        attempts++;
    }
}

void NeuralNetwork::mutateAddNode()
{
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
    int newNodeId = nextNodeId++;
    addNode(newNodeId);
    hiddenNodes.push_back(newNodeId);

    // Add connections to and from the new node with innovation tracking
    if (innovationTracker)
    {
        // Get innovation numbers for the new connections
        int inno1 = innovationTracker->getInnovationNumber(conn.fromNode, newNodeId, false);
        int inno2 = innovationTracker->getInnovationNumber(newNodeId, conn.toNode, false);

        // Add the connections with proper innovation numbers
        Connection newConn1;
        newConn1.fromNode = conn.fromNode;
        newConn1.toNode = newNodeId;
        newConn1.weight = 1.0;
        newConn1.enabled = true;
        newConn1.innovationNumber = inno1;
        connections.push_back(newConn1);

        Connection newConn2;
        newConn2.fromNode = newNodeId;
        newConn2.toNode = conn.toNode;
        newConn2.weight = conn.weight;
        newConn2.enabled = true;
        newConn2.innovationNumber = inno2;
        connections.push_back(newConn2);
    }
    else
    {
        // Fallback to simple connection addition
        addConnection(conn.fromNode, newNodeId, 1.0);
        addConnection(newNodeId, conn.toNode, conn.weight);
    }
}

double NeuralNetwork::calculateDistance(const NeuralNetwork &other) const
{
    // NEAT distance calculation parameters
    const double c1 = 1.0; // Weight for excess connections
    const double c2 = 1.0; // Weight for disjoint connections
    const double c3 = 0.4; // Weight for connection weights
    const double N = 1.0;  // Normalization factor (max number of connections)

    int excess = 0;
    int disjoint = 0;
    double weightDiff = 0.0;
    int matchingConnections = 0;

    // Find the highest innovation number in both networks
    int maxInnovation1 = -1;
    int maxInnovation2 = -1;

    for (const auto &conn : connections)
    {
        if (conn.innovationNumber > maxInnovation1)
            maxInnovation1 = conn.innovationNumber;
    }

    for (const auto &conn : other.connections)
    {
        if (conn.innovationNumber > maxInnovation2)
            maxInnovation2 = conn.innovationNumber;
    }

    // Count excess and disjoint connections
    for (const auto &conn1 : connections)
    {
        bool found = false;
        for (const auto &conn2 : other.connections)
        {
            if (conn1.innovationNumber == conn2.innovationNumber)
            {
                found = true;
                matchingConnections++;
                weightDiff += std::abs(conn1.weight - conn2.weight);
                break;
            }
        }

        if (!found)
        {
            if (conn1.innovationNumber > maxInnovation2)
                excess++;
            else
                disjoint++;
        }
    }

    // Count disjoint connections in the other network
    for (const auto &conn2 : other.connections)
    {
        bool found = false;
        for (const auto &conn1 : connections)
        {
            if (conn2.innovationNumber == conn1.innovationNumber)
            {
                found = true;
                break;
            }
        }

        if (!found && conn2.innovationNumber <= maxInnovation1)
            disjoint++;
    }

    // Calculate normalized distance
    double distance = (c1 * excess + c2 * disjoint) / N;

    if (matchingConnections > 0)
        distance += c3 * (weightDiff / matchingConnections);

    return distance;
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