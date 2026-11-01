#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <list>
#include <random>
#include <span>
#include <string>
#include <utility>
#include <vector>

import genex.actions.filter;
import genex.actions.remove;
import genex.actions.remove_if;
import genex.algorithms.contains;
import genex.algorithms.equals;
import genex.algorithms.find;

namespace {
  struct FirstFieldOnly {
    int a;
    int b;
    auto operator==(const FirstFieldOnly &that) const -> bool { return a == that.a; }
  };

  enum class Colour : int { red, green };

  template <typename T>
  auto check_find_matches_std(std::mt19937 &engine) -> void {
    for (auto trial = 0; trial < 2000; ++trial) {
      auto data = std::vector<T>(engine() % 70);
      for (auto &x : data) { x = static_cast<T>(engine() % 5); }
      const auto wanted = static_cast<T>(engine() % 7);
      const auto expected = std::find(data.begin(), data.end(), wanted);
      EXPECT_EQ(genex::find(data, wanted), expected);
      EXPECT_EQ(genex::contains(data, wanted), expected != data.end());
    }
  }

  template <typename T>
  auto check_keep_matches_std(std::mt19937 &engine, const auto make) -> void {
    for (auto trial = 0; trial < 1500; ++trial) {
      auto data = std::vector<T>(engine() % 70);
      for (auto &x : data) { x = make(engine() % 8); }
      auto expected = data;
      const auto pred = [&](const T &x) { return x == make(0) or x == make(3); };
      std::erase_if(expected, pred);
      auto actual = data;
      actual |= genex::actions::remove_if(pred);
      EXPECT_EQ(actual, expected);
    }
  }

  // Removes "x % 2 == 0" from "data", and checks it against the standard library.
  auto check_long(std::vector<int> data) -> void {
    auto expected = data;
    std::erase_if(expected, [](const int x) { return x % 2 == 0; });
    data |= genex::actions::remove_if([](const int x) { return x % 2 == 0; });
    EXPECT_EQ(data, expected);
  }
}

TEST(GenexLibcFind, IntPositions) {
  auto vec = std::vector{5, -1, 7, 7, 0, std::numeric_limits<int>::min()};
  EXPECT_EQ(genex::find(vec, 5), vec.begin());
  EXPECT_EQ(genex::find(vec, 7), vec.begin() + 2);
  EXPECT_EQ(genex::find(vec, -1), vec.begin() + 1);
  EXPECT_EQ(genex::find(vec, std::numeric_limits<int>::min()), vec.begin() + 5);
  EXPECT_EQ(genex::find(vec, 42), vec.end());
}

TEST(GenexLibcFind, Empty) {
  auto ints = std::vector<int>{};
  auto bytes = std::vector<char>{};
  EXPECT_EQ(genex::find(ints, 1), ints.end());
  EXPECT_EQ(genex::find(bytes, 'a'), bytes.end());
  EXPECT_FALSE(genex::contains(ints, 1));
}

TEST(GenexLibcFind, HighBitBytes) {
  auto chars = std::vector<char>{'a', static_cast<char>(0xFF), 'b'};
  auto uchars = std::vector<unsigned char>{1, 0xFF, 0};
  auto schars = std::vector<std::int8_t>{1, -1, 0};
  EXPECT_EQ(genex::find(chars, static_cast<char>(0xFF)), chars.begin() + 1);
  EXPECT_EQ(genex::find(uchars, static_cast<unsigned char>(0xFF)), uchars.begin() + 1);
  EXPECT_EQ(genex::find(uchars, static_cast<unsigned char>(0)), uchars.begin() + 2);
  EXPECT_EQ(genex::find(schars, static_cast<std::int8_t>(-1)), schars.begin() + 1);
}

TEST(GenexLibcFind, OtherIntegerWidths) {
  auto high = std::vector<std::uint32_t>{1, 0xFFFFFFFFu, 3};
  auto shorts = std::vector<std::int16_t>{1, -2, 3};
  auto longs = std::vector<std::int64_t>{1, -2, std::numeric_limits<std::int64_t>::max()};
  auto small = std::vector<std::uint8_t>{0, 1};
  auto wide = std::vector<wchar_t>{L'a', L'b'};
  EXPECT_EQ(genex::find(high, 0xFFFFFFFFu), high.begin() + 1);
  EXPECT_EQ(genex::find(shorts, static_cast<std::int16_t>(-2)), shorts.begin() + 1);
  EXPECT_EQ(genex::find(longs, std::numeric_limits<std::int64_t>::max()), longs.begin() + 2);
  EXPECT_EQ(genex::find(small, static_cast<std::uint8_t>(1)), small.begin() + 1);
  EXPECT_EQ(genex::find(wide, L'b'), wide.begin() + 1);
}

// A value that a byte can't hold mustn't be cut down to one that it can.
TEST(GenexLibcFind, DoesNotTruncateAWiderValue) {
  auto bytes = std::vector<char>{'a', ',', 'b'};
  EXPECT_EQ(genex::find(bytes, 300), bytes.end());
  EXPECT_EQ(genex::find(bytes, static_cast<int>(',')), bytes.begin() + 1);
  auto ints = std::vector{1, 2};
  EXPECT_EQ(genex::find(ints, 4294967297LL), ints.end());
  EXPECT_EQ(genex::find(ints, 2LL), ints.begin() + 1);
}

TEST(GenexLibcFind, ConstAndSpanAndSubrange) {
  const auto vec = std::vector{1, 2, 3, 4};
  EXPECT_EQ(genex::find(vec, 3), vec.begin() + 2);
  const auto span = std::span(vec).subspan(1, 2);
  EXPECT_EQ(genex::find(span, 3), span.begin() + 1);
  EXPECT_EQ(genex::find(span, 1), span.end());
  EXPECT_EQ(genex::find(span, 4), span.end());
  EXPECT_EQ(genex::find(vec.begin() + 1, vec.begin() + 3, 4), vec.begin() + 3);
  const auto bytes = std::string("hello");
  EXPECT_EQ(genex::find(bytes, 'l'), bytes.begin() + 2);
  const auto array = std::array{9, 8, 7};
  EXPECT_EQ(genex::find(array, 7), array.begin() + 2);
}

TEST(GenexLibcFind, ValuesThatBytesCannotDecide) {
  auto floats = std::vector{1.0f, 0.0f, std::nanf("")};
  EXPECT_EQ(genex::find(floats, -0.0f), floats.begin() + 1);
  EXPECT_EQ(genex::find(floats, std::nanf("")), floats.end());
  auto structs = std::vector{FirstFieldOnly{1, 2}, FirstFieldOnly{3, 4}};
  EXPECT_EQ(genex::find(structs, FirstFieldOnly{3, 99}), structs.begin() + 1);
}

TEST(GenexLibcFind, ProjectionAndNonContiguous) {
  auto vec = std::vector{1, 2, 3};
  EXPECT_EQ(genex::find(vec, 6, [](const int x) { return x * 2; }), vec.begin() + 2);
  auto lst = std::list{1, 2, 3};
  EXPECT_EQ(genex::find(lst, 3), std::next(lst.begin(), 2));
  EXPECT_EQ(genex::find(lst, 9), lst.end());
}

TEST(GenexLibcFind, ConstantEvaluated) {
  static constexpr auto ints = std::array{1, 2, 3};
  static constexpr auto bytes = std::array{'a', 'b'};
  static_assert(genex::find(ints, 3) == ints.begin() + 2);
  static_assert(genex::find(ints, 9) == ints.end());
  static_assert(genex::find(bytes, 'b') == bytes.begin() + 1);
  static_assert(genex::contains(ints, 2));
}

TEST(GenexLibcFind, MatchesStdOnRandomData) {
  auto engine = std::mt19937(11);
  check_find_matches_std<int>(engine);
  check_find_matches_std<char>(engine);
  check_find_matches_std<unsigned char>(engine);
  check_find_matches_std<std::uint32_t>(engine);
  check_find_matches_std<std::int16_t>(engine);
  check_find_matches_std<std::int64_t>(engine);
}

TEST(GenexLibcEquals, Basics) {
  auto a = std::vector{1, 2, 3};
  auto b = std::vector{1, 2, 3};
  auto later = std::vector{1, 2, 4};
  auto shorter = std::vector{1, 2};
  auto longer = std::vector{1, 2, 3, 4};
  auto none = std::vector<int>{};
  EXPECT_TRUE(genex::equals(a, b));
  EXPECT_FALSE(genex::equals(a, later));
  EXPECT_FALSE(genex::equals(a, shorter));
  EXPECT_FALSE(genex::equals(a, longer));
  EXPECT_FALSE(genex::equals(none, a));
  EXPECT_TRUE(genex::equals(none, none));
}

TEST(GenexLibcEquals, MixedRanges) {
  auto vec = std::vector{1, 2, 3};
  auto arr = std::array{1, 2, 3};
  auto lst = std::list{1, 2, 3};
  EXPECT_TRUE(genex::equals(vec, arr));
  EXPECT_TRUE(genex::equals(vec, std::span(arr)));
  EXPECT_TRUE(genex::equals(vec, lst));
  auto wider = std::vector<long long>{1, 2, 3};
  EXPECT_TRUE(genex::equals(vec, wider));
}

TEST(GenexLibcEquals, ValuesThatBytesCannotDecide) {
  EXPECT_TRUE(genex::equals(std::vector{0.0}, std::vector{-0.0}));
  const auto nan = std::vector{std::nan("")};
  EXPECT_FALSE(genex::equals(nan, nan));
  EXPECT_TRUE(genex::equals(
    std::vector{FirstFieldOnly{1, 2}}, std::vector{FirstFieldOnly{1, 99}}));
  EXPECT_TRUE(genex::equals(std::vector{Colour::red}, std::vector{Colour::red}));
  EXPECT_FALSE(genex::equals(std::vector{Colour::red}, std::vector{Colour::green}));
}

TEST(GenexLibcEquals, Pointers) {
  int x = 0, y = 0;
  EXPECT_TRUE(genex::equals(std::vector{&x, &y}, std::vector{&x, &y}));
  EXPECT_FALSE(genex::equals(std::vector{&x, &y}, std::vector{&y, &x}));
}

TEST(GenexLibcEquals, CustomComparisonAndProjection) {
  auto a = std::vector{1, 2};
  auto b = std::vector{2, 3};
  EXPECT_TRUE(genex::equals(a, b, [](const int l, const int r) { return l + 1 == r; }));
  EXPECT_FALSE(genex::equals(a, b));
  EXPECT_TRUE(genex::equals(a, b, {}, [](const int x) { return x + 1; }));
}

TEST(GenexLibcEquals, ConstantEvaluated) {
  static constexpr auto a = std::array{1, 2, 3};
  static constexpr auto b = std::array{1, 2, 3};
  static constexpr auto c = std::array{1, 2, 4};
  static_assert(genex::equals(a, b));
  static_assert(not genex::equals(a, c));
}

TEST(GenexLibcEquals, MatchesStdOnRandomData) {
  auto engine = std::mt19937(13);
  for (auto trial = 0; trial < 3000; ++trial) {
    auto a = std::vector<int>(engine() % 70);
    for (auto &x : a) { x = static_cast<int>(engine()); }
    auto b = a;
    if (engine() % 2 == 0 and not b.empty()) { b[engine() % b.size()] ^= 1; }
    if (engine() % 5 == 0) { b.push_back(1); }
    EXPECT_EQ(genex::equals(a, b), std::equal(a.begin(), a.end(), b.begin(), b.end()));
  }
}

TEST(GenexKeepIf, PatternsOfRemoval) {
  const auto is_even = [](const int x) { return x % 2 == 0; };
  const auto check = [&](std::vector<int> input, const std::vector<int> &expected) {
    input |= genex::actions::remove_if(is_even);
    EXPECT_EQ(input, expected);
  };
  check({}, {});
  check({2}, {});
  check({1}, {1});
  check({2, 4, 6}, {});
  check({1, 3, 5}, {1, 3, 5});
  check({2, 1, 3}, {1, 3});
  check({1, 3, 2}, {1, 3});
  check({1, 2, 3, 4, 5}, {1, 3, 5});
  check({2, 1, 4, 3, 6}, {1, 3});
}

TEST(GenexKeepIf, ActionsAgree) {
  auto removed = std::vector{1, 2, 1, 3, 1};
  removed |= genex::actions::remove(1);
  EXPECT_EQ(removed, (std::vector{2, 3}));

  auto filtered = std::vector{1, 2, 3, 4};
  filtered |= genex::actions::filter([](const int x) { return x > 2; });
  EXPECT_EQ(filtered, (std::vector{3, 4}));

  auto points = std::vector<std::pair<int, int>>{{1, 2}, {3, 4}, {5, 6}};
  points |= genex::actions::remove_if([](const int x) { return x == 3; }, &std::pair<int, int>::first);
  EXPECT_EQ(points, (std::vector<std::pair<int, int>>{{1, 2}, {5, 6}}));
}

TEST(GenexKeepIf, EachElementJudgedOnceInOrder) {
  auto engine = std::mt19937(19);
  auto input = std::vector<int>(4000);
  for (auto &x : input) { x = static_cast<int>(engine()); }
  const auto original = input;
  auto seen = std::vector<int>{};
  input |= genex::actions::remove_if([&](const int x) {
    seen.push_back(x);
    return x % 2 == 0;
  });
  EXPECT_EQ(seen, original);
}

// Around the size of a sample and of what's handled between samples.
TEST(GenexKeepIf, LengthsAroundItsInternalSizes) {
  auto engine = std::mt19937(23);
  for (const auto base : {62, 64, 66, 1086, 1088, 1090, 1152, 2176, 2178}) {
    for (auto extra = -3; extra <= 3; ++extra) {
      auto random = std::vector<int>(static_cast<std::size_t>(base + extra));
      for (auto &x : random) { x = static_cast<int>(engine()); }
      check_long(random);
      auto all_kept = std::vector<int>(random.size(), 1);
      check_long(all_kept);
      auto all_removed = std::vector<int>(random.size(), 2);
      check_long(all_removed);
      auto alternating = std::vector<int>(random.size());
      for (auto i = 0uz; i < alternating.size(); ++i) { alternating[i] = static_cast<int>(i); }
      check_long(alternating);
    }
  }
}

// Each stretch can be handled differently, so it's what's at the joins that matters.
TEST(GenexKeepIf, DataThatChangesHardnessPartWay) {
  auto engine = std::mt19937(29);
  for (auto trial = 0; trial < 200; ++trial) {
    auto data = std::vector<int>{};
    for (auto part = 0; part < 6; ++part) {
      const auto length = engine() % 1500;
      const auto kind = engine() % 4;
      for (auto i = 0u; i < length; ++i) {
        switch (kind) {
          case 0: data.push_back(1); break;
          case 1: data.push_back(2); break;
          case 2: data.push_back(static_cast<int>(engine())); break;
          default: data.push_back(i % 100 == 0 ? 2 : 1); break;
        }
      }
    }
    check_long(std::move(data));
  }
}

TEST(GenexKeepIf, ConstantEvaluated) {
  static_assert([] {
    auto data = std::vector<int>(200);
    for (auto i = 0; i < 200; ++i) { data[static_cast<std::size_t>(i)] = i; }
    data |= genex::actions::remove_if([](const int x) { return x % 2 == 0; });
    return data.size() == 100 and data.front() == 1 and data.back() == 199;
  }());
}

TEST(GenexKeepIf, MatchesStdOnRandomData) {
  auto engine = std::mt19937(17);
  check_keep_matches_std<int>(engine, [](const unsigned x) { return static_cast<int>(x); });
  check_keep_matches_std<char>(engine, [](const unsigned x) { return static_cast<char>(x); });
  check_keep_matches_std<double>(engine, [](const unsigned x) { return static_cast<double>(x); });
  check_keep_matches_std<std::pair<int, long>>(
    engine, [](const unsigned x) { return std::pair<int, long>(static_cast<int>(x), x * 2l); });
  check_keep_matches_std<std::string>(
    engine, [](const unsigned x) { return std::string(40, static_cast<char>('a' + x)); });
}

TEST(GenexKeepIf, NonRandomAccessStillWorks) {
  auto lst = std::list{1, 2, 3, 4};
  lst |= genex::actions::remove_if([](const int x) { return x % 2 == 0; });
  EXPECT_EQ(lst, (std::list{1, 3}));
}
