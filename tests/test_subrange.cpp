#include <gtest/gtest.h>

import genex.concepts;
import genex.subrange;
import std;

TEST(GenexSubrange, FromIterators) {
  auto vec = std::vector{1, 2, 3, 4};
  const auto s = genex::subrange(vec.begin() + 1, vec.end());
  EXPECT_TRUE((std::same_as<decltype(s), const genex::subrange<std::vector<int>::iterator>>));
  EXPECT_EQ(s.size(), 3uz);
  EXPECT_EQ(s.front(), 2);
  EXPECT_EQ(s.back(), 4);
}

TEST(GenexSubrange, DeducesFromARange) {
  auto vec = std::vector{1, 2, 3};
  const auto s = genex::subrange(vec);
  EXPECT_TRUE((std::same_as<decltype(s), const genex::subrange<std::vector<int>::iterator>>));
  EXPECT_EQ(s.size(), 3uz);
  EXPECT_EQ(s[1], 2);

  const auto &cref = vec;
  const auto c = genex::subrange(cref);
  EXPECT_TRUE((std::same_as<decltype(c), const genex::subrange<std::vector<int>::const_iterator>>));
  EXPECT_EQ(c.data(), vec.data());
}

TEST(GenexSubrange, FromARangeOfAnotherSentinel) {
  // An iterator and sentinel of different types, as a view's are.
  auto vec = std::vector{1, 2, 3};
  auto first = genex::subrange(vec);
  const auto again = genex::subrange(first);
  EXPECT_TRUE((std::same_as<decltype(again), const decltype(first)>));
  EXPECT_EQ(again.size(), 3uz);
}

TEST(GenexSubrange, OnlyFromARangeThatDoesNotDangle) {
  using sub = genex::subrange<std::vector<int>::iterator>;
  EXPECT_TRUE((std::constructible_from<sub, std::vector<int>&>));
  EXPECT_FALSE((std::constructible_from<sub, std::vector<int>&&>));
}
