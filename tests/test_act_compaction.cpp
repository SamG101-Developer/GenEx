#include <gtest/gtest.h>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

import genex.actions.concat;
import genex.actions.filter;
import genex.actions.remove;
import genex.actions.remove_if;
import genex.actions.reverse;
import genex.actions.unique;
import genex.to_container;

namespace {
  constexpr auto is_even = [](const int x) { return x % 2 == 0; };
}

// These shift elements down rather than erasing one at a time, so order and every position in the input matter.
TEST(GenexActionsCompaction, RemoveIfKeepsOrder) {
  for (const auto &[in, exp] : std::vector<std::pair<std::vector<int>, std::vector<int>>>{
         {{}, {}}, {{2}, {}}, {{1}, {1}}, {{2, 4}, {}}, {{1, 3}, {1, 3}}, {{2, 1, 4, 3}, {1, 3}}, {{1, 2, 3, 4, 5}, {1, 3, 5}}
       }) {
    auto vec = in;
    vec |= genex::actions::remove_if(is_even);
    EXPECT_EQ(vec, exp);
  }
}

TEST(GenexActionsCompaction, PredicateRunsOncePerElement) {
  auto vec = std::vector{1, 2, 3, 4, 5, 6};
  auto calls = 0;
  vec |= genex::actions::remove_if([&calls](const int x) {
    ++calls;
    return x % 2 == 0;
  });
  EXPECT_EQ(calls, 6);
}

TEST(GenexActionsCompaction, RemoveAndFilter) {
  auto removed = std::vector{1, 2, 1, 3, 1};
  removed |= genex::actions::remove(1);
  EXPECT_EQ(removed, (std::vector{2, 3}));

  auto filtered = std::vector{1, 2, 3, 4};
  filtered |= genex::actions::filter(is_even);
  EXPECT_EQ(filtered, (std::vector{2, 4}));
}

TEST(GenexActionsCompaction, MoveOnlyElements) {
  auto vec = std::vector<std::unique_ptr<int>>{};
  for (auto i = 0; i < 5; ++i) { vec.push_back(std::make_unique<int>(i)); }
  vec |= genex::actions::remove_if([](const std::unique_ptr<int> &p) { return *p % 2 == 0; });

  ASSERT_EQ(vec.size(), 2uz);
  EXPECT_EQ(*vec[0], 1);
  EXPECT_EQ(*vec[1], 3);
}

TEST(GenexActionsCompaction, OverAList) {
  auto lst = std::list{1, 2, 3, 4};
  lst |= genex::actions::remove_if(is_even);
  EXPECT_EQ(lst, (std::list{1, 3}));
}

TEST(GenexActionsCompaction, ElementsThatCantBeAssigned) {
  auto set = std::set{1, 2, 3, 4};
  set |= genex::actions::filter(is_even);
  EXPECT_EQ(set, (std::set{2, 4}));
}

TEST(GenexActionsCompaction, Unique) {
  for (const auto &[in, exp] : std::vector<std::pair<std::vector<int>, std::vector<int>>>{
         {{}, {}}, {{1}, {1}}, {{1, 1}, {1}}, {{1, 2}, {1, 2}}, {{1, 1, 2, 2, 1, 1}, {1, 2, 1}}, {{1, 2, 2, 3}, {1, 2, 3}}
       }) {
    auto vec = in;
    vec |= genex::actions::unique();
    EXPECT_EQ(vec, exp);
  }
}

TEST(GenexActionsCompaction, ReverseOddAndEven) {
  for (const auto &[in, exp] : std::vector<std::pair<std::vector<int>, std::vector<int>>>{
         {{}, {}}, {{1}, {1}}, {{1, 2}, {2, 1}}, {{1, 2, 3}, {3, 2, 1}}, {{1, 2, 3, 4}, {4, 3, 2, 1}}
       }) {
    auto vec = in;
    vec |= genex::actions::reverse();
    EXPECT_EQ(vec, exp);
  }
}

TEST(GenexActionsCompaction, ConcatCopiesAnLvalueAndLeavesItIntact) {
  auto lhs = std::vector<std::string>{"a"};
  const auto rhs = std::vector<std::string>{"b", "c"};
  lhs |= genex::actions::concat(rhs);
  EXPECT_EQ(lhs, (std::vector<std::string>{"a", "b", "c"}));
  EXPECT_EQ(rhs.size(), 2uz);
}

TEST(GenexActionsCompaction, ConcatMovesFromAnRvalue) {
  auto lhs = std::vector<std::unique_ptr<int>>{};
  auto rhs = std::vector<std::unique_ptr<int>>{};
  rhs.push_back(std::make_unique<int>(1));
  lhs |= genex::actions::concat(std::move(rhs));

  ASSERT_EQ(lhs.size(), 1uz);
  EXPECT_EQ(*lhs[0], 1);
}

TEST(GenexActionsCompaction, ConcatDifferentContainers) {
  auto lhs = std::vector{1};
  auto rhs = std::list{2, 3};
  lhs |= genex::actions::concat(rhs);
  EXPECT_EQ(lhs, (std::vector{1, 2, 3}));
}
