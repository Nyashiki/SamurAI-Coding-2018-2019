#include <gtest/gtest.h>

#include "../types.h"

TEST(typesh_square_basic_operands, handmade_1) {
  Square s1(1, 2);
  Square s2(3, 4);

  ASSERT_TRUE(s1 + s2 == Square(4, 6));
  ASSERT_TRUE(s2 + s1 == Square(4, 6));
  ASSERT_TRUE(s1 - s2 == Square(-2, -2));
  ASSERT_TRUE(s2 - s1 == Square(2, 2));
}

TEST(typesh_doescross, handmade_1) {
  Square s1(0, 0);
  Square s2(1, 0);
  Square s3(0, 1);
  Square s4(1, 1);

  ASSERT_FALSE(DoesCross(s1, s2, s3, s4));
}

TEST(typesh_doescross, handmade_2) {
  Square s1(0, 0);
  Square s2(0, 1);
  Square s3(1, 0);
  Square s4(1, 1);

  ASSERT_FALSE(DoesCross(s1, s2, s3, s4));
}

TEST(typesh_doescross, handmade_3) {
  Square s1(0, 0);
  Square s2(1, 1);
  Square s3(0, 1);
  Square s4(1, 0);

  ASSERT_TRUE(DoesCross(s1, s2, s3, s4));
}

TEST(typesh_doescross, handmade_4) {
  Square s1(7, 0);
  Square s2(7, 0);
  Square s3(7, 5);
  Square s4(7, 4);

  ASSERT_FALSE(DoesCross(s1, s2, s3, s4));
}

TEST(typesh_doescross, handmade_5) {
  Square s1(7, 1);
  Square s2(7, 0);
  Square s3(7, 5);
  Square s4(7, 4);

  ASSERT_FALSE(DoesCross(s1, s2, s3, s4));
}

TEST(typesh_doescross, handmade_6) {
  Square s1(1, 32);
  Square s2(4, 33);
  Square s3(0, 33);
  Square s4(4, 32);

  ASSERT_TRUE(DoesCross(s1, s2, s3, s4));
}
