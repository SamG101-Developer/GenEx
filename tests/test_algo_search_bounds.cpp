#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <deque>
#include <forward_list>
#include <functional>
#include <list>
#include <random>
#include <string>
#include <string_view>
#include <vector>

import genex.algorithms.binary_search;
import genex.algorithms.lower_bound;
import genex.algorithms.upper_bound;

namespace {
  auto sorted_with_duplicates(const std::size_t count) -> std::vector<int> {
    auto engine = std::mt19937(static_cast<unsigned>(count) + 1);
    auto data = std::vector<int>(count);
    for (auto &x : data) { x = static_cast<int>(engine() % (count / 2 + 2)); }
    std::sort(data.begin(), data.end());
    return data;
  }

  template <typename Container>
  auto check_bounds(const std::size_t count) -> void {
    const auto input = sorted_with_duplicates(count);
    const auto data = Container(input.begin(), input.end());
    for (auto key = -1; key < static_cast<int>(count / 2) + 3; ++key) {
      const auto lower = genex::lower_bound(data, key);
      const auto upper = genex::upper_bound(data, key);
      EXPECT_EQ(
        std::distance(data.begin(), lower),
        std::distance(input.begin(), std::lower_bound(input.begin(), input.end(), key)));
      EXPECT_EQ(
        std::distance(data.begin(), upper),
        std::distance(input.begin(), std::upper_bound(input.begin(), input.end(), key)));
      EXPECT_EQ(genex::binary_search(data, key), std::binary_search(input.begin(), input.end(), key));
    }
  }

  struct record {
    int key;
    int payload;
  };
}

TEST(GenexBounds, EachContainer) {
  for (const auto count : {0uz, 1uz, 2uz, 3uz, 4uz, 5uz, 7uz, 8uz, 9uz, 16uz, 33uz, 100uz, 1000uz}) {
    check_bounds<std::vector<int>>(count);
    check_bounds<std::deque<int>>(count);
    check_bounds<std::list<int>>(count);
    check_bounds<std::forward_list<int>>(count);
  }
}

TEST(GenexBounds, ComparatorAndProjection) {
  auto descending = std::vector{9, 7, 7, 5, 3};
  EXPECT_EQ(genex::lower_bound(descending, 7, std::greater{}), descending.begin() + 1);
  EXPECT_EQ(genex::upper_bound(descending, 7, std::greater{}), descending.begin() + 3);

  auto records = std::vector<record>{{1, 0}, {3, 0}, {3, 1}, {5, 0}};
  EXPECT_EQ(genex::lower_bound(records, 3, {}, &record::key), records.begin() + 1);
  EXPECT_EQ(genex::upper_bound(records, 3, {}, &record::key), records.begin() + 3);
  EXPECT_TRUE(genex::binary_search(records, 3, {}, &record::key));
  EXPECT_FALSE(genex::binary_search(records, 4, {}, &record::key));
}

TEST(GenexBounds, ADifferentTypeOfKey) {
  const auto words = std::vector<std::string>{"apple", "banana", "cherry"};
  EXPECT_EQ(genex::lower_bound(words, std::string_view("banana")), words.begin() + 1);
  EXPECT_EQ(genex::upper_bound(words, std::string_view("banana")), words.begin() + 2);
}

TEST(GenexBounds, IteratorForm) {
  const auto data = std::vector{1, 2, 3, 4, 5};
  EXPECT_EQ(genex::lower_bound(data.begin(), data.begin() + 2, 4), data.begin() + 2);
  EXPECT_EQ(genex::upper_bound(data.begin(), data.end(), 3), data.begin() + 3);
}

static_assert([] {
  constexpr auto data = std::array{1, 3, 3, 5};
  return genex::lower_bound(data, 3) == data.begin() + 1
    and genex::upper_bound(data, 3) == data.begin() + 3
    and genex::lower_bound(data, 9) == data.end()
    and genex::binary_search(data, 5)
    and not genex::binary_search(data, 2);
}());
