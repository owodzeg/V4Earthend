#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "StateManager.h"
#include "CoreManager.h"

StateManager::StateManager()
{
}

StateManager::~StateManager()
{
    for (auto& t : loadingThreads)
    {
        t.detach();
    }
}

StateManager& StateManager::getInstance()
{
    static StateManager instance;
    return instance;
}

void StateManager::updateCurrentState()
{
    switch (currentGameState)
    {
        case ENTRY: {

            break;
        }
    }
}

void StateManager::initState(int state)
{
    
}

void StateManager::initStateMT(int state)
{
    loadingThreads.push_back(std::thread(&StateManager::initState, this, state));
}

void StateManager::setState(int state)
{
    // Change the state
    SPDLOG_DEBUG("Changing state to {}", state);
    prevGameState = currentGameState;
    currentGameState = state;
}

int StateManager::getState()
{
    return currentGameState;
}