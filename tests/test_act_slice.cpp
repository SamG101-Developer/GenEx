#include <gtest/gtest.h>
#include <list>
#include <set>
#include <vector>

import genex.actions.slice;

namespace {
  auto make_vec() -> std::vector<int> { return {0, 1, 2, 3, 4}; }
}

TEST(GenexActionsSlice, KeepsARange) {
  auto vec = make_vec();
  vec |= genex::actions::slice(1, 3);
  EXPECT_EQ(vec, (std::vector{1, 2}));
}

TEST(GenexActionsSlice, FromAPositionToTheEnd) {
  auto vec = make_vec();
  vec |= genex::actions::slice(3);
  EXPECT_EQ(vec, (std::vector{3, 4}));
}

TEST(GenexActionsSlice, CalledDirectly) {
  auto vec = make_vec();
  static_cast<void>(genex::actions::slice(vec, 1, 2));
  EXPECT_EQ(vec, (std::vector{1}));

  auto rest = make_vec();
  static_cast<void>(genex::actions::slice(rest, 4));
  EXPECT_EQ(rest, (std::vector{4}));
}

TEST(GenexActionsSlice, TheWholeRange) {
  auto vec = make_vec();
  vec |= genex::actions::slice(0, 5);
  EXPECT_EQ(vec, make_vec());
}

TEST(GenexActionsSlice, AnEmptySlice) {
  auto vec = make_vec();
  vec |= genex::actions::slice(2, 2);
  EXPECT_TRUE(vec.empty());
}

// These are clamped to the range, as they are in views::slice.
TEST(GenexActionsSlice, EndPastTheEndIsClamped) {
  auto vec = make_vec();
  vec |= genex::actions::slice(3, 100);
  EXPECT_EQ(vec, (std::vector{3, 4}));
}

TEST(GenexActionsSlice, StartPastTheEndEmptiesTheRange) {
  auto ranged = make_vec();
  ranged |= genex::actions::slice(50, 100);
  EXPECT_TRUE(ranged.empty());

  auto open = make_vec();
  open |= genex::actions::slice(50);
  EXPECT_TRUE(open.empty());
}

TEST(GenexActionsSlice, EndBeforeStartEmptiesTheRange) {
  auto vec = make_vec();
  vec |= genex::actions::slice(3, 1);
  EXPECT_TRUE(vec.empty());
}

TEST(GenexActionsSlice, EmptyInput) {
  auto vec = std::vector<int>{};
  vec |= genex::actions::slice(1, 3);
  EXPECT_TRUE(vec.empty());
}

TEST(GenexActionsSlice, OtherContainers) {
  auto list = std::list{0, 1, 2, 3};
  list |= genex::actions::slice(1, 3);
  EXPECT_EQ(list, (std::list{1, 2}));

  auto clamped = std::list{0, 1, 2, 3};
  clamped |= genex::actions::slice(2, 100);
  EXPECT_EQ(clamped, (std::list{2, 3}));

  auto set = std::set{0, 1, 2, 3};
  set |= genex::actions::slice(1, 2);
  EXPECT_EQ(set, (std::set{1}));
}

TEST(GenexActionsSlice, OnAnRvalue) {
  const auto vec = make_vec() | genex::actions::slice(1, 3);
  EXPECT_EQ(vec, (std::vector{1, 2}));
}
