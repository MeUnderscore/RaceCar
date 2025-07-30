#pragma once
#include <vector>
#include <random>

class NeuralNetwork
{
private:
    struct Node
    {
        int id;
        double value;
        double bias;
        std::vector<int> incomingConnections;
    };

    struct Connection
    {
        int fromNode;
        int toNode;
        double weight;
        bool enabled;
        int innovationNumber;
    };

    std::vector<Node> nodes;
    std::vector<Connection> connections;
    std::vector<int> inputNodes;
    std::vector<int> outputNodes;
    std::vector<int> hiddenNodes;

public:
    NeuralNetwork();

    // Initialize a simple feedforward network
    void initializeSimple(int numInputs, int numOutputs, int numHidden = 0);

    // Process inputs through the network
    std::vector<double> process(const std::vector<double> &inputs);

    // Genetic operations
    void mutate();
    NeuralNetwork crossover(const NeuralNetwork &other) const;

    // Getters
    int getNumInputs() const { return inputNodes.size(); }
    int getNumOutputs() const { return outputNodes.size(); }

private:
    void addNode(int nodeId, double bias = 0.0);
    void addConnection(int fromNode, int toNode, double weight, bool enabled = true);
    void mutateWeights();
    void mutateBias();
    void mutateAddConnection();
    void mutateAddNode();
};