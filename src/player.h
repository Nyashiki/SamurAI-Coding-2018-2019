#ifndef PLAYER_H_
#define PLAYER_H_

#include "types.h"

/*
 * プレイヤーの状態
 */
struct Player {
  Square square;
  Velocity velocity;

  Player() = default;

  // 速度を失う（停止状態）
  void Stop();
};

#endif  // PLAYER_H_
