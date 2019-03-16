#include <gtest/gtest.h>

#include "../geometry.h"

TEST(geometry, handmade_1) {
  Point p1(0, 0);
  Point p2(2, 2);
  Point p3(2, 0);
  Point p4(0, 2);

  ASSERT_TRUE(DoesCross(p1, p2, p3, p4));
}

TEST(geometry, handmade_2) {
  Point p1(0, 0);
  Point p2(0, 1);
  Point p3(1, 0);
  Point p4(1, 1);

  ASSERT_FALSE(DoesCross(p1, p2, p3, p4));
}

TEST(geometry, handmade_3) {
  Point p1(0, 0);
  Point p2(0.0001, 0.0001);
  Point p3(0.0001, 0);
  Point p4(0, 0.0001);

  ASSERT_TRUE(DoesCross(p1, p2, p3, p4));
}
