#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE 

#include "V4Earthend.h"
#include "CoreManager.h"
#include "StateManager.h"
#include <chrono>
#include <string>

V4Earthend::V4Earthend()
{
    /** Load config from config.cfg **/
    Config* config = CoreManager::getInstance().getConfig();
    config->LoadConfig();

    /** Apply logging level from config **/
    switch (config->GetInt("logLevel")) //i can't get int to convert to a spdlog::set_level argument, so i'm making a switch
    {
        case 0: {
            spdlog::set_level(spdlog::level::trace);
            SPDLOG_INFO("Logging level set to TRACE");
            break;
        }
        case 1: {
            spdlog::set_level(spdlog::level::debug);
            SPDLOG_INFO("Logging level set to DEBUG");
            break;
        }
        case 2: {
            spdlog::set_level(spdlog::level::info);
            SPDLOG_INFO("Logging level set to INFO");
            break;
        }
    }

    /** Load Resource Manager **/
    ResourceManager::getInstance().getQuality();
}

float V4Earthend::getFPS()
{
    return fps;
}

// Core function that runs the entirety of Patafour
void V4Earthend::init()
{
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
    
    // Gather the Config pointer so we can get values from there
    Config* config = CoreManager::getInstance().getConfig();

    if (config->GetInt("enableFullscreen")) // open in fullscreen
        window->create(sf::VideoMode(config->GetInt("resX"), config->GetInt("resY")), "Patafour Launcher", sf::Style::Fullscreen, settings);
    else if (config->GetInt("enableBorderlessWindow")) // open as borderless window
        window->create(sf::VideoMode(config->GetInt("resX"), config->GetInt("resY")), "Patafour Launcher", sf::Style::None, settings);
    else // open as a regular window
        window->create(sf::VideoMode(config->GetInt("resX"), config->GetInt("resY")), "Patafour Launcher", sf::Style::Titlebar | sf::Style::Close, settings);

    // Get current framerate limit
    framerate_limit = config->GetInt("framerateLimit");
    
    // Despite having an "Unlimited" framerate option, we limit it to 500.
    if (framerate_limit == 0)
        framerate_limit = 500;
    if (framerate_limit > 500)
        framerate_limit = 500;

    // Apply window settings (fps limit, vsync)
    SPDLOG_INFO("Applying window settings");
    window->setFramerateLimit(framerate_limit);
    window->setKeyRepeatEnabled(false);
    window->setVerticalSyncEnabled(config->GetInt("verticalSync"));

    // Load language data and appropriate font
    SPDLOG_DEBUG("Loading language data");
    StringRepository* strRepo = CoreManager::getInstance().getStrRepo();
    strRepo->LoadLanguageFiles(config->GetInt("lang"));

    if(strRepo->langFonts.size() > config->GetInt("lang"))
    {
        config->fontPath = "resources/fonts/" + strRepo->langFonts[config->GetInt("lang") - 1];
    }
    else
    {
        SPDLOG_ERROR("Could not find a font file for language with id {}", config->GetInt("lang"));
        config->fontPath = "";
    }

    SPDLOG_DEBUG("Font path is {}", config->fontPath);

    /** Version and fps text **/
    SPDLOG_DEBUG("Creating text for version and FPS");
    strRepo->font.loadFromFile(config->fontPath);

    t_version.setFont(strRepo->font);
    t_version.setCharacterSize(24);
    t_version.setFillColor(sf::Color(255, 255, 255, 32));
    t_version.setString("V4Hero Client " + hero_version);

    t_fps.setFont(strRepo->font);
    t_fps.setCharacterSize(24);
    t_fps.setFillColor(sf::Color(255, 255, 255, 96));
    t_fps.setOutlineColor(sf::Color(0, 0, 0, 96));
    t_fps.setOutlineThickness(1);
    t_fps.setString("FPS: ");

    // Get Input controller
    InputController* inputCtrl = CoreManager::getInstance().getInputController();
    inputCtrl->LoadKeybinds();

    StateManager::getInstance().setState(StateManager::ENTRY);

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

        window->clear();

        // Draw whatever state is currently in use
        StateManager::getInstance().updateCurrentState();

        // Reset view for static GUI
        auto lastView = window->getView();
        window->setView(window->getDefaultView());

        // Draw version number
        t_version.setPosition(4, 4);
        window->draw(t_version);

        // If FPS counter is enabled, draw it
        if (config->GetInt("showFPS"))
        {
            t_fps.setString("FPS: " + std::to_string(int(ceil(rawFps))));
            t_fps.setOrigin(t_fps.getLocalBounds().width, 0);
            t_fps.setPosition(window->getSize().x - 4, 4);
            window->draw(t_fps);
        }

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

    SPDLOG_INFO("Main game loop exited. Shutting down...");
}
