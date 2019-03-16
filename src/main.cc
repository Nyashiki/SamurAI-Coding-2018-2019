#include "game.h"

#include <iostream>


int main(int argc, char* argv[]) {
  Game::Init();

  Game::InitializationInput();

  while (true) {
    Game::StepInput();

    Game::OutputAction();
  }

  return 0;
}
