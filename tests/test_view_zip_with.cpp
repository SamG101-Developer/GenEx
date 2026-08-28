#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.zip_with;

TEST(GenexViewsZipWith, VecInput) {
  auto vec1 = std::vector{1, 2, 3};
  auto vec2 = std::vector{10, 20, 30};
  const auto rng = vec1
    | genex::views::zip_with(vec2, [](const int a, const int b) { return a + b; })
    | genex::to<std::vector>();
  const auto exp = std::vector{11, 22, 33};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsZipWith, StopsAtShorterRange) {
  auto vec1 = std::vector{1, 2, 3, 4};
  auto vec2 = std::vector{10, 20};
  const auto rng = genex::views::zip_with(vec1, vec2, [](const int a, const int b) { return a * b; })
    | genex::to<std::vector>();
  const auto exp = std::vector{10, 40};
  EXPECT_EQ(rng, exp);
}
