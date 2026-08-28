#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.stride;

TEST(GenexViewsStride, VecInput) {
  auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto rng = vec
    | genex::views::stride(3)
    | genex::to<std::vector>();
  const auto exp = std::vector{0, 3, 6, 9};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsStride, StrInput) {
  auto str = std::string("Hello, world!");
  const auto rng = str
    | genex::views::stride(2)
    | genex::to<std::string>();
  const auto exp = std::string("Hlo ol!");
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsStride, StepOfOneIsIdentity) {
  auto vec = std::vector{1, 2, 3};
  const auto rng = vec
    | genex::views::stride(1)
    | genex::to<std::vector>();
  EXPECT_EQ(rng, vec);
}

TEST(GenexViewsStride, EmptyInput) {
  auto vec = std::vector<int>{};
  const auto rng = vec
    | genex::views::stride(2)
    | genex::to<std::vector>();
  EXPECT_TRUE(rng.empty());
}