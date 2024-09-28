#ifndef ENTRY_H
#define ENTRY_H

#include <SFML/Graphics.hpp>
#include "../Graphics/SpriteWrapper.h"
#include "../Graphics/PText.h"
#include "../Mechanics/Background.h"
#include "../Mechanics/AnimatedObject.h"
#include "../Dialog/MessageCloud.h"

class Entry {
public:
  sf::CircleShape p_head, p_white, p_pupil;
  SpriteWrapper logo, sword, pedestal;

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
  float swordOffset = 0;

  sf::RectangleShape background;

  sf::Clock a_clock;
  int a_state = -1;
  int day_state = 0;

  PText p_login,p_register,p_offline;
  PText p_news_header;
  PText p_news;

  PText p_play, p_settings, p_exit;

  Camera bg_camera;
  Camera entry_camera;
  Background bg;

  float cam_placement = 29500;
  AnimatedObject pon_greet, pon_menu1, pon_menu2, pon_menu3;
  int p_active = 0;

  PText debugtext;
  sf::VertexArray h1,h2,h3;

  std::vector<MessageCloud> messageclouds;

  void init();
  void draw();
};

#endif //ENTRY_H
