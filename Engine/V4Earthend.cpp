#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE 

#include "V4Earthend.h"
#include "CoreManager.h"
#include "StateManager.h"
#include <chrono>
#include <string>

V4Earthend::V4Earthend()
{
    /** Load Resource Manager **/
    ResourceManager::getInstance().getQuality();
}

float V4Earthend::getFPS()
{
    return fps;
}

// Core function that runs the entirety of Patafour
void V4Earthend::init(std::vector<std::string>& cmd_args)
{
    if(cmd_args.size() >= 2)
    {
        if(cmd_args[1] == "-log")
        {
            int level = atoi(cmd_args[2].c_str());
            spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
        }
    }
    // Create a random seed for all of the randomization 
    srand(time(NULL));
    std::random_device rd; // https://stackoverflow.com/questions/13445688
    seed = rd() ^ ((std::mt19937::result_type)
                           std::chrono::duration_cast<std::chrono::seconds>(
                                   std::chrono::system_clock::now().time_since_epoch())
                                   .count() +
                   (std::mt19937::result_type)
                           std::chrono::duration_cast<std::chrono::microseconds>(
                                   std::chrono::high_resolution_clock::now().time_since_epoch())
                                   .count());
    std::mt19937 tmp(seed);
    gen = tmp;

    // Set antialiasing level (hardcoded, apply from config)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 16;

    // Create the game window
    SPDLOG_INFO("Creating window");
    sf::RenderWindow* window = CoreManager::getInstance().getWindow();
    window->create(sf::VideoMode(1280, 720), "Patafour Launcher", sf::Style::Titlebar | sf::Style::Close, settings);

    // Apply window settings (fps limit, vsync)
    SPDLOG_INFO("Applying window settings");
    window->setFramerateLimit(framerate_limit);
    window->setKeyRepeatEnabled(false);

    // Get Input controller
    InputController* inputCtrl = CoreManager::getInstance().getInputController();
    inputCtrl->LoadKeybinds();

    std::ifstream fr("resources/firstrun");
    if(fr.good())
    {
        StateManager::getInstance().setState(StateManager::ENTRY);
    }
    else
    {
        StateManager::getInstance().initState(StateManager::FIRSTRUN);
        StateManager::getInstance().setState(StateManager::FIRSTRUN);
    }

    // Execute the main game loop
    while (window->isOpen())
    {
        // Check for window events
        sf::Event event;
        while (window->pollEvent(event))
        {
            // Close the window when cross is clicked
            if (event.type == sf::Event::Closed)
                window->close();

            // Forward events to InputController for keyboard and controller usage
            inputCtrl->parseEvents(event);
        }

        // Calculate framerate per second (delta time)
        fps = float(1000000) / fpsclock.getElapsedTime().asMicroseconds();
        float rawFps = fps;
        frame_times.push_back(fps);
        fpsclock.restart();

        auto n = frame_times.size();
        float average = 0.0f;
        if (n != 0)
        {
            average = accumulate(frame_times.begin(), frame_times.end(), 0.0) / (n - 1);
        }

        if (fps <= 1)
            fps = average;
        else
            fps = rawFps;

        while (frame_times.size() > framerate_limit)
            frame_times.erase(frame_times.begin());

        window->clear(sf::Color(64,64,64));

        // Draw whatever state is currently in use
        StateManager::getInstance().updateCurrentState();

        // Reset view for static GUI
        auto lastView = window->getView();
        window->setView(window->getDefaultView());

        // Display everything in the window
        window->display();

        // Return to last view
        window->setView(lastView);

        // Clear the key inputs
        inputCtrl->Flush();

        // Close the window through in-game means
        if (close_window)
        {
            window->close();
        }
    }

    SPDLOG_INFO("Main loop exited. Shutting down...");
}
