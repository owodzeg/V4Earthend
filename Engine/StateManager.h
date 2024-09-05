#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <spdlog/spdlog.h>
#include <thread>
#include <vector>

#include "Menu/FirstRun.h"

// Class for holding the pointers to commonly used important game contents
class StateManager
{
public:
    enum GameState
    {
        ENTRY = 0,
        FIRSTRUN = 1,
        INITWORKER = 2
    };

    static StateManager& getInstance();
    void updateCurrentState();
    void initState(int state);
    void initStateMT(int state);
    void setState(int state);
    int getState();

    FirstRun* firstRun;

private:
    StateManager();
    ~StateManager();

    int prevGameState = 0;
    int currentGameState = 0;
    int afterTipState = 0;

    std::vector<std::thread> loadingThreads;
};

#endif