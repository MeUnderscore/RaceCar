#pragma once
#include <vector>
#include <random>
#include <memory>

// Forward declaration
class InnovationTracker;

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
    
    // NEAT-specific members
    static std::shared_ptr<InnovationTracker> innovationTracker;
    int nextNodeId;

public:
    NeuralNetwork();

    // Initialize a simple feedforward network
    void initializeSimple(int numInputs, int numOutputs, int numHidden = 0);

    // Process inputs through the network
    std::vector<double> process(const std::vector<double> &inputs);

    // Genetic operations
    void mutate();
    NeuralNetwork crossover(const NeuralNetwork &other) const;
    
    // NEAT-specific operations
    void mutateAddConnection();
    void mutateAddNode();
    double calculateDistance(const NeuralNetwork &other) const;
    
    // Innovation tracker management
    static void setInnovationTracker(std::shared_ptr<InnovationTracker> tracker);
    static std::shared_ptr<InnovationTracker> getInnovationTracker();

    // Getters
    int getNumInputs() const { return inputNodes.size(); }
    int getNumOutputs() const { return outputNodes.size(); }
    int getNumHidden() const { return hiddenNodes.size(); }
    const std::vector<Connection>& getConnections() const { return connections; }

private:
    void addNode(int nodeId, double bias = 0.0);
    void addConnection(int fromNode, int toNode, double weight, bool enabled = true);
    void mutateWeights();
    void mutateBias();
    
    // NEAT-specific helper methods
    bool connectionExists(int fromNode, int toNode) const;
    std::vector<int> getConnectableNodes(int fromNode) const;
    int getRandomNode() const;
    int getRandomHiddenNode() const;
};