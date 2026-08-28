#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.pairwise;

TEST(GenexViewsPairwise, VecInput) {
  auto vec = std::vector{1, 2, 4, 7};
  const auto rng = vec
    | genex::views::pairwise
    | genex::to<std::vector>();
  const auto exp = std::vector<std::pair<int, int>>{{1, 2}, {2, 4}, {4, 7}};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsPairwise, SingleElementHasNoPairs) {
  auto vec = std::vector{1};
  const auto rng = vec
    | genex::views::pairwise
    | genex::to<std::vector>();
  EXPECT_TRUE(rng.empty());
}

TEST(GenexViewsPairwise, EmptyInput) {
  auto vec = std::vector<int>{};
  const auto rng = vec
    | genex::views::pairwise
    | genex::to<std::vector>();
  EXPECT_TRUE(rng.empty());
}