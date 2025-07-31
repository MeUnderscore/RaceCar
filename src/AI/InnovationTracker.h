#pragma once
#include <vector>
#include <unordered_map>
#include <string>

struct Innovation
{
    int innovationNumber;
    int fromNode;
    int toNode;
    bool isNode; // true if this innovation added a node
    int nodeId;  // only used when isNode is true
    
    Innovation(int number, int from, int to, bool node = false, int nodeId = -1)
        : innovationNumber(number), fromNode(from), toNode(to), isNode(node), nodeId(nodeId) {}
};

class InnovationTracker
{
private:
    std::vector<Innovation> innovations;
    int nextInnovationNumber;
    
    // Hash function for connection lookup
    std::string getConnectionKey(int fromNode, int toNode) const;
    
public:
    InnovationTracker();
    
    // Get or create innovation number for a connection
    int getInnovationNumber(int fromNode, int toNode, bool isNode = false, int nodeId = -1);
    
    // Check if innovation already exists
    bool innovationExists(int fromNode, int toNode, bool isNode = false) const;
    
    // Get innovation by number
    const Innovation* getInnovation(int innovationNumber) const;
    
    // Reset for new generation
    void reset();
    
    // Getters
    int getNextInnovationNumber() const { return nextInnovationNumber; }
    const std::vector<Innovation>& getInnovations() const { return innovations; }
}; 