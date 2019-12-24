#include <iostream>

#include "V4Updater.h"
#include <windows.h>

using namespace std;

V4Updater::V4Updater()
{

}

void V4Updater::Init()
{
    sf::RenderWindow window(sf::VideoMode(1280,720),"Patafour Launcher");

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

        window.clear(sf::Color::Black);
        earthend.fps = fps;
        earthend.mouseX = mouseX;
        earthend.mouseY = mouseY;
        earthend.mouseLeftClick = mouseLeftClick;
        earthend.Init(window);

        if(!earthend.mouseLeftClick)
        mouseLeftClick = false;

        if(earthend.exit_launcher)
        {
            if(earthend.b_run)
            {
                string dir = earthend.installdir+"\\Patafour.exe";
                cout << dir << endl;
                PROCESS_INFORMATION pi;
                STARTUPINFO si;

                ZeroMemory( & pi, sizeof( pi ) );
                ZeroMemory( & si, sizeof( si ) );
                si.cb = sizeof( si );

                if(!CreateProcess(NULL, (LPSTR)dir.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
                {
                    printf("Could not create process. (%ld)", GetLastError());
                }

                break;
            }
            else
            {
                break;
            }
        }

        if(earthend.selfupdate)
        {
            string dir = working_exe;
            cout << dir << endl;
            PROCESS_INFORMATION pi;
            STARTUPINFO si;

            ZeroMemory( & pi, sizeof( pi ) );
            ZeroMemory( & si, sizeof( si ) );
            si.cb = sizeof( si );

            if(!CreateProcess(NULL, (LPSTR)dir.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            {
                printf("Could not create process. (%ld)", GetLastError());
            }

            break;
        }

        if(earthend.runhero)
        {
            string dir = working_dir+"\\game\\V4Hero.exe";
            cout << dir << endl;
            PROCESS_INFORMATION pi;
            STARTUPINFO si;

            ZeroMemory( & pi, sizeof( pi ) );
            ZeroMemory( & si, sizeof( si ) );
            si.cb = sizeof( si );

            if(!CreateProcess(NULL, (LPSTR)dir.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            {
                printf("Could not create process. (%ld)", GetLastError());
            }

            break;
        }

        window.display();

        keyMap.clear();
    }
}
