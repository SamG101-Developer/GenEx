#include <list>
#include <gtest/gtest.h>
#include <set>

import genex.actions.unique;

TEST(GenexActionsUnique, VecInput) {
  auto vec = std::vector{1, 1, 2, 2, 2, 3, 1, 1};
  vec |= genex::actions::unique;
  const auto exp = std::vector{1, 2, 3, 1};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsUnique, NoAdjacentDuplicates) {
  auto vec = std::vector{1, 2, 3};
  vec |= genex::actions::unique;
  const auto exp = std::vector{1, 2, 3};
  EXPECT_EQ(vec, exp);
}

TEST(GenexActionsUnique, EmptyInput) {
  auto vec = std::vector<int>{};
  vec |= genex::actions::unique;
  EXPECT_TRUE(vec.empty());
}

TEST(GenexActionsUnique, ListInput) {
  auto list = std::list{1, 1, 2, 3, 3};
  list |= genex::actions::unique;
  const auto exp = std::list{1, 2, 3};
  EXPECT_EQ(list, exp);
}

// A multiset's elements can't be assigned to, so these are erased one at a time.
TEST(GenexActionsUnique, MultisetInput) {
  auto set = std::multiset{1, 1, 2, 3, 3, 3};
  set |= genex::actions::unique;
  EXPECT_EQ(set, (std::multiset{1, 2, 3}));
}

TEST(GenexActionsUnique, MultisetWithAComparison) {
  auto set = std::multiset{1, 2, 3, 4, 8};
  set |= genex::actions::unique([](const int a, const int b) { return a / 2 == b / 2; });
  EXPECT_EQ(set, (std::multiset{1, 2, 4, 8}));
}

TEST(GenexActionsUnique, EmptyMultiset) {
  auto set = std::multiset<int>{};
  set |= genex::actions::unique;
  EXPECT_TRUE(set.empty());
}
