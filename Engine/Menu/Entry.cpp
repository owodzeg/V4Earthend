#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "Entry.h"
#include "../Func.h"
#include "CoreManager.h"
#include <fstream>
#include <spdlog/spdlog.h>

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

    pon_greet.setGlobalPosition(sf::Vector2f(cam_placement*3 + 220, 1590));

    if(localTime->tm_hour >= 5 && localTime->tm_hour <= 8)
    {
        bg.Load("earthend_1");

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("ln_greeting1", true);
        messageclouds.push_back(tmp);

        day_state = 1;
    }

    if(localTime->tm_hour >= 9 && localTime->tm_hour <= 16)
    {
        bg.Load("earthend_2");
        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("ln_greeting2", true);
        messageclouds.push_back(tmp);

        day_state = 2;
    }

    if(localTime->tm_hour >= 17 && localTime->tm_hour <= 19)
    {
        bg.Load("earthend_3");

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("ln_greeting3", true);
        messageclouds.push_back(tmp);

        day_state = 3;
    }

    if(localTime->tm_hour >= 20 && localTime->tm_hour <= 22)
    {
        bg.Load("earthend_4");

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("ln_greeting4", true);
        messageclouds.push_back(tmp);

        day_state = 4;
    }

    if(localTime->tm_hour >= 23 || localTime->tm_hour <= 4)
    {
        bg.Load("earthend_5");

        MessageCloud tmp;
        tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+320, pon_greet.getGlobalPosition().y+45), sf::Color(255, 255, 255, 255), false, 3);
        tmp.msgcloud_ID = 0;
        tmp.AddDialog("ln_greeting5", true);
        messageclouds.push_back(tmp);

        pon_greet.setAnimationSpeed(0); // workaround for lack of animation stalling
        pon_greet.setGlobalPosition(sf::Vector2f(cam_placement*3 + 230, 1612));

        day_state = 5;
    }

    background.setSize(sf::Vector2f(1280, 720));
    background.setFillColor(sf::Color(0,0,0,128));

    ib_login.load("resources/graphics/ui/inputbox.png");
    ib_email.load("resources/graphics/ui/inputbox.png");
    ib_password.load("resources/graphics/ui/inputbox.png");

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

    if(a_state == -1 && a_clock.getElapsedTime().asMilliseconds() > 1500)
    {
        speed = 0.02;

        r_head_d = 62*0.75;
        r_white_d = 41*0.75;
        r_pupil_d = 27*0.75;

        pon_x_offset_d = 56;
        pon_y_offset_d = 15;

        a_state = -2;
        a_clock.restart();
    }

    if(a_state == -2 && a_clock.getElapsedTime().asMilliseconds() > 2500)
    {
        worker->setAction(Worker::LOAD_UNITS);
        a_state = 0;
        a_clock.restart();
    }

    if(a_state == 0 && a_clock.getElapsedTime().asMilliseconds() > 2500)
    {
        if(!worker->isBusy())
        {
            speed = 0.015;

            pon_y_d -= 200;
            logo_y_d -= 600;

            a_state = 1;
            a_clock.restart();
        }
        else
        {
            std::string loadText = "";
            for(int i=0; i<=int(floor(a_clock.getElapsedTime().asSeconds())) % 3; i++)
                loadText += ".";

            debugtext.setFont(font);
            debugtext.setCharacterSize(30);
            debugtext.setString("{color 255 255 255}{outline 0 0 0 2}"+loadText);
            debugtext.setOrigin(debugtext.getLocalBounds().width/2, debugtext.getLocalBounds().height/2);
            debugtext.setColor(sf::Color::White);
            debugtext.setPosition(3840/2, 1500);
        }
    }

    if(a_state == 1)
    {
        pon_greet.setAnimation("Units_Patapon_Dance_3");
        if(day_state == 5)
            pon_greet.setAnimation("Units_Patapon_Sleep");
        pon_menu1.setAnimation("Units_Patapon_Idle_1");
        pon_menu2.setAnimation("Units_Patapon_Idle_1");
        pon_menu3.setAnimation("Units_Patapon_Idle_1");
        pon_menu4.setAnimation("Units_Patapon_Idle_1");

        std::ifstream lastLogin("loginDetails.dat");
        if(lastLogin.good())
        {
            a_state = 3;
            worker->setAction(Worker::LOGIN);
        }
        else
        {
            a_state = 2;
        }
    }

    entry_camera.Work(window->getView());
    bg.Draw(bg_camera);
    if(a_state <= 4)
        window->draw(background);

    if(a_state == 0)
        debugtext.draw();

    if((a_state >= 2 && a_state <= 4) || a_state == 17)
    {
        swordOffset += 10 * speed_delta;
        if(swordOffset > 20)
            swordOffset = -10;
    }

    if(a_state == 2 && dialogboxes.size() == 0)
    {
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
                whichInputBox = 0;
                str_login = "";
                str_password = "";
                CoreManager::getInstance().getTextInputController()->latchOn(str_login);
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
                whichInputBox = 0;
                str_login = "";
                str_email = "";
                str_password = "";
                CoreManager::getInstance().getTextInputController()->latchOn(str_login);
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
                PataDialogBox newdb;
                newdb.Create(font, "ln_offline_warning", {"ln_option_yes", "ln_option_no"}, 3, 1);
                newdb.id = 0;
                dialogboxes.push_back(newdb);
            }
        }

        p_login.draw();
        p_register.draw();
        p_offline.draw();
    }

    // LOGIN
    if(a_state == 3)
    {
        if(!worker->isBusy() && worker->rtn == 1)
        {
            worker->rtn = -1;
            a_state = 10;

            bg_camera.debug_x_dest = cam_placement;
            entry_camera.debug_x_dest = cam_placement;

            logo_x_d = cam_placement*3 + 1425;
            logo_y_d = 460;
            logo_x_c = logo_x_d;
            logo_y_c = logo_y_d;

            background.setSize(sf::Vector2f(322, 720));
            background.setPosition(cam_placement+960, 0);
        }

        if(!worker->isBusy() && worker->rtn == 100)
        {
            PataDialogBox newdb;
            newdb.Create(font, "ln_error1", {"ln_option_understood"}, 3, 2);
            newdb.id = 4;
            dialogboxes.push_back(newdb);

            worker->rtn = -1;
        }

        if(!worker->isBusy() && worker->rtn == 101)
        {
            PataDialogBox newdb;
            newdb.Create(font, "ln_error3", {"ln_option_understood"}, 3, 2);
            newdb.id = 4;
            dialogboxes.push_back(newdb);

            worker->rtn = -1;
        }

        if(!worker->isBusy() && worker->rtn == 102)
        {
            PataDialogBox newdb;
            newdb.Create(font, "ln_error5", {"ln_option_understood"}, 3, 2);
            newdb.id = 4;
            dialogboxes.push_back(newdb);

            worker->rtn = -1;
        }

        if(!worker->isBusy() && dialogboxes.size() == 0)
        {
            b_login.setFont(font);
            b_login.setCharacterSize(36);
            b_login.setStringKey("ln_button_login");
            b_login.setOrigin(b_login.getLocalBounds().width/2, b_login.getLocalBounds().height/2);
            b_login.setPosition(3840/2, 1400);
            b_login.setColor(sf::Color::White);

            b_goback.setFont(font);
            b_goback.setCharacterSize(36);
            b_goback.setStringKey("ln_settings_button2");
            b_goback.setOrigin(b_goback.getLocalBounds().width/2, b_goback.getLocalBounds().height/2);
            b_goback.setPosition(3840/2, 1540);
            b_goback.setColor(sf::Color::White);

            auto pos = b_login.getPosition();
            auto bounds = b_login.getLocalBounds();
            if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y-bounds.height/2 && mouse.y < pos.y+bounds.height/2)
            {
                b_login.setColor(sf::Color::Green);

                sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
                sword.setScale(1,1);
                sword.setPosition(pos.x - bounds.width/2 - 130 - swordOffset, pos.y + 30);
                sword.draw();
                sword.setScale(-1,1);
                sword.setPosition(pos.x + bounds.width/2 + 130 + swordOffset, pos.y + 30);
                sword.draw();

                if(mouseCtrl->getClick(0) && !worker->isBusy())
                {
                    worker->setAction(Worker::LOGIN);
                }
            }

            pos = b_goback.getPosition();
            bounds = b_goback.getLocalBounds();
            if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y-bounds.height/2 && mouse.y < pos.y+bounds.height/2)
            {
                b_goback.setColor(sf::Color::Green);

                sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
                sword.setScale(1,1);
                sword.setPosition(pos.x - bounds.width/2 - 130 - swordOffset, pos.y + 30);
                sword.draw();
                sword.setScale(-1,1);
                sword.setPosition(pos.x + bounds.width/2 + 130 + swordOffset, pos.y + 30);
                sword.draw();

                if(mouseCtrl->getClick(0))
                {
                    a_state = 2;
                }
            }

            b_login.draw();
            b_goback.draw();

            ib_login.setOrigin(ib_login.getLocalBounds().width/2, ib_login.getLocalBounds().height/2);
            ib_login.setPosition(3840/2, 1000);
            ib_login.setColor(sf::Color::White);
            if(whichInputBox == 0)
            {
                ib_login.setColor(sf::Color(222, 255, 227, 255));
            }
            ib_login.draw();
            ib_password.setOrigin(ib_password.getLocalBounds().width/2, ib_password.getLocalBounds().height/2);
            ib_password.setPosition(3840/2, 1220);
            ib_password.setColor(sf::Color::White);
            if(whichInputBox == 1)
            {
                ib_password.setColor(sf::Color(222, 255, 227, 255));
            }
            ib_password.draw();

            sf::String cursor = "";
            if(int(floor((a_clock.getElapsedTime().asSeconds()*2))) % 2 == 0)
                cursor += "|";

            auto inputCtrl = CoreManager::getInstance().getInputController();
            auto textCtrl = CoreManager::getInstance().getTextInputController();

            if(inputCtrl->isKeyPressed(Input::Keys::UP))
            {
                whichInputBox = 0;
                CoreManager::getInstance().getTextInputController()->latchOn(str_login);

            }
            if(inputCtrl->isKeyPressed(Input::Keys::DOWN) || textCtrl->sendSpecial() == 1)
            {
                whichInputBox = 1;
                CoreManager::getInstance().getTextInputController()->latchOn(str_password);
            }

            if(whichInputBox == 0)
                CoreManager::getInstance().getTextInputController()->latchOn(str_login);

            if(whichInputBox == 1)
                CoreManager::getInstance().getTextInputController()->latchOn(str_password);

            ib_t_login.setFont(font);
            ib_t_login.setCharacterSize(30);
            ib_t_login.setString(str_login);
            if(whichInputBox == 0)
                ib_t_login.setString(str_login+cursor);
            ib_t_login.setOrigin(0, ib_t_login.getLocalBounds().height/2);
            ib_t_login.setPosition(ib_login.getPosition().x - ib_login.getLocalBounds().width/2 + 42, ib_login.getPosition().y - 12);
            ib_t_login.setColor(sf::Color::White);
            ib_t_login.draw();

            sf::String password_hidden;
            for(auto x:str_password)
            {
                password_hidden += "*";
            }

            ib_t_password.setFont(font);
            ib_t_password.setCharacterSize(30);
            ib_t_password.setString(password_hidden);
            if(whichInputBox == 1)
                ib_t_password.setString(password_hidden+cursor);
            ib_t_password.setOrigin(0, ib_t_password.getLocalBounds().height/2);
            ib_t_password.setPosition(ib_password.getPosition().x - ib_password.getLocalBounds().width/2 + 42, ib_password.getPosition().y - 12);
            ib_t_password.setColor(sf::Color::White);
            ib_t_password.draw();

            sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
            sword.setScale(-1,1);
            sword.setPosition(ib_login.getPosition().x + ib_login.getGlobalBounds().width/2 + 120 + swordOffset, ib_login.getPosition().y + 220*whichInputBox);
            sword.draw();

            pos = ib_login.getPosition();
            bounds = ib_login.getLocalBounds();

            if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y - bounds.height/2 && mouse.y < pos.y + bounds.height/2)
            {
                if(mouseCtrl->getClick(0))
                {
                    whichInputBox = 0;
                }
            }

            pos = ib_password.getPosition();
            bounds = ib_password.getLocalBounds();

            if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y - bounds.height/2 && mouse.y < pos.y + bounds.height/2)
            {
                if(mouseCtrl->getClick(0))
                {
                    whichInputBox = 1;
                }
            }

            p_username.setFont(font);
            p_username.setCharacterSize(26);
            p_username.setStringKey("ln_username");
            p_username.setOrigin(p_username.getLocalBounds().width, p_username.getLocalBounds().height/2);
            p_username.setPosition(ib_login.getPosition().x - ib_login.getLocalBounds().width/2 - 42, ib_login.getPosition().y - 6);
            p_username.setColor(sf::Color::White);
            p_username.draw();
            p_password.setFont(font);
            p_password.setCharacterSize(26);
            p_password.setStringKey("ln_password");
            p_password.setOrigin(p_password.getLocalBounds().width, p_password.getLocalBounds().height/2);
            p_password.setPosition(ib_password.getPosition().x - ib_password.getLocalBounds().width/2 - 42, ib_password.getPosition().y - 6);
            p_password.setColor(sf::Color::White);
            p_password.draw();
        }
    }

    // REGISTER
    if(a_state == 4 && dialogboxes.size() == 0)
    {
        if(!worker->isBusy() && worker->rtn == 2)
        {
            worker->rtn = -1;

            PataDialogBox newdb;
            newdb.Create(font, "ln_info1", {"ln_option_understood"}, 3, 1);
            newdb.id = 2;
            dialogboxes.push_back(newdb);
        }

        if(!worker->isBusy() && worker->rtn == 100)
        {
            PataDialogBox newdb;
            newdb.Create(font, "ln_error1", {"ln_option_understood"}, 3, 2);
            newdb.id = 4;
            dialogboxes.push_back(newdb);

            worker->rtn = -1;
        }

        if(!worker->isBusy() && worker->rtn == 101)
        {
            PataDialogBox newdb;
            newdb.Create(font, "ln_error3", {"ln_option_understood"}, 3, 2);
            newdb.id = 4;
            dialogboxes.push_back(newdb);

            worker->rtn = -1;
        }

        if(!worker->isBusy() && worker->rtn == 102)
        {
            PataDialogBox newdb;
            newdb.Create(font, "ln_error5", {"ln_option_understood"}, 3, 2);
            newdb.id = 4;
            dialogboxes.push_back(newdb);

            worker->rtn = -1;
        }

        if(!worker->isBusy())
        {
            b_register.setFont(font);
            b_register.setCharacterSize(36);
            b_register.setStringKey("ln_button_register");
            b_register.setOrigin(b_register.getLocalBounds().width/2, b_register.getLocalBounds().height/2);
            b_register.setPosition(3840/2, 1560);
            b_register.setColor(sf::Color::White);

            b_goback.setFont(font);
            b_goback.setCharacterSize(36);
            b_goback.setStringKey("ln_settings_button2");
            b_goback.setOrigin(b_goback.getLocalBounds().width/2, b_goback.getLocalBounds().height/2);
            b_goback.setPosition(3840/2, 1700);
            b_goback.setColor(sf::Color::White);

            auto pos = b_register.getPosition();
            auto bounds = b_register.getLocalBounds();
            if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y-bounds.height/2 && mouse.y < pos.y+bounds.height/2)
            {
                b_register.setColor(sf::Color::Green);
                sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
                sword.setScale(1,1);
                sword.setPosition(pos.x - bounds.width/2 - 130 - swordOffset, pos.y + 30);
                sword.draw();
                sword.setScale(-1,1);
                sword.setPosition(pos.x + bounds.width/2 + 130 + swordOffset, pos.y + 30);
                sword.draw();

                if(mouseCtrl->getClick(0) && !worker->isBusy())
                {
                    worker->setAction(Worker::REGISTER);
                }
            }

            pos = b_goback.getPosition();
            bounds = b_goback.getLocalBounds();
            if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y-bounds.height/2 && mouse.y < pos.y+bounds.height/2)
            {
                b_goback.setColor(sf::Color::Green);
                sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
                sword.setScale(1,1);
                sword.setPosition(pos.x - bounds.width/2 - 130 - swordOffset, pos.y + 30);
                sword.draw();
                sword.setScale(-1,1);
                sword.setPosition(pos.x + bounds.width/2 + 130 + swordOffset, pos.y + 30);
                sword.draw();

                if(mouseCtrl->getClick(0))
                {
                    a_state = 2;
                }
            }

            b_register.draw();
            b_goback.draw();

            ib_login.setOrigin(ib_login.getLocalBounds().width/2, ib_login.getLocalBounds().height/2);
            ib_login.setPosition(3840/2, 900);
            ib_login.setColor(sf::Color::White);
            if(whichInputBox == 0)
            {
                ib_login.setColor(sf::Color(222, 255, 227, 255));
            }
            ib_login.draw();
            ib_email.setOrigin(ib_email.getLocalBounds().width/2, ib_email.getLocalBounds().height/2);
            ib_email.setPosition(3840/2, 1120);
            ib_email.setColor(sf::Color::White);
            if(whichInputBox == 1)
            {
                ib_email.setColor(sf::Color(222, 255, 227, 255));
            }
            ib_email.draw();
            ib_password.setOrigin(ib_password.getLocalBounds().width/2, ib_password.getLocalBounds().height/2);
            ib_password.setPosition(3840/2, 1340);
            ib_password.setColor(sf::Color::White);
            if(whichInputBox == 2)
            {
                ib_password.setColor(sf::Color(222, 255, 227, 255));
            }
            ib_password.draw();

            sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
            sword.setScale(-1,1);
            sword.setPosition(ib_login.getPosition().x + ib_login.getGlobalBounds().width/2 + 120 + swordOffset, ib_login.getPosition().y + 220*whichInputBox);
            sword.draw();

            sf::String cursor = "";
            if(int(floor((a_clock.getElapsedTime().asSeconds()*2))) % 2 == 0)
                cursor += "|";

            auto inputCtrl = CoreManager::getInstance().getInputController();
            auto textCtrl = CoreManager::getInstance().getTextInputController();

            if(inputCtrl->isKeyPressed(Input::Keys::UP))
            {
                if(whichInputBox>0)
                    whichInputBox--;
            }
            if(inputCtrl->isKeyPressed(Input::Keys::DOWN) || textCtrl->sendSpecial() == 1)
            {
                if(whichInputBox<2)
                    whichInputBox++;
            }

            if(whichInputBox == 0)
                CoreManager::getInstance().getTextInputController()->latchOn(str_login);

            if(whichInputBox == 1)
                CoreManager::getInstance().getTextInputController()->latchOn(str_email);

            if(whichInputBox == 2)
                CoreManager::getInstance().getTextInputController()->latchOn(str_password);

            ib_t_login.setFont(font);
            ib_t_login.setCharacterSize(30);
            ib_t_login.setString(str_login);
            if(whichInputBox == 0)
                ib_t_login.setString(str_login+cursor);
            ib_t_login.setOrigin(0, ib_t_login.getLocalBounds().height/2);
            ib_t_login.setPosition(ib_login.getPosition().x - ib_login.getLocalBounds().width/2 + 42, ib_login.getPosition().y - 12);
            ib_t_login.setColor(sf::Color::White);
            ib_t_login.draw();

            ib_t_email.setFont(font);
            ib_t_email.setCharacterSize(30);
            ib_t_email.setString(str_email);
            if(whichInputBox == 1)
                ib_t_email.setString(str_email+cursor);
            ib_t_email.setOrigin(0, ib_t_email.getLocalBounds().height/2);
            ib_t_email.setPosition(ib_email.getPosition().x - ib_email.getLocalBounds().width/2 + 42, ib_email.getPosition().y - 12);
            ib_t_email.setColor(sf::Color::White);
            ib_t_email.draw();

            sf::String password_hidden;
            for(auto x:str_password)
            {
                password_hidden += "*";
            }

            ib_t_password.setFont(font);
            ib_t_password.setCharacterSize(30);
            ib_t_password.setString(password_hidden);
            if(whichInputBox == 2)
                ib_t_password.setString(password_hidden+cursor);
            ib_t_password.setOrigin(0, ib_t_password.getLocalBounds().height/2);
            ib_t_password.setPosition(ib_password.getPosition().x - ib_password.getLocalBounds().width/2 + 42, ib_password.getPosition().y - 12);
            ib_t_password.setColor(sf::Color::White);
            ib_t_password.draw();

            pos = ib_login.getPosition();
            bounds = ib_login.getLocalBounds();

            if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y - bounds.height/2 && mouse.y < pos.y + bounds.height/2)
            {
                if(mouseCtrl->getClick(0))
                {
                    whichInputBox = 0;
                }
            }

            pos = ib_email.getPosition();
            bounds = ib_email.getLocalBounds();

            if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y - bounds.height/2 && mouse.y < pos.y + bounds.height/2)
            {
                if(mouseCtrl->getClick(0))
                {
                    whichInputBox = 1;
                }
            }

            pos = ib_password.getPosition();
            bounds = ib_password.getLocalBounds();

            if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y - bounds.height/2 && mouse.y < pos.y + bounds.height/2)
            {
                if(mouseCtrl->getClick(0))
                {
                    whichInputBox = 2;
                }
            }

            p_username.setFont(font);
            p_username.setCharacterSize(26);
            p_username.setStringKey("ln_username");
            p_username.setOrigin(p_username.getLocalBounds().width, p_username.getLocalBounds().height/2);
            p_username.setPosition(ib_login.getPosition().x - ib_login.getLocalBounds().width/2 - 30, ib_login.getPosition().y - 6);
            p_username.setColor(sf::Color::White);
            p_username.draw();
            p_email.setFont(font);
            p_email.setCharacterSize(26);
            p_email.setStringKey("ln_email");
            p_email.setOrigin(p_email.getLocalBounds().width, p_email.getLocalBounds().height/2);
            p_email.setPosition(ib_email.getPosition().x - ib_email.getLocalBounds().width/2 - 30, ib_email.getPosition().y - 6);
            p_email.setColor(sf::Color::White);
            p_email.draw();
            p_password.setFont(font);
            p_password.setCharacterSize(26);
            p_password.setStringKey("ln_password");
            p_password.setOrigin(p_password.getLocalBounds().width, p_password.getLocalBounds().height/2);
            p_password.setPosition(ib_password.getPosition().x - ib_password.getLocalBounds().width/2 - 30, ib_password.getPosition().y - 6);
            p_password.setColor(sf::Color::White);
            p_password.draw();
        }
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
        h1.clear();
        h1.setPrimitiveType(sf::TriangleStrip);
        h1.append(sf::Vertex(sf::Vector2f(273+cam_placement, 440), sf::Color(255,255,255,0)));
        h1.append(sf::Vertex(sf::Vector2f(460+cam_placement, 440), sf::Color(255,255,255,0)));
        h1.append(sf::Vertex(sf::Vector2f(273+cam_placement, 610), sf::Color(255,255,255,32)));
        h1.append(sf::Vertex(sf::Vector2f(460+cam_placement, 610), sf::Color(255,255,255,32)));

        h2.clear();
        h2.setPrimitiveType(sf::TriangleStrip);
        h2.append(sf::Vertex(sf::Vector2f(272+cam_placement + 217, 440), sf::Color(255,255,255,0)));
        h2.append(sf::Vertex(sf::Vector2f(460+cam_placement + 217, 440), sf::Color(255,255,255,0)));
        h2.append(sf::Vertex(sf::Vector2f(272+cam_placement + 217, 610), sf::Color(255,255,255,32)));
        h2.append(sf::Vertex(sf::Vector2f(460+cam_placement + 217, 610), sf::Color(255,255,255,32)));

        h3.clear();
        h3.setPrimitiveType(sf::TriangleStrip);
        h3.append(sf::Vertex(sf::Vector2f(272+cam_placement + 217 + 217, 440), sf::Color(255,255,255,0)));
        h3.append(sf::Vertex(sf::Vector2f(459+cam_placement + 217 + 217, 440), sf::Color(255,255,255,0)));
        h3.append(sf::Vertex(sf::Vector2f(272+cam_placement + 217 + 217, 610), sf::Color(255,255,255,32)));
        h3.append(sf::Vertex(sf::Vector2f(459+cam_placement + 217 + 217, 610), sf::Color(255,255,255,32)));

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
                a_state = 11;
            }
        }
        if(mouse.x > 1100+650-pedestal.getGlobalBounds().width/2 && mouse.x < 1100+680+pedestal.getGlobalBounds().width/2 && mouse.y >= 1200 && mouse.y <= 2160)
        {
            p_active = 2;
            if(mouseCtrl->getClick(0))
            {
                a_state = 16;

                cam_placement = 15000;

                pon_menu1.setGlobalPosition(sf::Vector2f(cam_placement*3 + 323, 1310));
                pon_menu2.setGlobalPosition(sf::Vector2f(cam_placement*3 + 323 + 710, 1310));
                pon_menu3.setGlobalPosition(sf::Vector2f(cam_placement*3 + 323 + 710*2, 1310));
                pon_menu4.setGlobalPosition(sf::Vector2f(cam_placement*3 + 323 + 710*3, 1310));
                pon_menu5.setGlobalPosition(sf::Vector2f(cam_placement*3 + 323 + 710*4, 1310));

                messageclouds.clear();

                MessageCloud tmp1;
                tmp1.setFontSize(18);
                tmp1.Create(20, sf::Vector2f(pon_menu1.getGlobalPosition().x+280, pon_menu1.getGlobalPosition().y+30), sf::Color(255, 255, 255, 255), false, 3);
                tmp1.msgcloud_ID = 0;
                tmp1.AddDialog("ln_settings1", true);
                messageclouds.push_back(tmp1);

                MessageCloud tmp2;
                tmp2.setFontSize(18);
                tmp2.Create(20, sf::Vector2f(pon_menu2.getGlobalPosition().x+260, pon_menu2.getGlobalPosition().y+30), sf::Color(255, 255, 255, 255), false, 3);
                tmp2.msgcloud_ID = 1;
                tmp2.AddDialog("ln_settings2", true);
                messageclouds.push_back(tmp2);

                MessageCloud tmp3;
                tmp3.setFontSize(18);
                tmp3.Create(20, sf::Vector2f(pon_menu3.getGlobalPosition().x+260, pon_menu3.getGlobalPosition().y+30), sf::Color(255, 255, 255, 255), false, 3);
                tmp3.msgcloud_ID = 2;
                tmp3.AddDialog("ln_settings5", true);
                messageclouds.push_back(tmp3);

                MessageCloud tmp4;
                tmp4.setFontSize(18);
                tmp4.Create(20, sf::Vector2f(pon_menu4.getGlobalPosition().x+240, pon_menu4.getGlobalPosition().y+30), sf::Color(255, 255, 255, 255), false, 3);
                tmp4.msgcloud_ID = 3;
                tmp4.AddDialog("ln_settings4", true);
                messageclouds.push_back(tmp4);

                MessageCloud tmp5;
                tmp5.setFontSize(18);
                tmp5.Create(20, sf::Vector2f(pon_menu5.getGlobalPosition().x+220, pon_menu5.getGlobalPosition().y+30), sf::Color(255, 255, 255, 255), false, 3);
                tmp5.msgcloud_ID = 4;
                tmp5.AddDialog("fr_back", true);
                messageclouds.push_back(tmp5);
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

        if(p_active == 1)
        {
            window->draw(h1);
        }
        if(p_active == 2)
        {
            window->draw(h2);
        }
        if(p_active == 3)
        {
            window->draw(h3);
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

        curBranch.setFont(font);
        curBranch.setCharacterSize(36);
        curBranch.setString("branch: "+worker->branch);
        curBranch.setOrigin(curBranch.getLocalBounds().width/2, curBranch.getLocalBounds().height/2);
        curBranch.setPosition(cam_placement*3 + 3360, 2000);
        curBranch.setColor(sf::Color::White);
        curBranch.draw();

        auto pos = p_news_header.getPosition();
        auto bounds = p_news_header.getGlobalBounds();

        sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
        sword.setScale(1,1);
        sword.setPosition(pos.x - bounds.width*3/2 - sword.getGlobalBounds().width/2 - 10, pos.y + 25);
        sword.draw();
        sword.setScale(-1,1);
        sword.setPosition(pos.x + bounds.width*3/2 + sword.getGlobalBounds().width/2 + 10, pos.y + 25);
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

    if(a_state == 11)
    {
        worker->setAction(Worker::CHECK_HERO_UPDATE);
        a_state = 13;
    }

    if(a_state >= 11 && a_state <= 15)
    {
        cam_placement = 29500;

        logo.setOrigin(logo.getGlobalBounds().width/2, logo.getGlobalBounds().height/2);
        logo.setPosition(logo_x_c, logo_y_c);
        logo.draw();

        entry_camera.debug_x_dest += 60 * speed_delta;
        bg_camera.debug_x_dest += 180 * speed_delta;

        pedestal.setOrigin(pedestal.getGlobalBounds().width/2, pedestal.getGlobalBounds().height);
        pedestal.setPosition(cam_placement*3 + 1100, 1860);
        pedestal.setColor(sf::Color::White);
        pedestal.draw();
        pedestal.setPosition(cam_placement*3 + 1100 + 650, 1860);
        pedestal.setColor(sf::Color::White);
        pedestal.draw();
        pedestal.setPosition(cam_placement*3 + 1100 + 650 * 2, 1860);
        pedestal.setColor(sf::Color::White);
        pedestal.draw();

        pon_menu1.setAnimation("Units_Patapon_Idle_1");
        pon_menu2.setAnimation("Units_Patapon_Idle_1");
        pon_menu3.setAnimation("Units_Patapon_Idle_1");
        pon_menu1.setGlobalPosition(sf::Vector2f(cam_placement*3 + 920, 1310));
        pon_menu2.setGlobalPosition(sf::Vector2f(cam_placement*3 + 920 + 650, 1310));
        pon_menu3.setGlobalPosition(sf::Vector2f(cam_placement*3 + 920 + 650*2, 1310));

        pon_greet.Draw();
        pon_menu1.Draw();
        pon_menu2.Draw();
        pon_menu3.Draw();
    }

    if(a_state == 12)
    {
        if(!worker->isBusy())
        {
            if(worker->error == 1)
            {
                PataDialogBox newdb;
                newdb.Create(font, "ln_error4", {"ln_option_yes", "ln_option_no"}, 3, 1);
                newdb.id = 1;
                dialogboxes.push_back(newdb);

                worker->error = -1; //acknowledged
            }
        }
    }

    // download hero state
    if(a_state == 13 || a_state == 14)
    {
        auto view = window->getView();
        window->setView(window->getDefaultView());

        float sumProg = worker->currentTaskTotal;
        float curProg = 0;

        for(auto x : worker->downloaded_files)
        {
            curProg += x.progress;
        }

        if(a_clock.getElapsedTime().asMilliseconds() > 1000)
        {
            if(!worker->isBusy())
            {
                if(worker->rtn != 1)
                {
                    a_state = 14;
                }
                else
                {
                    if(worker->rtn == 1)
                    {
                        PataDialogBox newdb;
                        newdb.Create(font, "ln_gameupdatefound", {"ln_option_yes", "ln_option_no"}, 3, 1);
                        newdb.id = 3;
                        dialogboxes.push_back(newdb);

                        a_state = 12;
                        worker->rtn = -1;
                    }
                }
            }
        }

        prog_main.setFont(font);
        prog_main.setCharacterSize(50);
        prog_main.setOrigin(prog_main.getLocalBounds().width/2, prog_main.getLocalBounds().height/2);
        prog_main.setPosition(3840/2, 960);
        prog_main.setColor(sf::Color(255, 255, 255));
        prog_main.draw();

        if(worker->downloadStarted)
        {
            prog_supp.setFont(font);
            prog_supp.setCharacterSize(30);
            prog_supp.setOrigin(prog_supp.getLocalBounds().width/2, prog_supp.getLocalBounds().height/2);
            prog_supp.setString("{color 255 255 255}{outline 1 0 0 0}"+std::to_string(int(curProg/1024))+"/"+std::to_string(int(sumProg/1024))+" kB");
            prog_supp.setPosition(3840/2, 1200);
            prog_supp.setColor(sf::Color(255, 255, 255));
            prog_supp.draw();
        }

        window->setView(view);
    }

    if(a_state == 14 && a_clock.getElapsedTime().asMilliseconds() > 2000)
    {
        /// run!
        worker->setAction(Worker::RUN_HERO);
        a_state = 15;
    }

    // SETTINGS!!!
    if(a_state == 16 && dialogboxes.size() == 0)
    {
        h1.clear();
        h1.setPrimitiveType(sf::TriangleStrip);
        h1.append(sf::Vertex(sf::Vector2f(76+cam_placement, 440), sf::Color(255,255,255,0)));
        h1.append(sf::Vertex(sf::Vector2f(263+cam_placement, 440), sf::Color(255,255,255,0)));
        h1.append(sf::Vertex(sf::Vector2f(76+cam_placement, 610), sf::Color(255,255,255,32)));
        h1.append(sf::Vertex(sf::Vector2f(263+cam_placement, 610), sf::Color(255,255,255,32)));

        h2.clear();
        h2.setPrimitiveType(sf::TriangleStrip);
        h2.append(sf::Vertex(sf::Vector2f(74+cam_placement + 236, 440), sf::Color(255,255,255,0)));
        h2.append(sf::Vertex(sf::Vector2f(262+cam_placement + 236, 440), sf::Color(255,255,255,0)));
        h2.append(sf::Vertex(sf::Vector2f(74+cam_placement + 236, 610), sf::Color(255,255,255,32)));
        h2.append(sf::Vertex(sf::Vector2f(262+cam_placement + 236, 610), sf::Color(255,255,255,32)));

        h3.clear();
        h3.setPrimitiveType(sf::TriangleStrip);
        h3.append(sf::Vertex(sf::Vector2f(75+cam_placement + 236*2, 440), sf::Color(255,255,255,0)));
        h3.append(sf::Vertex(sf::Vector2f(262+cam_placement + 236*2, 440), sf::Color(255,255,255,0)));
        h3.append(sf::Vertex(sf::Vector2f(75+cam_placement + 236*2, 610), sf::Color(255,255,255,32)));
        h3.append(sf::Vertex(sf::Vector2f(262+cam_placement + 236*2, 610), sf::Color(255,255,255,32)));

        h4.clear();
        h4.setPrimitiveType(sf::TriangleStrip);
        h4.append(sf::Vertex(sf::Vector2f(76+cam_placement + 236*3, 440), sf::Color(255,255,255,0)));
        h4.append(sf::Vertex(sf::Vector2f(263+cam_placement + 236*3, 440), sf::Color(255,255,255,0)));
        h4.append(sf::Vertex(sf::Vector2f(76+cam_placement + 236*3, 610), sf::Color(255,255,255,32)));
        h4.append(sf::Vertex(sf::Vector2f(263+cam_placement + 236*3, 610), sf::Color(255,255,255,32)));

        h5.clear();
        h5.setPrimitiveType(sf::TriangleStrip);
        h5.append(sf::Vertex(sf::Vector2f(77+cam_placement + 236*4, 440), sf::Color(255,255,255,0)));
        h5.append(sf::Vertex(sf::Vector2f(264+cam_placement + 236*4, 440), sf::Color(255,255,255,0)));
        h5.append(sf::Vertex(sf::Vector2f(77+cam_placement + 236*4, 610), sf::Color(255,255,255,32)));
        h5.append(sf::Vertex(sf::Vector2f(264+cam_placement + 236*4, 610), sf::Color(255,255,255,32)));

        bg_camera.debug_x_dest = cam_placement;
        entry_camera.debug_x_dest = cam_placement;

        p_active = 0;

        if(mouse.x > 503-pedestal.getGlobalBounds().width/2 && mouse.x < 503+pedestal.getGlobalBounds().width/2 && mouse.y >= 1200 && mouse.y <= 2160)
        {
            p_active = 1;
            if(mouseCtrl->getClick(0))
            {

                std::vector<sf::String> text_branches;
                for(auto x:worker->all_branches)
                    text_branches.push_back(sf::String(x));
                // CHANGE BRANCH
                PataDialogBox newdb;
                newdb.CreateCustom(font, "ln_settings1", text_branches, 3, 1);
                newdb.id = 5;
                dialogboxes.push_back(newdb);
            }
        }
        if(mouse.x > 503+710-pedestal.getGlobalBounds().width/2 && mouse.x < 503+710+pedestal.getGlobalBounds().width/2 && mouse.y >= 1200 && mouse.y <= 2160)
        {
            p_active = 2;
            if(mouseCtrl->getClick(0))
            {
                // OPEN GAME PATH
                Func::openFileExplorer(std::filesystem::current_path().string());
            }
        }
        if(mouse.x > 503+710*2-pedestal.getGlobalBounds().width/2 && mouse.x < 503+710*2+pedestal.getGlobalBounds().width/2 && mouse.y >= 1200 && mouse.y <= 2160)
        {
            p_active = 3;
            if(mouseCtrl->getClick(0))
            {
                // CHANGE LANGUAGE
                a_state = 17;
            }
        }
        if(mouse.x > 503+710*3-pedestal.getGlobalBounds().width/2 && mouse.x < 503+710*3+pedestal.getGlobalBounds().width/2 && mouse.y >= 1200 && mouse.y <= 2160)
        {
            p_active = 4;
            if(mouseCtrl->getClick(0))
            {
                str_login = "";
                str_email = "";
                str_password = "";
                worker->token = "";
                bg_camera.debug_x_dest = 0;
                entry_camera.debug_x_dest = 0;
                logo_x_d = 3840/2;
                logo_y_d = 2160/2;
                logo_y_d -= 600;

                logo_x_c = logo_x_d;
                logo_y_c = logo_y_d;
                a_state = 2;

                auto now = std::chrono::system_clock::now();
                std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
                std::tm* localTime = std::localtime(&currentTime);

                pon_greet.setGlobalPosition(sf::Vector2f(cam_placement*3 + 220, 1590));

                messageclouds.clear();

                if(localTime->tm_hour >= 5 && localTime->tm_hour <= 8)
                {
                    MessageCloud tmp;
                    tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
                    tmp.msgcloud_ID = 0;
                    tmp.AddDialog("ln_greeting1", true);
                    messageclouds.push_back(tmp);
                }

                if(localTime->tm_hour >= 9 && localTime->tm_hour <= 16)
                {
                    MessageCloud tmp;
                    tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
                    tmp.msgcloud_ID = 0;
                    tmp.AddDialog("ln_greeting2", true);
                    messageclouds.push_back(tmp);
                }

                if(localTime->tm_hour >= 17 && localTime->tm_hour <= 19)
                {
                    MessageCloud tmp;
                    tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
                    tmp.msgcloud_ID = 0;
                    tmp.AddDialog("ln_greeting3", true);
                    messageclouds.push_back(tmp);
                }

                if(localTime->tm_hour >= 20 && localTime->tm_hour <= 22)
                {
                    MessageCloud tmp;
                    tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
                    tmp.msgcloud_ID = 0;
                    tmp.AddDialog("ln_greeting4", true);
                    messageclouds.push_back(tmp);
                }

                if(localTime->tm_hour >= 23 || localTime->tm_hour <= 4)
                {
                    MessageCloud tmp;
                    tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+320, pon_greet.getGlobalPosition().y+45), sf::Color(255, 255, 255, 255), false, 3);
                    tmp.msgcloud_ID = 0;
                    tmp.AddDialog("ln_greeting5", true);
                    messageclouds.push_back(tmp);

                    pon_greet.setAnimationSpeed(0); // workaround for lack of animation stalling
                    pon_greet.setGlobalPosition(sf::Vector2f(cam_placement*3 + 230, 1612));
                }

                std::filesystem::remove("loginDetails.dat");
                worker->all_branches.clear();
                worker->all_branches.push_back("main");
            }
        }
        if(mouse.x > 503+710*4-pedestal.getGlobalBounds().width/2 && mouse.x < 503+710*4+pedestal.getGlobalBounds().width/2 && mouse.y >= 1200 && mouse.y <= 2160)
        {
            p_active = 5;
            if(mouseCtrl->getClick(0))
            {
                a_state = 10;
                cam_placement = 29500;

                bg_camera.debug_x_dest = cam_placement;
                entry_camera.debug_x_dest = cam_placement;

                logo_x_d = cam_placement*3 + 1425;
                logo_y_d = 460;
                logo_x_c = logo_x_d;
                logo_y_c = logo_y_d;

                background.setSize(sf::Vector2f(322, 720));
                background.setPosition(cam_placement+960, 0);

                messageclouds.clear();

                auto now = std::chrono::system_clock::now();
                std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
                std::tm* localTime = std::localtime(&currentTime);

                if(localTime->tm_hour >= 5 && localTime->tm_hour <= 8)
                {
                    MessageCloud tmp;
                    tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
                    tmp.msgcloud_ID = 0;
                    tmp.AddDialog("ln_greeting1", true);
                    messageclouds.push_back(tmp);
                }

                if(localTime->tm_hour >= 9 && localTime->tm_hour <= 16)
                {
                    MessageCloud tmp;
                    tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
                    tmp.msgcloud_ID = 0;
                    tmp.AddDialog("ln_greeting2", true);
                    messageclouds.push_back(tmp);
                }

                if(localTime->tm_hour >= 17 && localTime->tm_hour <= 19)
                {
                    MessageCloud tmp;
                    tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
                    tmp.msgcloud_ID = 0;
                    tmp.AddDialog("ln_greeting3", true);
                    messageclouds.push_back(tmp);
                }

                if(localTime->tm_hour >= 20 && localTime->tm_hour <= 22)
                {
                    MessageCloud tmp;
                    tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+360, pon_greet.getGlobalPosition().y), sf::Color(255, 255, 255, 255), false, 3);
                    tmp.msgcloud_ID = 0;
                    tmp.AddDialog("ln_greeting4", true);
                    messageclouds.push_back(tmp);
                }

                if(localTime->tm_hour >= 23 || localTime->tm_hour <= 4)
                {
                    MessageCloud tmp;
                    tmp.Create(20, sf::Vector2f(pon_greet.getGlobalPosition().x+320, pon_greet.getGlobalPosition().y+45), sf::Color(255, 255, 255, 255), false, 3);
                    tmp.msgcloud_ID = 0;
                    tmp.AddDialog("ln_greeting5", true);
                    messageclouds.push_back(tmp);

                    pon_greet.setAnimationSpeed(0); // workaround for lack of animation stalling
                    pon_greet.setGlobalPosition(sf::Vector2f(cam_placement*3 + 230, 1612));
                }
            }
        }

        if(p_active == 1)
        {
            window->draw(h1);
        }
        if(p_active == 2)
        {
            window->draw(h2);
        }
        if(p_active == 3)
        {
            window->draw(h3);
        }
        if(p_active == 4)
        {
            window->draw(h4);
        }
        if(p_active == 5)
        {
            window->draw(h5);
        }

        pedestal.setOrigin(pedestal.getGlobalBounds().width/2, pedestal.getGlobalBounds().height);
        pedestal.setPosition(cam_placement*3 + 503, 1860);
        pedestal.setColor(sf::Color::White);
        if(p_active == 1) { pedestal.setColor(sf::Color(255, 197, 31)); }
        pedestal.draw();
        pedestal.setPosition(cam_placement*3 + 503 + 710, 1860);
        pedestal.setColor(sf::Color::White);
        if(p_active == 2) { pedestal.setColor(sf::Color(255, 197, 31)); }
        pedestal.draw();
        pedestal.setPosition(cam_placement*3 + 503 + 710 * 2, 1860);
        pedestal.setColor(sf::Color::White);
        if(p_active == 3) { pedestal.setColor(sf::Color(255, 197, 31)); }
        pedestal.draw();
        pedestal.setPosition(cam_placement*3 + 503 + 710 * 3, 1860);
        pedestal.setColor(sf::Color::White);
        if(p_active == 4) { pedestal.setColor(sf::Color(255, 197, 31)); }
        pedestal.draw();
        pedestal.setPosition(cam_placement*3 + 503 + 710 * 4, 1860);
        pedestal.setColor(sf::Color::White);
        if(p_active == 5) { pedestal.setColor(sf::Color(255, 197, 31)); }
        pedestal.draw();

        pon_menu1.setAnimation("Units_Patapon_Idle_1");
        pon_menu2.setAnimation("Units_Patapon_Idle_1");
        pon_menu3.setAnimation("Units_Patapon_Idle_1");
        pon_menu4.setAnimation("Units_Patapon_Idle_1");
        pon_menu5.setAnimation("Units_Patapon_Idle_1");
        pon_menu1.setGlobalPosition(sf::Vector2f(cam_placement*3 + 323, 1310));
        pon_menu2.setGlobalPosition(sf::Vector2f(cam_placement*3 + 323 + 710, 1310));
        pon_menu3.setGlobalPosition(sf::Vector2f(cam_placement*3 + 323 + 710*2, 1310));
        pon_menu4.setGlobalPosition(sf::Vector2f(cam_placement*3 + 323 + 710*3, 1310));
        pon_menu5.setGlobalPosition(sf::Vector2f(cam_placement*3 + 323 + 710*4, 1310));

        if(p_active == 1)
        {
            pon_menu1.setAnimation("Units_Patapon_Walk");
            pon_menu1.setGlobalPosition(sf::Vector2f(cam_placement*3 + 371, 1364));
        }
        if(p_active == 2)
        {
            pon_menu2.setAnimation("Units_Patapon_Walk");
            pon_menu2.setGlobalPosition(sf::Vector2f(cam_placement*3 + 371 + 710, 1364));
        }
        if(p_active == 3)
        {
            pon_menu3.setAnimation("Units_Patapon_Walk");
            pon_menu3.setGlobalPosition(sf::Vector2f(cam_placement*3 + 371 + 710*2, 1364));
        }
        if(p_active == 4)
        {
            pon_menu4.setAnimation("Units_Patapon_Walk");
            pon_menu4.setGlobalPosition(sf::Vector2f(cam_placement*3 + 371 + 710*3, 1364));
        }
        if(p_active == 5)
        {
            pon_menu5.setAnimation("Units_Patapon_Walk");
            pon_menu5.setGlobalPosition(sf::Vector2f(cam_placement*3 + 371 + 710*4, 1364));
        }

        pon_menu1.Draw();
        pon_menu2.Draw();
        pon_menu3.Draw();
        pon_menu4.Draw();
        pon_menu5.Draw();

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

    if(a_state == 17)
    {
        auto langs = strRepo->GetAvailableLanguages();
        int f = 0;

        bool hover = false;
        mouse.x += cam_placement*3;

        for(auto lang : langs)
        {
            auto flag = ResourceManager::getInstance().getSprite("resources/lang/"+lang.first+"/"+lang.first+".png");
            int row = floor(f/6);
            int col = f % 6;

            auto name = Func::ConvertToUtf8String(lang.second);

            flag.setPosition(cam_placement*3 + 260 + col * 600, 100 + row * 400);
            flag.draw();

            auto font = strRepo->GetFontNameForLanguage(lang.first);

            flagnames[lang.first].setFont(font);
            flagnames[lang.first].setTextQuality(3);
            flagnames[lang.first].setCharacterSize(28);

            if(lang.first == strRepo->GetCurrentLanguage())
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

            if(((mouse.x > pos.x) && (mouse.x < pos.x + lb.width) && (mouse.y > pos.y) && (mouse.y < pos.y + lb.height)) || ((mouse.x > tpos.x - tlb.width/2) && (mouse.x < tpos.x + tlb.width/2) && (mouse.y > tpos.y - tlb.height/2) && (mouse.y < tpos.y + tlb.height/2)))
            {
                hover = true;

                flagnames[lang.first].setString("{color 255 192 64}"+name);

                if(mouseCtrl->getClick(0))
                {
                    strRepo->SetCurrentLanguage(lang.first);
                }
            }

            flagnames[lang.first].draw();
        }

        b_goback.setFont(font);
        b_goback.setCharacterSize(36);
        b_goback.setStringKey("ln_settings_button2");
        b_goback.setOrigin(b_goback.getLocalBounds().width/2, b_goback.getLocalBounds().height/2);
        b_goback.setPosition(cam_placement*3 + 3840/2, 1540);
        b_goback.setColor(sf::Color::White);

        auto pos = b_goback.getPosition();
        auto bounds = b_goback.getLocalBounds();
        if(mouse.x > pos.x-bounds.width/2 && mouse.x < pos.x+bounds.width/2 && mouse.y > pos.y-bounds.height/2 && mouse.y < pos.y+bounds.height/2)
        {
            b_goback.setColor(sf::Color::Green);

            sword.setOrigin(sword.getGlobalBounds().width/2, sword.getGlobalBounds().height/2);
            sword.setScale(1,1);
            sword.setPosition(pos.x - bounds.width/2 - 130 - swordOffset, pos.y + 30);
            sword.draw();
            sword.setScale(-1,1);
            sword.setPosition(pos.x + bounds.width/2 + 130 + swordOffset, pos.y + 30);
            sword.draw();

            if(mouseCtrl->getClick(0))
            {
                a_state = 16;

                std::ofstream langFile("resources/lang.txt", std::ios::trunc);
                langFile << strRepo->GetCurrentLanguage();
                langFile.close();
            }
        }

        b_goback.draw();
    }

    std::vector<int> db_e; ///dialog box erase

    auto view = window->getView();
    window->setView(window->getDefaultView());

    for (int i = 0; i < dialogboxes.size(); i++)
    {
        dialogboxes[i].x = 1920;
        dialogboxes[i].y = 1080;
        dialogboxes[i].Draw();

        if (dialogboxes[i].closed)
            db_e.push_back(i);
    }

    window->setView(view);

    for (int i = 0; i < db_e.size(); i++)
    {
        dialogboxes.erase(dialogboxes.begin() + db_e[i] - i);
    }

    auto inputCtrl = CoreManager::getInstance().getInputController();

    if (inputCtrl->isKeyPressed(Input::Keys::CROSS) || inputCtrl->isKeyPressed(Input::Keys::START))
    {
        if(dialogboxes.size() > 0)
        {
            auto& box = dialogboxes.back();
            if(box.id == 5)
            {
                worker->branch = box.options_saved[box.CheckSelectedOption()];
                SPDLOG_INFO("Branch changed to {}", worker->branch);
                box.Close();
            }

            switch (box.CheckSelectedOption())
            {
                case 0: {
                    if (box.id == 0)
                    {
                        SPDLOG_DEBUG("Yes");
                        box.Close();

                        // play offline
                        a_state = 10;
                        worker->isOffline = true;

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

                    if (box.id == 1)
                    {
                        box.Close();

                        // OFFLINE MODE - download the game
                        worker->branch = "main";
                        worker->setAction(Worker::DOWNLOAD_HERO);

                        a_state = 13;
                    }

                    if (box.id == 2)
                    {
                        box.Close();

                        a_state = 2;
                    }

                    if (box.id == 3)
                    {
                        box.Close();
                        worker->setAction(Worker::DOWNLOAD_HERO);

                        a_state = 13;
                    }

                    if (box.id == 4)
                    {
                        box.Close();
                    }

                    break;
                }

                case 1: {
                    if (box.id == 0)
                    {
                        SPDLOG_DEBUG("No");
                        box.Close();
                        a_state = 2;
                    }

                    if (box.id == 1)
                    {
                        SPDLOG_DEBUG("No");
                        box.Close();

                        bg_camera.debug_x_dest = cam_placement;
                        entry_camera.debug_x_dest = cam_placement;

                        a_state = 10;
                    }

                    if (box.id == 3)
                    {
                        SPDLOG_INFO("Not updating.");
                        box.Close();
                        worker->setAction(Worker::RUN_HERO);
                    }

                    break;
                }
            }
        }
    }
}