#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.chunk_by;

TEST(GenexViewsChunkBy, RunsOfEqualElements) {
  auto vec = std::vector{1, 1, 2, 3, 3, 3};
  const auto rng = vec
    | genex::views::chunk_by([](const int a, const int b) { return a == b; })
    | genex::to<std::vector>();
  const auto exp = std::vector<std::vector<int>>{{1, 1}, {2}, {3, 3, 3}};
  ASSERT_EQ(rng.size(), exp.size());
  for (auto i = 0uz; i < rng.size(); ++i) {
    EXPECT_EQ(rng[i] | genex::to<std::vector>(), exp[i]);
  }
}

TEST(GenexViewsChunkBy, AscendingRuns) {
  auto vec = std::vector{1, 2, 3, 1, 2, 0};
  const auto rng = vec
    | genex::views::chunk_by([](const int a, const int b) { return a <= b; })
    | genex::to<std::vector>();
  const auto exp = std::vector<std::vector<int>>{{1, 2, 3}, {1, 2}, {0}};
  ASSERT_EQ(rng.size(), exp.size());
  for (auto i = 0uz; i < rng.size(); ++i) {
    EXPECT_EQ(rng[i] | genex::to<std::vector>(), exp[i]);
  }
}

TEST(GenexViewsChunkBy, EmptyInput) {
  auto vec = std::vector<int>{};
  const auto rng = vec
    | genex::views::chunk_by([](const int a, const int b) { return a == b; })
    | genex::to<std::vector>();
  EXPECT_TRUE(rng.empty());
}