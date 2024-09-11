#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>
#include "CoreManager.h"
#include "FirstRun.h"

void FirstRun::init()
{
    p_head.setFillColor(sf::Color::Black);
    p_white.setFillColor(sf::Color::White);
    p_pupil.setFillColor(sf::Color::Black);
    p_pupil_closed.setFillColor(sf::Color::Black);

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
    Worker* worker = CoreManager::getInstance().getWorker();

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

    if(a_state == 0 && a_clock.getElapsedTime().asSeconds() > 2)
    {
        r_head_d = 62;
        r_white_d = 41;
        r_pupil_d = 27;

        a_state = 1;
        a_clock.restart();

        worker->setAction(Worker::INIT_FIRSTRUN);

        speed = 0.025;
        initTime = 1;
    }

    if(a_state == 1 && a_clock.getElapsedTime().asSeconds() > initTime)
    {
        pupil_offset_x_d = -17;
        speed = 0.025 + (worker->currentDLProgress / (worker->currentDLTotal+1) * 0.175);
        initTime = 1 - (worker->currentDLProgress / (worker->currentDLTotal+1) * 0.875);

        SPDLOG_INFO("{} {} {}%", speed, initTime, worker->currentDLProgress / worker->currentDLTotal * 100);

        a_state = 2;
        a_clock.restart();
    }

    if(a_state == 2 && a_clock.getElapsedTime().asSeconds() > initTime)
    {
        pupil_offset_x_d = 17;

        a_state = 3;
        a_clock.restart();
    }

    if(a_state == 3 && a_clock.getElapsedTime().asSeconds() > initTime)
    {
        pupil_offset_x_d = -17;

        a_state = 4;
        a_clock.restart();
    }

    if(a_state == 4 && a_clock.getElapsedTime().asSeconds() > initTime)
    {
        pupil_offset_x_d = 17;

        a_state = 5;
        a_clock.restart();
    }

    if(a_state == 5 && a_clock.getElapsedTime().asSeconds() > initTime)
    {
        pupil_offset_x_d = -17;

        a_state = 6;
        a_clock.restart();
    }

    if(a_state == 6 && a_clock.getElapsedTime().asSeconds() > initTime)
    {
        pupil_offset_x_d = 17;

        if(worker->isBusy())
        {
            a_state = 1;
            a_clock.restart();
        }
        else
        {
            a_state = 7;
            CoreManager::getInstance().getCore()->global_font.loadFromMemory(worker->files[0].data(), worker->files[0].size());
            text.setFont(CoreManager::getInstance().getCore()->global_font);
            text.setString("Choose your language");
        }
    }

    if(a_state == 7)
    {
        speed = 0.005;

        pupil_offset_x_d = -17;
        pupil_offset_x_c = -17;

        pupil_angle_d = 1400;

        a_state = 8;
        a_clock.restart();
    }

    if(a_state == 8 && a_clock.getElapsedTime().asSeconds() > 2.5)
    {
        pupil_angle_d = pupil_angle_c;
        speed = 0.03;

        pupil_offset_x_d = 0;

        a_state = 9;
        a_clock.restart();
    }

    if(a_state == 9 && a_clock.getElapsedTime().asSeconds() > 1)
    {
        speed = 0.015;
        pupil_angle_d = 0;
        pupil_angle_c = 0;
        pupil_offset_x_d = -17;
        pon_x_d = 128;

        a_state = 10;
        a_clock.restart();

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f(pon_x_d+r_head_d, pon_y_d-r_head_d-10), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("Choose your language", true);
        messageclouds.push_back(tmp);
    }

    if(a_state == 10 && a_clock.getElapsedTime().asSeconds() > 1.2)
    {
        // follow movement
        speed = 0.05;

        sf::Vector2i mouseo = sf::Mouse::getPosition(*window);

        auto pon = window->mapPixelToCoords(sf::Vector2i(pon_x_c, pon_y_c));
        sf::Vector2f mouse = window->mapPixelToCoords(mouseo);

        auto len = sf::Vector2f(mouse.x - pon.x, mouse.y - pon.y);
        prev_deg = deg;
        deg = atan2(len.y, len.x) * 180 / 3.14159;
        //deg -= 90;

        if (deg < 0)
            deg += 360;

        prev_real_deg = real_deg;
        real_deg = deg;

        auto diff = real_deg - prev_real_deg;

        if (diff > 180)
            diff -= 360;
        else if (diff < -180)
            diff += 360;

        pupil_angle_d += diff;

        // don't touch!
        //SPDLOG_DEBUG("mouse {} {}", mouse.x, mouse.y);
        if(mouse.x > pon_x_c-r_white_c && mouse.x < pon_x_c+r_white_c && mouse.y > pon_y_c-r_white_c && mouse.y < pon_y_c+r_white_c)
        {
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                if(!peck)
                {
                    a_state = 11;
                    shake = -50;
                    a_clock.restart();
                    peck = true;
                }
                SPDLOG_INFO("peck");
            }
        }
    }

    if(a_state == 11)
    {
        if(shake < 0)
        {
            shake += 50 / fps;
            pon_x_offset_d = pow(std::numbers::e, -0.1*shake)*sin(shake);

        }

        if(a_clock.getElapsedTime().asSeconds() >= 1.2)
        {
            peck = false;
            a_state = 10;
        }
    }

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

    p_pupil_closed.setSize(sf::Vector2f(r_pupil_c*2,r_pupil_c));
    p_pupil_closed.setOrigin(r_pupil_c,r_pupil_c*0.5);
    p_pupil_closed.setPosition(pon_x_c + pon_x_offset_c, pon_y_c + pon_y_offset_c);

    window->draw(p_head);
    window->draw(p_white);
    if(!peck)
        window->draw(p_pupil);
    else
        window->draw(p_pupil_closed);

    std::vector<int> m_rm;

    for (int i = 0; i < messageclouds.size(); i++)
    {
        if (messageclouds[i].firstrender)
            messageclouds[i].Show();

        messageclouds[i].Draw();

        if ((!messageclouds[i].active) && (messageclouds[i].done))
            m_rm.push_back(i);
    }

    for (int i = 0; i < m_rm.size(); i++)
    {
        SPDLOG_DEBUG("Erasing MessageCloud id {}", m_rm[i]);
        messageclouds.erase(messageclouds.begin() + m_rm[i] - i);
    }
}