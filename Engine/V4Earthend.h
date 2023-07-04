#ifndef V4EARTHEND_H
#define V4EARTHEND_H

#include <SFML/Graphics.hpp>
#include <random>

// Main game class that handles the engine
class V4Earthend
{
private:
    // ...
public:
    // Static GUI always on screen
    sf::Font f_font;
    sf::Text t_version;
    sf::Text t_fps;

    // Variables for delta time calc
    float fps = 60;
    sf::Clock fpsclock;
    unsigned int framerate_limit = 60;
    std::vector<float> frame_times;

    // Fetch current version from CMakeLists.txt
    std::string hero_version = PATAFOUR_VERSION;

    // Randomization purposes
    std::mt19937::result_type seed;
    std::mt19937 gen;

    // Misc
    bool close_window = false;

    // Functions
    V4Earthend();
    float getFPS();
    void init();
};
#endif // V4EARTHEND_H
