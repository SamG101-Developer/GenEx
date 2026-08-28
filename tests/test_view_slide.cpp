#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.slide;

TEST(GenexViewsSlide, VecInput) {
  auto vec = std::vector{1, 2, 3, 4};
  const auto rng = vec
    | genex::views::slide(2)
    | genex::to<std::vector>();
  const auto exp = std::vector<std::vector<int>>{{1, 2}, {2, 3}, {3, 4}};
  ASSERT_EQ(rng.size(), exp.size());
  for (auto i = 0uz; i < rng.size(); ++i) {
    EXPECT_EQ(rng[i] | genex::to<std::vector>(), exp[i]);
  }
}

TEST(GenexViewsSlide, WindowEqualToLength) {
  auto vec = std::vector{1, 2, 3};
  const auto rng = vec
    | genex::views::slide(3)
    | genex::to<std::vector>();
  ASSERT_EQ(rng.size(), 1uz);
  EXPECT_EQ(rng[0] | genex::to<std::vector>(), vec);
}

TEST(GenexViewsSlide, WindowLongerThanRange) {
  auto vec = std::vector{1, 2, 3};
  const auto rng = vec
    | genex::views::slide(4)
    | genex::to<std::vector>();
  EXPECT_TRUE(rng.empty());
}