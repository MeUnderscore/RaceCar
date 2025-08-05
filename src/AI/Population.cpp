#include "Population.h"
#include <iostream>
#include <algorithm>
#include <random>

Population::Population(int size, int numInputs, int numOutputs, int numHidden)
    : generation(0), populationSize(size), compatibilityThreshold(3.0), 
      bestFitness(0.0), generationsWithoutImprovement(0),
      c1(1.0), c2(1.0), c3(0.4),
      weightMutationRate(0.8), weightMutationPower(0.1),
      addConnectionRate(0.05), addNodeRate(0.03),
      disableConnectionRate(0.001), enableConnectionRate(0.001)
{
    // Create innovation tracker
    innovationTracker = std::make_shared<InnovationTracker>();
    NeuralNetwork::setInnovationTracker(innovationTracker);
    
    // Create initial population
    for (int i = 0; i < populationSize; ++i)
    {
        auto controller = std::make_shared<AIController>();
        controller->initialize(numInputs, numOutputs, numHidden);
        controllers.push_back(controller);
    }
    
    // Initial speciation
    speciate();
}

void Population::evolve()
{
    // Calculate fitness for all controllers
    calculateAdjustedFitness();
    
    // Remove stale species
    removeStaleSpecies();
    
    // Reproduce to create new population
    reproduce();
    
    // Mutate the new population
    mutate();
    
    // Reset innovation tracker for new generation
    innovationTracker->reset();
    
    generation++;
    
    // Print statistics
    printStatistics();
}

void Population::speciate()
{
    species.clear();
    
    for (auto& controller : controllers)
    {
        bool placed = false;
        
        // Try to place in existing species
        for (auto& species : species)
        {
            if (species->belongsToSpecies(controller->getBrain(), compatibilityThreshold))
            {
                species->addMember(std::make_shared<NeuralNetwork>(controller->getBrain()));
                placed = true;
                break;
            }
        }
        
        // Create new species if not placed
        if (!placed)
        {
            auto newSpecies = std::make_shared<Species>(std::make_shared<NeuralNetwork>(controller->getBrain()));
            species.push_back(newSpecies);
        }
    }
}

void Population::calculateAdjustedFitness()
{
    // First, update species with actual fitness values from controllers
    for (auto& species : species)
    {
        // Clear the species members and rebuild with current controllers
        species->clearMembers();
    }
    
    // Re-speciate with current controllers and their fitness values
    speciate();
    
    // Now calculate adjusted fitness for each species
    for (auto& species : species)
    {
        species->calculateAdjustedFitness();
    }
}

void Population::removeStaleSpecies()
{
    species.erase(
        std::remove_if(species.begin(), species.end(),
            [](const std::shared_ptr<Species>& s) { return s->isStale(); }),
        species.end()
    );
}

void Population::reproduce()
{
    std::vector<std::shared_ptr<AIController>> newControllers;
    
    // Calculate total adjusted fitness
    double totalAdjustedFitness = 0.0;
    for (const auto& species : species)
    {
        totalAdjustedFitness += species->getAverageFitness() * species->getSize();
    }
    
    // Determine how many offspring each species should produce
    for (const auto& species : species)
    {
        if (species->getSize() == 0) continue;
        
        double speciesFitness = species->getAverageFitness() * species->getSize();
        int offspringCount = static_cast<int>(speciesFitness / totalAdjustedFitness * populationSize);
        
        // Ensure at least one offspring per species
        offspringCount = std::max(1, offspringCount);
        
        // Cull species to best member
        species->cullToBest();
        
        // Create offspring
        for (int i = 0; i < offspringCount && newControllers.size() < populationSize; ++i)
        {
            auto offspringBrain = species->reproduce();
            if (offspringBrain)
            {
                auto controller = std::make_shared<AIController>();
                controller->getBrain() = *offspringBrain;
                newControllers.push_back(controller);
            }
        }
    }
    
    // If we don't have enough offspring, create more
    while (newControllers.size() < populationSize)
    {
        // Select random species
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, species.size() - 1);
        
        int speciesIndex = dis(gen);
        if (speciesIndex < species.size())
        {
            auto offspringBrain = species[speciesIndex]->reproduce();
            if (offspringBrain)
            {
                auto controller = std::make_shared<AIController>();
                controller->getBrain() = *offspringBrain;
                newControllers.push_back(controller);
            }
        }
    }
    
    // Replace old population
    controllers = newControllers;
}

void Population::mutate()
{
    for (auto& controller : controllers)
    {
        controller->getBrain().mutate();
    }
}

void Population::addController(std::shared_ptr<AIController> controller)
{
    controllers.push_back(controller);
}

void Population::clearControllers()
{
    controllers.clear();
}

void Population::resetControllers()
{
    for (auto& controller : controllers)
    {
        controller->resetFitness();
    }
}

void Population::printStatistics() const
{
    std::cout << "Generation " << generation << ":" << std::endl;
    std::cout << "  Species count: " << species.size() << std::endl;
    std::cout << "  Population size: " << controllers.size() << std::endl;
    std::cout << "  Best fitness: " << bestFitness << std::endl;
    std::cout << "  Generations without improvement: " << generationsWithoutImprovement << std::endl;
    
    for (size_t i = 0; i < species.size(); ++i)
    {
        std::cout << "  Species " << i << ": " << species[i]->getSize() 
                  << " members, avg fitness: " << species[i]->getAverageFitness() << std::endl;
    }
    std::cout << std::endl;
} 