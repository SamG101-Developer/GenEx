#include <gtest/gtest.h>
#include <concepts>
#include <iterator>
#include <list>
#include <type_traits>
#include <utility>
#include <vector>

import genex.iterators.distance;
import genex.iterators.next;
import genex.iterators.prev;
import genex.views.filter;
import genex.views.move_reverse;
import genex.views.reverse;
import genex.views.stride;
import genex.views.take;
import genex.views.transform;
import genex.to_container;

namespace {
  constexpr auto twice = [](const int x) { return x * 2; };
  constexpr auto is_even = [](const int x) { return x % 2 == 0; };
  constexpr auto as_is = [](int &x) -> int& { return x; };

  template <typename F>
  using transformed_t = decltype(genex::views::transform(std::declval<std::vector<int>&>(), std::declval<F>()).begin());

  using twice_t = transformed_t<decltype(twice)>;
  using as_is_t = transformed_t<decltype(as_is)>;
  using filtered_t = decltype(genex::views::filter(std::declval<std::vector<int>&>(), is_even).begin());
  using move_reversed_t = decltype(genex::views::move_reverse(std::declval<std::vector<int>&>()).begin());
  using strided_t = decltype(genex::views::stride(std::declval<std::vector<int>&>(), 2).begin());

  template <typename I>
  concept decrementable = requires(I it) { --it; };
}

// Returning a prvalue is an input iterator to legacy algorithms, but keeps its full strength everywhere else.
static_assert(std::same_as<std::iterator_traits<twice_t>::iterator_category, std::input_iterator_tag>);
static_assert(std::same_as<std::iterator_traits<as_is_t>::iterator_category, std::random_access_iterator_tag>);
static_assert(std::random_access_iterator<twice_t>);
static_assert(std::random_access_iterator<as_is_t>);

// The strength survives a chain, which a tag read from the inner iterator's legacy category would lose.
static_assert(std::random_access_iterator<decltype(
  genex::views::transform(genex::views::transform(std::declval<std::vector<int>&>(), twice), twice).begin())>);

// A filtered view can't walk backwards.
static_assert(not std::bidirectional_iterator<filtered_t>);
static_assert(not decrementable<filtered_t>);

// Subtracting positions in the inner iterator would miscount these, or for a reversed iterator give a negative size.
static_assert(not std::sized_sentinel_for<move_reversed_t, move_reversed_t>);
static_assert(not std::sized_sentinel_for<strided_t, strided_t>);

// A count is an integer, even though an iterator is also weakly incrementable.
static_assert(std::is_invocable_v<decltype(genex::views::take), std::vector<int>&, int>);
static_assert(not std::is_invocable_v<decltype(genex::views::take), std::vector<int>&, std::vector<int>::iterator>);
static_assert(not std::is_invocable_v<decltype(genex::views::take), std::vector<int>::iterator>);

TEST(GenexIteratorCategory, ReverseOverTransform) {
  auto vec = std::vector{1, 2, 3};
  const auto rng = vec
    | genex::views::transform(twice)
    | genex::views::reverse
    | genex::to<std::vector>();
  const auto exp = std::vector{6, 4, 2};
  EXPECT_EQ(rng, exp);
}

// These dispatch on what an iterator models. By its legacy category alone, this would be walked forwards.
TEST(GenexIteratorCategory, NextAndPrevOverTransform) {
  auto vec = std::vector{1, 2, 3};
  auto rng = genex::views::transform(vec, twice);

  const auto second = genex::iterators::next(rng.begin());
  EXPECT_EQ(*second, 4);
  EXPECT_EQ(*genex::iterators::prev(second), 2);
  EXPECT_EQ(*genex::iterators::next(second, -1), 2);
}

TEST(GenexIteratorCategory, SizeOverMoveReverse) {
  auto vec = std::vector{1, 2, 3};
  auto rng = vec | genex::views::move_reverse | genex::views::transform(twice);
  EXPECT_EQ(rng.size(), 3);
}

TEST(GenexIteratorCategory, NextAndPrevOverBidirectional) {
  auto lst = std::list{1, 2, 3};
  const auto second = genex::iterators::next(lst.begin());
  EXPECT_EQ(*second, 2);
  EXPECT_EQ(*genex::iterators::prev(second), 1);
  EXPECT_EQ(*genex::iterators::next(second, -1), 1);
}

TEST(GenexIteratorCategory, DistanceOverTransform) {
  auto vec = std::vector{1, 2, 3};
  auto rng = genex::views::transform(vec, twice);
  EXPECT_EQ(genex::iterators::distance(rng.begin(), genex::iterators::next(rng.begin(), 2)), 2);
}

// The range constructor can't size itself up front from an input-category iterator, so `to` reserves instead.
TEST(GenexIteratorCategory, ToReservesOverTransform) {
  auto vec = std::vector<int>(100);
  const auto rng = vec | genex::views::transform(twice) | genex::to<std::vector>();

  auto reserved = std::vector<int>{};
  reserved.reserve(100);
  EXPECT_EQ(rng.capacity(), reserved.capacity());
}
