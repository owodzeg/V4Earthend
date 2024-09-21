#ifndef FIRSTRUN_H
#define FIRSTRUN_H

#include <SFML/Graphics.hpp>
#include "../Dialog/MessageCloud.h"

class FirstRun {
public:
  sf::CircleShape p_head, p_white, p_pupil;
  sf::RectangleShape p_pupil_closed;
  sf::Clock a_clock;
  sf::Font font;
  PText lang_ptext;
  PText b_next;
  sf::RectangleShape dir_rect;

  std::string gamePath;
  PText p_path;

  int a_state = 0;

  float pon_x_c = 0, pon_y_c = 0;
  float pon_x_d = 0, pon_y_d = 0;

  float pon_x_offset_c = 0, pon_y_offset_c = 0;
  float pon_x_offset_d = 0, pon_y_offset_d = 0;

  float r_head_c=0, r_white_c=0, r_pupil_c=0; //current
  float r_head_d=0, r_white_d=0, r_pupil_d=0; //destination

  float pupil_offset_x_c = 0, pupil_offset_x_d = 0;
  float pupil_offset_y_c = 0, pupil_offset_y_d = 0;
  float pupil_angle_c = 0, pupil_angle_d = 0;

  float real_deg = 0, prev_real_deg = 0;
  float deg = 0, prev_deg = 0;

  float speed = 0.015;
  float shake = -50;
  bool peck = false;

  float initTime = 1;

  std::vector<MessageCloud> messageclouds;
  std::unordered_map<std::string, PText> flagnames;

  std::string selectedLang = "us";

  void init();
  void draw();
};

#endif //FIRSTRUN_H
