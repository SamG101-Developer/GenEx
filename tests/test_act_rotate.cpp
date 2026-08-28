#include <coroutine>
#include <gtest/gtest.h>

import genex.actions.rotate;

TEST(GenexActionsRotate, VecInput) {
  auto vec = std::vector{1, 2, 3, 4, 5};
  vec |= genex::actions::rotate(2);
  const auto exp = std::vector{3, 4, 5, 1, 2};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsRotate, RotateByZeroIsIdentity) {
  auto vec = std::vector{1, 2, 3};
  vec |= genex::actions::rotate(0);
  const auto exp = std::vector{1, 2, 3};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsRotate, RotateBeyondEndIsClamped) {
  auto vec = std::vector{1, 2, 3};
  vec |= genex::actions::rotate(5);
  const auto exp = std::vector{1, 2, 3};
  EXPECT_EQ(vec, exp);
}
