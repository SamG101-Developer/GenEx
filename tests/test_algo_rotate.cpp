#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

import genex.actions.reverse;
import genex.actions.rotate;
import genex.algorithms.reverse;
import genex.algorithms.rotate;

namespace {
  template <typename Container, typename T = typename Container::value_type>
  auto check_every_rotation(const std::size_t count, auto make) -> void {
    auto input = std::vector<T>();
    for (auto i = 0uz; i < count; ++i) { input.push_back(make(static_cast<int>(i))); }

    for (auto m = 0uz; m <= count; ++m) {
      auto data = Container(input.begin(), input.end());
      auto exp = input;
      const auto exp_result = std::rotate(exp.begin(), exp.begin() + static_cast<std::ptrdiff_t>(m), exp.end());

      const auto result = genex::rotate(data, std::next(data.begin(), static_cast<std::ptrdiff_t>(m)));
      EXPECT_TRUE(std::equal(data.begin(), data.end(), exp.begin(), exp.end())) << "count " << count << " mid " << m;
      EXPECT_EQ(std::distance(data.begin(), result), std::distance(exp.begin(), exp_result));
    }
  }

  struct big {
    int value;
    int padding[200];
    auto operator==(const big &that) const -> bool { return value == that.value; }
  };

  const auto lengths = std::vector<std::size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 17, 64, 100, 129, 300};
}

TEST(GenexRotate, EveryRotationOfEachContainer) {
  const auto same = [](const int i) { return i; };
  for (const auto count : lengths) {
    check_every_rotation<std::vector<int>>(count, same);
    check_every_rotation<std::deque<int>>(count, same);
    check_every_rotation<std::list<int>>(count, same);
    check_every_rotation<std::forward_list<int>>(count, same);
  }
}

TEST(GenexRotate, EachWayThroughElementSize) {
  // Whether a side fits on the stack depends on how many bytes it is, so these get either side of that from both ends.
  for (const auto count : {10uz, 60uz, 63uz, 64uz, 65uz, 66uz, 127uz, 128uz, 129uz, 256uz, 257uz, 1000uz}) {
    check_every_rotation<std::vector<char>>(count, [](const int i) { return static_cast<char>(i); });
    check_every_rotation<std::vector<int>>(count, [](const int i) { return i; });
    check_every_rotation<std::vector<long long>>(count, [](const int i) { return static_cast<long long>(i); });
  }
  for (const auto count : {1uz, 2uz, 5uz, 40uz}) {
    check_every_rotation<std::vector<big>>(count, [](const int i) { return big{.value = i, .padding = {}}; });
  }
}

TEST(GenexRotate, Strings) {
  for (const auto count : lengths) {
    check_every_rotation<std::vector<std::string>>(count, [](const int i) { return std::to_string(i) + "-padding-to-allocate-it"; });
    check_every_rotation<std::list<std::string>>(count, [](const int i) { return std::to_string(i) + "-padding-to-allocate-it"; });
  }
}

TEST(GenexRotate, MoveOnlyElements) {
  for (const auto count : {0uz, 1uz, 5uz, 100uz}) {
    for (auto m = 0uz; m <= count; ++m) {
      auto data = std::vector<std::unique_ptr<int>>();
      for (auto i = 0uz; i < count; ++i) { data.push_back(std::make_unique<int>(static_cast<int>(i))); }
      genex::rotate(data, data.begin() + static_cast<std::ptrdiff_t>(m));
      for (auto i = 0uz; i < count; ++i) { EXPECT_EQ(*data[i], static_cast<int>((i + m) % count)); }
    }
  }
}

TEST(GenexRotate, ProxyReferences) {
  auto data = std::vector<bool>{true, true, false, true, false, false, true};
  auto exp = data;
  std::rotate(exp.begin(), exp.begin() + 3, exp.end());
  const auto result = genex::rotate(data, data.begin() + 3);
  EXPECT_EQ(data, exp);
  EXPECT_EQ(result, data.begin() + 4);
}

TEST(GenexRotate, IteratorForm) {
  auto data = std::vector{1, 2, 3, 4, 5};
  const auto result = genex::rotate(data.begin(), data.begin() + 2, data.begin() + 4);
  EXPECT_EQ(data, (std::vector{3, 4, 1, 2, 5}));
  EXPECT_EQ(result, data.begin() + 2);
}

TEST(GenexReverse, EachContainer) {
  for (const auto count : lengths) {
    auto input = std::vector<int>(count);
    std::iota(input.begin(), input.end(), 0);
    auto exp = input;
    std::reverse(exp.begin(), exp.end());

    auto vec = input;
    EXPECT_EQ(genex::reverse(vec), vec.end());
    EXPECT_EQ(vec, exp);

    auto list = std::list<int>(input.begin(), input.end());
    genex::reverse(list);
    EXPECT_TRUE(std::equal(list.begin(), list.end(), exp.begin(), exp.end()));
  }

  auto bits = std::vector<bool>{true, false, false};
  genex::reverse(bits);
  EXPECT_EQ(bits, (std::vector<bool>{false, false, true}));
}

static_assert([] {
  for (auto m = 0; m <= 8; ++m) {
    auto data = std::array{0, 1, 2, 3, 4, 5, 6, 7};
    const auto result = genex::rotate(data, data.begin() + m);
    if (result - data.begin() != 8 - m) { return false; }
    for (auto i = 0; i < 8; ++i) {
      if (data[i] != (i + m) % 8) { return false; }
    }
  }
  return true;
}());

static_assert([] {
  auto data = std::array{1, 2, 3, 4};
  genex::reverse(data);
  return data == std::array{4, 3, 2, 1};
}());

TEST(GenexRotate, ThroughTheActions) {
  auto data = std::vector{1, 2, 3, 4, 5};
  data |= genex::actions::rotate(2);
  EXPECT_EQ(data, (std::vector{3, 4, 5, 1, 2}));
  data |= genex::actions::reverse;
  EXPECT_EQ(data, (std::vector{2, 1, 5, 4, 3}));

  auto list = std::list{1, 2, 3};
  list |= genex::actions::rotate(1);
  EXPECT_EQ(list, (std::list{2, 3, 1}));
  list |= genex::actions::reverse;
  EXPECT_EQ(list, (std::list{1, 3, 2}));
}
