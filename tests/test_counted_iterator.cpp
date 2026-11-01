#include <gtest/gtest.h>
#include <array>
#include <forward_list>
#include <iterator>
#include <list>
#include <vector>

import genex.concepts;
import genex.detail.counted_iterator;
import genex.to_container;
import genex.views.drop_last;
import genex.views.filter;
import genex.views.slice;
import genex.views.take;
import genex.views.transform;

using genex::detail::counted_iterator;
using genex::detail::counted_sentinel;

// It's as strong as what it walks, and always sized against itself.
static_assert(std::contiguous_iterator<counted_iterator<int*>>);
static_assert(std::random_access_iterator<counted_iterator<std::vector<int>::iterator>>);
static_assert(std::bidirectional_iterator<counted_iterator<std::list<int>::iterator>>);
static_assert(not std::random_access_iterator<counted_iterator<std::list<int>::iterator>>);
static_assert(std::forward_iterator<counted_iterator<std::forward_list<int>::iterator>>);
static_assert(not std::bidirectional_iterator<counted_iterator<std::forward_list<int>::iterator>>);
static_assert(std::sized_sentinel_for<
  counted_iterator<std::list<int>::iterator>, counted_iterator<std::list<int>::iterator>>);

static_assert(std::sized_sentinel_for<counted_sentinel<int*>, counted_iterator<int*>>);
static_assert(std::sized_sentinel_for<counted_sentinel<genex::unreachable_sentinel>, counted_iterator<int*>>);
static_assert(not std::sized_sentinel_for<
  counted_sentinel<std::list<int>::iterator>, counted_iterator<std::list<int>::iterator>>);

TEST(GenexCountedIterator, CountsDownWhileWalking) {
  auto vec = std::vector{1, 2, 3, 4};
  auto it = counted_iterator(vec.begin(), 3);
  EXPECT_EQ(*it, 1);
  EXPECT_EQ(it.count(), 3);

  ++it;
  EXPECT_EQ(*it, 2);
  EXPECT_EQ(it.count(), 2);
  EXPECT_EQ(it.base(), vec.begin() + 1);

  --it;
  EXPECT_EQ(it.count(), 3);

  it += 2;
  EXPECT_EQ(*it, 3);
  EXPECT_EQ(it.count(), 1);
  EXPECT_EQ(it[1], 4);
}

TEST(GenexCountedIterator, ComparedAndSubtractedByCount) {
  auto vec = std::vector{1, 2, 3, 4};
  const auto first = counted_iterator(vec.begin(), 3);
  const auto last = first + 3;
  EXPECT_EQ(last - first, 3);
  EXPECT_EQ(first - last, -3);
  EXPECT_LT(first, last);
  EXPECT_EQ(first + 3, last);
  EXPECT_NE(first, last);

  // A list has no subtraction of its own, so this is the count doing the work.
  auto lst = std::list{1, 2, 3};
  auto a = counted_iterator(lst.begin(), 3);
  auto b = a;
  ++b;
  EXPECT_EQ(b - a, 1);
}

TEST(GenexCountedIterator, SentinelEndsAtWhicheverComesFirst) {
  auto vec = std::vector{1, 2, 3};
  const auto end = counted_sentinel<std::vector<int>::iterator>{vec.end()};
  EXPECT_EQ(end - counted_iterator(vec.begin(), 2), 2);
  EXPECT_EQ(end - counted_iterator(vec.begin(), 9), 3);
  EXPECT_EQ(counted_iterator(vec.begin(), 2) - end, -2);
  EXPECT_TRUE(counted_iterator(vec.begin(), 0) == end);
  EXPECT_TRUE(counted_iterator(vec.end(), 5) == end);
  EXPECT_FALSE(counted_iterator(vec.begin(), 1) == end);

  const auto only_count = counted_sentinel<genex::unreachable_sentinel>{};
  EXPECT_EQ(only_count - counted_iterator(vec.begin(), 2), 2);
  EXPECT_TRUE(counted_iterator(vec.begin(), 0) == only_count);
  EXPECT_FALSE(counted_iterator(vec.end(), 1) == only_count);
}

TEST(GenexCountedIterator, ThroughTheViews) {
  auto lst = std::list{1, 2, 3, 4, 5};
  EXPECT_EQ(lst | genex::views::take(2) | genex::to<std::vector>(), (std::vector{1, 2}));
  EXPECT_EQ(lst | genex::views::take(9) | genex::to<std::vector>(), (std::vector{1, 2, 3, 4, 5}));
  EXPECT_EQ(lst | genex::views::slice(1, 3) | genex::to<std::vector>(), (std::vector{2, 3}));

  auto fwd = std::forward_list{1, 2, 3, 4};
  EXPECT_EQ(fwd | genex::views::drop_last(1) | genex::to<std::vector>(), (std::vector{1, 2, 3}));

  auto vec = std::vector{1, 2, 3, 4};
  const auto is_even = [](const int x) { return x % 2 == 0; };
  EXPECT_EQ(vec | genex::views::filter(is_even) | genex::views::take(1) | genex::to<std::vector>(), (std::vector{2}));

  const auto twice = [](const int x) { return x * 2; };
  EXPECT_EQ(
    vec | genex::views::transform(twice) | genex::views::take(2) | genex::to<std::vector>(),
    (std::vector{2, 4}));
}

TEST(GenexCountedIterator, TakeHintsAtItsCountWhenUnsized) {
  auto lst = std::list{1, 2, 3, 4, 5};
  auto rng = lst | genex::views::transform([](const int x) { return x; }) | genex::views::take(2);
  EXPECT_EQ(rng.reserve_hint(), 2uz);
}
