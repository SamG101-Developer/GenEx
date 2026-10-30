#include <gtest/gtest.h>
#include <concepts>
#include <utility>
#include <vector>

import genex.actions.sort;
import genex.algorithms.find_if;
import genex.algorithms.min_element;
import genex.concepts;
import genex.operations.access;
import genex.to_container;
import genex.views.divide;
import genex.views.filter;

namespace {
  constexpr auto is_even = [](const int x) { return x % 2 == 0; };

  auto make_vec() -> std::vector<int> { return {3, 1, 2}; }

  using vec_t = std::vector<int>;
}

// An iterator into an rvalue would dangle, so the result can't be dereferenced.
static_assert(std::same_as<decltype(genex::find_if(std::declval<vec_t>(), is_even)), genex::dangling>);
static_assert(std::same_as<decltype(genex::find_if(std::declval<vec_t&>(), is_even)), vec_t::iterator>);

// A view's iterators don't point into the view, so finding through one is still useful.
static_assert(std::same_as<
  decltype(genex::find_if(genex::views::filter(std::declval<vec_t&>(), is_even), is_even)),
  genex::iterator_t<decltype(genex::views::filter(std::declval<vec_t&>(), is_even))>>);

// An element of an rvalue is copied out, and of an lvalue is referred to.
static_assert(std::same_as<decltype(genex::min_element(std::declval<vec_t>())), int>);
static_assert(std::same_as<decltype(genex::min_element(std::declval<vec_t&>())), int&>);
static_assert(std::same_as<decltype(genex::operations::front(std::declval<vec_t>())), int>);
static_assert(std::same_as<decltype(genex::operations::front(std::declval<vec_t&>())), int&>);

// An action hands an rvalue back by value, and an lvalue by reference.
static_assert(std::same_as<decltype(genex::actions::sort(std::declval<vec_t>())), vec_t>);
static_assert(std::same_as<decltype(genex::actions::sort(std::declval<vec_t&>())), vec_t&>);

TEST(GenexDangling, ElementOfRvalueOutlivesTheCall) {
  const auto &min = genex::min_element(make_vec());
  EXPECT_EQ(min, 1);
}

TEST(GenexDangling, ActionOnRvalueOutlivesTheStatement) {
  auto &&sorted = make_vec() | genex::actions::sort();
  const auto exp = vec_t{1, 2, 3};
  EXPECT_EQ(sorted, exp);
}

TEST(GenexDangling, ActionOnLvalueSortsInPlace) {
  auto vec = make_vec();
  auto &sorted = vec | genex::actions::sort();
  EXPECT_EQ(&sorted, &vec);
  EXPECT_EQ(vec[0], 1);
}

TEST(GenexDangling, DivideRvalueOwnsItsParts) {
  const auto [a, b] = vec_t{1, 2, 3, 4} | genex::views::divide<2>;
  EXPECT_EQ(a, (vec_t{1, 2}));
  EXPECT_EQ(b, (vec_t{3, 4}));
}

TEST(GenexDangling, DivideRvalueWithCallSyntax) {
  const auto [a, b] = vec_t{1, 2, 3} | genex::views::divide<2>();
  EXPECT_EQ(a, (vec_t{1, 2}));
  EXPECT_EQ(b, (vec_t{3}));
}
