#include "../think.h"
#include "../game.h"
#include "../player.h"
#include "../stopwatch.h"
#include "../types.h"

#include <cstring>
#include <cassert>
#include <vector>
#include <queue>
#include <algorithm>

namespace {

const int INF = 1e9;
constexpr Action INVALID_ACTION = Action(-2, -2);

constexpr Action ALL_ACTION[] = {
  Action( 1, -1), Action( 1,  0), Action( 1,  1),
  Action( 0, -1), Action( 0,  0), Action( 0,  1),
  Action(-1, -1), Action(-1,  0), Action(-1,  1)
};

bool IsZeroAction(const Action& action) {
  return (action.y == 0 && action.x == 0);
}

int L1norm(const Square& square) {
  return (std::abs(square.y) + std::abs(square.x));
}

bool visited[128][32] = { };
int distance_from_goal[128][32] = { };

struct TranspositionNode {
  bool used;
  int depth;
  int score;

  TranspositionNode() = default;
  TranspositionNode(int d, int s): depth(d), score(s) { }
};

namespace TranspositionTable {

TranspositionNode ttnode[128][32][40][40];

void Clear() {
  memset(ttnode, 0, sizeof(ttnode));
}

TranspositionNode* Withdraw(const Player& player) {
  return &ttnode[player.square.y][player.square.x][player.velocity.y + 20][player.velocity.x + 20];
}

void Register(const Player& player, int depth, int score) {
  TranspositionNode* n = Withdraw(player);
  if (!n->used || n->depth < depth) {
    *n = TranspositionNode(depth, score);
  }
}

}  // namespace TranspositionTable

struct Search {
  State state;
  Player players[2];
  Action next_action;

  Search(const State& s, const Player p[2]) {
    state = s;
    players[0] = p[0];
    players[1] = p[1];
  }

  int Eval(int depth) const {
    if (players[0].square.y >= state.height - 1) {
      return INF - 100 * depth + players[0].velocity.y;  // depthを引いてできるだけ早くゴールするようにする
    }

    int distance_to_op = std::abs(distance_from_goal[Game::players[1].square.y][Game::players[1].square.x]
                      - distance_from_goal[players[0].square.y][players[0].square.x]);

    int score = 100 * -distance_from_goal[players[0].square.y][players[0].square.x];  // ゴールに近いほうが良い

    return score;
  }

  // UNVISIBLEの部分のStateを予測する
  void StateAugmentation(bool obstacle_hard) {
    bool flag = true;
    bool prev_line_obstacle = false;

    for (int y = 1; y < state.height - 1; y++) {
      bool line_obstacle = false;
      for (int x = 0; x < state.width; x++) {
        if (state.cells[y][x] == UNVISIBLE) {
          // state.cells[y][x] = state.cells[y - 1][x];
          state.cells[y][x] = EMPTY;

          if (flag) {
            if (!line_obstacle && !prev_line_obstacle && obstacle_hard && x == players[0].square.x) {
              state.cells[y][x] = OBSTACLE;
              if (x > 0) {
                state.cells[y][x - 1] = OBSTACLE;
              }
              if (x + 1 < state.width) {
                state.cells[y][x + 1] = OBSTACLE;
              }
            }
            flag = false;
          }
          if (state.cells[y][x] == OBSTACLE) {
            line_obstacle = true;
          }
        }
      }
      prev_line_obstacle = line_obstacle;
    }
  }

  void CalculateDistance() {
    for (int i = 0; i < 128; i++) {
      for (int j = 0; j < 32; j++) {
        distance_from_goal[i][j] = INF;
      }
    }

    std::queue<Square> que;

    for (int i = 0; i < 32; i++) {
      distance_from_goal[state.height][i] = 0;
      que.push(Square(state.height, i));
    }

    while (!que.empty()) {
      Square s = que.front();
      que.pop();

      const int dy[4] = { -1, 0, 1, 0 };
      const int dx[4] = { 0, 1, 0, -1 };
      for (int i = 0; i < 4; i++) {
        int ny = s.y + dy[i];
        int nx = s.x + dx[i];
        int nd = distance_from_goal[s.y][s.x] + 1;

        if (ny < 0 || ny > state.height || nx < 0 || nx >= state.width) {
          continue;
        }

        if (state.cells[ny][nx] == OBSTACLE) {
          continue;
        }

        if (nd >= distance_from_goal[ny][nx]) {
          continue;
        }

        distance_from_goal[ny][nx] = nd;
        que.push(Square(ny, nx));
      }
    }
  }

  int DFS(int depth, int depth_max, Action prev_action, int known_score_max = -INF) {
    assert(players[0].square.y < state.height);
    assert(players[0].square.x >= 0);
    assert(players[0].square.x < state.width);

    // 探索深さ上限到達
    if (players[0].square.y >= state.height - 1 || depth >= depth_max) {
      return Eval(depth);
    }

    // TranspositionTableに格納されている状態なら、それを利用
    TranspositionNode* ttnode = TranspositionTable::Withdraw(players[0]);
    if (ttnode->used && ttnode->depth >= (depth_max - depth)) {
      return ttnode->score;
    }

    int score_max = -INF;

    bool didnt_move_flag = false;

    Player temp_players[2] = { players[0], players[1] };

    bool prev_visited = visited[players[0].square.y][players[0].square.x];
    visited[players[0].square.y][players[0].square.x] = true;

    for (auto action : ALL_ACTION) {
      // 枝刈り
      // スタート地点より手前に行かない
      if (action.y + players[0].square.y + players[0].velocity.y < 0) {
        continue;
      }
      // 水溜りでは、何もしない、をしない
      if (state.cells[players[0].square.y][players[0].square.x] == PUDDLE) {
        if (IsZeroAction(action)) {
          continue;
        }
      }
      // 加速しすぎは探索しない
      if (std::abs(players[0].velocity.y) > 15 || std::abs(players[0].velocity.x) > 15) {
        continue;
      }

      int score = INF;

      // rootかつ相手との距離が近いときは、相手のことも考慮
      if (depth == 0 && L1norm(players[0].square - players[1].square) <= L1norm(players[0].velocity - players[1].velocity)) {
        int block_count = 0;
        int my_block_count = 0;

        for (auto op_action : ALL_ACTION) {
          if (IsZeroAction(op_action)) {
            continue;
          }
          // Do
          Action actions[2] = { action, op_action };
          state.Simulate(players, actions);

          bool didnt_move = (IsZeroAction(players[0].velocity) && players[0].square == temp_players[0].square);
          bool op_didnt_move = (players[1].square == temp_players[1].square);
          int temp_score = DFS(depth + 1, depth_max - 2, action, score_max);

          // 相手を動けなくする行動は強い
          if (op_didnt_move) {
            temp_score += 90;
            block_count++;
          }
          if (didnt_move) {
            temp_score -= 90;
            my_block_count++;
          }

          if (temp_score < score) {
            score = temp_score;
          }

          // Undo
          players[0] = temp_players[0];
          players[1] = temp_players[1];

          // 動けなくされるより悪い行動はない
          // 動けなくされたら、次のactionを調べにいく
          if (didnt_move && !op_didnt_move) {
            break;
          }
        }

        score += 5 * block_count;
        score -= my_block_count;
      } else {
        // 3手目以降は相手を考慮しない
        if (depth >= 2) {
          players[1].square = Square(-1, -1);
        }

        // Do
        Action actions[2] = { action, Action() };
        state.Simulate(players, actions);

        bool didnt_move = (IsZeroAction(players[0].velocity) && players[0].square == temp_players[0].square);

        if (didnt_move) {
          if (didnt_move_flag) {
            // 既に動かない行動を試した場合には、もう試す必要はない
            players[0] = temp_players[0];
            players[1] = temp_players[1];

            continue;
          }
          didnt_move_flag = true;
        }

        score = DFS(depth + 1, depth_max, action, score_max);

        if (state.cells[players[0].square.y][players[0].square.x] == PUDDLE) {
          score -= 1;
        }
        if (players[0].square.x == 0 && players[0].velocity.x < 0) {
          score -= 1;
        } else if (players[0].square.x == state.width - 1 && players[0].velocity.x > 0) {
          score -= 1;
        }
        if (IsZeroAction(players[0].velocity)) {
          score -= 5;
        }

        // Undo
        players[0] = temp_players[0];
        players[1] = temp_players[1];
      }

      if (score > score_max) {
        score_max = score;
        if (depth == 0) {
          next_action = action;
        }
      }
    }

    visited[players[0].square.y][players[0].square.x] = prev_visited;

    // TranspositionTableに格納
    TranspositionTable::Register(players[0], depth_max - depth, score_max);

    return score_max;
  }
};


}  // namespace

Action Think::nyashiki_Agent() {
  TranspositionTable::Clear();

  Search search(*Game::current_state, Game::players);

  if (search.players[1].square.y >= search.state.height - 1) {
    search.players[1].square = Square(-1, -1);
  }

  search.StateAugmentation(true);
  search.CalculateDistance();

  bool is_unreachable = (distance_from_goal[search.players[0].square.y][search.players[0].square.x] == INF);

  if (is_unreachable) {
    // 強いOBSTACLEの補間によってゴールに到達不可能になった場合には、OBSTALCEの補間なしで再度距離を計算
    search = Search(*Game::current_state, Game::players);
    search.StateAugmentation(false);
    search.CalculateDistance();
  }

  if (Game::remain_time < 2000) {
    // 残り思考時間が2秒を切ったら、急ぐ
    search.DFS(0, 4, INVALID_ACTION);
  } else {
    Stopwatch sw;
    sw.Start();
    search.DFS(0, 7, INVALID_ACTION);
    // 時間に余裕がある場合には、depth 8で探索
    if (sw.Elapsed() < 200 / 9) {
      search.DFS(0, 8, INVALID_ACTION);
    }
  }

  visited[Game::players[0].square.y][Game::players[0].square.x] = true;

  return search.next_action;
}
