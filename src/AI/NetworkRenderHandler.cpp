#include "NetworkRenderHandler.h"

NetworkRenderHandler::NetworkRenderHandler()
    : currentNetwork(nullptr), isVisible(false),
      displayX(20.0f), displayY(800.0f), displayWidth(300.0f), displayHeight(250.0f)
{
    networkRender = std::make_unique<NetworkRender>();
    networkRender->setPosition(displayX, displayY);
    networkRender->setSize(displayWidth, displayHeight);
}

void NetworkRenderHandler::setNetwork(const NeuralNetwork *network)
{
    currentNetwork = network;

    if (currentNetwork && networkRender)
    {
        networkRender->setNetwork(*currentNetwork);
    }
}

void NetworkRenderHandler::update()
{
    if (currentNetwork && networkRender && isVisible)
    {
        networkRender->updateValues(*currentNetwork);
    }
}

void NetworkRenderHandler::draw(sf::RenderWindow &window) const
{
    if (isVisible && networkRender)
    {
        networkRender->draw(window);
    }
}

void NetworkRenderHandler::setDisplayPosition(float x, float y)
{
    displayX = x;
    displayY = y;

    if (networkRender)
    {
        networkRender->setPosition(x, y);
    }
}

void NetworkRenderHandler::setDisplaySize(float width, float height)
{
    displayWidth = width;
    displayHeight = height;

    if (networkRender)
    {
        networkRender->setSize(width, height);
    }
}