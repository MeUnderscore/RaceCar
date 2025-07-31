#include "InnovationTracker.h"
#include <algorithm>

InnovationTracker::InnovationTracker() : nextInnovationNumber(0)
{
}

std::string InnovationTracker::getConnectionKey(int fromNode, int toNode) const
{
    return std::to_string(fromNode) + "->" + std::to_string(toNode);
}

int InnovationTracker::getInnovationNumber(int fromNode, int toNode, bool isNode, int nodeId)
{
    // Check if this innovation already exists
    for (const auto& innovation : innovations)
    {
        if (innovation.fromNode == fromNode && 
            innovation.toNode == toNode && 
            innovation.isNode == isNode)
        {
            return innovation.innovationNumber;
        }
    }
    
    // Create new innovation
    Innovation newInnovation(nextInnovationNumber, fromNode, toNode, isNode, nodeId);
    innovations.push_back(newInnovation);
    
    return nextInnovationNumber++;
}

bool InnovationTracker::innovationExists(int fromNode, int toNode, bool isNode) const
{
    for (const auto& innovation : innovations)
    {
        if (innovation.fromNode == fromNode && 
            innovation.toNode == toNode && 
            innovation.isNode == isNode)
        {
            return true;
        }
    }
    return false;
}

const Innovation* InnovationTracker::getInnovation(int innovationNumber) const
{
    for (const auto& innovation : innovations)
    {
        if (innovation.innovationNumber == innovationNumber)
        {
            return &innovation;
        }
    }
    return nullptr;
}

void InnovationTracker::reset()
{
    innovations.clear();
    nextInnovationNumber = 0;
} 