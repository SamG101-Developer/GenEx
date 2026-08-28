#include <coroutine>
#include <list>
#include <gtest/gtest.h>

import genex.actions.unique;

TEST(GenexActionsUnique, VecInput) {
  auto vec = std::vector{1, 1, 2, 2, 2, 3, 1, 1};
  vec |= genex::actions::unique;
  const auto exp = std::vector{1, 2, 3, 1};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsUnique, NoAdjacentDuplicates) {
  auto vec = std::vector{1, 2, 3};
  vec |= genex::actions::unique;
  const auto exp = std::vector{1, 2, 3};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsUnique, EmptyInput) {
  auto vec = std::vector<int>{};
  vec |= genex::actions::unique;
  EXPECT_TRUE(vec.empty());
}

TEST(GenexActionsUnique, ListInput) {
  auto list = std::list{1, 1, 2, 3, 3};
  list |= genex::actions::unique;
  const auto exp = std::list{1, 2, 3};
  EXPECT_EQ(list, exp);
}
