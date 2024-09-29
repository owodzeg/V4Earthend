#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>
#include "CoreManager.h"
#include "FirstRun.h"
#include "../Func.h"

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

    //SPDLOG_INFO("pon_x_c {} pon_x_d {} pon_x_offset_c {} pon_x_offset_d {}", pon_x_c, pon_x_d, pon_x_offset_c, pon_x_offset_d);
    //SPDLOG_INFO("pon_y_c {} pon_y_d {} pon_y_offset_c {} pon_y_offset_d {}", pon_y_c, pon_y_d, pon_y_offset_c, pon_y_offset_d);

    float fps = CoreManager::getInstance().getCore()->fps;
    float speed_delta = speed / fps * 240;

    Func::smoothTransition(r_head_c, r_head_d, speed_delta);
    Func::smoothTransition(r_white_c, r_white_d, speed_delta);
    Func::smoothTransition(r_pupil_c, r_pupil_d, speed_delta);
    Func::smoothTransition(pupil_offset_x_c, pupil_offset_x_d, speed_delta);
    Func::smoothTransition(pupil_offset_y_c, pupil_offset_y_d, speed_delta);
    Func::smoothTransition(pupil_angle_c, pupil_angle_d, speed_delta);
    Func::smoothTransition(pon_x_c, pon_x_d, speed_delta);
    Func::smoothTransition(pon_y_c, pon_y_d, speed_delta);
    Func::smoothTransition(pon_y_offset_c, pon_y_offset_d, speed_delta);
    Func::smoothTransition(pon_x_offset_c, pon_x_offset_d, speed_delta);

    MouseController* mouseCtrl = CoreManager::getInstance().getMouseController();
    auto mouse = sf::Vector2i(mouseCtrl->getMousePos().x*3, mouseCtrl->getMousePos().y*3);

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
        speed = 0.025 + (worker->currentTaskProgress / (worker->currentTaskTotal+1) * 0.175);
        initTime = 1 - (worker->currentTaskProgress / (worker->currentTaskTotal+1) * 0.875);

        SPDLOG_INFO("{} {} {}%", speed, initTime, worker->currentDLProgress / worker->currentDLTotal * 100);

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

        messageclouds.clear();

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f((pon_x_d+r_head_d)*3, (pon_y_d-r_head_d-10)*3), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("fr_chooselanguage", true);
        messageclouds.push_back(tmp);
    }

    if(a_state == 10 && a_clock.getElapsedTime().asSeconds() > 1.2)
    {
        // follow movement
        speed = 0.05;

        sf::Vector2i mouseo = sf::Mouse::getPosition(*window);

        auto pon = window->mapPixelToCoords(sf::Vector2i(pon_x_c, pon_y_c));
        sf::Vector2f mouse2 = window->mapPixelToCoords(mouseo);

        auto len = sf::Vector2f(mouse2.x - pon.x, mouse2.y - pon.y);
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

        cumulative_angle += diff;
        pupil_angle_d += diff;

        cumulative_angle /= 1 + (0.1 * speed_delta);

        if(fabs(cumulative_angle) >= 500)
        {
            float shook = -((fabs(cumulative_angle)-500) / 20);
            pon_x_offset_d = pow(std::exp(1.0), -0.1*shook)*sin(shook);
            pon_y_offset_d = pow(std::exp(1.0), -0.01*shook)*sin(shook);
        }

        if(fabs(cumulative_angle) >= 1500)
        {
            messageclouds.clear();

            a_state = 11;
            shake = -50;
            cumulative_angle = 0;
            a_clock.restart();

            peck = true;
        }

        // don't touch!
        //SPDLOG_DEBUG("mouse {} {}", mouse.x, mouse.y);
        if(mouse.x > (pon_x_c-r_white_c)*3 && mouse.x < (pon_x_c+r_white_c)*3 && mouse.y > (pon_y_c-r_white_c)*3 && mouse.y < (pon_y_c+r_white_c)*3)
        {
            if(mouseCtrl->getClick(0))
            {
                if(!peck)
                {
                    a_state = 11;
                    shake = -50;
                    a_clock.restart();

                    std::vector<std::string> ouch = {"fr_egg_1", "fr_egg_2", "fr_egg_4"};

                    std::random_device rd;
                    std::mt19937 g(rd());

                    std::shuffle(ouch.begin(), ouch.end(), g);

                    messageclouds.clear();

                    MessageCloud tmp;
                    tmp.Create(20, sf::Vector2f((pon_x_d+r_head_d)*3, (pon_y_d-r_head_d-10)*3), sf::Color(255, 255, 255, 255), false, 3);
                    tmp.msgcloud_ID = 0;
                    tmp.AddDialog(ouch[0], true);
                    messageclouds.push_back(tmp);

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
            pon_x_offset_d = pow(std::exp(1.0), -0.1*shake)*sin(shake);
        }

        if(a_clock.getElapsedTime().asSeconds() >= 1.2)
        {
            peck = false;

            messageclouds.clear();

            MessageCloud tmp;
            tmp.Create(20, sf::Vector2f((pon_x_d+r_head_d)*3, (pon_y_d-r_head_d-10)*3), sf::Color(255, 255, 255, 255), false, 3);
            tmp.msgcloud_ID = 0;
            tmp.AddDialog("fr_chooselanguage", true);
            messageclouds.push_back(tmp);

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

    for (unsigned long i = 0; i < messageclouds.size(); i++)
    {
        if (messageclouds[i].firstrender)
            messageclouds[i].Show();

        messageclouds[i].Draw();

        if ((!messageclouds[i].active) && (messageclouds[i].done))
            m_rm.push_back(i);
    }

    for (unsigned long i = 0; i < m_rm.size(); i++)
    {
        SPDLOG_DEBUG("Erasing MessageCloud id {}", m_rm[i]);
        messageclouds.erase(messageclouds.begin() + m_rm[i] - i);
    }

    StringRepository* strRepo = CoreManager::getInstance().getStrRepo();

    if(a_state == 10 || a_state == 11)
    {
        auto langs = strRepo->GetAvailableLanguages();
        int f = 0;

        bool hover = false;

        for(auto lang : langs)
        {
            auto flag = ResourceManager::getInstance().getSprite(lang.first+".png");
            int row = floor(f/4);
            int col = f % 4;

            auto name = Func::ConvertToUtf8String(lang.second);

            flag.setPosition(1400 + col * 600, 100 + row * 400);
            flag.draw();

            auto font = strRepo->GetFontNameForLanguage(lang.first);

            flagnames[lang.first].setFont(font);
            flagnames[lang.first].setTextQuality(3);
            flagnames[lang.first].setCharacterSize(28);

            if(lang.first == selectedLang)
                flagnames[lang.first].setString("{color 0 192 0}"+name);
            else
                flagnames[lang.first].setString("{color 255 255 255}"+name);

            flagnames[lang.first].setOrigin(flagnames[lang.first].getLocalBounds().width/2, flagnames[lang.first].getLocalBounds().height/2);
            flagnames[lang.first].setPosition(flag.getPosition().x+150, flag.getPosition().y+240);

            f++;

            auto pos = flag.getPosition();
            auto lb = flag.getLocalBounds();

            auto tpos = flagnames[lang.first].getPosition();
            auto tlb = flagnames[lang.first].getLocalBounds();

            if(a_state != 11)
            {
                if(((mouse.x > pos.x) && (mouse.x < pos.x + lb.width) && (mouse.y > pos.y) && (mouse.y < pos.y + lb.height)) || ((mouse.x > tpos.x - tlb.width/2) && (mouse.x < tpos.x + tlb.width/2) && (mouse.y > tpos.y - tlb.height/2) && (mouse.y < tpos.y + tlb.height/2)))
                {
                    hover = true;

                    flagnames[lang.first].setString("{color 255 192 64}"+name);

                    if(mouseCtrl->getClick(0))
                    {
                        selectedLang = lang.first;
                    }

                    if(CoreManager::getInstance().getStrRepo()->GetCurrentLanguage() != lang.first)
                    {
                        CoreManager::getInstance().getStrRepo()->SetCurrentLanguage(lang.first);

                        messageclouds.clear();

                        MessageCloud tmp;
                        tmp.Create(20, sf::Vector2f((pon_x_d+r_head_d)*3, (pon_y_d-r_head_d-10)*3), sf::Color(255, 255, 255, 255), false, 3);
                        tmp.msgcloud_ID = 0;
                        tmp.AddDialog("fr_chooselanguage", true);
                        messageclouds.push_back(tmp);
                    }
                }
            }

            flagnames[lang.first].draw();
        }

        if(!hover)
        {
            if(strRepo->GetCurrentLanguage() != selectedLang)
            {
                CoreManager::getInstance().getStrRepo()->SetCurrentLanguage(selectedLang);

                messageclouds.clear();

                MessageCloud tmp;
                tmp.Create(20, sf::Vector2f((pon_x_d+r_head_d)*3, (pon_y_d-r_head_d-10)*3), sf::Color(255, 255, 255, 255), false, 3);
                tmp.msgcloud_ID = 0;
                tmp.AddDialog("fr_chooselanguage", true);
                messageclouds.push_back(tmp);
            }
        }

        auto font = strRepo->GetFontNameForLanguage(selectedLang);
        b_next.setFont(font);
        b_next.setCharacterSize(50);
        b_next.setStringKey("fr_next");
        b_next.setOrigin(b_next.getLocalBounds().width/2, b_next.getLocalBounds().height/2);
        b_next.setPosition(2440, 550 + ceil(langs.size()/4) * 400);

        auto pos = b_next.getPosition();
        auto lb = b_next.getLocalBounds();

        if((mouse.x > pos.x - lb.width/2) && (mouse.x < pos.x + lb.width/2) && (mouse.y > pos.y - lb.height/2) && (mouse.y < pos.y + lb.height/2))
        {
            b_next.setColor(sf::Color(0, 192, 0));

            if(mouseCtrl->getClick(0))
            {
                a_state = 12;
                a_clock.restart();

                messageclouds.clear();
            }
        }
        else
        {
            b_next.setColor(sf::Color::White);
        }

        b_next.draw();
    }

    if(a_state == 12)
    {
        peck = false;
        speed = 0.025;

        pon_x_d = 640;
        pon_y_d = 300;
        pupil_offset_x_d = 0;
        pupil_angle_d = 0;

        if(a_clock.getElapsedTime().asMilliseconds() > 1000)
        {
            a_state = 13;
            a_clock.restart();
        }
    }

    if(a_state == 13)
    {
        messageclouds.clear();

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f((pon_x_d+r_head_d)*3, (pon_y_d-r_head_d-10)*3), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("fr_welcome1", true);
        tmp.AddDialog("fr_welcome2", false);
        messageclouds.push_back(tmp);

        a_clock.restart();
        a_state = 14;
    }

    if(a_state == 14 && a_clock.getElapsedTime().asMilliseconds() >= 2000)
    {
        pupil_offset_y_d = -17;
        messageclouds.back().NextDialog();

        a_clock.restart();
        a_state = 15;

        gamePath = Func::getInstallDirectory("Patafour");
        Func::create_directory(gamePath);
    }

    if(a_state == 15)
    {
        dir_rect.setFillColor(sf::Color(96,96,96));
        dir_rect.setSize(sf::Vector2f(1000, 60));
        dir_rect.setPosition(140, 400);

        if(mouse.x >= 140*3 && mouse.x <= 1140*3 && mouse.y >= 400*3 && mouse.y <= 460*3)
        {
            dir_rect.setFillColor(sf::Color(140,140,140));

            if(mouseCtrl->getClick(0))
            {
                auto path = Func::openDirectoryDialog();
                if(path)
                {
                    gamePath = *path;
                }
            }
        }

        window->draw(dir_rect);

        auto font = strRepo->GetFontNameForLanguage(selectedLang);
        p_path.setFont(font);
        p_path.setCharacterSize(30);
        p_path.setString("{color 255 255 255}"+gamePath);
        p_path.setPosition(140*3+40, 400*3+26);
        p_path.draw();

        b_next.setFont(font);
        b_next.setCharacterSize(50);
        b_next.setStringKey("fr_finish");
        b_next.setOrigin(b_next.getLocalBounds().width/2, b_next.getLocalBounds().height/2);
        b_next.setPosition(3840/2, 1600);

        auto pos = b_next.getPosition();
        auto lb = b_next.getLocalBounds();

        if((mouse.x > pos.x - lb.width/2) && (mouse.x < pos.x + lb.width/2) && (mouse.y > pos.y - lb.height/2) && (mouse.y < pos.y + lb.height/2))
        {
            b_next.setColor(sf::Color(0, 192, 0));

            if(mouseCtrl->getClick(0))
            {
                speed = 0.025;
                a_state = 16;
                a_clock.restart();

                messageclouds.clear();
            }
        }
        else
        {
            b_next.setColor(sf::Color::White);
        }

        b_next.draw();
    }

    if(a_state == 16)
    {
        pon_x_d = 640;
        pon_y_d = 360;

        if(a_clock.getElapsedTime().asMilliseconds() > 2000)
        {
            speed = 0.005;

            a_state = 17;
            messageclouds.clear();

            MessageCloud tmp;
            tmp.Create(20, sf::Vector2f((pon_x_d+r_head_d)*3, (pon_y_d-r_head_d-10)*3), sf::Color(255, 255, 255, 255), false, 3);
            tmp.msgcloud_ID = 0;
            tmp.AddDialog("fr_downloading", false);
            messageclouds.push_back(tmp);

            worker->gamePath = gamePath;
            worker->setAction(Worker::DOWNLOAD_EARTHEND);

            a_clock.restart();
        }
    }

    if(a_state == 17)
    {
        pupil_angle_d += 5 * speed_delta;

        float sumProg = worker->currentTaskTotal;
        float curProg = 0;

        for(auto x : worker->downloaded_files)
        {
            curProg += x.progress;
        }

        speed = 0.005 + (curProg / (sumProg + 1) * 0.555);
        SPDLOG_INFO("progress: {}", curProg/(sumProg+1));

        auto font = strRepo->GetFontNameForLanguage(selectedLang);

        p_progress.setFont(font);
        p_progress.setCharacterSize(20);
        p_progress.setString("{color 255 255 255}"+std::to_string(int(curProg/1024))+"/"+std::to_string(int(sumProg/1024))+" kB");
        p_progress.setOrigin(p_progress.getLocalBounds().width/2, p_progress.getLocalBounds().height/2);
        p_progress.setPosition(3840/2, 1400);
        p_progress.draw();

        if(a_clock.getElapsedTime().asMilliseconds() > 1000)
        {
            if(!worker->isBusy())
            {
                a_state = 18;
            }
        }
    }

    if(a_state == 18)
    {
        speed = 0;
        messageclouds.back().NextDialog();
        a_clock.restart();
        a_state = 19;
    }

    if(a_state == 19 && a_clock.getElapsedTime().asMilliseconds() > 1000)
    {
        speed = 0.025;

        pupil_offset_x_d = 0;
        pupil_offset_y_d = 0;
        pon_y_d = 240;

        messageclouds.clear();

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f((pon_x_d+r_head_d)*3, (pon_y_d-r_head_d-10)*3), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("fr_finished", false);
        messageclouds.push_back(tmp);

        a_state = 20;
    }

    if(a_state == 20)
    {
        auto font = strRepo->GetFontNameForLanguage(selectedLang);

        p_installedin.setFont(font);
        p_installedin.setCharacterSize(30);
        p_installedin.setStringKey("fr_install_location");
        p_installedin.setColor(sf::Color::White);
        p_installedin.setPosition(140*3+40, 340*3);
        p_installedin.draw();

        p_path.setFont(font);
        p_path.setCharacterSize(30);
        p_path.setString("{color 255 255 255}"+gamePath);
        p_path.setPosition(140*3+40, 360*3+26);
        p_path.draw();

        clicky1.setSize(sf::Vector2f(32, 32));
        clicky2.setSize(sf::Vector2f(32, 32));

        if(opt1)
            clicky1.setFillColor(sf::Color(32,32,32));
        else
            clicky1.setFillColor(sf::Color(192,192,192));

        if(opt2)
            clicky2.setFillColor(sf::Color(32,32,32));
        else
            clicky2.setFillColor(sf::Color(192,192,192));

        clicky1.setPosition(140, 420);
        clicky2.setPosition(140, 464);

        window->draw(clicky1);
        window->draw(clicky2);

        p_runlauncher.setFont(font);
        p_runlauncher.setCharacterSize(30);
        p_runlauncher.setStringKey("fr_option1");
        p_runlauncher.setColor(sf::Color::White);
        p_runlauncher.setPosition(clicky1.getPosition().x*3+112, clicky1.getPosition().y*3-9);

        p_shortcut.setFont(font);
        p_shortcut.setCharacterSize(30);
        p_shortcut.setStringKey("fr_option2");
        p_shortcut.setColor(sf::Color::White);
        p_shortcut.setPosition(clicky2.getPosition().x*3+112, clicky2.getPosition().y*3-9);

        b_next.setFont(font);
        b_next.setCharacterSize(50);
        b_next.setStringKey("fr_finish");
        b_next.setOrigin(b_next.getLocalBounds().width/2, b_next.getLocalBounds().height/2);
        b_next.setPosition(3840/2, 1700);

        if(((mouse.x > clicky1.getPosition().x*3) && (mouse.x < clicky1.getPosition().x*3 + 96) && (mouse.y > clicky1.getPosition().y*3) && (mouse.y < clicky1.getPosition().y*3 + 96)) || ((mouse.x > p_runlauncher.getPosition().x) && (mouse.x < p_runlauncher.getPosition().x + p_runlauncher.getLocalBounds().width) && (mouse.y > p_runlauncher.getPosition().y) && (mouse.y < p_runlauncher.getPosition().y + p_runlauncher.getLocalBounds().height)))
        {
            p_runlauncher.setColor(sf::Color(255, 192, 64));

            if(mouseCtrl->getClick(0))
            {
                opt1 = !opt1;
            }
        }

        if(((mouse.x > clicky2.getPosition().x*3) && (mouse.x < clicky2.getPosition().x*3 + 96) && (mouse.y > clicky2.getPosition().y*3) && (mouse.y < clicky2.getPosition().y*3 + 96)) || ((mouse.x > p_shortcut.getPosition().x) && (mouse.x < p_shortcut.getPosition().x + p_shortcut.getLocalBounds().width) && (mouse.y > p_shortcut.getPosition().y) && (mouse.y < p_shortcut.getPosition().y + p_shortcut.getLocalBounds().height)))
        {
            p_shortcut.setColor(sf::Color(255, 192, 64));

            if(mouseCtrl->getClick(0))
            {
                opt2 = !opt2;
            }
        }

        if(opt1)
            p_runlauncher.setColor(sf::Color(0, 192, 0));
        if(opt2)
            p_shortcut.setColor(sf::Color(0, 192, 0));

        p_runlauncher.draw();
        p_shortcut.draw();

        auto pos = b_next.getPosition();
        auto lb = b_next.getLocalBounds();

        if((mouse.x > pos.x - lb.width/2) && (mouse.x < pos.x + lb.width/2) && (mouse.y > pos.y - lb.height/2) && (mouse.y < pos.y + lb.height/2))
        {
            b_next.setColor(sf::Color(0, 192, 0));

            if(mouseCtrl->getClick(0))
            {
                speed = 0.002;
                a_state = 21;
                a_clock.restart();

                pon_x_d = 640;
                pon_y_d = 360;

                r_head_d = 62*50;
                r_white_d = 41*50;
                r_pupil_d = 27*50;

                messageclouds.clear();
            }
        }
        else
        {
            b_next.setColor(sf::Color::White);
        }

        b_next.draw();
    }

    if(a_state == 21 && a_clock.getElapsedTime().asMilliseconds() > 2500)
    {
        CoreManager::getInstance().getCore()->close_window = true;

        if(opt2)
        {
            #if defined(_WIN32)
                Func::CreateDesktopShortcut(gamePath+"/Patafour.exe", "Patafour Launcher");
            #elif defined(__linux__) || defined(__APPLE__)
                Func::CreateDesktopShortcut(gamePath+"/Patafour", "Patafour Launcher");
            #endif
        }

        if(opt1)
        {
            std::filesystem::path p = gamePath;
            p /= worker->launcherExecName;
            Func::RunExecutable(p.string(), {});
        }

        a_state = 22;
    }
}