#include <SFML/Graphics.hpp>
#include <application.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <string>

using namespace std;

application::application(int w, int h, const std::string& str)
    : screen_width{w}, screen_height{h}, exe_dir{str} {
  resize();
}

void application::resize() {
  window.setView(sf::View{sf::FloatRect{0, 0, static_cast<float>(screen_width),
                                        static_cast<float>(screen_height)}});
}

void application::resize(int w, int h) {
  screen_width = w;
  screen_height = h;
  resize();
}

void application::zeit(float t) {
  const auto start = chrono::high_resolution_clock::now();
  while (chrono::duration<float>(chrono::high_resolution_clock::now() - start)
             .count() < t) {
  }
}

void application::compute_viewport() {
  view_dim.x = view_dim.y * screen_width / screen_height;
  view_min = origin - 0.5f * view_dim;
  view_max = origin + 0.5f * view_dim;
}

void application::init_food() {
  //über normalverteilte Zufallszahlen Futter platzieren
  static mt19937 rng{
      random_device{}()};  // static, damit es nicht immer wieder neue
                           // Zufallszahlen berechnet, sondern nur beim ersten
                           // Mal wenn die Funktion aufgerufen wird
  uniform_int_distribution<int> dist_x{0, game_width - 1};
  uniform_int_distribution<int> dist_y{0, game_height - 1};
  bool food_in_snake;
  do {
    food = {dist_x(rng), dist_y(rng)};
    food_in_snake = false;
    // wenn das Essen auf der Schlange liegen würde, soll es da nicht hingelegt
    // werden, sondern die Schleife neu durchlaufen werden
    for (auto it = snake.begin(); it != snake.end(); ++it) {
      if (food == *it) {
        food_in_snake = true;
        break;
      }
    }
  } while (food_in_snake);
}

void application::draw() {
  for (int j = 0; j < game_height; ++j) {
    for (int i = 0; i < game_width; ++i) {
      sf::RectangleShape gitter(sf::Vector2f(0.9f * screen_width / view_dim.x,
                                             0.9f * screen_width / view_dim.x));
      gitter.setPosition((i - view_min.x) * screen_width / view_dim.x,
                         (j - view_min.y) * screen_height / view_dim.y);
      gitter.setFillColor(sf::Color(255, 255, 255));
      window.draw(gitter);
    }
  }
  // snake malen:
  for (auto it = snake.begin(); it != snake.end(); ++it) {
    sf::RectangleShape rec(sf::Vector2f(0.9f * screen_width / view_dim.x,
                                        0.9f * screen_width / view_dim.x));
    rec.setPosition(((*it)[0] - view_min.x) * screen_width / view_dim.x,
                    ((*it)[1] - view_min.y) * screen_height / view_dim.y);
    rec.setFillColor(sf::Color(0, 0, 255));
    window.draw(rec);
  }
  // Essen malen:
  sf::RectangleShape rec(sf::Vector2f(0.9f * screen_width / view_dim.x,
                                      0.9f * screen_width / view_dim.x));
  rec.setPosition((food[0] - view_min.x) * screen_width / view_dim.x,
                  (food[1] - view_min.y) * screen_height / view_dim.y);
  rec.setFillColor(sf::Color(255, 0, 0));
  window.draw(rec);
}

void application::moving() {
  array<int, 2> head = snake.front();
  switch (direction) {
    case 0:
      head[1] = (head[1] - 1 + game_height) % game_height;  // hoch
      break;
    case 1:
      head[0] = (head[0] + 1) % game_width;  // rechts
      break;
    case 2:
      head[1] = (head[1] + 1) % game_height;  // runter
      break;
    case 3:
      head[0] = (head[0] - 1 + game_width) % game_width;  // links
      break;
  }
  // vorn ein Kästchen anhängen, hinten eins löschen
  snake.push_front(head);
  snake.pop_back();
}

void application::eating() {
  // wenn gefressen, soll es hinten länger werden
  if (add_segment) {
    snake.push_back(eaten_food.front());
    eaten_food.pop_front();  // lösche gegessenes Essen
    add_segment = false;
  }

  // es soll dann ein Segment an die Schlange anhängen, wenn es mit dem
  // letzten Glied über das Essen drüber ist
  if (!eaten_food.empty() && (eaten_food.front() == snake.back())) {
    add_segment = true;

    points += 1;
  }
  // wenn snake mit Kopf über das Essen gegangen ist, soll ein neues Essen
  // erscheinen
  if (snake.front() == food) {
    eaten_food.push_back(food);
    init_food();
  }
}

// Hindernis
void application::init_obstacle() {
  static mt19937 rng{random_device{}()};
  uniform_int_distribution<int> dist_x{0, game_width - 1};
  uniform_int_distribution<int> dist_y{0, game_height - 1};
  bool obs_in_snake;
  do {
    obs = {dist_x(rng), dist_y(rng)};
    obs_in_snake = false;
    for (auto it = snake.begin(); it != snake.end(); ++it) {
      if (obs == *it) {
        obs_in_snake = true;
        break;
      }
    }
  } while (obs_in_snake);
}

void application::draw_obstacle() {
  // obstacle malen:
  sf::RectangleShape rec(sf::Vector2f(0.9f * screen_width / view_dim.x,
                                      0.9f * screen_width / view_dim.x));
  rec.setPosition((obs[0] - view_min.x) * screen_width / view_dim.x,
                  (obs[1] - view_min.y) * screen_height / view_dim.y);
  rec.setFillColor(sf::Color(0, 0, 0));
  window.draw(rec);
}

bool application::bite() {
  array<int, 2> head = snake.front();
  // erstelle Schlangenkörper:
  std::list<std::array<int, 2>> snake_without_head{};
  snake_without_head = snake;
  snake_without_head.pop_front();
  snake_without_head.pop_back();
  // wenn Kopf auf restlichen Körper trifft, dann ist es aus
  for (auto it = snake_without_head.begin(); it != snake_without_head.end();
       ++it) {
    if (head == *it) {
      over = true;  //= verloren
    }
  }
  if (head == obs) over = true;  //= verloren
  return over;
}

void application::drawText(int zahl, const int Size, const float xposition,
                           const float yposition, const int r, const int g,
                           const int b) {
  text.setString("Punkte");
  if (zahl == 1) text.setString("Punkt");
  text.setCharacterSize(Size);
  text.setFillColor(sf::Color(r, g, b, 255));
  text.setPosition(xposition, yposition);
  text.setFont(mainGameFont);
  window.draw(text);
  punkte.setString(to_string(zahl));
  punkte.setCharacterSize(Size);
  punkte.setFillColor(sf::Color(r, g, b, 255));
  punkte.setPosition(xposition - screen_width / 8, yposition);
  punkte.setFont(mainGameFont);
  window.draw(punkte);
}

void application::startbild() {
  start1.setString("Hindernisse sind schwarz");
  start1.setCharacterSize(screen_width / 10);
  start1.setFillColor(sf::Color(0, 0, 0, 255));
  start1.setPosition(screen_width / 6, screen_height / 4);
  start1.setFont(mainGameFont);
  window.draw(start1);
  start2.setString("Essen ist rot");
  start2.setCharacterSize(screen_width / 10);
  start2.setFillColor(sf::Color(255, 0, 0, 255));
  start2.setPosition(screen_width / 6, 3 * screen_height / 8);
  start2.setFont(mainGameFont);
  window.draw(start2);
  start3.setString("Start mit <Enter>");
  start3.setCharacterSize(screen_width / 8);
  start3.setFillColor(sf::Color(0, 150, 0, 255));
  start3.setPosition(screen_width / 6, screen_height / 2);
  start3.setFont(mainGameFont);
  window.draw(start3);
}

void application::execute() {
  using namespace std;

  auto beg = chrono::high_resolution_clock::now();

  init_food();
  init_obstacle();
  mainGameFont.loadFromFile(exe_dir + "font.ttf");
  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      // Decide what to do if certain events are happening.
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::Resized:
          resize(event.size.width, event.size.height);
          break;
          // es darf nicht die Richtung ändern, nur im 90°-Winkel
        case sf::Event::KeyPressed:
          switch (event.key.code) {
            case sf::Keyboard::Escape:
              window.close();
              break;
            case sf::Keyboard::Up:
              if (direction != 2) {
                direction = 0;
              }
              break;
            case sf::Keyboard::Right:
              if (direction != 3) {
                direction = 1;
              }
              break;
            case sf::Keyboard::Down:
              if (direction != 0) {
                direction = 2;
              }
              break;
            case sf::Keyboard::Left:
              if (direction != 1) {
                direction = 3;
              }
              break;
            case sf::Keyboard::Return:
              starten = false;
              break;
          }
          break;
      }
    }

    compute_viewport();

    window.clear();
    if (starten == true) {  // Startbildschirm
      draw();
      startbild();
      window.display();
    } else {
      if (over == false) {
        draw();    // Zeichne Gitter, snake und Futter
        moving();  // Bewegung der snake in eine Richtung
        eating();

        if (chrono::duration<float>(chrono::high_resolution_clock::now() - beg)
                .count() > 5) {
          init_obstacle();
          beg = chrono::high_resolution_clock::now();
        }
        draw_obstacle();
        window.display();
      } else {
        draw();
        drawText(points, screen_width / 5, 4 * screen_width / 10,
                 screen_height / 3, 0, 150, 0);
        window.display();
      }
      bite();  // Abfrage, ob Schlange sich selbst beißt
      zeit(0.1 - points * 0.001);
    }
  }
}
