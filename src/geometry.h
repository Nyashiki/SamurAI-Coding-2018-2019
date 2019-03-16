#ifndef GEOMETRY_H_
#define GEOMETRY_H_

struct Point {
  double y, x;

  Point() = default;
  constexpr Point(double y, double x): y(y), x(x) { }
};

inline Point operator-(const Point& p1, const Point& p2) {
  return Point(p1.y - p2.y, p1.x - p2.x);
}

// 2次元ベクトル同士の外積を計算する。
inline double CrossProduct(Point p1, Point p2) {
  return p1.x * p2.y - p1.y * p2.x;
}

// 2次元ベクトル同士の内積を計算する。
inline double InnerProduct(Point p1, Point p2) {
  return p1.x * p2.x + p1.y * p2.y;
}

// 点を2次元ベクトルとしてみたときのノルムを計算
inline double Norm(Point p) {
  return p.x * p.x + p.y * p.y;
}

/*
                    CCW

 -- BEHIND -- [a -- ON -- b] --- FRONT --

                    CW
 */
enum CCWResult {
  CCW = +1, CW = -1, BEHIND = +2, FRONT = -2, ON = 0
};

inline CCWResult GetCCW(Point a, Point b, Point c) {
  const double kEPS = 1e-6;

  b = b - a;
  c = c - a;
  if (CrossProduct(b, c) > kEPS) return CCW;    // counter clockwise
  if (CrossProduct(b, c) < -kEPS) return CW;    // clockwise
  if (InnerProduct(b, c) < 0) return BEHIND;     // c--a--b on line
  if (Norm(b) < Norm(c)) return FRONT;  // a--b--c on line
  return ON;
}

// 線分a1->a2, b1->b2 が交差するかを調べる
inline bool DoesCross(Point a1, Point a2, Point b1, Point b2) {
  return GetCCW(a1, a2, b1) * GetCCW(a1, a2, b2) <= 0 &&
          GetCCW(b1, b2, a1) * GetCCW(b1, b2, a2) <= 0;
}

#endif // GEOMETRY_H_
