#include <forward_list>
#include <gtest/gtest.h>
#include <list>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

import genex.actions.clear;
import genex.actions.filter;
import genex.actions.pop_back;
import genex.actions.remove;
import genex.actions.take;
import genex.iterators.distance;
import genex.iterators.iter_swap;
import genex.iterators.next;
import genex.iterators.prev;
import genex.meta;
import genex.operations.access;
import genex.operations.cmp;
import genex.operations.empty;
import genex.operations.size;
import genex.operations.tuple;
import genex.span;
import genex.views.drop;
import genex.views.filter;
import genex.views.materialize;
import genex.views.stride;
import genex.views.zip;

namespace {
  constexpr auto keep = [](const int) { return true; };

  constexpr auto always_throws = [](const int) -> bool { throw std::runtime_error("predicate"); };

  constexpr auto throws_on_three = [](const int x) -> bool {
    if (x == 3) { throw std::runtime_error("predicate"); }
    return true;
  };

  struct ThrowsOnCopy {
    ThrowsOnCopy() = default;
    ThrowsOnCopy(ThrowsOnCopy &&) noexcept = default;
    ThrowsOnCopy(const ThrowsOnCopy &) { throw std::runtime_error("copy"); }
    auto operator=(ThrowsOnCopy &&) noexcept -> ThrowsOnCopy& = default;
    auto operator=(const ThrowsOnCopy &) -> ThrowsOnCopy& { throw std::runtime_error("copy"); }
  };

  struct CountedCopy {
    static inline int live = 0;
    bool armed = false;

    explicit CountedCopy(const bool armed) : armed(armed) { ++live; }
    CountedCopy(CountedCopy &&that) noexcept : armed(that.armed) { ++live; }

    CountedCopy(const CountedCopy &that) : armed(that.armed) {
      if (armed) { throw std::runtime_error("copy"); }
      ++live;
    }

    ~CountedCopy() { --live; }
  };

  struct ThrowingBegin {
    auto begin() -> int* { throw std::runtime_error("begin"); }
    auto end() noexcept -> int* { return nullptr; }
  };

  struct ThrowingEq {
    auto operator==(const ThrowingEq &) const -> bool { throw std::runtime_error("eq"); }
  };

  using vec_ref = std::vector<int>&;
  using list_iter = std::list<int>::iterator;
}

// Building a view only moves its arguments, so it stays noexcept over a range whose begin and end are.
static_assert(noexcept(genex::views::filter(std::declval<vec_ref>(), keep)));
static_assert(noexcept(genex::views::stride(std::declval<vec_ref>(), 1)));

// begin on an inner view can run a predicate, and an outer view calls it while being built.
static_assert(not noexcept(genex::views::filter(genex::views::filter(std::declval<vec_ref>(), keep), keep)));

// Allocating, and copying a range into a closure.
static_assert(not noexcept(genex::views::materialize(std::declval<vec_ref>())));
static_assert(not noexcept(genex::views::zip(std::declval<vec_ref>())));

// A partial action only stores its arguments.
static_assert(noexcept(genex::actions::take(1)));
static_assert(noexcept(genex::actions::filter(keep)));

// A partial action moves an rvalue it's given, but has to copy an lvalue.
static_assert(noexcept(genex::actions::remove(std::declval<ThrowsOnCopy>())));
static_assert(not noexcept(genex::actions::remove(std::declval<ThrowsOnCopy&>())));

// An action making one member call is as safe as that call.
static_assert(noexcept(genex::actions::clear(std::declval<vec_ref>())));
static_assert(
  noexcept(genex::actions::pop_back(std::declval<vec_ref>())) == noexcept(std::declval<vec_ref>().pop_back()));

// A span built from a range calls its begin and end.
static_assert(noexcept(genex::span<int>(std::declval<vec_ref>())));
static_assert(not noexcept(genex::span<int>(std::declval<ThrowingBegin&>())));

// Moving an iterator by more than a jump walks it, which can run anything.
static_assert(noexcept(genex::iterators::next(std::declval<int*>())));
static_assert(noexcept(genex::iterators::prev(std::declval<int*>())));
static_assert(not noexcept(genex::iterators::next(std::declval<list_iter>())));
static_assert(not noexcept(genex::iterators::prev(std::declval<list_iter>())));

// The same goes for measuring a range.
static_assert(noexcept(genex::iterators::distance(std::declval<int*>(), std::declval<int*>())));
static_assert(noexcept(genex::iterators::distance(std::declval<vec_ref>())));
static_assert(not noexcept(genex::iterators::distance(std::declval<std::list<int>&>())));
static_assert(noexcept(genex::iterators::iter_swap(std::declval<int*>(), std::declval<int*>())));

// Sizing a range only has to walk it if it can't say.
static_assert(noexcept(genex::operations::size(std::declval<vec_ref>())));
static_assert(noexcept(genex::operations::empty(std::declval<vec_ref>())));
static_assert(not noexcept(genex::operations::size(std::declval<std::forward_list<int>&>())));

// An element is a copy from an rvalue range, and a reference otherwise.
static_assert(noexcept(genex::operations::at(std::declval<vec_ref>(), 0)) == noexcept(std::declval<vec_ref>()[0]));
static_assert(not noexcept(genex::operations::front(std::declval<std::vector<ThrowsOnCopy>>())));

// A comparison is as safe as the operator, and the projection if there is one.
static_assert(noexcept(genex::operations::eq{}(1, 2)));
static_assert(noexcept(genex::operations::eq{}(1, 2, genex::meta::identity{})));
static_assert(noexcept(genex::operations::eq_fixed<int>{1}(2)));
static_assert(not noexcept(genex::operations::eq{}(1, 2, keep)));
static_assert(not noexcept(genex::operations::eq{}(std::declval<ThrowingEq&>(), std::declval<ThrowingEq&>())));

// Getting from a tuple or a pair is a reference, so nothing is copied.
static_assert(noexcept(genex::get<0>(std::declval<std::tuple<ThrowsOnCopy>&>())));
static_assert(noexcept(genex::get<0>(std::declval<std::pair<ThrowsOnCopy, int>>())));
static_assert(noexcept(genex::get<0>(std::declval<std::pair<ThrowsOnCopy, int>&>())));
static_assert(noexcept(genex::get<0>(std::declval<const std::pair<ThrowsOnCopy, int>&>())));

TEST(GenexNoexcept, ThrowingPredicateEscapesBegin) {
  auto vec = std::vector{1, 2, 3};
  auto rng = genex::views::filter(vec, always_throws);
  EXPECT_THROW(static_cast<void>(rng.begin()), std::runtime_error);
}

TEST(GenexNoexcept, ThrowingPredicateEscapesOuterView) {
  auto vec = std::vector{1, 2, 3};
  EXPECT_THROW(
    static_cast<void>(genex::views::filter(genex::views::filter(vec, always_throws), keep)), std::runtime_error);
}

TEST(GenexNoexcept, ThrowingPredicateEscapesEagerTraversal) {
  auto vec = std::vector{1, 2, 3};
  EXPECT_THROW(
    static_cast<void>(genex::views::drop(genex::views::filter(vec, throws_on_three), 2)), std::runtime_error);
}

TEST(GenexNoexcept, ThrowingCopyEscapesMaterialize) {
  auto vec = std::vector<ThrowsOnCopy>(1);
  EXPECT_THROW(static_cast<void>(genex::views::materialize(vec)), std::runtime_error);
}

TEST(GenexBox, ThrowingCopyAssignmentLeavesBoxEmptyAndDestroysOnce) {
  CountedCopy::live = 0;
  {
    auto lhs = genex::meta::box<CountedCopy>(CountedCopy(false));
    auto rhs = genex::meta::box<CountedCopy>(CountedCopy(true));
    EXPECT_THROW(lhs = rhs, std::runtime_error);
    EXPECT_EQ(CountedCopy::live, 1);
  }
  EXPECT_EQ(CountedCopy::live, 0);
}

TEST(GenexBox, CopyAssignmentReplacesValue) {
  CountedCopy::live = 0;
  {
    auto lhs = genex::meta::box<CountedCopy>(CountedCopy(false));
    const auto rhs = genex::meta::box<CountedCopy>(CountedCopy(false));
    lhs = rhs;
    EXPECT_FALSE((*lhs).armed);
    EXPECT_EQ(CountedCopy::live, 2);
  }
  EXPECT_EQ(CountedCopy::live, 0);
}
