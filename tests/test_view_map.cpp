#include <concepts>
#include <cstddef>
#include <gtest/gtest.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

import genex.to_container;
import genex.views.enumerate;
import genex.views.map;
import genex.views.move;

namespace {
  auto make_map() -> std::map<std::string, int> {
    return {{"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5}, {"six", 6}};
  }
}

TEST(GenexViewsKeys, MapInput) {
  auto map = make_map();

  const auto keys = map
    | genex::views::keys
    | genex::to<std::vector>();

  const auto exp = std::vector<std::string>{"five", "four", "one", "six", "three", "two"};
  EXPECT_EQ(keys, exp);
}

TEST(GenexViewsVals, MapInput) {
  auto map = make_map();

  const auto vals = map
    | genex::views::vals
    | genex::to<std::vector>();

  const auto exp = std::vector{5, 4, 1, 6, 3, 2};
  EXPECT_EQ(vals, exp);
}

TEST(GenexViewsKeys, RefersToTheMapsKeys) {
  auto map = make_map();
  auto keys = map | genex::views::keys;
  EXPECT_EQ(&*keys.begin(), &map.begin()->first);
}

TEST(GenexViewsVals, WritesThroughToTheMap) {
  auto map = make_map();
  for (auto &val : map | genex::views::vals) { val *= 2; }
  EXPECT_EQ(map.at("one"), 2);
  EXPECT_EQ(map.at("six"), 12);
}

// Long enough to be on the heap, so that a dangling reference doesn't read back as the right string by luck.
TEST(GenexViewsKeys, TemporaryPairInput) {
  auto vec = std::vector<std::string>{"the first string, too long to be small", "the second string, also too long"};
  auto rng = vec | genex::views::enumerate | genex::views::keys;
  static_assert(std::same_as<decltype(*rng.begin()), std::size_t>);

  const auto keys = rng | genex::to<std::vector>();
  const auto exp = std::vector<std::size_t>{0, 1};
  EXPECT_EQ(keys, exp);
}

TEST(GenexViewsVals, TemporaryPairInput) {
  const auto exp = std::vector<std::string>{"the first string, too long to be small", "the second string, also too long"};
  auto vec = exp;
  auto rng = vec | genex::views::enumerate | genex::views::vals;
  static_assert(std::same_as<decltype(*rng.begin()), std::string>);

  const auto vals = rng | genex::to<std::vector>();
  EXPECT_EQ(vals, exp);
  EXPECT_EQ(vec, exp);
}

// A reference to a pair isn't a temporary, even as an rvalue, so dereferencing can't take anything from it.
TEST(GenexViewsKeys, RvalueReferenceInputIsNotMovedFrom) {
  const auto key = std::string{"a key that's too long to be stored as a small string"};
  auto vec = std::vector<std::pair<std::string, int>>{{key, 1}};
  auto rng = vec | genex::views::move | genex::views::keys;
  static_assert(std::same_as<decltype(*rng.begin()), std::string&&>);

  static_cast<void>(*rng.begin());
  EXPECT_EQ(vec.front().first, key);
}
