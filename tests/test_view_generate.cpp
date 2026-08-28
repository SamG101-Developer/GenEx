#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.generate;
import genex.views2.take;

TEST(GenexViewsGenerateN, CountedGenerate) {
  auto next = 0;
  const auto rng = genex::views::generate_n([next]() mutable { return next++; }, 4)
    | genex::to<std::vector>();
  const auto exp = std::vector{0, 1, 2, 3};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsGenerateN, ZeroCountIsEmpty) {
  const auto rng = genex::views::generate_n([] { return 1; }, 0)
    | genex::to<std::vector>();
  EXPECT_TRUE(rng.empty());
}

TEST(GenexViewsGenerate, UnboundedGenerateIsTakeable) {
  auto next = 0;
  const auto rng = genex::views::generate([next]() mutable { return next += 2; })
    | genex::views::take(3)
    | genex::to<std::vector>();
  const auto exp = std::vector{2, 4, 6};
  EXPECT_EQ(rng, exp);
}
