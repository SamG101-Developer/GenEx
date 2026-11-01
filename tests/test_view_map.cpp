#include <gtest/gtest.h>
#include <map>
#include <string>
#include <vector>

import genex.to_container;
import genex.views.map;

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
