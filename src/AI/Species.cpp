#include "Species.h"
#include "AIController.h"
#include <algorithm>
#include <random>
#include <numeric>

Species::Species(std::shared_ptr<NeuralNetwork> firstMember)
    : averageFitness(0.0), bestFitness(0.0), staleness(0), maxStaleness(15)
{
    representative = firstMember;
    addMember(firstMember);
}

void Species::addMember(std::shared_ptr<NeuralNetwork> member)
{
    members.push_back(member);
}

void Species::clearMembers()
{
    members.clear();
    if (representative)
    {
        members.push_back(representative);
    }
}

bool Species::belongsToSpecies(const NeuralNetwork &network, double compatibilityThreshold) const
{
    if (!representative)
        return false;

    double distance = representative->calculateDistance(network);
    return distance <= compatibilityThreshold;
}

void Species::calculateAdjustedFitness(const std::vector<std::shared_ptr<AIController>> &controllers)
{
    if (members.empty())
    {
        averageFitness = 0.0;
        bestFitness = 0.0;
        return;
    }

    // Calculate average fitness using actual controller fitness values
    double totalFitness = 0.0;
    bestFitness = 0.0;
    int validMembers = 0;

    for (const auto &member : members)
    {
        // Find the corresponding controller for this neural network
        double fitness = 0.0;
        bool found = false;

        for (const auto &controller : controllers)
        {
            // Use a more reliable matching method - check if the neural network is the same object
            if (controller->getBrain().getConnections().size() == member->getConnections().size())
            {
                // Check if all connections match (same innovation numbers and weights)
                bool networksMatch = true;
                const auto &controllerConnections = controller->getBrain().getConnections();
                const auto &memberConnections = member->getConnections();

                if (controllerConnections.size() == memberConnections.size())
                {
                    for (size_t i = 0; i < controllerConnections.size(); ++i)
                    {
                        if (controllerConnections[i].innovationNumber != memberConnections[i].innovationNumber ||
                            std::abs(controllerConnections[i].weight - memberConnections[i].weight) > 0.001)
                        {
                            networksMatch = false;
                            break;
                        }
                    }

                    if (networksMatch)
                    {
                        fitness = controller->getFitness();
                        found = true;
                        break;
                    }
                }
            }
        }

        if (found)
        {
            totalFitness += fitness;
            validMembers++;

            if (fitness > bestFitness)
                bestFitness = fitness;
        }
    }

    averageFitness = validMembers > 0 ? totalFitness / validMembers : 0.0;
}

void Species::updateStaleness()
{
    staleness++;
}

std::shared_ptr<NeuralNetwork> Species::selectParent(const std::vector<std::shared_ptr<AIController>> &controllers) const
{
    if (members.empty())
        return nullptr;

    // Tournament selection
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, members.size() - 1);

    const int tournamentSize = 3;
    std::shared_ptr<NeuralNetwork> best = nullptr;
    double bestFitness = -1.0;

    for (int i = 0; i < tournamentSize; ++i)
    {
        int index = dis(gen);

        // Find the corresponding controller for this neural network
        double fitness = 0.0;
        bool found = false;

        for (const auto &controller : controllers)
        {
            // Use the same reliable matching method
            if (controller->getBrain().getConnections().size() == members[index]->getConnections().size())
            {
                bool networksMatch = true;
                const auto &controllerConnections = controller->getBrain().getConnections();
                const auto &memberConnections = members[index]->getConnections();

                if (controllerConnections.size() == memberConnections.size())
                {
                    for (size_t j = 0; j < controllerConnections.size(); ++j)
                    {
                        if (controllerConnections[j].innovationNumber != memberConnections[j].innovationNumber ||
                            std::abs(controllerConnections[j].weight - memberConnections[j].weight) > 0.001)
                        {
                            networksMatch = false;
                            break;
                        }
                    }

                    if (networksMatch)
                    {
                        fitness = controller->getFitness();
                        found = true;
                        break;
                    }
                }
            }
        }

        if (found && fitness > bestFitness)
        {
            bestFitness = fitness;
            best = members[index];
        }
    }

    return best ? best : members[0];
}

std::shared_ptr<NeuralNetwork> Species::reproduce(const std::vector<std::shared_ptr<AIController>> &controllers) const
{
    if (members.empty())
        return nullptr;

    if (members.size() == 1)
    {
        // Clone the single member
        NeuralNetwork offspring = *members[0];
        offspring.mutate();
        return std::make_shared<NeuralNetwork>(offspring);
    }

    // Select two parents
    auto parent1 = selectParent(controllers);
    auto parent2 = selectParent(controllers);

    if (!parent1 || !parent2)
        return nullptr;

    // Crossover and mutate
    NeuralNetwork offspring = parent1->crossover(*parent2);
    offspring.mutate();

    return std::make_shared<NeuralNetwork>(offspring);
}

void Species::cullToBest()
{
    if (members.size() <= 1)
        return;

    // Keep only the best member as representative (standard NEAT approach)
    auto best = members[0];
    members.clear();
    members.push_back(best);
    representative = best;
}