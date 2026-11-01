#include <gtest/gtest.h>
#include <stdexcept>
#include <utility>
#include <vector>

import genex.actions.clear;
import genex.actions.filter;
import genex.actions.pop_back;
import genex.actions.remove;
import genex.actions.take;
import genex.meta;
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

  using vec_ref = std::vector<int>&;
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
