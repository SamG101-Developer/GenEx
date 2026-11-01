#include <gtest/gtest.h>
#include <list>
#include <set>
#include <vector>

import genex.actions.drop_while;

namespace {
  constexpr auto is_small = [](const int x) { return x < 3; };
  auto make_vec() -> std::vector<int> { return {1, 2, 3, 4}; }
}

TEST(GenexActionsDropWhile, RemovesTheLeadingMatches) {
  auto vec = make_vec();
  vec |= genex::actions::drop_while(is_small);
  EXPECT_EQ(vec, (std::vector{3, 4}));
}

TEST(GenexActionsDropWhile, CalledDirectly) {
  auto vec = make_vec();
  static_cast<void>(genex::actions::drop_while(vec, is_small));
  EXPECT_EQ(vec, (std::vector{3, 4}));
}

TEST(GenexActionsDropWhile, KeepsLaterMatches) {
  auto vec = std::vector{1, 9, 1};
  vec |= genex::actions::drop_while(is_small);
  EXPECT_EQ(vec, (std::vector{9, 1}));
}

TEST(GenexActionsDropWhile, NoneAndAllMatching) {
  auto none = std::vector{5, 6};
  none |= genex::actions::drop_while(is_small);
  EXPECT_EQ(none, (std::vector{5, 6}));

  auto all = std::vector{0, 1};
  all |= genex::actions::drop_while(is_small);
  EXPECT_TRUE(all.empty());
}

TEST(GenexActionsDropWhile, EmptyInput) {
  auto vec = std::vector<int>{};
  vec |= genex::actions::drop_while(is_small);
  EXPECT_TRUE(vec.empty());
}

TEST(GenexActionsDropWhile, OtherContainers) {
  auto list = std::list{1, 2, 3};
  list |= genex::actions::drop_while(is_small);
  EXPECT_EQ(list, (std::list{3}));

  auto set = std::set{1, 2, 3};
  set |= genex::actions::drop_while(is_small);
  EXPECT_EQ(set, (std::set{3}));
}

TEST(GenexActionsDropWhile, OnAnRvalue) {
  const auto vec = make_vec() | genex::actions::drop_while(is_small);
  EXPECT_EQ(vec, (std::vector{3, 4}));
}
