#include "Population.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <numeric> // Required for std::accumulate

Population::Population(int size, int numInputs, int numOutputs, int numHidden)
    : generation(0), populationSize(size), compatibilityThreshold(3.0), // Increased from 1.0 to 3.0
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

    // Adaptive speciation: adjust compatibility threshold based on target species count
    const int targetSpeciesCount = std::max(1, populationSize / 5); // Target ~5 individuals per species
    const double minThreshold = 1.0;
    const double maxThreshold = 10.0;

    // Try speciation with current threshold
    speciateWithThreshold(compatibilityThreshold);

    // If too many species, increase threshold
    if (species.size() > targetSpeciesCount * 1.5 && compatibilityThreshold < maxThreshold)
    {
        compatibilityThreshold = std::min(maxThreshold, compatibilityThreshold * 1.2);
        species.clear();
        speciateWithThreshold(compatibilityThreshold);
    }
    // If too few species, decrease threshold
    else if (species.size() < targetSpeciesCount * 0.5 && compatibilityThreshold > minThreshold)
    {
        compatibilityThreshold = std::max(minThreshold, compatibilityThreshold * 0.8);
        species.clear();
        speciateWithThreshold(compatibilityThreshold);
    }

    // Debug output for speciation
    std::cout << "Speciation: " << species.size() << " species created (threshold: " << compatibilityThreshold << ")" << std::endl;
    for (size_t i = 0; i < species.size(); ++i)
    {
        std::cout << "  Species " << i << ": " << species[i]->getSize() << " members" << std::endl;
    }

    // Merge very small species (less than 2 members) with larger ones
    mergeSmallSpecies();
}

void Population::speciateWithThreshold(double threshold)
{
    for (auto &controller : controllers)
    {
        bool placed = false;

        // Try to place in existing species
        for (auto &species : species)
        {
            double distance = species->getRepresentative()->calculateDistance(controller->getBrain());
            if (distance <= threshold)
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

void Population::mergeSmallSpecies()
{
    const int minSpeciesSize = 2;

    // Find small species
    std::vector<size_t> smallSpeciesIndices;
    for (size_t i = 0; i < species.size(); ++i)
    {
        if (species[i]->getSize() < minSpeciesSize)
        {
            smallSpeciesIndices.push_back(i);
        }
    }

    // Merge small species into larger ones
    for (size_t smallIdx : smallSpeciesIndices)
    {
        if (smallIdx >= species.size())
            continue; // Skip if already removed

        auto &smallSpecies = species[smallIdx];
        bool merged = false;

        // Try to merge with the closest larger species
        for (size_t i = 0; i < species.size(); ++i)
        {
            if (i == smallIdx || species[i]->getSize() < minSpeciesSize)
                continue;

            // Check if members can be merged (using a more lenient threshold)
            bool canMerge = true;
            for (const auto &member : smallSpecies->getMembers())
            {
                double distance = species[i]->getRepresentative()->calculateDistance(*member);
                if (distance > compatibilityThreshold * 2.0) // More lenient for merging
                {
                    canMerge = false;
                    break;
                }
            }

            if (canMerge)
            {
                // Merge members
                for (const auto &member : smallSpecies->getMembers())
                {
                    species[i]->addMember(member);
                }
                merged = true;
                break;
            }
        }

        if (!merged)
        {
            // If can't merge, try to find any species with higher threshold
            for (size_t i = 0; i < species.size(); ++i)
            {
                if (i == smallIdx)
                    continue;

                bool canMerge = true;
                for (const auto &member : smallSpecies->getMembers())
                {
                    double distance = species[i]->getRepresentative()->calculateDistance(*member);
                    if (distance > compatibilityThreshold * 3.0) // Even more lenient
                    {
                        canMerge = false;
                        break;
                    }
                }

                if (canMerge)
                {
                    for (const auto &member : smallSpecies->getMembers())
                    {
                        species[i]->addMember(member);
                    }
                    merged = true;
                    break;
                }
            }
        }
    }

    // Remove empty species
    species.erase(
        std::remove_if(species.begin(), species.end(),
                       [](const std::shared_ptr<Species> &s)
                       { return s->getSize() == 0; }),
        species.end());
}

void Population::calculateAdjustedFitness()
{
    // Clear all species members first
    for (auto &species : species)
    {
        species->clearMembers();
    }

    // Re-speciate with current controllers and their fitness values
    speciate();

    // Now calculate adjusted fitness for each species using actual controller fitness values
    for (auto &species : species)
    {
        species->calculateAdjustedFitness(controllers);
    }

    // Update the population's best fitness
    double currentBestFitness = 0.0;
    for (const auto &controller : controllers)
    {
        if (controller->getFitness() > currentBestFitness)
        {
            currentBestFitness = controller->getFitness();
        }
    }

    // Only update best fitness if we've improved
    if (currentBestFitness > bestFitness)
    {
        bestFitness = currentBestFitness;
        generationsWithoutImprovement = 0;
    }
    else
    {
        generationsWithoutImprovement++;
    }
}

void Population::removeStaleSpecies()
{
    species.erase(
        std::remove_if(species.begin(), species.end(),
                       [](const std::shared_ptr<Species> &s)
                       { return s->isStale(); }),
        species.end());
}

void Population::reproduce()
{
    std::vector<std::shared_ptr<AIController>> newControllers;

    // Find the best controller to preserve
    std::shared_ptr<AIController> bestController = nullptr;
    double bestFitnessValue = -1.0;

    for (const auto &controller : controllers)
    {
        if (controller->getFitness() > bestFitnessValue)
        {
            bestFitnessValue = controller->getFitness();
            bestController = controller;
        }
    }

    // Always preserve the best individual (elitism)
    if (bestController)
    {
        auto eliteController = std::make_shared<AIController>();
        eliteController->getBrain() = bestController->getBrain();
        newControllers.push_back(eliteController);
    }

    // Calculate total adjusted fitness
    double totalAdjustedFitness = 0.0;
    for (const auto &species : species)
    {
        totalAdjustedFitness += species->getAverageFitness() * species->getSize();
    }

    // Ensure minimum offspring per species to prevent extinction
    const int minOffspringPerSpecies = 2;
    int totalMinOffspring = species.size() * minOffspringPerSpecies;

    // If we need more than remaining population size, reduce minimum
    int remainingSlots = populationSize - newControllers.size();
    if (totalMinOffspring > remainingSlots)
    {
        totalMinOffspring = remainingSlots;
    }

    // Determine how many offspring each species should produce
    for (const auto &species : species)
    {
        if (species->getSize() == 0)
            continue;

        double speciesFitness = species->getAverageFitness() * species->getSize();
        int offspringCount = static_cast<int>(speciesFitness / totalAdjustedFitness * remainingSlots);

        // Ensure minimum offspring per species
        offspringCount = std::max(minOffspringPerSpecies, offspringCount);

        // Cull species to best member (but keep some diversity)
        species->cullToBest();

        // Create offspring
        for (int i = 0; i < offspringCount && newControllers.size() < populationSize; ++i)
        {
            auto offspringBrain = species->reproduce(controllers);
            if (offspringBrain)
            {
                auto controller = std::make_shared<AIController>();
                controller->getBrain() = *offspringBrain;
                newControllers.push_back(controller);
            }
        }
    }

    // If we don't have enough offspring, create more from the best species
    while (newControllers.size() < populationSize)
    {
        // Find the species with the best average fitness
        std::shared_ptr<Species> bestSpecies = nullptr;
        double bestFitness = -1.0;

        for (const auto &species : species)
        {
            if (species->getSize() > 0 && species->getAverageFitness() > bestFitness)
            {
                bestFitness = species->getAverageFitness();
                bestSpecies = species;
            }
        }

        if (bestSpecies)
        {
            auto offspringBrain = bestSpecies->reproduce(controllers);
            if (offspringBrain)
            {
                auto controller = std::make_shared<AIController>();
                controller->getBrain() = *offspringBrain;
                newControllers.push_back(controller);
            }
        }
        else
        {
            // Fallback: select random species
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, species.size() - 1);

            int speciesIndex = dis(gen);
            if (speciesIndex < species.size() && species[speciesIndex]->getSize() > 0)
            {
                auto offspringBrain = species[speciesIndex]->reproduce(controllers);
                if (offspringBrain)
                {
                    auto controller = std::make_shared<AIController>();
                    controller->getBrain() = *offspringBrain;
                    newControllers.push_back(controller);
                }
            }
        }
    }

    // Replace old population
    controllers = newControllers;

    // Verify that we haven't lost the best individual
    if (bestController && bestFitnessValue > 0)
    {
        bool bestPreserved = false;
        for (const auto &controller : controllers)
        {
            if (controller->getBrain().getConnections().size() == bestController->getBrain().getConnections().size())
            {
                bool networksMatch = true;
                const auto &newConnections = controller->getBrain().getConnections();
                const auto &bestConnections = bestController->getBrain().getConnections();

                if (newConnections.size() == bestConnections.size())
                {
                    for (size_t i = 0; i < newConnections.size(); ++i)
                    {
                        if (newConnections[i].innovationNumber != bestConnections[i].innovationNumber ||
                            std::abs(newConnections[i].weight - bestConnections[i].weight) > 0.001)
                        {
                            networksMatch = false;
                            break;
                        }
                    }

                    if (networksMatch)
                    {
                        bestPreserved = true;
                        break;
                    }
                }
            }
        }

        if (!bestPreserved)
        {
            std::cout << "WARNING: Best individual was lost! Restoring..." << std::endl;
            // Replace the worst individual with the best one
            auto worstController = controllers[0];
            double worstFitness = worstController->getFitness();

            for (auto &controller : controllers)
            {
                if (controller->getFitness() < worstFitness)
                {
                    worstFitness = controller->getFitness();
                    worstController = controller;
                }
            }

            worstController->getBrain() = bestController->getBrain();
        }
    }
}

void Population::mutate()
{
    for (auto &controller : controllers)
    {
        controller->getBrain().mutate();
    }
}

std::shared_ptr<AIController> Population::getBestController() const
{
    if (controllers.empty())
        return nullptr;

    std::shared_ptr<AIController> bestController = controllers[0];
    double bestFitnessValue = bestController->getFitness();

    for (const auto &controller : controllers)
    {
        if (controller->getFitness() > bestFitnessValue)
        {
            bestFitnessValue = controller->getFitness();
            bestController = controller;
        }
    }

    return bestController;
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
    for (auto &controller : controllers)
    {
        controller->resetFitness();
    }
}

void Population::printStatistics() const
{
    std::cout << "Generation " << generation << ":" << std::endl;
    std::cout << "  Species count: " << species.size() << " (threshold: " << compatibilityThreshold << ")" << std::endl;
    std::cout << "  Population size: " << controllers.size() << std::endl;
    std::cout << "  Best fitness: " << bestFitness << std::endl;
    std::cout << "  Generations without improvement: " << generationsWithoutImprovement << std::endl;

    // Calculate species size distribution
    std::vector<int> sizeDistribution;
    for (const auto &species : species)
    {
        sizeDistribution.push_back(species->getSize());
    }

    if (!sizeDistribution.empty())
    {
        std::sort(sizeDistribution.begin(), sizeDistribution.end());
        std::cout << "  Species size distribution: ";
        std::cout << "min=" << sizeDistribution.front()
                  << ", max=" << sizeDistribution.back()
                  << ", avg=" << (controllers.size() / species.size()) << std::endl;
    }

    // Show fitness distribution
    std::vector<double> fitnessValues;
    for (const auto &controller : controllers)
    {
        fitnessValues.push_back(controller->getFitness());
    }

    if (!fitnessValues.empty())
    {
        std::sort(fitnessValues.begin(), fitnessValues.end(), std::greater<double>());
        std::cout << "  Fitness distribution: ";
        std::cout << "best=" << fitnessValues.front()
                  << ", worst=" << fitnessValues.back()
                  << ", avg=" << (std::accumulate(fitnessValues.begin(), fitnessValues.end(), 0.0) / fitnessValues.size()) << std::endl;
    }

    for (size_t i = 0; i < species.size(); ++i)
    {
        std::cout << "  Species " << i << ": " << species[i]->getSize()
                  << " members, avg fitness: " << species[i]->getAverageFitness() << std::endl;
    }
    std::cout << std::endl;
}