#include <coroutine>
#include <gtest/gtest.h>

import genex.actions.fill;
import genex.actions.transform;

TEST(GenexActionsTransform, VecInput) {
  auto vec = std::vector{1, 2, 3};
  vec |= genex::actions::transform([](const int x) { return x * 2; });
  const auto exp = std::vector{2, 4, 6};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsTransform, StrInput) {
  auto str = std::string("abc");
  str |= genex::actions::transform([](const char c) { return static_cast<char>(c - 32); });
  const auto exp = std::string("ABC");
  EXPECT_EQ(str, exp);
}

TEST(GenexActionsFill, VecInput) {
  auto vec = std::vector{1, 2, 3};
  vec |= genex::actions::fill(0);
  const auto exp = std::vector{0, 0, 0};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsFill, EmptyInput) {
  auto vec = std::vector<int>{};
  vec |= genex::actions::fill(0);
  EXPECT_TRUE(vec.empty());
}
