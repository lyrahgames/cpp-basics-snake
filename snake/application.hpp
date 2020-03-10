#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <chrono>
#include <list>
#include <string>
#include <vector2.hpp>

class application {
 public:
  application() { resize(); }
  application(int w, int h, const std::string& str = {});
  ~application() = default;

  void execute();

 private:
  void resize();
  void resize(int w, int h);
  void compute_viewport();
  void zeit(float t);
  void init_food();
  void draw();
  void moving();
  void eating();
  void init_obstacle();
  void draw_obstacle();
  bool bite();
  void drawText(int zahl, const int Size, const float xposition,
                const float yposition, const int r, const int g, const int b);
  void startbild();

 private:
  std::string exe_dir;
  int screen_width = 500;
  int screen_height = 500;
  int game_width = 30;
  int game_height = 30;
  bool over = false;

  // viewport
  me::vector2f origin{0.5f * game_width, 0.5f * game_height};
  me::vector2f view_dim =
      40.0f * me::vector2f{1, static_cast<float>(screen_width) / screen_height};
  me::vector2f view_min;
  me::vector2f view_max;

  // rendering
  sf::RenderWindow window{sf::VideoMode(screen_width, screen_height), "snake"};

  // Snake als Liste mit Startwert
  std::list<std::array<int, 2>> snake{{1, 1}, {2, 1}, {2, 2},
                                      {2, 3}, {2, 4}, {3, 4}};
  // Speichere gegessenes Futter
  std::list<std::array<int, 2>> eaten_food{};
  // Futter
  std::array<int, 2> food{};
  // obstacle
  std::array<int, 2> obs{};
  // Anfangsrichtung
  int direction = 0;

  bool add_segment = false;
  int points = 0;
  bool starten = true;
  sf::Text text;
  sf::Text punkte;
  sf::Text start1;
  sf::Text start2;
  sf::Text start3;
  sf::Font mainGameFont;
};