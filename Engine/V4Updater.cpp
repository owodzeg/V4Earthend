#include <iostream>

#include "V4Updater.h"

using namespace std;

V4Updater::V4Updater()
{

}

void V4Updater::Init()
{
    sf::RenderWindow window(sf::VideoMode(1280,720),"Earthend");

    window.setFramerateLimit(240);
    window.setKeyRepeatEnabled(false);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if(event.type == sf::Event::KeyPressed)
			{
			    keyMap[event.key.code] = true;
			}

			if(event.type == sf::Event::KeyReleased)
            {
                keyMap[event.key.code] = false;
            }

            if(event.type == sf::Event::MouseMoved)
            {
                mouseX = event.mouseMove.x;
                mouseY = event.mouseMove.y;
            }

            if(event.type == sf::Event::MouseButtonPressed)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    mouseLeftClick = true;
                }
            }

            if(event.type == sf::Event::MouseButtonReleased)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    mouseLeftClick = false;
                }
            }
        }

        fps = float(1000000) / fpsclock.getElapsedTime().asMicroseconds();
        fpsclock.restart();

        window.clear(sf::Color::White);
        earthend.Init();
        window.display();

        keyMap.clear();
    }
}
