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
            entry->draw();
            break;
        }

        case FIRSTRUN: {
            firstRun->draw();
            break;
        }
    }
}

void StateManager::initState(int state)
{
    Worker* worker = CoreManager::getInstance().getWorker();

    switch (state)
    {
        case ENTRY: {
            if(entry == nullptr)
            {
                entry = new Entry;
            }

            entry->init();
            initStateMT(INITWORKER);
            break;
        }

        case FIRSTRUN: {
            if(firstRun == nullptr)
            {
                firstRun = new FirstRun;
            }

            firstRun->init();
            initStateMT(INITWORKER);
            break;
        }

        case INITWORKER: {
            if(worker == nullptr)
            {
                worker = new Worker;
            }

            worker->init();
            break;
        }
    }
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