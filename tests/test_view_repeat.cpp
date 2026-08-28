#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.empty;
import genex.views2.repeat;
import genex.views2.single;
import genex.views2.take;

TEST(GenexViewsRepeat, CountedRepeat) {
  const auto rng = genex::views::repeat(7, 3)
    | genex::to<std::vector>();
  const auto exp = std::vector{7, 7, 7};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsRepeat, ZeroCountIsEmpty) {
  const auto rng = genex::views::repeat(7, 0)
    | genex::to<std::vector>();
  EXPECT_TRUE(rng.empty());
}

TEST(GenexViewsRepeat, UnboundedRepeatIsTakeable) {
  const auto rng = genex::views::repeat(std::string("ab"))
    | genex::views::take(2)
    | genex::to<std::vector>();
  const auto exp = std::vector<std::string>{"ab", "ab"};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsSingle, SingleElement) {
  const auto rng = genex::views::single(42)
    | genex::to<std::vector>();
  const auto exp = std::vector{42};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsEmpty, NoElements) {
  const auto rng = genex::views::empty<int>
    | genex::to<std::vector>();
  EXPECT_TRUE(rng.empty());
}