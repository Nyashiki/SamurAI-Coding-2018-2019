#include <gtest/gtest.h>

#include "../state.h"
#include "../player.h"
#include "../types.h"

TEST(state_doescollide, handmade_1) {
  State state(3, 2);
  state.cells[1][0] = OBSTACLE;

  ASSERT_TRUE(state.DoesCollide(Square(0, 0), Square(2, 0)));
  ASSERT_TRUE(state.DoesCollide(Square(2, 0), Square(0, 0)));
}

TEST(state_doescollide, handmade_2) {
  State state(3, 2);
  state.cells[1][0] = OBSTACLE;

  ASSERT_TRUE(state.DoesCollide(Square(0, 0), Square(1, 0)));
  ASSERT_TRUE(state.DoesCollide(Square(1, 0), Square(0, 0)));
}

TEST(state_doescollide, handmade_3) {
  State state(3, 2);
  state.cells[0][1] = OBSTACLE;

  ASSERT_FALSE(state.DoesCollide(Square(0, 0), Square(2, 0)));
  ASSERT_FALSE(state.DoesCollide(Square(2, 0), Square(0, 0)));
}

TEST(state_doescollide, handmade_4) {
  State state(3, 2);
  state.cells[1][0] = OBSTACLE;

  ASSERT_TRUE(state.DoesCollide(Square(0, 0), Square(2, 1)));
  ASSERT_TRUE(state.DoesCollide(Square(2, 1), Square(0, 0)));
}

TEST(state_doescollide, handmade_5) {
  State state(3, 2);
  state.cells[1][1] = OBSTACLE;

  ASSERT_TRUE(state.DoesCollide(Square(0, 0), Square(2, 1)));
  ASSERT_TRUE(state.DoesCollide(Square(2, 1), Square(0, 0)));
}

TEST(state_doescollide, handmade_6) {
  State state(3, 2);
  state.cells[2][0] = OBSTACLE;
  state.cells[0][1] = OBSTACLE;

  ASSERT_FALSE(state.DoesCollide(Square(0, 0), Square(2, 1)));
  ASSERT_FALSE(state.DoesCollide(Square(2, 1), Square(0, 0)));
}

TEST(state_doescollide, handemade_7) {
  State state(2, 2);
  state.cells[1][0] = OBSTACLE;

  ASSERT_TRUE(state.DoesCollide(Square(0, 0), Square(1, 1)));
}

TEST(state_doesprotrude, handmade_1) {
  State state(3, 2);

  ASSERT_FALSE(state.DoesProtrude(Square(0, 0)));
  ASSERT_FALSE(state.DoesProtrude(Square(0, 1)));
  ASSERT_FALSE(state.DoesProtrude(Square(1, 0)));
  ASSERT_FALSE(state.DoesProtrude(Square(1, 1)));
  ASSERT_FALSE(state.DoesProtrude(Square(2, 0)));
  ASSERT_FALSE(state.DoesProtrude(Square(2, 1)));

  ASSERT_TRUE(state.DoesProtrude(Square(0, 2)));
  ASSERT_TRUE(state.DoesProtrude(Square(0, -1)));
}

// お互いが関わりなく移動する場合
TEST(state_simulate, handmade_1) {
  State state(96, 20);
  Player player[2];

  player[0].square = Square(0, 8);
  player[0].velocity = Velocity(0, 0);

  player[1].square = Square(0, 16);
  player[1].velocity = Velocity(0, 0);

  Action action[2];
  action[0] = Action(1, 0);
  action[1] = Action(1, 0);

  state.Simulate(player, action);

  ASSERT_TRUE(player[0].square == Square(1, 8));
  ASSERT_TRUE(player[1].square == Square(1, 16));
  ASSERT_TRUE(player[0].velocity == Velocity(1, 0));
  ASSERT_TRUE(player[1].velocity == Velocity(1, 0));
}

//
// player2によって、player1の移動が防がれる場合
// https://tastasgit.github.io/software-for-SamurAI-Coding-2018-19/documents/rule-jp.html
// のCase 1を参考に生成
//
TEST(state_simulate, handmade_2) {
  State state(96, 20);
  Player player[2];

  player[0].square = Square(1, 8);
  player[0].velocity = Velocity(2, 0);

  player[1].square = Square(0, 9);
  player[1].velocity = Velocity(3, 0);

  Action action[2];
  action[0] = Action(1, 1);
  action[1] = Action(1, -1);

  state.Simulate(player, action);

  ASSERT_TRUE(player[0].square == Square(1, 8));
  ASSERT_TRUE(player[1].square == Square(4, 8));
  ASSERT_TRUE(player[0].velocity == Velocity(0, 0));
  ASSERT_TRUE(player[1].velocity == Velocity(4, -1));
}

//
// 優先権が奪われそうだが、player2の動線がplayer1のスタート地点と
// 交差しているため、player1が優先権を得る場合
// Case 2を参考に生成
//
TEST(state_simulate, handmade_3) {
  State state(96, 20);
  Player player[2];

  player[0].square = Square(1, 8);
  player[0].velocity = Velocity(2, 0);

  player[1].square = Square(0, 9);
  player[1].velocity = Velocity(3, 0);

  Action action[2];
  action[0] = Action(1, 1);
  action[1] = Action(0, -1);

  state.Simulate(player, action);

  ASSERT_TRUE(player[0].square == Square(4, 9));
  ASSERT_TRUE(player[1].square == Square(0, 9));
  ASSERT_TRUE(player[0].velocity == Velocity(3, 1));
  ASSERT_TRUE(player[1].velocity == Velocity(0, 0));
}

//
// player2の動線が、ぎりぎりplayer1の開始地点に接しない場合
// Case 3
//
TEST(state_simulate, handmade_4) {
  State state(96, 20);
  Player player[2];

  player[0].square = Square(1, 8);
  player[0].velocity = Velocity(1, 1);

  player[1].square = Square(0, 9);
  player[1].velocity = Velocity(1, -1);

  Action action[2];
  action[0] = Action(1, 0);
  action[1] = Action(0, -1);

  state.Simulate(player, action);

  ASSERT_TRUE(player[0].square == Square(3, 9));
  ASSERT_TRUE(player[1].square == Square(1, 7));
  ASSERT_TRUE(player[0].velocity == Velocity(2, 1));
  ASSERT_TRUE(player[1].velocity == Velocity(1, -2));
}

//
// どちらも優先権がない場合
// Case 4
//
TEST(state_simulate, handmade_5) {
  State state(96, 20);
  Player player[2];

  player[0].square = Square(1, 8);
  player[0].velocity = Velocity(1, 1);

  player[1].square = Square(1, 9);
  player[1].velocity = Velocity(1, -1);

  Action action[2];
  action[0] = Action(1, 1);
  action[1] = Action(0, -1);

  state.Simulate(player, action);

  ASSERT_TRUE(player[0].square == Square(1, 8));
  ASSERT_TRUE(player[1].square == Square(1, 9));
  ASSERT_TRUE(player[0].velocity == Velocity(0, 0));
  ASSERT_TRUE(player[1].velocity == Velocity(0, 0));
}

//
// 障害物がある場合
//
TEST(state_simulate, handmade_7) {
  State state(96, 20);
  Player player[2];

  player[0].square = Square(0, 8);
  player[0].velocity = Velocity(0, 0);

  player[1].square = Square(0, 16);
  player[1].velocity = Velocity(0, 0);

  state.cells[1][8] = OBSTACLE;

  Action action[2];
  action[0] = Action(1, 0);
  action[1] = Action(1, 0);

  state.Simulate(player, action);

  ASSERT_TRUE(player[0].square == Square(0, 8));
  ASSERT_TRUE(player[1].square == Square(1, 16));
  ASSERT_TRUE(player[0].velocity == Velocity(0, 0));
  ASSERT_TRUE(player[1].velocity == Velocity(1, 0));
}

//
// 水たまりからの移動
//
TEST(state_simulate, handmade_8) {
  State state(96, 20);
  Player player[2];

  player[0].square = Square(0, 8);
  player[0].velocity = Velocity(0, 0);

  player[1].square = Square(0, 16);
  player[1].velocity = Velocity(0, 0);

  state.cells[0][8] = PUDDLE;

  Action action[2];
  action[0] = Action(1, 0);
  action[1] = Action(1, 0);

  state.Simulate(player, action);

  ASSERT_TRUE(player[0].square == Square(1, 8));
  ASSERT_TRUE(player[1].square == Square(1, 16));
  ASSERT_TRUE(player[0].velocity == Velocity(1, 0));
  ASSERT_TRUE(player[1].velocity == Velocity(1, 0));
}
