#include <coroutine>
#include <gtest/gtest.h>

import genex.concepts;
import genex.span;
import std;

TEST(GenexSpan, FromPointerAndSize) {
  auto arr = std::array{1, 2, 3};
  const auto s = genex::span<int>(arr.data(), arr.size());
  EXPECT_EQ(s.size(), 3uz);
  EXPECT_EQ(s.front(), 1);
  EXPECT_EQ(s.back(), 3);
}

TEST(GenexSpan, FromVector) {
  auto vec = std::vector{1, 2, 3, 4};
  const genex::span<int> s = vec;
  EXPECT_EQ(s.data(), vec.data());
  EXPECT_EQ(s.size(), 4uz);
  s[0] = 9;
  EXPECT_EQ(vec[0], 9);
}

TEST(GenexSpan, ConstFromVector) {
  const auto vec = std::vector{1, 2, 3};
  const genex::span<int const> s = vec;
  EXPECT_EQ(s.size(), 3uz);
  EXPECT_EQ(s[2], 3);
}

TEST(GenexSpan, ConstFromMutable) {
  auto vec = std::vector{1, 2, 3};
  const auto s = genex::span<int>(vec);
  const genex::span<int const> c = s;
  EXPECT_EQ(c.data(), vec.data());
  EXPECT_EQ(c.size(), 3uz);
}

TEST(GenexSpan, RvalueOnlyForConst) {
  EXPECT_TRUE((std::constructible_from<genex::span<int const>, std::vector<int>&&>));
  EXPECT_FALSE((std::constructible_from<genex::span<int>, std::vector<int>&&>));
  EXPECT_FALSE((std::constructible_from<genex::span<int>, std::vector<int> const&>));
}

TEST(GenexSpan, FirstLastSubspan) {
  auto vec = std::vector{1, 2, 3, 4, 5};
  const auto s = genex::span<int>(vec);
  EXPECT_EQ(s.first(2).size(), 2uz);
  EXPECT_EQ(s.first(2).back(), 2);
  EXPECT_EQ(s.last(2).front(), 4);
  EXPECT_EQ(s.subspan(1).size(), 4uz);
  EXPECT_EQ(s.subspan(1).front(), 2);
  EXPECT_EQ(s.subspan(1, 2).back(), 3);
  EXPECT_TRUE(s.subspan(5).empty());
}

TEST(GenexSpan, IsAContiguousRange) {
  auto vec = std::vector{1, 2, 3};
  const auto s = genex::span<int>(vec);
  EXPECT_TRUE(genex::contiguous_range<genex::span<int>>);
  auto sum = 0;
  for (const auto x : s) { sum += x; }
  EXPECT_EQ(sum, 6);
}
