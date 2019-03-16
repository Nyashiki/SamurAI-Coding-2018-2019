#include "state.h"
#include "player.h"
#include "geometry.h"

#include <cstring>
#include <iostream>
#include <map>
#include <string>

namespace {

// 動線s1s2がマスtargetに接触しているかどうかを調べる。
bool DoesContact(Square s1, Square s2, Square target) {
  Point a = Point(s1.y + 0.5, s1.x + 0.5),
        b = Point(s2.y + 0.5, s2.x + 0.5);

  if (s1 == target or s2 == target) return true;

  // targetの4辺との交差判定をする
  Point lt = Point(target.y + 1, target.x),
        rt = Point(target.y + 1, target.x + 1),
        lb = Point(target.y, target.x),
        rb = Point(target.y, target.x + 1);

  if (DoesCross(a, b, lt, rt) or DoesCross(a, b, rt, rb) or
        DoesCross(a, b, rb, lb) or DoesCross(a, b, lb, lt)) return true;
  else return false;
}

/*
 * 移動元と移動先の点受け取り、優先権を持つプレイヤを返す
 * 注意：コースアウト等をする移動を与えないこと
 */
PlayerType GetPriority(Square my_square, Square my_planned_square, Square op_square, Square op_planned_square) {
  //
  //  優先権を持つプレイヤが動くことができる
  //  移動前のy座標が小さいプレイヤが優先権を持つ
  //  y座標が同じ場合には、x座標が小さいプレイヤが優先権を持つ

  // 動線が相手プレイヤの開始時の位置の升目と交差あるいは接する場合には、
  // 優先権は相手プレイヤに移る

  // 両方の動線が互いのプレイヤーの開始時の位置のマス目と交差あるいは接する場合には、
  // 優先権は失われる。
  bool my_plan_failed = DoesContact(my_square, my_planned_square, op_square),
       op_plan_failed = DoesContact(op_square, op_planned_square, my_square);

  if (my_plan_failed and op_plan_failed) return NO_PLAYER_TYPE;
  if (my_plan_failed) return OPPONENT;
  if (op_plan_failed) return ME;

  PlayerType priority = ME;

  if (my_square.y > op_square.y) {
    priority = OPPONENT;
  } else if (my_square.y == op_square.y and my_square.x > op_square.x) {
    priority = OPPONENT;
  }
  return priority;
}

}  // namespace


State::State(int h, int w): height(h), width(w) {
  memset(cells, 0, sizeof(cells));
}

State::~State() {
}

void State::Print(Player* player) const {
  std::cerr << "STEP: " << step << std::endl;
  std::cerr << "HEIGHT: " << height << ", WIDTH: " << width << std::endl;

  const std::map<Cell, std::string> cell_to_console = {
    { EMPTY,     "\033[38;2;190;255;130mE" },
    { OBSTACLE,  "\033[38;2;255;190;130mO" },
    { PUDDLE,    "\033[38;2;200;220;255mP" },
    { UNVISIBLE, "\033[38;2;220;255;220m?" }
  };

  for (int i = height - 1; i >= 0; i--) {
    for (int j = 0; j < width; j++) {
      bool player_flag = false;
      // プレイヤの表示
      if (player != nullptr) {
        if (i == player[0].square.y && j == player[0].square.x) {
          std::cerr << "\033[38;2;240;0;0m1 ";
          player_flag = true;
        } else if (i == player[1].square.y && j == player[1].square.x) {
          std::cerr << "\033[38;2;0;100;200m2 ";
          player_flag = true;
        }
      }

      if (!player_flag) {
        std::cerr << cell_to_console.at(cells[i][j]) << " ";
      }
    }
    std::cerr << "\033[0m" << std::endl;
  }
}

// s1--s2間に障害物があるかを判定する
bool State::DoesCollide(const Square& s1, const Square& s2) const {
  // s1--s2はどちらも盤面からはみ出ていないことを仮定

  for (int y = std::min(s1.y, s2.y); y <= std::max(s1.y, s2.y); y++) {
    for (int x = std::min(s1.x, s2.x); x <= std::max(s1.x, s2.x); x++) {
      if (DoesContact(s1, s2, Square(y, x)) and cells[y][x] == OBSTACLE) {
        return true;
      }
    }
  }
  return false;
}

// マスpがコースからはみ出ているかを判定する
bool State::DoesProtrude(const Square& s) const {
  return s.x < 0 or s.x >= width;
}

void State::Simulate(Player* player, Action* action) const {
  Player* my_player = &player[0];
  Player* op_player = &player[1];

  Action* my_action = &action[0];
  Action* op_action = &action[1];

  // 予定位置を計算する
  Square my_planned_square = my_player->square + my_player->velocity + *my_action;
  Square op_planned_square = op_player->square + op_player->velocity + *op_action;

  bool my_does_collide = DoesCollide(my_player->square, my_planned_square);
  bool op_does_collide = DoesCollide(op_player->square, op_planned_square);

  bool my_does_protrude = DoesProtrude(my_planned_square);
  bool op_does_protrude = DoesProtrude(op_planned_square);

  // 衝突が生じる場合
  bool does_cross = DoesCross(my_player->square, my_planned_square, op_player->square, op_planned_square);

  // 相手プレイヤがコースアウト等をしている場合には、衝突は生じない
  if (my_does_collide || op_does_collide || my_does_protrude || op_does_protrude) {
    does_cross = false;
  }

  if (does_cross) {
    PlayerType priority = GetPriority(my_player->square, my_planned_square, op_player->square, op_planned_square);
    if (priority == ME) {
      my_player->square = my_planned_square;
      my_player->velocity += *my_action;
      op_player->Stop();
    } else if (priority == OPPONENT) {
      op_player->square = op_planned_square;
      op_player->velocity += *op_action;
      my_player->Stop();
    } else {
      my_player->Stop();
      op_player->Stop();
    }
  }

  // 何事もなかった場合には、単純に移動する
  else {
    if (my_does_collide || my_does_protrude) {
      my_player->Stop();
    } else {
      my_player->square = my_planned_square;
      my_player->velocity += *my_action;
    }

    if (op_does_collide || op_does_protrude) {
      op_player->Stop();
    } else {
      op_player->square = op_planned_square;
      op_player->velocity += *op_action;
    }
  }

  // 終了時の位置が水溜りの場合には、速度を失う
  if (cells[my_planned_square.y][my_planned_square.x] == PUDDLE) {
    my_player->Stop();
  }
  if (cells[op_planned_square.y][op_planned_square.x] == PUDDLE) {
    op_player->Stop();
  }

  // ゴールを通り過ぎないようにする
  if (my_player->square.y >= height) {
    my_player->square.y = height - 1;
  }
  if (op_player->square.y >= height) {
    op_player->square.y = height - 1;
  }
}
