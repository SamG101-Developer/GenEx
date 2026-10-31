#include <gtest/gtest.h>
#include <concepts>
#include <cstddef>
#include <list>
#include <string>
#include <type_traits>
#include <vector>

import genex.to_container;
import genex.views.as_const;
import genex.views.chunk;
import genex.views.concat;
import genex.views.enumerate;
import genex.views.generate;
import genex.views.intersperse;
import genex.views.iota;
import genex.views.join;
import genex.views.join_with;
import genex.views.move_reverse;
import genex.views.pairwise;
import genex.views.repeat;
import genex.views.replace;
import genex.views.reverse;
import genex.views.scan;
import genex.views.slide;
import genex.views.split;
import genex.views.stride;
import genex.views.take;
import genex.views.transform;
import genex.views.zip;

namespace {
  constexpr auto twice = [](const int x) { return x * 2; };

  // `to` trusts size() to fill a container in bulk, so a wrong one would read past the end. This walks instead.
  template <typename Rng>
  auto expect_exact_size(Rng &&rng) -> void {
    auto count = std::size_t{0};
    auto first = rng.begin();
    const auto last = rng.end();
    for (; first != last; ++first) { ++count; }
    EXPECT_EQ(static_cast<std::size_t>(rng.size()), count);
  }

  auto vectors() -> std::vector<std::vector<int>> {
    return {{}, {1}, {1, 2}, {1, 2, 3}, {1, 2, 3, 4}, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}};
  }
}

TEST(GenexViewSizes, OneToOne) {
  for (auto vec : vectors()) {
    expect_exact_size(vec | genex::views::transform(twice));
    expect_exact_size(vec | genex::views::as_const);
    expect_exact_size(vec | genex::views::enumerate);
    expect_exact_size(vec | genex::views::reverse);
    expect_exact_size(vec | genex::views::replace(1, 9));
    expect_exact_size(vec | genex::views::scan(0));
  }
}

TEST(GenexViewSizes, Stride) {
  for (auto vec : vectors()) {
    for (auto step : {1, 2, 3, 4, 7, 20}) { expect_exact_size(vec | genex::views::stride(step)); }
  }
}

TEST(GenexViewSizes, Chunk) {
  for (auto vec : vectors()) {
    for (auto n : {1, 2, 3, 4, 7, 20}) { expect_exact_size(vec | genex::views::chunk(n)); }
  }
}

TEST(GenexViewSizes, Slide) {
  for (auto vec : vectors()) {
    for (auto n : {1, 2, 3, 4, 7, 20}) { expect_exact_size(vec | genex::views::slide(n)); }
  }
}

TEST(GenexViewSizes, Pairwise) {
  for (auto vec : vectors()) { expect_exact_size(vec | genex::views::pairwise); }
}

TEST(GenexViewSizes, Intersperse) {
  for (auto vec : vectors()) { expect_exact_size(vec | genex::views::intersperse(9)); }
}

TEST(GenexViewSizes, Zip) {
  for (auto lhs : vectors()) {
    for (auto rhs : vectors()) { expect_exact_size(genex::views::zip(lhs, rhs)); }
  }
}

TEST(GenexViewSizes, Concat) {
  for (auto lhs : vectors()) {
    for (auto rhs : vectors()) { expect_exact_size(genex::views::concat(lhs, rhs)); }
  }
}

TEST(GenexViewSizes, Join) {
  auto nested = vectors();
  expect_exact_size(nested | genex::views::join);
  expect_exact_size(nested | genex::views::join_with(9));

  auto empty = std::vector<std::vector<int>>{};
  expect_exact_size(empty | genex::views::join);
  expect_exact_size(empty | genex::views::join_with(9));

  auto empties = std::vector<std::vector<int>>{{}, {}};
  expect_exact_size(empties | genex::views::join);
  expect_exact_size(empties | genex::views::join_with(9));
}

TEST(GenexViewSizes, Split) {
  for (auto vec : std::vector<std::vector<int>>{{}, {1}, {0}, {1, 0}, {0, 1}, {1, 0, 2}, {0, 0}, {1, 0, 0, 2, 0}}) {
    expect_exact_size(vec | genex::views::split(0));
  }
}

TEST(GenexViewSizes, Iota) {
  expect_exact_size(genex::views::iota(0, 0));
  expect_exact_size(genex::views::iota(0, 10));
  expect_exact_size(genex::views::iota(0, 10, 3));
  expect_exact_size(genex::views::iota(0, 9, 3));
}

TEST(GenexViewSizes, Counted) {
  expect_exact_size(genex::views::repeat(1, 0));
  expect_exact_size(genex::views::repeat(1, 5));
  expect_exact_size(genex::views::generate_n([] { return 1; }, 0));
  expect_exact_size(genex::views::generate_n([] { return 1; }, 5));
}

TEST(GenexViewSizes, TakeFromEndless) {
  const auto rng = genex::views::repeat(7) | genex::views::take(3) | genex::to<std::vector>();
  EXPECT_EQ(rng, (std::vector{7, 7, 7}));

  auto next = 0;
  const auto gen = genex::views::generate([next]() mutable { return next++; })
    | genex::views::take(4)
    | genex::to<std::vector>();
  EXPECT_EQ(gen, (std::vector{0, 1, 2, 3}));
}

TEST(GenexViewSizes, BulkFillOverAList) {
  auto lst = std::list{1, 2, 3};
  const auto rng = lst | genex::views::transform(twice) | genex::to<std::vector>();
  EXPECT_EQ(rng, (std::vector{2, 4, 6}));
}

TEST(GenexViewSizes, BulkFillIntoAString) {
  auto str = std::string("abc");
  const auto rng = str
    | genex::views::transform([](const char c) { return static_cast<char>(c + 1); })
    | genex::to<std::string>();
  EXPECT_EQ(rng, "bcd");
}

namespace {
  // `to` takes several routes depending on the view, and they should all give what iterating does.
  template <typename Rng>
  auto expect_to_matches_iterating(Rng &&rng) -> void {
    auto walked = std::vector<std::remove_cvref_t<decltype(*rng.begin())>>{};
    for (auto first = rng.begin(), last = first; first != rng.end(); ++first) { walked.push_back(*first); }
    const auto out = rng | genex::to<std::vector>();
    EXPECT_EQ(out, walked);
  }
}

TEST(GenexToRoutes, Intersperse) {
  for (auto vec : vectors()) { expect_to_matches_iterating(vec | genex::views::intersperse(9)); }
}

TEST(GenexToRoutes, Stride) {
  for (auto vec : vectors()) {
    for (auto step : {1, 2, 3, 4, 7, 20}) { expect_to_matches_iterating(vec | genex::views::stride(step)); }
  }
}

TEST(GenexToRoutes, Concat) {
  for (auto lhs : vectors()) {
    for (auto rhs : vectors()) { expect_to_matches_iterating(genex::views::concat(lhs, rhs)); }
  }
}

TEST(GenexToRoutes, ConcatOfTransforms) {
  auto lhs = std::vector{1, 2};
  auto rhs = std::vector{3};
  expect_to_matches_iterating(
    genex::views::concat(lhs | genex::views::transform(twice), rhs | genex::views::transform(twice)));
}

TEST(GenexToRoutes, OverAList) {
  auto lst = std::list{1, 2, 3, 4};
  expect_to_matches_iterating(lst | genex::views::intersperse(9));
  expect_to_matches_iterating(lst | genex::views::stride(2));
}

TEST(GenexToRoutes, IntoAString) {
  auto str = std::string("abc");
  EXPECT_EQ(str | genex::views::intersperse('-') | genex::to<std::string>(), "a-b-c");
  EXPECT_EQ(str | genex::views::stride(2) | genex::to<std::string>(), "ac");
}

TEST(GenexToRoutes, NonTrivialElements) {
  auto vec = std::vector<std::string>{"a", "b"};
  const auto out = vec | genex::views::intersperse(std::string("-")) | genex::to<std::vector>();
  EXPECT_EQ(out, (std::vector<std::string>{"a", "-", "b"}));
}
