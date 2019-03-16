#include "../think.h"

#include <random>

Action Think::RandomPlayer() {
  static std::mt19937 mt(20181220);

  int valid[3] = { -1, 0, 1 };

  return Action(valid[mt() % 3], valid[mt() % 3]);
}
