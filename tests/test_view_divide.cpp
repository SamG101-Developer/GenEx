#include <gtest/gtest.h>
#include <coroutine>
#include <list>

import genex.to_container;
import genex.views2.divide;

TEST(GenexViewsDivide, VecInputEven) {
  auto vec = std::vector{0, 1, 2, 3, 4, 5};

  const auto [a, b] = vec | genex::views::divide<2>;
  EXPECT_EQ(a | genex::to<std::vector>(), (std::vector{0, 1, 2}));
  EXPECT_EQ(b | genex::to<std::vector>(), (std::vector{3, 4, 5}));
}

TEST(GenexViewsDivide, VecInputUneven) {
  auto vec = std::vector{0, 1, 2, 3, 4, 5, 6};

  const auto [a, b, c] = vec | genex::views::divide<3>;
  EXPECT_EQ(a | genex::to<std::vector>(), (std::vector{0, 1, 2}));
  EXPECT_EQ(b | genex::to<std::vector>(), (std::vector{3, 4}));
  EXPECT_EQ(c | genex::to<std::vector>(), (std::vector{5, 6}));
}

TEST(GenexViewsDivide, VecInputCall) {
  auto vec = std::vector{0, 1, 2, 3};

  const auto [a, b] = vec | genex::views::divide<2>();
  EXPECT_EQ(a | genex::to<std::vector>(), (std::vector{0, 1}));
  EXPECT_EQ(b | genex::to<std::vector>(), (std::vector{2, 3}));
}

TEST(GenexViewsDivide, VecInputMorePartsThanElements) {
  auto vec = std::vector{0, 1};

  const auto [a, b, c, d] = vec | genex::views::divide<4>;
  EXPECT_EQ(a | genex::to<std::vector>(), (std::vector{0}));
  EXPECT_EQ(b | genex::to<std::vector>(), (std::vector{1}));
  EXPECT_TRUE(c.empty());
  EXPECT_TRUE(d.empty());
}

TEST(GenexViewsDivide, VecInputSpansAlias) {
  auto vec = std::vector{0, 1, 2, 3};

  const auto [a, b] = vec | genex::views::divide<2>;
  b[0] = 100;
  EXPECT_EQ(vec, (std::vector{0, 1, 100, 3}));
  EXPECT_EQ(a.data(), vec.data());
  EXPECT_EQ(b.data(), vec.data() + 2);
}

TEST(GenexViewsDivide, EmptyInput) {
  auto vec = std::vector<int>{};

  const auto [a, b] = vec | genex::views::divide<2>;
  EXPECT_TRUE(a.empty());
  EXPECT_TRUE(b.empty());
}

TEST(GenexViewsDivide, SinglePart) {
  auto vec = std::vector{0, 1, 2};

  const auto [a] = vec | genex::views::divide<1>;
  EXPECT_EQ(a | genex::to<std::vector>(), (std::vector{0, 1, 2}));
}

TEST(GenexViewsDivide, ListInput) {
  auto lst = std::list{0, 1, 2, 3, 4};

  const auto [a, b] = lst | genex::views::divide<2>;
  EXPECT_EQ(a | genex::to<std::vector>(), (std::vector{0, 1, 2}));
  EXPECT_EQ(b | genex::to<std::vector>(), (std::vector{3, 4}));
}

TEST(GenexViewsDivide, IterInput) {
  auto vec = std::vector{0, 1, 2, 3, 4, 5};

  const auto [a, b] = genex::views::divide<2>(vec.begin(), vec.end());
  EXPECT_EQ(a | genex::to<std::vector>(), (std::vector{0, 1, 2}));
  EXPECT_EQ(b | genex::to<std::vector>(), (std::vector{3, 4, 5}));
}