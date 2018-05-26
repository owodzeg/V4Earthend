#ifndef V4UPDATER_H
#define V4UPDATER_H

#include <SFML/Graphics.hpp>
#include "Earthend.h"

class V4Updater
{
    private:
    Earthend earthend;

    std::vector<float> tot_fps;
    std::map<int,bool> keyMap;

    float fps = 60;
    sf::Clock fpsclock;

    int a = 0;

    public:
    int mouseX,mouseY;
    bool mouseLeftClick = false;

    V4Updater();
    void Init();
};

#endif // V4CREATOR_H
