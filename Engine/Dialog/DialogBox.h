#ifndef DIALOGBOX_H
#define DIALOGBOX_H

#include "../Graphics/PSprite.h"
#include "../Graphics/PText.h"
#include "../Input/InputController.h"
#include "RoundedRect.h"
#include <SFML/Graphics.hpp>

class PataDialogBox
{
public:
    int id = 0; ///for finding what dialog box is what

    bool rendered = false;

    RoundedRect rr_main;
    RoundedRect rr_shadow;
    sf::Font d_font;
    PText t_dialogType, t_dialogText;
    std::vector<PText> t_options;

    sf::RectangleShape highlight;
    unsigned int option = 0;

    PSprite arrow;

    float arrow_x;

    float width = 0, height = 0;
    float x = 0, y = 0;

    bool closed = false;

    PataDialogBox();
    //void Create(sf::Font font, std::string text, std::vector<std::string> options, int qualitySetting);
    void Create(sf::Font font, sf::String text, std::vector<sf::String> options, int qualitySetting, int type = 1);
    void Readjust();
    int CheckSelectedOption();
    void MoveUp();
    void MoveDown();
    void Close();
    void Draw();
};

#endif // DIALOGBOX_H
