#ifndef COREMANAGER_H
#define COREMANAGER_H

#include <filesystem>
#include <spdlog/spdlog.h>

#include "V4Earthend.h"
#include "Config.h"
#include "ResourceManager.h"
#include "TextureManager.h"
#include "Input/InputController.h"
#include "Input/MouseController.h"
#include "Input/TextInputController.h"
#include "StringRepository.h"
#include "Mechanics/Globals.h"
#include "Worker.h"

// Class for holding the pointers to commonly used important game contents
class CoreManager
{
public:
    static CoreManager& getInstance();
    void init();
    V4Earthend* getCore();
    Config* getConfig();
    StringRepository* getStrRepo();
    InputController* getInputController();
    MouseController* getMouseController();
    TextInputController* getTextInputController();
    sf::RenderWindow* getWindow();
    Globals* getGlobals();
    Worker* getWorker();

private:
    CoreManager();
    ~CoreManager();
    
    // Pointer to V4Earthend
    V4Earthend* core;

    // Pointer to Config
    Config* config;

    // Pointer to String repository (for localization)
    StringRepository* strRepo;

    // Pointer to Input controller
    InputController* inputCtrl;

    // Pointer to Text Input controller
    TextInputController* textCtrl;

    // Pointer to Mouse controller
    MouseController* mouseCtrl;

    // Pointer to current window
    sf::RenderWindow* window;

    // Pointer to globals
    Globals* globals;

    // Pointer to worker
    Worker* worker;
};

#endif