#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE 

#include "MessageCloud.h"
#include <iostream>
#include <math.h>
#include <spdlog/spdlog.h>
#include "../CoreManager.h"

using namespace std;

MessageCloud::MessageCloud()
{
}

void MessageCloud::setSize(float new_x, float new_y)
{
    xsize = new_x;
    ysize = new_y;
}

void MessageCloud::Create(int speed, sf::Vector2f start_pos, sf::Color color, bool can_speedup, int q)
{
    SPDLOG_DEBUG("MessageCloud::Create()");

    timeout = speed;
    regular_timeout = timeout;
    quality = q;

    //speedable = can_speedup;
    speedable = true; //unskippable dialogues are dumb, let the player speed up anything

    cloud.load("resources/graphics/ui/dialog/message.png");
    cloud.setOrigin(cloud.getLocalBounds().width, cloud.getLocalBounds().height);
    cloud.setColor(color);

    cur_color = color;

    cross.loadFromFile("resources/graphics/ui/dialog/cross.png", q);
    cross_highlight.loadFromFile("resources/graphics/ui/dialog/crosshighlight.png", q);
    cross_arrow.loadFromFile("resources/graphics/ui/dialog/crossarrow.png", q);

    triangle.setPointCount(3);
    triangle.setFillColor(cur_color);

    startpos = start_pos;

    dialogue_ptext.createText(CoreManager::getInstance().getCore()->global_font, 28, sf::Color(32, 32, 32, 255), "", quality, 1);

    visual_ptext.createText(CoreManager::getInstance().getCore()->global_font, 28, sf::Color(32, 32, 32, 255), "", quality, 1);
    visual_ptext.force_nonspeech = true;

    SPDLOG_DEBUG("MessageCloud::Create(): finished");
}

void MessageCloud::AddDialog(sf::String text, bool nextdialog)
{
    // here i dont think we need to use more than one ptext anymore
    // just push the new dialogues into a vector of sf::Strings and let PText parse it :)

    SPDLOG_DEBUG("MessageCloud::AddDialog()");

    dialogue_strings.push_back(text);
    next_dialog.push_back(nextdialog);

    SPDLOG_DEBUG("MessageCloud::AddDialog(): finished");
}

void MessageCloud::Show()
{
    if (!active)
    {
        SPDLOG_DEBUG("MessageCloud::Show()");

        active = true;
        done = false;

        if (dialogue_strings.size() <= 0)
        {
            SPDLOG_ERROR("No messages found in message cloud.");
            AddDialog(sf::String("no message"), true);
        }

        dialogue_ptext.setString(dialogue_strings[cur_dialog]);
        visual_ptext.setString(dialogue_strings[cur_dialog]);

        sf::RenderWindow* window = CoreManager::getInstance().getWindow();
        float rX = window->getSize().x / float(3840);
        float rY = window->getSize().y / float(2160);

        dest_xsize = (visual_ptext.getLocalBounds().width + (visual_ptext.getLocalBounds().width / 90)) * rX;
        dest_ysize = (visual_ptext.getLocalBounds().height + (visual_ptext.getLocalBounds().height / 13.5)) * rY;

        text_timeout.restart();

        SPDLOG_DEBUG("MessageCloud::Show(): finished");
    }
}

void MessageCloud::Hide()
{
    active = false;
}

void MessageCloud::End()
{
    dest_xsize = 0;
    dest_ysize = 0;

    done = true;
    interrupt = true;
}

void MessageCloud::NextDialog()
{
    if (cur_dialog < dialogue_strings.size() - 1)
    {
        ready = false;

        cur_dialog++;

        cur_char = 0;

        old_xsize = dest_xsize;
        old_ysize = dest_ysize;

        dialogue_ptext.setString(dialogue_strings[cur_dialog]);
        visual_ptext.setString(dialogue_strings[cur_dialog]);

        sf::RenderWindow* window = CoreManager::getInstance().getWindow();
        float rX = window->getSize().x / float(3840);
        float rY = window->getSize().y / float(2160);

        dest_xsize = (visual_ptext.getLocalBounds().width + (visual_ptext.getLocalBounds().width / 90)) * rX;
        dest_ysize = (visual_ptext.getLocalBounds().height + 30 + (visual_ptext.getLocalBounds().height / 13.5)) * rY;

        text_timeout.restart();
    } else
    {
        if(dialogue_ptext.goback)
        {
            goback = true;
        }
        
        dest_xsize = 0;
        dest_ysize = 0;

        done = true;
    }
}

void MessageCloud::SpeedUp()
{
    speedup = true;
}

void MessageCloud::Draw()
{
    sf::RenderWindow* window = CoreManager::getInstance().getWindow();
    InputController* inputCtrl = CoreManager::getInstance().getInputController();
    float fps = CoreManager::getInstance().getCore()->getFPS();

    if (!firstrender)
        firstrender = true;

    if (speedable)
    {
        // here we should do something like
        // ptext timeout = 20ms=50letters/s temporarily
        // then when key is not held, return to normal speed

        if (inputCtrl->isKeyHeld(Input::Keys::CIRCLE))
            SpeedUp();
    }

    if (ready)
    {
        // advance current text id

        if (inputCtrl->isKeyPressed(Input::Keys::CROSS))
        {
            NextDialog();
        }
    }

    //for (unsigned int i = 0; i < dialogue_strings.size(); i++)
    //    ptext[i].update(window);

    if (active)
    {
        x = (startpos.x*3) + (xsize*3) / 6;
        y = (startpos.y*3) - (ysize*3) * 3;

        float xsize_diff = (dest_xsize - xsize) * 10 / fps;
        float ysize_diff = (dest_ysize - ysize) * 10 / fps;

        //cout << "diff: " << xsize << " " << ysize << " " << dest_xsize << " " << dest_ysize << " " << xsize_diff << " " << ysize_diff << endl;

        setSize(xsize + xsize_diff, ysize + ysize_diff);

        if (((floor(xsize) <= 1) || (floor(ysize) <= 1)) && (done))
        {
            if(goback && !interrupt)
            {
                SPDLOG_DEBUG("Go back to first message");
                cur_dialog = 0;
                done = false;
                active = false;
                ready = false;
                Show();
            }
            else
            {
                active = false;
            }
        }

        /// adjusting the size of clouds and drawing them
        /// they are no longer an array, instead we just transform the single texture 

        float scale_x = 4500.f / xsize;
        float scale_y = 750.f / ysize;

        cloud.setColor(cur_color);
        cloud.setPosition(x, y);

        cloud.setScale(1.f / scale_x, 1.f / scale_y);
        cloud.draw();

        cloud.setScale(-1.f / scale_x, 1.f / scale_y);
        cloud.draw();

        cloud.setScale(1.f / scale_x, -1.f / scale_y);
        cloud.draw();

        cloud.setScale(-1.f / scale_x, -1.f / scale_y);
        cloud.draw();

        float rX = window->getSize().x / float(3840);
        float rY = window->getSize().y / float(2160);

        triangle.setPoint(0, sf::Vector2f(startpos.x, startpos.y));
        triangle.setPoint(1, sf::Vector2f((x - (xsize / 10)) * rX, y * rY));
        triangle.setPoint(2, sf::Vector2f((x + (xsize / 10)) * rX, y * rY));

        triangle.setFillColor(cur_color);
        window->draw(triangle);

        if (!done)
        {
            dialogue_ptext.speedup = speedup;
            dialogue_ptext.setPosition(x - visual_ptext.getLocalBounds().width / 2, y - 12 - visual_ptext.getLocalBounds().height / 2);
            dialogue_ptext.draw(window);

            //loaded_text[cur_dialog].setPosition(x - ptext[cur_dialog].getLocalBounds().width / 2, y - 4 - ptext[cur_dialog].getLocalBounds().height / 2);
            //showtext[cur_dialog].setString(viewed_text[cur_dialog]);
            //loaded_text[cur_dialog].draw(window);

            if (dialogue_ptext.speech_done)
            {
                ready = true;
            }
        }

        speedup = false;
    }
}
