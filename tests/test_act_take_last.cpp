#include <coroutine>
#include <gtest/gtest.h>

import genex.actions.drop_last;
import genex.actions.take_last;

TEST(GenexActionsTakeLast, VecInput) {
  auto vec = std::vector{1, 2, 3, 4, 5};
  vec |= genex::actions::take_last(2);
  const auto exp = std::vector{4, 5};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsTakeLast, MoreThanLengthKeepsAll) {
  auto vec = std::vector{1, 2, 3};
  vec |= genex::actions::take_last(10);
  const auto exp = std::vector{1, 2, 3};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsDropLast, VecInput) {
  auto vec = std::vector{1, 2, 3, 4, 5};
  vec |= genex::actions::drop_last(2);
  const auto exp = std::vector{1, 2, 3};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsDropLast, MoreThanLengthEmptiesRange) {
  auto vec = std::vector{1, 2, 3};
  vec |= genex::actions::drop_last(10);
  EXPECT_TRUE(vec.empty());
}
