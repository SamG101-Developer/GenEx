#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.scan;

TEST(GenexViewsPartialSum, VecInput) {
  auto vec = std::vector{1, 2, 3, 4};
  const auto rng = vec
    | genex::views::partial_sum
    | genex::to<std::vector>();
  const auto exp = std::vector{1, 3, 6, 10};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsPartialSum, CustomOperation) {
  auto vec = std::vector{1, 2, 3, 4};
  const auto rng = vec
    | genex::views::partial_sum(std::multiplies{})
    | genex::to<std::vector>();
  const auto exp = std::vector{1, 2, 6, 24};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsPartialSum, EmptyInput) {
  auto vec = std::vector<int>{};
  const auto rng = vec
    | genex::views::partial_sum
    | genex::to<std::vector>();
  EXPECT_TRUE(rng.empty());
}

TEST(GenexViewsScan, SeededScan) {
  auto vec = std::vector{1, 2, 3};
  const auto rng = vec
    | genex::views::scan(100)
    | genex::to<std::vector>();
  const auto exp = std::vector{101, 103, 106};
  EXPECT_EQ(rng, exp);
}
