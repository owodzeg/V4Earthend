#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE 

#include "CoreManager.h"

CoreManager::CoreManager()
{

}

CoreManager::~CoreManager()
{

}

CoreManager& CoreManager::getInstance()
{
    static CoreManager instance;
    return instance;
}

// Initializes necessary parts of the game.
void CoreManager::init()
{
    // Config must be loaded first. V4Earthend uses config for initialization.
    config = new Config;

    // String repository and save reader both use config values, so we initialize it after config
    strRepo = new StringRepository;

    // Create InputController before window, because we take events from it
    inputCtrl = new InputController;

    // Create MouseController before window, because we take events from it
    mouseCtrl = new MouseController;

    // Window must be created beforehand so V4Earthend can use it.
    window = new sf::RenderWindow;

    // After we created prerequisities for V4Earthend, we can safely create it.
    core = new V4Earthend;

    // Initialize the globals set
    globals = new Globals;

    // Worker for all updater needs
    worker = new Worker;
}

// Returns a pointer to V4Earthend.
V4Earthend* CoreManager::getCore()
{
    return core;
}

// Returns a pointer to Config.
Config* CoreManager::getConfig()
{
    return config;
}

// Returns a pointer to String repository.
StringRepository* CoreManager::getStrRepo()
{
    return strRepo;
}

// Returns a pointer to InputController.
InputController* CoreManager::getInputController()
{
    return inputCtrl;
}

// Returns a pointer to InputController.
MouseController* CoreManager::getMouseController()
{
    return mouseCtrl;
}

// Returns a pointer to the window game uses.
sf::RenderWindow* CoreManager::getWindow()
{
    return window;
}

// Returns a pointer to the worker.
Globals* CoreManager::getGlobals()
{
    return globals;
}

// Returns a pointer to the worker.
Worker* CoreManager::getWorker()
{
    return worker;
}