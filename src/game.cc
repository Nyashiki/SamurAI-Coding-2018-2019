#include "game.h"
#include "think.h"
#include "stopwatch.h"

#include <iostream>
#include <fstream>
#include <string>

namespace {

#ifdef DEBUG_MODE
std::ofstream stdin_log;
std::ofstream game_log;
#endif

}  // namespace

// Variables
int Game::remain_time;
int Game::step_max;
int Game::height, Game::width;
int Game::sight_range;

State* Game::current_state;
Player Game::players[2];

// Functions

void Game::Init() {
#ifdef DEBUG_MODE
  stdin_log = std::ofstream("./stdin_log.txt");
  game_log = std::ofstream("./game.log.txt");
#endif
}

void Game::InitializationInput() {
#ifdef DEBUG_MODE
  Stopwatch sw;
  sw.Start();

  game_log << "Game::InitializationInput() started." << std::endl;
#endif

  std::cin >> remain_time;
  std::cin >> step_max;
  std::cin >> width >> height;
  std::cin >> sight_range;

#ifdef DEBUG_MODE
  stdin_log << remain_time << std::endl;
  stdin_log << step_max << std::endl;
  stdin_log << width << " " << height << std::endl;
  stdin_log << sight_range << std::endl;
#endif

  current_state = new State(height + 1, width);

  std::cout << 0 << std::endl;

#ifdef DEBUG_MODE
  game_log << "Game::InitializationInput() ended. [ " << sw.Elapsed() << " ms ]" << std::endl;
#endif
}

void Game::StepInput() {
#ifdef DEBUG_MODE
  Stopwatch sw;
  sw.Start();

  game_log << "Game::StepInput() started." << std::endl;
#endif
  std::cin >> current_state->step;
  std::cin >> remain_time;

#ifdef DEBUG_MODE
  stdin_log << current_state->step << std::endl;
  stdin_log << remain_time << std::endl;
#endif

  Player* my_player = &players[ME];
  Player* op_player = &players[OPPONENT];

  std::cin >> my_player->square.x >> my_player->square.y >> my_player->velocity.x >> my_player->velocity.y;
  std::cin >> op_player->square.x >> op_player->square.y >> op_player->velocity.x >> op_player->velocity.y;

#ifdef DEBUG_MODE
  stdin_log << my_player->square.x << " " << my_player->square.y << " " << my_player->velocity.x << " " << my_player->velocity.y << std::endl;
  stdin_log << op_player->square.x << " " << op_player->square.y << " " << op_player->velocity.x << " " << op_player->velocity.y << std::endl;
#endif

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int cell_info;
      std::cin >> cell_info;
      current_state->cells[i][j] = (Cell)(cell_info);

#ifdef DEBUG_MODE
      stdin_log << cell_info << " ";
#endif
    }
#ifdef DEBUG_MODE
    stdin_log << std::endl;
#endif
  }

#ifdef DEBUG_MODE
  game_log << "Game::StepInput() ended. [ " << sw.Elapsed() << " ms ]" << std::endl;
#endif
}

void Game::OutputAction() {
#ifdef DEBUG_MODE
  Stopwatch sw;
  sw.Start();
  game_log << "Game::OutputAction() started." << std::endl;
#endif

  // Action next_action = Think::RandomPlayer();
  Action next_action = Think::nyashiki_Agent();
  std::cout << next_action.x << " " << next_action.y << std::endl;

#ifdef DEBUG_MODE
  game_log << "Game::OutputAction() ended. [ " << sw.Elapsed() << " ms ]" << std::endl;
#endif
}
