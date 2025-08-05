#include "AIController.h"
#include "../Car/car.h"
#include <algorithm>

AIController::AIController()
    : fitness(0.0), isAlive(true), checkpointsHit(0), timeAlive(0.0f)
{
}

void AIController::initialize(int numInputs, int numOutputs, int numHidden)
{
    brain.initializeSimple(numInputs, numOutputs, numHidden);
    resetFitness();
}

void AIController::controlCar(Car &car, const std::vector<float> &rayDistances)
{
    if (!isAlive)
        return;

    // Prepare inputs for the neural network
    std::vector<double> inputs;

    // Add ray distances as inputs (normalized to 0-1 range)
    for (float distance : rayDistances)
    {
        // Normalize distance (0 = close, 1 = far)
        double normalizedDistance = std::min(1.0, distance / 200.0); // 200 is max ray length
        inputs.push_back(normalizedDistance);
    }

    // Add car speed as input
    sf::Vector2f velocity = car.getVelocity();
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    inputs.push_back(std::min(1.0, speed / 500.0)); // Normalize speed

    // Add car rotation as input (normalized to -1 to 1)
    float rotation = car.getRotation();
    inputs.push_back(std::sin(rotation * 3.14159f / 180.0f)); // Convert to radians and get sin

    // Process through neural network
    std::vector<double> outputs = brain.process(inputs);

    // Apply outputs to car controls
    if (outputs.size() >= 2)
    {
        // Output 0: Steering (-1 = left, 1 = right)
        float steering = static_cast<float>(outputs[0]);

        // Output 1: Acceleration (-1 = brake, 1 = accelerate)
        float acceleration = static_cast<float>(outputs[1]);

        // Apply controls to car
        car.setAIInputs(steering, acceleration);
    }
}

void AIController::updateFitness(float deltaTime)
{
    if (!isAlive)
        return;

    timeAlive += deltaTime;

    // Base fitness on time alive and checkpoints hit
    // Fitness formula: time alive + checkpoint bonus
    fitness = timeAlive + (checkpointsHit * 1000.0); // Each checkpoint worth 1000 time units
}

void AIController::resetFitness()
{
    fitness = 0.0;
    checkpointsHit = 0;
    timeAlive = 0.0f;
    isAlive = true;
}

void AIController::reset(const sf::Vector2f &position, float rotation)
{
    startPosition = position;
    startRotation = rotation;
    resetFitness();
}

AIController AIController::crossover(const AIController &other) const
{
    AIController child;
    child.brain = brain.crossover(other.brain);
    child.resetFitness();
    return child;
}