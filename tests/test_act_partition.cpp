#include <coroutine>
#include <gtest/gtest.h>

import genex.actions.partition;

TEST(GenexActionsPartition, EvensBeforeOdds) {
  auto vec = std::vector{1, 2, 3, 4, 5, 6};
  vec |= genex::actions::partition([](const int x) { return x % 2 == 0; });
  for (auto i = 0uz; i < 3uz; ++i) { EXPECT_EQ(vec[i] % 2, 0); }
  for (auto i = 3uz; i < 6uz; ++i) { EXPECT_EQ(vec[i] % 2, 1); }
}

TEST(GenexActionsStablePartition, KeepsRelativeOrder) {
  auto vec = std::vector{1, 2, 3, 4, 5, 6};
  vec |= genex::actions::stable_partition([](const int x) { return x % 2 == 0; });
  const auto exp = std::vector{2, 4, 6, 1, 3, 5};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsPartition, EmptyInput) {
  auto vec = std::vector<int>{};
  vec |= genex::actions::partition([](const int x) { return x > 0; });
  EXPECT_TRUE(vec.empty());
}
