#pragma once
#include "NeuralNetwork.h"
#include <SFML/Graphics.hpp>

class Car; // Forward declaration

class AIController
{
private:
    NeuralNetwork brain;
    double fitness;
    bool isAlive;
    int checkpointsHit;
    float timeAlive;
    sf::Vector2f startPosition;
    float startRotation;

public:
    AIController();

    // Initialize the AI with a neural network
    void initialize(int numInputs, int numOutputs, int numHidden = 4);

    // Control the car based on sensor inputs
    void controlCar(Car &car, const std::vector<float> &rayDistances);

    // Fitness evaluation
    void updateFitness(float deltaTime);
    double getFitness() const { return fitness; }
    void resetFitness();

    // Life management
    bool isCarAlive() const { return isAlive; }
    void kill() { isAlive = false; }
    void reset(const sf::Vector2f &position, float rotation);

    // Checkpoint tracking
    void onCheckpointHit() { checkpointsHit++; }
    int getCheckpointsHit() const { return checkpointsHit; }
    void setCheckpointsHit(int count) { checkpointsHit = count; }
    float getTimeAlive() const { return timeAlive; }

    // Genetic operations
    void mutate() { brain.mutate(); }
    AIController crossover(const AIController &other) const;

    // Getters for the brain
    const NeuralNetwork &getBrain() const { return brain; }
    NeuralNetwork &getBrain() { return brain; }
};