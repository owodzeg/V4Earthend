#ifndef ENTRY_H
#define ENTRY_H

#include <SFML/Graphics.hpp>
#include "../Graphics/SpriteWrapper.h"
#include "../Graphics/PText.h"
#include "../Mechanics/Background.h"

class Entry {
public:
  sf::CircleShape p_head, p_white, p_pupil;
  SpriteWrapper logo;

  float pon_x_c = 0, pon_y_c = 0;
  float pon_x_d = 0, pon_y_d = 0;

  float pon_x_offset_c = 0, pon_y_offset_c = 0;
  float pon_x_offset_d = 0, pon_y_offset_d = 0;

  float r_head_c=0, r_white_c=0, r_pupil_c=0; //current
  float r_head_d=0, r_white_d=0, r_pupil_d=0; //destination

  float pupil_offset_x_c = 0, pupil_offset_x_d = 0;
  float pupil_offset_y_c = 0, pupil_offset_y_d = 0;
  float pupil_angle_c = 0, pupil_angle_d = 0;

  float logo_x_c = 0, logo_y_c = 0;
  float logo_x_d = 0, logo_y_d = 0;

  float speed = 0.015;

  sf::RectangleShape background;

  sf::Clock a_clock;
  int a_state = 0;

  Camera camera;
  Background bg;

  void init();
  void draw();
};

#endif //ENTRY_H
