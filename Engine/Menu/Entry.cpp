#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>
#include <fstream>
#include "CoreManager.h"
#include "Entry.h"
#include "../Func.h"

void Entry::init()
{
    StringRepository* strRepo = CoreManager::getInstance().getStrRepo();

    // get fonts
    std::ifstream fontFile("resources/lang/fonts.txt");
    std::string line;

    while(std::getline(fontFile, line))
    {
        std::vector<std::string> param = Func::Split(line, ',');
        std::string name = param[0];
        std::string fontf = param[1];

        strRepo->LoadFontFromFile("resources/font/"+fontf, name);
    }

    // get langs
    std::ifstream langFile("resources/lang/languages.txt");

    while(std::getline(fontFile, line))
    {
        std::vector<std::string> param = Func::Split(line, ',');
        std::string code = param[0];
        std::string name = param[1];
        std::string font = param[2];

        strRepo->LoadLanguageFile(code, name, "resources/lang/"+code+"/"+code+".txt");
        strRepo->langToFontMapping[code] = font;
    }

    p_head.setFillColor(sf::Color::Black);
    p_white.setFillColor(sf::Color::White);
    p_pupil.setFillColor(sf::Color::Black);

    sf::RenderWindow* window = CoreManager::getInstance().getWindow();
    pon_x_c = window->getSize().x/2;
    pon_y_c = window->getSize().y/2;
    pon_x_d = pon_x_c;
    pon_y_d = pon_y_c;

    /*
    r_head_d = 62;
    r_white_d = 41;
    r_pupil_d = 27;
    */

    r_head_d = 62 * 100;
    r_white_d = 41 * 100;
    r_pupil_d = 27 * 100;

    r_head_c = r_head_d;
    r_white_c = r_white_d;
    r_pupil_c = r_pupil_d;

    logo.load("resources/graphics/ui/logo.png");

    logo_x_d = 3840/2;
    logo_y_d = 2160/2;

    logo_x_c = logo_x_d;
    logo_y_c = logo_y_d;

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);

    if(localTime->tm_hour >= 5 && localTime->tm_hour <= 9)
        bg.Load("earthend_1");
    if(localTime->tm_hour >= 10 && localTime->tm_hour <= 16)
        bg.Load("earthend_2");
    if(localTime->tm_hour >= 17 && localTime->tm_hour <= 19)
        bg.Load("earthend_3");
    if(localTime->tm_hour >= 20 && localTime->tm_hour <= 22)
        bg.Load("earthend_4");
    if(localTime->tm_hour >= 23 || localTime->tm_hour <= 4)
        bg.Load("earthend_5");

    a_clock.restart();

    SPDLOG_INFO("Entry initialized.");
}

void Entry::draw()
{
    sf::RenderWindow* window = CoreManager::getInstance().getWindow();
    Worker* worker = CoreManager::getInstance().getWorker();

    //SPDLOG_INFO("pon_x_c {} pon_x_d {} pon_x_offset_c {} pon_x_offset_d {}", pon_x_c, pon_x_d, pon_x_offset_c, pon_x_offset_d);
    //SPDLOG_INFO("pon_y_c {} pon_y_d {} pon_y_offset_c {} pon_y_offset_d {}", pon_y_c, pon_y_d, pon_y_offset_c, pon_y_offset_d);

    float fps = CoreManager::getInstance().getCore()->fps;
    float speed_delta = speed / fps * 240;

    if(fabs(r_head_c - r_head_d) > 0)
    {
        if(r_head_c > r_head_d)
        {
            r_head_c -= fabs(r_head_c - r_head_d) * speed_delta;
        }
        else
        {
            r_head_c += fabs(r_head_c - r_head_d) * speed_delta;
        }
    }
    if(fabs(r_white_c - r_white_d) > 0)
    {
        if(r_white_c > r_white_d)
        {
            r_white_c -= fabs(r_white_c - r_white_d) * speed_delta;
        }
        else
        {
            r_white_c += fabs(r_white_c - r_white_d) * speed_delta;
        }
    }
    if(fabs(r_pupil_c - r_pupil_d) > 0)
    {
        if(r_pupil_c > r_pupil_d)
        {
            r_pupil_c -= fabs(r_pupil_c - r_pupil_d) * speed_delta;
        }
        else
        {
            r_pupil_c += fabs(r_pupil_c - r_pupil_d) * speed_delta;
        }
    }
    if(fabs(pupil_offset_x_c - pupil_offset_x_d) > 0)
    {
        if(pupil_offset_x_c > pupil_offset_x_d)
        {
            pupil_offset_x_c -= fabs(pupil_offset_x_c - pupil_offset_x_d) * speed_delta;
        }
        else
        {
            pupil_offset_x_c += fabs(pupil_offset_x_c - pupil_offset_x_d) * speed_delta;
        }
    }
    if(fabs(pupil_offset_y_c - pupil_offset_y_d) > 0)
    {
        if(pupil_offset_y_c > pupil_offset_y_d)
        {
            pupil_offset_y_c -= fabs(pupil_offset_y_c - pupil_offset_y_d) * speed_delta;
        }
        else
        {
            pupil_offset_y_c += fabs(pupil_offset_y_c - pupil_offset_y_d) * speed_delta;
        }
    }
    if(fabs(pupil_angle_c - pupil_angle_d) > 0)
    {
        if(pupil_angle_c > pupil_angle_d)
        {
            pupil_angle_c -= fabs(pupil_angle_c - pupil_angle_d) * speed_delta;
        }
        else
        {
            pupil_angle_c += fabs(pupil_angle_c - pupil_angle_d) * speed_delta;
        }
    }
    if(fabs(pon_x_c - pon_x_d) > 0)
    {
        if(pon_x_c > pon_x_d)
        {
            pon_x_c -= fabs(pon_x_c - pon_x_d) * speed_delta;
        }
        else
        {
            pon_x_c += fabs(pon_x_c - pon_x_d) * speed_delta;
        }
    }
    if(fabs(pon_y_c - pon_y_d) > 0)
    {
        if(pon_y_c > pon_y_d)
        {
            pon_y_c -= fabs(pon_y_c - pon_y_d) * speed_delta;
        }
        else
        {
            pon_y_c += fabs(pon_y_c - pon_y_d) * speed_delta;
        }
    }
    if(fabs(pon_y_offset_c - pon_y_offset_d) > 0)
    {
        if(pon_y_offset_c > pon_y_offset_d)
        {
            pon_y_offset_c -= fabs(pon_y_offset_c - pon_y_offset_d) * speed_delta;
        }
        else
        {
            pon_y_offset_c += fabs(pon_y_offset_c - pon_y_offset_d) * speed_delta;
        }
    }
    if(fabs(pon_x_offset_c - pon_x_offset_d) > 0)
    {
        if(pon_x_offset_c > pon_x_offset_d)
        {
            pon_x_offset_c -= fabs(pon_x_offset_c - pon_x_offset_d) * speed_delta;
        }
        else
        {
            pon_x_offset_c += fabs(pon_x_offset_c - pon_x_offset_d) * speed_delta;
        }
    }
    
    if(fabs(logo_x_c - logo_x_d) > 0)
    {
        if(logo_x_c > logo_x_d)
        {
            logo_x_c -= fabs(logo_x_c - logo_x_d) * speed_delta;
        }
        else
        {
            logo_x_c += fabs(logo_x_c - logo_x_d) * speed_delta;
        }
    }
    
    if(fabs(logo_y_c - logo_y_d) > 0)
    {
        if(logo_y_c > logo_y_d)
        {
            logo_y_c -= fabs(logo_y_c - logo_y_d) * speed_delta;
        }
        else
        {
            logo_y_c += fabs(logo_y_c - logo_y_d) * speed_delta;
        }
    }

    if(a_state == 0 && a_clock.getElapsedTime().asMilliseconds() > 1500)
    {
        a_state = 1;
        speed = 0.02;

        r_head_d = 62*0.75;
        r_white_d = 41*0.75;
        r_pupil_d = 27*0.75;

        pon_x_offset_d = 56;
        pon_y_offset_d = 15;

        a_clock.restart();
    }

    if(a_state == 1 && a_clock.getElapsedTime().asMilliseconds() > 2500)
    {
        speed = 0.015;

        pon_y_d -= 200;
        logo_y_d -= 600;

        a_state = 2;
    }

    bg.Draw(camera);

    logo.setOrigin(logo.getGlobalBounds().width/2, logo.getGlobalBounds().height/2);
    logo.setPosition(logo_x_c, logo_y_c);
    logo.draw();

    p_head.setRadius(r_head_c);
    p_white.setRadius(r_white_c);
    p_pupil.setRadius(r_pupil_c);
    p_head.setOrigin(r_head_c, r_head_c);
    p_white.setOrigin(r_white_c, r_white_c);
    p_pupil.setOrigin(r_pupil_c + pupil_offset_x_c, r_pupil_c + pupil_offset_y_c);
    p_pupil.setRotation(pupil_angle_c);

    p_head.setPosition(pon_x_c + pon_x_offset_c, pon_y_c + pon_y_offset_c);
    p_white.setPosition(pon_x_c + pon_x_offset_c, pon_y_c + pon_y_offset_c);
    p_pupil.setPosition(pon_x_c + pon_x_offset_c, pon_y_c + pon_y_offset_c);

    window->draw(p_head);
    window->draw(p_white);
    window->draw(p_pupil);
}