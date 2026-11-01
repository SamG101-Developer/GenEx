#include <gtest/gtest.h>
#include <list>
#include <set>
#include <string>
#include <vector>

import genex.actions.take;
import genex.actions.take_while;

namespace {
  constexpr auto is_small = [](const int x) { return x < 3; };
  auto make_vec() -> std::vector<int> { return {1, 2, 3, 4}; }
}

TEST(GenexActionsTake, KeepsTheFirstElements) {
  auto vec = make_vec();
  vec |= genex::actions::take(2);
  EXPECT_EQ(vec, (std::vector{1, 2}));
}

TEST(GenexActionsTake, CalledDirectly) {
  auto vec = make_vec();
  static_cast<void>(genex::actions::take(vec, 3));
  EXPECT_EQ(vec, (std::vector{1, 2, 3}));
}

TEST(GenexActionsTake, ZeroEmptiesTheRange) {
  auto vec = make_vec();
  vec |= genex::actions::take(0);
  EXPECT_TRUE(vec.empty());
}

TEST(GenexActionsTake, MoreThanTheLengthKeepsEverything) {
  auto vec = make_vec();
  vec |= genex::actions::take(100);
  EXPECT_EQ(vec, make_vec());
}

TEST(GenexActionsTake, EmptyInput) {
  auto vec = std::vector<int>{};
  vec |= genex::actions::take(2);
  EXPECT_TRUE(vec.empty());
}

TEST(GenexActionsTake, OtherContainers) {
  auto list = std::list{1, 2, 3};
  list |= genex::actions::take(2);
  EXPECT_EQ(list, (std::list{1, 2}));

  auto set = std::set{1, 2, 3};
  set |= genex::actions::take(1);
  EXPECT_EQ(set, (std::set{1}));

  auto strings = std::vector<std::string>{"a", "b", "c"};
  strings |= genex::actions::take(2);
  EXPECT_EQ(strings, (std::vector<std::string>{"a", "b"}));
}

TEST(GenexActionsTake, OnAnRvalue) {
  const auto vec = make_vec() | genex::actions::take(2);
  EXPECT_EQ(vec, (std::vector{1, 2}));
}

TEST(GenexActionsTakeWhile, KeepsTheLeadingMatches) {
  auto vec = make_vec();
  vec |= genex::actions::take_while(is_small);
  EXPECT_EQ(vec, (std::vector{1, 2}));
}

TEST(GenexActionsTakeWhile, CalledDirectly) {
  auto vec = make_vec();
  static_cast<void>(genex::actions::take_while(vec, is_small));
  EXPECT_EQ(vec, (std::vector{1, 2}));
}

TEST(GenexActionsTakeWhile, StopsAtTheFirstThatDoesNotMatch) {
  auto vec = std::vector{1, 9, 1};
  vec |= genex::actions::take_while(is_small);
  EXPECT_EQ(vec, (std::vector{1}));
}

TEST(GenexActionsTakeWhile, NoneAndAllMatching) {
  auto none = std::vector{5, 6};
  none |= genex::actions::take_while(is_small);
  EXPECT_TRUE(none.empty());

  auto all = std::vector{0, 1};
  all |= genex::actions::take_while(is_small);
  EXPECT_EQ(all, (std::vector{0, 1}));
}

TEST(GenexActionsTakeWhile, EmptyInput) {
  auto vec = std::vector<int>{};
  vec |= genex::actions::take_while(is_small);
  EXPECT_TRUE(vec.empty());
}

TEST(GenexActionsTakeWhile, OtherContainers) {
  auto list = std::list{1, 2, 3};
  list |= genex::actions::take_while(is_small);
  EXPECT_EQ(list, (std::list{1, 2}));

  auto set = std::set{1, 2, 3};
  set |= genex::actions::take_while(is_small);
  EXPECT_EQ(set, (std::set{1, 2}));
}

TEST(GenexActionsTakeWhile, OnAnRvalue) {
  const auto vec = make_vec() | genex::actions::take_while(is_small);
  EXPECT_EQ(vec, (std::vector{1, 2}));
}
