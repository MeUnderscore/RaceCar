#pragma once
#include "NeuralNetwork.h"
#include <vector>
#include <memory>

class Species
{
private:
    std::vector<std::shared_ptr<NeuralNetwork>> members;
    double averageFitness;
    double bestFitness;
    int staleness; // generations without improvement
    int maxStaleness;
    
    // Representative member (first member added)
    std::shared_ptr<NeuralNetwork> representative;
    
public:
    Species(std::shared_ptr<NeuralNetwork> firstMember);
    
    // Add a member to this species
    void addMember(std::shared_ptr<NeuralNetwork> member);
    
    // Check if a network belongs to this species
    bool belongsToSpecies(const NeuralNetwork& network, double compatibilityThreshold) const;
    
    // Calculate adjusted fitness for all members
    void calculateAdjustedFitness();
    
    // Remove stale species (no improvement for too long)
    bool isStale() const { return staleness >= maxStaleness; }
    
    // Update staleness counter
    void updateStaleness();
    
    // Select a parent for reproduction
    std::shared_ptr<NeuralNetwork> selectParent() const;
    
    // Reproduce (create offspring)
    std::shared_ptr<NeuralNetwork> reproduce() const;
    
    // Clear all members except the best one
    void cullToBest();
    
    // Getters
    int getSize() const { return members.size(); }
    double getAverageFitness() const { return averageFitness; }
    double getBestFitness() const { return bestFitness; }
    int getStaleness() const { return staleness; }
    const std::vector<std::shared_ptr<NeuralNetwork>>& getMembers() const { return members; }
    
    // Setters
    void setMaxStaleness(int max) { maxStaleness = max; }
}; 