#include <gtest/gtest.h>
#include <concepts>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

import genex.concepts;
import genex.to_container;
import genex.views.filter;
import genex.views.in;
import genex.views.materialize;
import genex.views.reverse;
import genex.views.transform;
import genex.views.zip;

namespace {
  constexpr auto is_even = [](const int x) { return x % 2 == 0; };
  constexpr auto twice = [](const int x) { return x * 2; };

  auto make_vec() -> std::vector<int> { return {1, 2, 3, 4, 5, 6}; }
}

// Every overload sees the iterator that iter_pair returns.
static_assert(std::same_as<genex::iterator_t<std::vector<int>>, std::vector<int>::iterator>);
static_assert(std::same_as<genex::iterator_t<std::vector<int>&>, std::vector<int>::iterator>);
static_assert(std::same_as<genex::iterator_t<const std::vector<int>&>, std::vector<int>::const_iterator>);

// A direct call has nowhere to keep an rvalue, so it is rejected instead of dangling.
static_assert(std::is_invocable_v<decltype(genex::views::filter), std::vector<int>&, decltype(is_even)>);
static_assert(not std::is_invocable_v<decltype(genex::views::filter), std::vector<int>, decltype(is_even)>);

TEST(GenexViewsOwning, RvalueOutlivesTheStatement) {
  auto rng = make_vec() | genex::views::filter(is_even);
  const auto exp = std::vector{2, 4, 6};
  EXPECT_EQ(rng | genex::to<std::vector>(), exp);
}

TEST(GenexViewsOwning, IteratesMoreThanOnce) {
  auto rng = make_vec() | genex::views::filter(is_even);
  const auto first = rng | genex::to<std::vector>();
  const auto second = rng | genex::to<std::vector>();
  EXPECT_EQ(first, second);
}

TEST(GenexViewsOwning, ChainOutlivesTheStatement) {
  auto rng = make_vec() | genex::views::filter(is_even) | genex::views::transform(twice);
  const auto exp = std::vector{4, 8, 12};
  EXPECT_EQ(rng | genex::to<std::vector>(), exp);
}

// A short string lives inside the object, so moving it invalidates any iterators into it.
TEST(GenexViewsOwning, SurvivesBeingMoved) {
  auto rng = std::string("ab") | genex::views::transform([](const char c) { return static_cast<char>(c + 1); });
  auto moved = std::move(rng);
  EXPECT_EQ(moved | genex::to<std::string>(), "bc");
}

TEST(GenexViewsOwning, MaterializeThenReverse) {
  auto rng = make_vec()
    | genex::views::filter(is_even)
    | genex::views::materialize
    | genex::views::reverse;
  const auto exp = std::vector{6, 4, 2};
  EXPECT_EQ(rng | genex::to<std::vector>(), exp);
}

TEST(GenexViewsOwning, LvalueSecondRangeIsNotCopied) {
  auto lhs = std::vector{1, 2};
  auto rhs = std::vector{3, 4};
  auto rng = lhs | genex::views::zip(rhs);
  rhs[0] = 9;

  const auto out = rng | genex::to<std::vector>();
  EXPECT_EQ(std::get<1>(out[0]), 9);
}

TEST(GenexViewsOwning, RvalueSecondRangeOutlivesTheStatement) {
  auto vec = make_vec();
  auto rng = vec | genex::views::in(std::vector{2, 4});
  const auto exp = std::vector{2, 4};
  EXPECT_EQ(rng | genex::to<std::vector>(), exp);
}

TEST(GenexViewsOwning, RvalueOnBothSidesOutlivesTheStatement) {
  auto rng = make_vec() | genex::views::in(std::vector{2, 4}) | genex::views::transform(twice);
  const auto exp = std::vector{4, 8};
  EXPECT_EQ(rng | genex::to<std::vector>(), exp);
}
