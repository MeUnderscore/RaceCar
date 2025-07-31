#include "Population.h"
#include <iostream>

void testNEAT()
{
    std::cout << "=== NEAT Algorithm Test ===" << std::endl;

    // Create a population with 10 individuals, 16 inputs (ray sensors), 2 outputs (steering, acceleration)
    Population population(10, 16, 2, 4);

    std::cout << "Initial population created with " << population.getPopulationSize() << " individuals" << std::endl;
    std::cout << "Species count: " << population.getSpeciesCount() << std::endl;

    // Simulate a few generations
    for (int gen = 0; gen < 5; ++gen)
    {
        std::cout << "\n--- Generation " << gen + 1 << " ---" << std::endl;

        // Simulate fitness evaluation (in real implementation, this would come from actual car performance)
        auto &controllers = population.getControllers();
        for (auto &controller : controllers)
        {
            // Simulate some fitness value based on random performance
            double simulatedFitness = static_cast<double>(rand()) / RAND_MAX * 1000.0;
            // In real implementation: controller->updateFitness(deltaTime);
        }

        // Evolve the population
        population.evolve();
    }

    std::cout << "\n=== NEAT Test Complete ===" << std::endl;
}