#include <gtest/gtest.h>
#include <array>
#include <list>
#include <string>
#include <utility>
#include <vector>

import genex.algorithms.count;
import genex.algorithms.count_if;

namespace {
  constexpr auto is_even = [](const int x) { return x % 2 == 0; };
  using keyed = std::pair<int, int>;
}

TEST(GenexCount, CountsMatches) {
  const auto vec = std::vector{1, 2, 2, 3, 2};
  EXPECT_EQ(genex::count(vec, 2), 3uz);
  EXPECT_EQ(genex::count(vec, 1), 1uz);
  EXPECT_EQ(genex::count(vec, 9), 0uz);
}

TEST(GenexCount, EmptyInput) {
  EXPECT_EQ(genex::count(std::vector<int>{}, 1), 0uz);
}

TEST(GenexCount, IteratorForm) {
  const auto vec = std::vector{2, 2, 2, 2};
  EXPECT_EQ(genex::count(vec.begin() + 1, vec.begin() + 3, 2), 2uz);
}

TEST(GenexCount, OtherContainersAndElements) {
  const auto list = std::list{1, 1, 2};
  EXPECT_EQ(genex::count(list, 1), 2uz);

  const auto words = std::vector<std::string>{"a", "b", "a"};
  EXPECT_EQ(genex::count(words, std::string("a")), 2uz);
}

TEST(GenexCount, Projection) {
  const auto pairs = std::vector<keyed>{{1, 7}, {2, 7}, {1, 8}};
  EXPECT_EQ(genex::count(pairs, 1, &keyed::first), 2uz);
  EXPECT_EQ(genex::count(pairs, 7, &keyed::second), 2uz);
}

TEST(GenexCountIf, CountsMatches) {
  const auto vec = std::vector{1, 2, 3, 4, 6};
  EXPECT_EQ(genex::count_if(vec, is_even), 3uz);
  EXPECT_EQ(genex::count_if(vec, [](const int) { return false; }), 0uz);
  EXPECT_EQ(genex::count_if(vec, [](const int) { return true; }), 5uz);
}

TEST(GenexCountIf, EmptyInput) {
  EXPECT_EQ(genex::count_if(std::vector<int>{}, is_even), 0uz);
}

TEST(GenexCountIf, IteratorForm) {
  const auto vec = std::vector{1, 2, 4, 6};
  EXPECT_EQ(genex::count_if(vec.begin(), vec.begin() + 2, is_even), 1uz);
}

TEST(GenexCountIf, OtherContainers) {
  const auto list = std::list{1, 2, 4};
  EXPECT_EQ(genex::count_if(list, is_even), 2uz);
}

TEST(GenexCountIf, Projection) {
  const auto pairs = std::vector<keyed>{{1, 2}, {2, 3}, {4, 5}};
  EXPECT_EQ(genex::count_if(pairs, is_even, &keyed::first), 2uz);
  EXPECT_EQ(genex::count_if(pairs, is_even, &keyed::second), 1uz);
}

static_assert([] {
  constexpr auto values = std::array{1, 2, 2};
  return genex::count(values, 2) == 2uz and genex::count_if(values, is_even) == 2uz;
}());
