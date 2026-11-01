#include <gtest/gtest.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

import genex.strings.cases;
import genex.to_container;
import genex.views.generate;
import genex.views.iota;
import genex.views.join;
import genex.views.repeat;
import genex.views.set_algorithms;
import genex.views.take;
import genex.views.transform;

// These pick an overload by what they're given, and a template is only checked once it's used, so each one is used here.

TEST(GenexStringCaseOverloads, Narrow) {
  auto str = std::string("aB1");
  EXPECT_EQ(str | genex::strings::upper_case | genex::to<std::string>(), "AB1");
  EXPECT_EQ(str | genex::strings::lower_case | genex::to<std::string>(), "ab1");
}

TEST(GenexStringCaseOverloads, Wide) {
  auto str = std::wstring(L"aB1");
  EXPECT_EQ(str | genex::strings::upper_case | genex::to<std::wstring>(), L"AB1");
  EXPECT_EQ(str | genex::strings::lower_case | genex::to<std::wstring>(), L"ab1");
}

TEST(GenexStringCaseOverloads, UtfCodeUnitsAreLeftAlone) {
  auto utf8 = std::u8string(u8"aB1");
  EXPECT_TRUE((utf8 | genex::strings::upper_case | genex::to<std::u8string>()) == u8"aB1");
  EXPECT_TRUE((utf8 | genex::strings::lower_case | genex::to<std::u8string>()) == u8"aB1");

  auto utf16 = std::u16string(u"aB1");
  EXPECT_TRUE((utf16 | genex::strings::upper_case | genex::to<std::u16string>()) == u"aB1");

  auto utf32 = std::u32string(U"aB1");
  EXPECT_TRUE((utf32 | genex::strings::lower_case | genex::to<std::u32string>()) == U"aB1");
}

TEST(GenexEndOverloads, RepeatCountedAndNot) {
  EXPECT_EQ(genex::views::repeat(7, 2) | genex::to<std::vector>(), (std::vector{7, 7}));
  EXPECT_EQ(genex::views::repeat(7) | genex::views::take(3) | genex::to<std::vector>(), (std::vector{7, 7, 7}));
}

TEST(GenexEndOverloads, GenerateCountedAndNot) {
  auto next = 0;
  auto count_up = [&next] { return next++; };
  EXPECT_EQ(genex::views::generate_n(count_up, 2) | genex::to<std::vector>(), (std::vector{0, 1}));
  next = 0;
  EXPECT_EQ(genex::views::generate(count_up) | genex::views::take(3) | genex::to<std::vector>(), (std::vector{0, 1, 2}));
}

namespace {
  // Can be counted through, but isn't an integer.
  struct stepped {
    int value = 0;
    using difference_type = int;
    constexpr auto operator++() -> stepped& { ++value; return *this; }
    constexpr auto operator++(int) -> stepped { auto old = *this; ++value; return old; }
    constexpr auto operator+=(const stepped that) -> stepped& { value += that.value; return *this; }
    constexpr auto operator-=(const stepped that) -> stepped& { value -= that.value; return *this; }
    constexpr auto operator<=>(const stepped &) const = default;
  };

  template <typename View>
  auto values_of(View view) -> std::vector<int> {
    auto out = std::vector<int>();
    for (auto first = view.begin(), last = view.end(); not (first == last); ++first) { out.push_back((*first).value); }
    return out;
  }
}

TEST(GenexIotaOverloads, Integral) {
  EXPECT_EQ(genex::views::iota(0, 3) | genex::to<std::vector>(), (std::vector{0, 1, 2}));
  EXPECT_EQ(genex::views::iota(0, 5, 2) | genex::to<std::vector>(), (std::vector{0, 2, 4}));
  EXPECT_EQ(genex::views::iota(5, 0) | genex::to<std::vector>(), std::vector<int>{});
}

TEST(GenexIotaOverloads, NotIntegralStopsPastTheEnd) {
  EXPECT_EQ(values_of(genex::views::iota(stepped{0}, stepped{3}, stepped{1})), (std::vector{0, 1, 2}));
  EXPECT_EQ(values_of(genex::views::iota(stepped{0}, stepped{3}, stepped{2})), (std::vector{0, 2}));
  EXPECT_EQ(values_of(genex::views::iota(stepped{3}, stepped{3}, stepped{1})), std::vector<int>{});
}

TEST(GenexJoinOverloads, InnerRangesThatAreStored) {
  auto nested = std::vector<std::vector<int>>{{1, 2}, {}, {3}};
  EXPECT_EQ(nested | genex::views::join | genex::to<std::vector>(), (std::vector{1, 2, 3}));
}

TEST(GenexJoinOverloads, InnerRangesThatAreMadeOnTheWay) {
  auto counts = std::vector{2, 0, 1};
  const auto rng = counts
    | genex::views::transform([](const int n) { return std::vector<int>(static_cast<std::size_t>(n), n); })
    | genex::views::join;
  EXPECT_EQ(rng | genex::to<std::vector>(), (std::vector{2, 2, 1}));
}

namespace {
  const auto evens = std::vector{0, 2, 2, 4, 6, 8, 12};
  const auto threes = std::vector{0, 3, 6, 6, 9, 12, 15};

  auto expected(auto algorithm) -> std::vector<int> {
    auto out = std::vector<int>();
    algorithm(evens.begin(), evens.end(), threes.begin(), threes.end(), std::back_inserter(out));
    return out;
  }
}

TEST(GenexSetAlgorithmOverloads, MatchTheStandard) {
  auto a = evens;
  auto b = threes;

  EXPECT_EQ(
    a | genex::views::set_difference(b) | genex::to<std::vector>(),
    expected([](auto... xs) { std::set_difference(xs...); }));
  EXPECT_EQ(
    a | genex::views::set_intersection(b) | genex::to<std::vector>(),
    expected([](auto... xs) { std::set_intersection(xs...); }));
  EXPECT_EQ(
    a | genex::views::set_symmetric_difference(b) | genex::to<std::vector>(),
    expected([](auto... xs) { std::set_symmetric_difference(xs...); }));
  EXPECT_EQ(
    a | genex::views::set_union(b) | genex::to<std::vector>(),
    expected([](auto... xs) { std::set_union(xs...); }));

  auto empty = std::vector<int>{};
  EXPECT_EQ(empty | genex::views::set_union(b) | genex::to<std::vector>(), threes);
  EXPECT_EQ(a | genex::views::set_difference(empty) | genex::to<std::vector>(), evens);
  EXPECT_EQ(a | genex::views::set_intersection(empty) | genex::to<std::vector>(), std::vector<int>{});
  EXPECT_EQ(empty | genex::views::set_symmetric_difference(b) | genex::to<std::vector>(), threes);
}
