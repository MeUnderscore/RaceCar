#pragma once
#include "Species.h"
#include "InnovationTracker.h"
#include "AIController.h"
#include <vector>
#include <memory>

class Population
{
private:
    std::vector<std::shared_ptr<Species>> species;
    std::vector<std::shared_ptr<AIController>> controllers;
    std::shared_ptr<InnovationTracker> innovationTracker;
    
    int generation;
    int populationSize;
    double compatibilityThreshold;
    double bestFitness;
    int generationsWithoutImprovement;
    
    // NEAT parameters
    double c1, c2, c3; // Distance calculation weights
    double weightMutationRate;
    double weightMutationPower;
    double addConnectionRate;
    double addNodeRate;
    double disableConnectionRate;
    double enableConnectionRate;
    
public:
    Population(int size, int numInputs, int numOutputs, int numHidden = 0);
    
    // Evolution methods
    void evolve();
    void speciate();
    void calculateAdjustedFitness();
    void removeStaleSpecies();
    void reproduce();
    void mutate();
    
    // Population management
    void addController(std::shared_ptr<AIController> controller);
    void clearControllers();
    void resetControllers();
    
    // Getters
    int getGeneration() const { return generation; }
    double getBestFitness() const { return bestFitness; }
    int getSpeciesCount() const { return species.size(); }
    int getPopulationSize() const { return populationSize; }
    const std::vector<std::shared_ptr<AIController>>& getControllers() const { return controllers; }
    std::vector<std::shared_ptr<AIController>>& getControllers() { return controllers; }
    
    // Setters
    void setCompatibilityThreshold(double threshold) { compatibilityThreshold = threshold; }
    void setWeightMutationRate(double rate) { weightMutationRate = rate; }
    void setAddConnectionRate(double rate) { addConnectionRate = rate; }
    void setAddNodeRate(double rate) { addNodeRate = rate; }
    void setBestFitness(double fitness) { bestFitness = fitness; }
    
    // Statistics
    void printStatistics() const;
}; 