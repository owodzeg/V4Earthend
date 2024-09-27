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

    while(std::getline(langFile, line))
    {
        std::vector<std::string> param = Func::Split(line, ',');
        std::string code = param[0];
        std::string name = param[1];
        std::string font = param[2];

        strRepo->LoadLanguageFile(code, name, "resources/lang/"+code+"/"+code+".txt");
        strRepo->langToFontMapping[code] = font;

        SPDLOG_INFO("Loaded language {} {}, font file: {}", code, name, font);
    }

    std::ifstream langConfig("resources/lang.txt");
    std::string buffer;
    std::getline(langConfig, buffer);
    strRepo->SetCurrentLanguage(buffer);

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
    sword.load("resources/graphics/ui/sword.png");
    pedestal.load("resources/graphics/ui/pedestal.png");

    logo_x_d = 3840/2;
    logo_y_d = 2160/2;

    logo_x_c = logo_x_d;
    logo_y_c = logo_y_d;

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);

    pon_greet.LoadConfig("resources/units/patapon.zip");
    pon_greet.setAnimation("Units_Patapon_Dance_3");
    pon_greet.setGlobalPosition(sf::Vector2f(cam_placement*3 + 220, 1590));

    if(localTime->tm_hour >= 5 && localTime->tm_hour <= 9)
    {
        bg.Load("earthend_1");

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("ln_greeting1", true);
        messageclouds.push_back(tmp);
    }

    if(localTime->tm_hour >= 10 && localTime->tm_hour <= 16)
    {
        bg.Load("earthend_2");
        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("ln_greeting2", true);
        messageclouds.push_back(tmp);
    }

    if(localTime->tm_hour >= 17 && localTime->tm_hour <= 19)
    {
        bg.Load("earthend_3");

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("ln_greeting3", true);
        messageclouds.push_back(tmp);
    }

    if(localTime->tm_hour >= 20 && localTime->tm_hour <= 22)
    {
        bg.Load("earthend_4");

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("ln_greeting4", true);
        messageclouds.push_back(tmp);
    }

    if(localTime->tm_hour >= 23 || localTime->tm_hour <= 4)
    {
        bg.Load("earthend_5");

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+320, pon_greet.getGlobalPosition().y+45), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("ln_greeting5", true);
        messageclouds.push_back(tmp);

        pon_greet.setAnimation("Units_Patapon_Sleep");
        pon_greet.setAnimationSpeed(0); // workaround for lack of animation stalling
        pon_greet.setGlobalPosition(sf::Vector2f(cam_placement*3 + 230, 1612));
    }

    background.setSize(sf::Vector2f(1280, 720));
    background.setFillColor(sf::Color(0,0,0,128));

    pon_menu1.LoadConfig("resources/units/patapon.zip");
    pon_menu1.setAnimation("Units_Patapon_Idle_1");
    pon_menu2.LoadConfig("resources/units/patapon.zip");
    pon_menu2.setAnimation("Units_Patapon_Idle_1");
    pon_menu3.LoadConfig("resources/units/patapon.zip");
    pon_menu3.setAnimation("Units_Patapon_Idle_1");

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

    MouseController* mouseCtrl = CoreManager::getInstance().getMouseController();
    auto mouse = sf::Vector2i(mouseCtrl->getMousePos().x*3, mouseCtrl->getMousePos().y*3);

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
    Func::smoothTransition(logo_x_c, logo_x_d, speed_delta);
    Func::smoothTransition(logo_y_c, logo_y_d, speed_delta);

    auto strRepo = CoreManager::getInstance().getStrRepo();
    auto font = strRepo->GetFontNameForLanguage(strRepo->GetCurrentLanguage());

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

    entry_camera.Work(window->getView());
    bg.Draw(bg_camera);
    if(a_state <= 4)
        window->draw(background);

    if(a_state == 2 && a_clock.getElapsedTime().asMilliseconds() > 1000)
    {
        swordOffset += 10 * speed_delta;
        if(swordOffset > 20)
            swordOffset = -10;

        p_login.setFont(font);
        p_login.setCharacterSize(50);
        p_login.setStringKey("ln_button_login");
        p_login.setOrigin(p_login.getLocalBounds().width/2, p_login.getLocalBounds().height/2);
        p_login.setColor(sf::Color::White);
        p_login.setPosition(3840/2, 1000);

        p_register.setFont(font);
        p_register.setCharacterSize(50);
        p_register.setStringKey("ln_button_register");
        p_register.setOrigin(p_register.getLocalBounds().width/2, p_register.getLocalBounds().height/2);
        p_register.setColor(sf::Color::White);
        p_register.setPosition(3840/2, 1200);

        p_offline.setFont(font);
        p_offline.setCharacterSize(50);
        p_offline.setStringKey("ln_button_playoffline");
        p_offline.setOrigin(p_offline.getLocalBounds().width/2, p_offline.getLocalBounds().height/2);
        p_offline.setColor(sf::Color::White);
        p_offline.setPosition(3840/2, 1400);

        auto pos = p_login.getPosition();
        auto bounds = p_login.getLocalBounds();
        if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y-bounds.height/2 && mouse.y < pos.y+bounds.height/2)
        {
            p_login.setColor(sf::Color::Green);

            sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
            sword.setScale(1,1);
            sword.setPosition(pos.x - bounds.width/2 - 130 - swordOffset, pos.y + 30);
            sword.draw();
            sword.setScale(-1,1);
            sword.setPosition(pos.x + bounds.width/2 + 130 + swordOffset, pos.y + 30);
            sword.draw();

            if(mouseCtrl->getClick(0))
            {
                // login
                a_state = 3;
            }
        }

        pos = p_register.getPosition();
        bounds = p_register.getLocalBounds();
        if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y-bounds.height/2 && mouse.y < pos.y+bounds.height/2)
        {
            p_register.setColor(sf::Color::Green);

            sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
            sword.setScale(1,1);
            sword.setPosition(pos.x - bounds.width/2 - 130 - swordOffset, pos.y + 30);
            sword.draw();
            sword.setScale(-1,1);
            sword.setPosition(pos.x + bounds.width/2 + 130 + swordOffset, pos.y + 30);
            sword.draw();

            if(mouseCtrl->getClick(0))
            {
                // register
                a_state = 4;
            }
        }

        pos = p_offline.getPosition();
        bounds = p_offline.getLocalBounds();
        if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y-bounds.height/2 && mouse.y < pos.y+bounds.height/2)
        {
            p_offline.setColor(sf::Color::Green);

            sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
            sword.setScale(1,1);
            sword.setPosition(pos.x - bounds.width/2 - 130 - swordOffset, pos.y + 30);
            sword.draw();
            sword.setScale(-1,1);
            sword.setPosition(pos.x + bounds.width/2 + 130 + swordOffset, pos.y + 30);
            sword.draw();

            if(mouseCtrl->getClick(0))
            {
                // play offline
                a_state = 10;
                bg_camera.debug_x_dest = cam_placement;
                entry_camera.debug_x_dest = cam_placement;

                logo_x_d = cam_placement*3 + 1425;
                logo_y_d = 460;
                logo_x_c = logo_x_d;
                logo_y_c = logo_y_d;

                background.setSize(sf::Vector2f(322, 720));
                background.setPosition(cam_placement+960, 0);

                CoreManager::getInstance().getGlobals()->set(1, std::string("Guest"));
            }
        }

        p_login.draw();
        p_register.draw();
        p_offline.draw();
    }

    if(a_state <= 4)
    {
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

    if(a_state == 10)
    {
        window->draw(background);

        logo.setOrigin(logo.getGlobalBounds().width/2, logo.getGlobalBounds().height/2);
        logo.setPosition(logo_x_c, logo_y_c);
        logo.draw();

        p_active = 0;

        if(mouse.x > 1100-pedestal.getGlobalBounds().width/2 && mouse.x < 1100+pedestal.getGlobalBounds().width/2 && mouse.y >= 1200 && mouse.y <= 2160)
        {
            p_active = 1;
            if(mouseCtrl->getClick(0))
            {

            }
        }
        if(mouse.x > 1100+650-pedestal.getGlobalBounds().width/2 && mouse.x < 1100+650+pedestal.getGlobalBounds().width/2 && mouse.y >= 1200 && mouse.y <= 2160)
        {
            p_active = 2;
            if(mouseCtrl->getClick(0))
            {

            }
        }
        if(mouse.x > 1100+650+650-pedestal.getGlobalBounds().width/2 && mouse.x < 1100+650+650+pedestal.getGlobalBounds().width/2 && mouse.y >= 1200 && mouse.y <= 2160)
        {
            p_active = 3;
            if(mouseCtrl->getClick(0))
            {
                CoreManager::getInstance().getCore()->close_window = true;
            }
        }

        pedestal.setOrigin(pedestal.getGlobalBounds().width/2, pedestal.getGlobalBounds().height);
        pedestal.setPosition(cam_placement*3 + 1100, 1860);
        pedestal.setColor(sf::Color::White);
        if(p_active == 1) { pedestal.setColor(sf::Color(255, 197, 31)); }
        pedestal.draw();
        pedestal.setPosition(cam_placement*3 + 1100 + 650, 1860);
        pedestal.setColor(sf::Color::White);
        if(p_active == 2) { pedestal.setColor(sf::Color(255, 197, 31)); }
        pedestal.draw();
        pedestal.setPosition(cam_placement*3 + 1100 + 650 * 2, 1860);
        pedestal.setColor(sf::Color::White);
        if(p_active == 3) { pedestal.setColor(sf::Color(255, 197, 31)); }
        pedestal.draw();

        p_news_header.setFont(font);
        p_news_header.setCharacterSize(36);
        p_news_header.setStringKey("ln_header1");
        p_news_header.setOrigin(p_news_header.getLocalBounds().width/2, p_news_header.getLocalBounds().height/2);
        p_news_header.setPosition(cam_placement*3 + 3360, 90);
        p_news_header.draw();

        auto pos = p_news_header.getPosition();
        auto bounds = p_news_header.getGlobalBounds();

        sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
        sword.setScale(1,1);
        sword.setPosition(pos.x - bounds.width*3/2 - sword.getGlobalBounds().width/2, pos.y + 25);
        sword.draw();
        sword.setScale(-1,1);
        sword.setPosition(pos.x + bounds.width*3/2 + sword.getGlobalBounds().width/2, pos.y + 25);
        sword.draw();

        p_news.setFont(font);
        p_news.setCharacterSize(22);
        p_news.setString("{color 255 255 255}Offline mode enabled - couldn't{n}connect to the news server.");
        p_news.setPosition(cam_placement*3 + 2900, 180);
        p_news.draw();

        p_play.setFont(font);
        p_play.setCharacterSize(36);
        p_play.setStringKey("ln_button_play");
        p_play.setOrigin(p_play.getLocalBounds().width/2, p_play.getLocalBounds().height/2);
        p_play.setPosition(cam_placement*3 + 1100, 1900);
        p_play.setColor(sf::Color(255, 255, 255));
        if(p_active == 1) { p_play.setColor(sf::Color(255, 197, 31)); }
        p_play.draw();

        p_settings.setFont(font);
        p_settings.setCharacterSize(36);
        p_settings.setStringKey("ln_button_settings");
        p_settings.setOrigin(p_settings.getLocalBounds().width/2, p_settings.getLocalBounds().height/2);
        p_settings.setPosition(cam_placement*3 + 1100 + 650, 1900);
        p_settings.setColor(sf::Color(255, 255, 255));
        if(p_active == 2) { p_settings.setColor(sf::Color(255, 197, 31)); }
        p_settings.draw();

        p_exit.setFont(font);
        p_exit.setCharacterSize(36);
        p_exit.setStringKey("ln_button_exit");
        p_exit.setOrigin(p_exit.getLocalBounds().width/2, p_exit.getLocalBounds().height/2);
        p_exit.setPosition(cam_placement*3 + 1100 + 650*2, 1900);
        p_exit.setColor(sf::Color(255, 255, 255));
        if(p_active == 3) { p_exit.setColor(sf::Color(255, 197, 31)); }
        p_exit.draw();

        pon_menu1.setAnimation("Units_Patapon_Idle_1");
        pon_menu2.setAnimation("Units_Patapon_Idle_1");
        pon_menu3.setAnimation("Units_Patapon_Idle_1");
        pon_menu1.setGlobalPosition(sf::Vector2f(cam_placement*3 + 920, 1310));
        pon_menu2.setGlobalPosition(sf::Vector2f(cam_placement*3 + 920 + 650, 1310));
        pon_menu3.setGlobalPosition(sf::Vector2f(cam_placement*3 + 920 + 650*2, 1310));

        if(p_active == 1)
        {
            pon_menu1.setAnimation("Units_Patapon_Walk");
            pon_menu1.setGlobalPosition(sf::Vector2f(cam_placement*3 + 968, 1364));
        }
        if(p_active == 2)
        {
            pon_menu2.setAnimation("Units_Patapon_Walk");
            pon_menu2.setGlobalPosition(sf::Vector2f(cam_placement*3 + 968 + 650, 1364));
        }
        if(p_active == 3)
        {
            pon_menu3.setAnimation("Units_Patapon_Walk");
            pon_menu3.setGlobalPosition(sf::Vector2f(cam_placement*3 + 968 + 650*2, 1364));
        }

        pon_greet.Draw();
        pon_menu1.Draw();
        pon_menu2.Draw();
        pon_menu3.Draw();

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

    }
}