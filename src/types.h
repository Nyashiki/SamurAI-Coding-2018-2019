#include "geometry.h"

#ifndef TYPES_H_
#define TYPES_H_

enum Cell {
  EMPTY = 0, OBSTACLE = 1, PUDDLE = 2, UNVISIBLE = -1
};

enum PlayerType {
  ME, OPPONENT, NO_PLAYER_TYPE
};

inline PlayerType GetOpPlayerType(const PlayerType& pt) {
  if (pt == ME) {
    return OPPONENT;
  } else if (pt == OPPONENT) {
    return ME;
  }

  return NO_PLAYER_TYPE;
}


/*
 * 盤面上の座標を表す構造体。
 * Square(0,0)の4隅の点は2次元平面上の点(y,x)=(0,0),(1,0),(1,1),(0,1)に相当する。
 */
struct Square {
  int y, x;

  Square() = default;
  constexpr Square(int y, int x): y(y), x(x) { }
};
inline Square operator+(const Square& s1, const Square& s2) {
  return Square(s1.y + s2.y, s1.x + s2.x);
}
inline Square operator-(const Square& s1, const Square& s2) {
  return Square(s1.y - s2.y, s1.x - s2.x);
}
inline Square operator+=(Square& s1, const Square s2) {
  return s1 = Square(s1.y + s2.y, s1.x + s2.x);
}
inline Square operator-=(Square& s1, const Square s2) {
  return s1 = Square(s1.y - s2.y, s1.x - s2.x);
}
inline bool operator==(const Square& s1, const Square& s2) {
  return s1.y == s2.y and s1.x == s2.x;
}
inline bool operator!=(const Square& s1, const Square& s2) {
  return s1.y != s2.y or s1.x != s2.x;
}

// 動線a1->a2, b1->b2 が交差するかを調べる
// ルールに愚直に従うならセルの中心の二次元座標をもとに交差判定をするべきだが、
// 互いにセルの左下の点を使って交差判定をすると考えると、この関数の結果と二次元座標を元に計算した結果は等価である。
inline bool DoesCross(Square a1, Square a2, Square b1, Square b2) {
  return DoesCross(Point(a1.y, a1.x), Point(a2.y, a2.x), Point(b1.y, b1.x), Point(b2.y, b2.x));
}

using Action = Square;
using Velocity = Square;

#endif  // TYPES_H_
