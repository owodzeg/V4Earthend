#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>
#include "CoreManager.h"
#include "FirstRun.h"

void FirstRun::init()
{
    p_head.setFillColor(sf::Color::Black);
    p_white.setFillColor(sf::Color::White);
    p_pupil.setFillColor(sf::Color::Black);

    sf::RenderWindow* window = CoreManager::getInstance().getWindow();
    pon_x_c = window->getSize().x/2;
    pon_y_c = window->getSize().y/2;
    pon_x_d = pon_x_c;
    pon_y_d = pon_y_c;

    SPDLOG_INFO("FirstRun initialized.");
}

void FirstRun::draw()
{
    sf::RenderWindow* window = CoreManager::getInstance().getWindow();

    if(fabs(r_head_c - r_head_d) > 0)
    {
        if(r_head_c > r_head_d)
        {
            r_head_c -= fabs(r_head_c - r_head_d) * speed;
        }
        else
        {
            r_head_c += fabs(r_head_c - r_head_d) * speed;
        }
    }
    if(fabs(r_white_c - r_white_d) > 0)
    {
        if(r_white_c > r_white_d)
        {
            r_white_c -= fabs(r_white_c - r_white_d) * speed;
        }
        else
        {
            r_white_c += fabs(r_white_c - r_white_d) * speed;
        }
    }
    if(fabs(r_pupil_c - r_pupil_d) > 0)
    {
        if(r_pupil_c > r_pupil_d)
        {
            r_pupil_c -= fabs(r_pupil_c - r_pupil_d) * speed;
        }
        else
        {
            r_pupil_c += fabs(r_pupil_c - r_pupil_d) * speed;
        }
    }
    if(fabs(pupil_offset_x_c - pupil_offset_x_d) > 0)
    {
        if(pupil_offset_x_c > pupil_offset_x_d)
        {
            pupil_offset_x_c -= fabs(pupil_offset_x_c - pupil_offset_x_d) * speed;
        }
        else
        {
            pupil_offset_x_c += fabs(pupil_offset_x_c - pupil_offset_x_d) * speed;
        }
    }
    if(fabs(pupil_offset_y_c - pupil_offset_y_d) > 0)
    {
        if(pupil_offset_y_c > pupil_offset_y_d)
        {
            pupil_offset_y_c -= fabs(pupil_offset_y_c - pupil_offset_y_d) * speed;
        }
        else
        {
            pupil_offset_y_c += fabs(pupil_offset_y_c - pupil_offset_y_d) * speed;
        }
    }
    if(fabs(pupil_angle_c - pupil_angle_d) > 0)
    {
        if(pupil_angle_c > pupil_angle_d)
        {
            pupil_angle_c -= fabs(pupil_angle_c - pupil_angle_d) * speed;
        }
        else
        {
            pupil_angle_c += fabs(pupil_angle_c - pupil_angle_d) * speed;
        }
    }
    if(fabs(pon_x_c - pon_x_d) > 0)
    {
        if(pon_x_c > pon_x_d)
        {
            pon_x_c -= fabs(pon_x_c - pon_x_d) * speed;
        }
        else
        {
            pon_x_c += fabs(pon_x_c - pon_x_d) * speed;
        }
    }
    if(fabs(pon_y_c - pon_y_d) > 0)
    {
        if(pon_y_c > pon_y_d)
        {
            pon_y_c -= fabs(pon_y_c - pon_y_d) * speed;
        }
        else
        {
            pon_y_c += fabs(pon_y_c - pon_y_d) * speed;
        }
    }

    if(a_state == 0 && a_clock.getElapsedTime().asSeconds() > 2)
    {
        r_head_d = 62;
        r_white_d = 41;
        r_pupil_d = 27;

        a_state = 1;
        a_clock.restart();
    }

    if(a_state == 1 && a_clock.getElapsedTime().asSeconds() > 1)
    {
        pupil_offset_x_d = -17;
        speed = 0.025;

        a_state = 2;
        a_clock.restart();
    }

    if(a_state == 2 && a_clock.getElapsedTime().asSeconds() > 1)
    {
        pupil_offset_x_d = 17;

        a_state = 3;
        a_clock.restart();
    }

    if(a_state == 3 && a_clock.getElapsedTime().asSeconds() > 1)
    {
        pupil_offset_x_d = 0;

        a_state = 4;
        a_clock.restart();
    }

    if(a_state == 4 && a_clock.getElapsedTime().asSeconds() > 1)
    {
        speed = 0.005;

        pupil_offset_x_d = 17;
        pupil_offset_x_c = 17;

        pupil_angle_d = 1400;

        a_state = 5;
        a_clock.restart();
    }

    if(a_state == 5 && a_clock.getElapsedTime().asSeconds() > 2.5)
    {
        pupil_angle_d = pupil_angle_c;
        speed = 0.03;

        pupil_offset_x_d = 0;

        a_state = 6;
        a_clock.restart();
    }

    if(a_state == 6 && a_clock.getElapsedTime().asSeconds() > 1)
    {
        speed = 0.01;
        pupil_angle_d = 0;
        pupil_angle_c = 0;
        pupil_offset_y_d = -17;
        pon_y_d = 96;

        a_state = 7;
        a_clock.restart();
    }

    if(a_state == 7 && a_clock.getElapsedTime().asSeconds() > 2)
    {
        speed = 0.05;

        sf::Vector2i mouseo = sf::Mouse::getPosition(*window);

        auto pon = window->mapPixelToCoords(sf::Vector2i(pon_x_c, pon_y_c));
        sf::Vector2f mouse = window->mapPixelToCoords(mouseo);

        auto len = sf::Vector2f(mouse.x-pon.x, mouse.y-pon.y);
        auto deg = atan2(len.x, len.y) * 180/3.14159;

        if(deg < -150)
            deg = -150;
        if(deg > 150)
            deg = 150;

        pupil_angle_d = -(deg);


        SPDLOG_DEBUG("mouse {} {} pon {} {} len {} {} deg {}", mouse.x, mouse.y, pon.x, pon.y, len.x, len.y, pupil_angle_d);
    }

    p_head.setRadius(r_head_c);
    p_white.setRadius(r_white_c);
    p_pupil.setRadius(r_pupil_c);
    p_head.setOrigin(r_head_c, r_head_c);
    p_white.setOrigin(r_white_c, r_white_c);
    p_pupil.setOrigin(r_pupil_c + pupil_offset_x_c, r_pupil_c + pupil_offset_y_c);
    p_pupil.setRotation(pupil_angle_c);

    p_head.setPosition(pon_x_c, pon_y_c);
    p_white.setPosition(pon_x_c, pon_y_c);
    p_pupil.setPosition(pon_x_c, pon_y_c);

    window->draw(p_head);
    window->draw(p_white);
    window->draw(p_pupil);
}