#ifndef POSITION_H_
#define POSITION_H_

#include "types.h"
#include "player.h"

/*
 * ゲームの状態
 */
struct State {
  int step;  // ステップ番号 (0-indexed)
  int height, width;

  Cell cells[256][32];

  State() = default;
  State(int h, int w);
  ~State();

  // コンソールに状態を表示（デバッグ用）
  void Print(Player* player = nullptr) const;

  // ２点間に障害物があるかを判定する
  bool DoesCollide(const Square& s1, const Square& s2) const;

  // マスsがコースからはみ出ているかどうかを判定する。
  bool DoesProtrude(const Square& s) const;

  // プレイヤと行動を受け取り、１ステップ進める
  // playerの値が書き換わることに注意
  void Simulate(Player* player, Action* action) const;
};

#endif  // POSITION_H_
