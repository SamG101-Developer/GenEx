#include <coroutine>
#include <gtest/gtest.h>

import genex.actions.filter;

TEST(GenexActionsFilter, KeepsMatchingElements) {
  auto vec = std::vector{1, 2, 3, 4, 5, 6};
  vec |= genex::actions::filter([](const int x) { return x % 2 == 0; });
  const auto exp = std::vector{2, 4, 6};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsFilter, NothingMatches) {
  auto vec = std::vector{1, 3, 5};
  vec |= genex::actions::filter([](const int x) { return x % 2 == 0; });
  EXPECT_TRUE(vec.empty());
}

TEST(GenexActionsFilter, EverythingMatches) {
  auto vec = std::vector{2, 4};
  vec |= genex::actions::filter([](const int x) { return x % 2 == 0; });
  const auto exp = std::vector{2, 4};
  EXPECT_EQ(vec, exp);
}
