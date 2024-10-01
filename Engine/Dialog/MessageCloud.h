#ifndef MESSAGECLOUD_H
#define MESSAGECLOUD_H

#include <SFML/Graphics.hpp>

#include "../Graphics/SpriteWrapper.h"
#include "../Graphics/PText.h"
#include "../Input/InputController.h"

class MessageCloud
{
public:
    SpriteWrapper cloud;
    PSprite cross, cross_highlight, cross_arrow;
    sf::ConvexShape triangle;

    PText dialogue_ptext; ///text that's going to appear on screen
    PText visual_ptext; ///text that will help calculating dimensions

    std::vector<sf::String> dialogue_strings;

    int cur_char = 0;
    int timeout = 50; ///milliseconds
    int regular_timeout = timeout;
    int speed_timeout = 5;

    bool speedup = false;

    sf::Clock text_timeout;
    sf::Clock arrow_timeout;

    float x = 0, y = 0;

    bool active = false;
    std::vector<bool> next_dialog;

    unsigned int cur_dialog = 0;

    float old_xsize = 0, old_ysize = 0;
    float xsize = 0, ysize = 0;
    float dest_xsize = 0, dest_ysize = 0;
    float arrow_y = 0;

    int quality = 0;
    int fontSize = 26;

    std::string cur_lang = "us";

    bool ready = false;
    bool done = false;
    bool canwrite = false;
    bool firstrender = false;
    bool speedable = false;
    bool goback = false;
    bool interrupt = false; //when moved away from the dialogbox source, forcefully quit

    sf::Vector2f startpos;

    sf::Color cur_color;

    int msgcloud_ID = 0;

    MessageCloud();
    void setSize(float new_x, float new_y);
    void Create(int speed, sf::Vector2f start_pos, sf::Color color, bool can_speedup, int q);
    void setFontSize(int newFontSize);
    void AddDialog(sf::String text, bool nextdialog);
    void NextDialog();
    void SpeedUp();
    void Show();
    void Hide();
    void End();
    void Draw();
};

#endif // MESSAGECLOUD_H
