#include <gtest/gtest.h>
#include <array>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

import genex.actions.fill;
import genex.actions.replace;
import genex.actions.replace_if;
import genex.actions.reverse;
import genex.actions.sort;
import genex.actions.transform;
import genex.algorithms.for_each;
import genex.concepts;
import genex.to_container;
import genex.views.address_of;
import genex.views.as_const;
import genex.views.cast_dynamic;
import genex.views.cast_static;
import genex.views.chunk;
import genex.views.chunk_by;
import genex.views.concat;
import genex.views.cycle;
import genex.views.divide;
import genex.views.drop;
import genex.views.drop_last;
import genex.views.drop_while;
import genex.views.duplicates;
import genex.views.empty;
import genex.views.enumerate;
import genex.views.filter;
import genex.views.generate;
import genex.views.in;
import genex.views.indirect;
import genex.views.interleave;
import genex.views.intersperse;
import genex.views.iota;
import genex.views.join;
import genex.views.join_with;
import genex.views.map;
import genex.views.materialize;
import genex.views.move;
import genex.views.move_reverse;
import genex.views.pairwise;
import genex.views.ptr;
import genex.views.remove;
import genex.views.remove_if;
import genex.views.replace;
import genex.views.replace_if;
import genex.views.reverse;
import genex.views.scan;
import genex.views.set_algorithms;
import genex.views.slice;
import genex.views.slide;
import genex.views.split;
import genex.views.stride;
import genex.views.take;
import genex.views.take_last;
import genex.views.take_while;
import genex.views.transform;
import genex.views.tuple_nth;
import genex.views.unique;
import genex.views.view;
import genex.views.zip;
import genex.views.zip_with;

namespace {
  constexpr auto yes = [](const int) { return true; };
  constexpr auto twice = [](const int x) { return x * 2; };
  constexpr auto same = [](const int a, const int b) { return a == b; };
  constexpr auto add = [](const int a, const int b) { return a + b; };

  template <typename V>
  concept a_view = genex::input_range<V> and genex::enable_view<std::remove_cvref_t<V>>;

  using ints = std::vector<int>;
  ints v;
  ints w;
  std::vector<ints> nested;
  std::vector<std::unique_ptr<int>> pointers;
  std::vector<std::tuple<int, int>> tuples;
}

// Every view has to be usable as a range.
#define CHECK_VIEW(...) static_assert(a_view<decltype(__VA_ARGS__)>)

CHECK_VIEW(v | genex::views::address_of);
CHECK_VIEW(v | genex::views::as_const);
CHECK_VIEW(v | genex::views::cast_static<long>);
CHECK_VIEW(v | genex::views::chunk(2));
CHECK_VIEW(v | genex::views::chunk_by(same));
CHECK_VIEW(genex::views::concat(v, w));
CHECK_VIEW(v | genex::views::cycle);
CHECK_VIEW(v | genex::views::drop(1));
CHECK_VIEW(v | genex::views::drop_last(1));
CHECK_VIEW(v | genex::views::drop_while(yes));
CHECK_VIEW(v | genex::views::duplicates);
CHECK_VIEW(genex::views::empty<int>);
CHECK_VIEW(v | genex::views::enumerate);
CHECK_VIEW(v | genex::views::filter(yes));
CHECK_VIEW(genex::views::generate([] { return 1; }));
CHECK_VIEW(v | genex::views::in(w));
CHECK_VIEW(pointers | genex::views::indirect);
CHECK_VIEW(v | genex::views::interleave(w));
CHECK_VIEW(v | genex::views::intersperse(0));
CHECK_VIEW(genex::views::iota(0, 3));
CHECK_VIEW(nested | genex::views::join);
CHECK_VIEW(nested | genex::views::join_with(0));
CHECK_VIEW(v | genex::views::move);
CHECK_VIEW(v | genex::views::move_reverse);
CHECK_VIEW(v | genex::views::pairwise);
CHECK_VIEW(pointers | genex::views::ptr);
CHECK_VIEW(v | genex::views::remove(1));
CHECK_VIEW(v | genex::views::remove_if(yes));
CHECK_VIEW(v | genex::views::replace(1, 2));
CHECK_VIEW(v | genex::views::replace_if(yes, 2));
CHECK_VIEW(v | genex::views::reverse);
CHECK_VIEW(v | genex::views::scan(0));
CHECK_VIEW(v | genex::views::partial_sum);
CHECK_VIEW(v | genex::views::set_union(w));
CHECK_VIEW(v | genex::views::set_intersection(w));
CHECK_VIEW(v | genex::views::set_difference(w));
CHECK_VIEW(v | genex::views::set_symmetric_difference(w));
CHECK_VIEW(v | genex::views::slice(0, 1));
CHECK_VIEW(v | genex::views::slide(2));
CHECK_VIEW(v | genex::views::split(0));
CHECK_VIEW(v | genex::views::stride(2));
CHECK_VIEW(v | genex::views::take(1));
CHECK_VIEW(v | genex::views::take_last(1));
CHECK_VIEW(v | genex::views::take_while(yes));
CHECK_VIEW(v | genex::views::transform(twice));
CHECK_VIEW(tuples | genex::views::tuple_nth<0>);
CHECK_VIEW(v | genex::views::unique);
CHECK_VIEW(v | genex::views::view);
CHECK_VIEW(genex::views::zip(v, w));
CHECK_VIEW(genex::views::zip_with(v, w, add));

// A view stores its callable, so it has to be able to move one.
struct cannot_move {
  cannot_move() = default;
  cannot_move(cannot_move&&) = delete;
  auto operator()(int) const -> int { return 0; }
};

template <typename F>
concept zip_withable = requires(F &&f) { genex::views::zip_with(v, w, std::forward<F>(f)); };

static_assert(zip_withable<decltype(add)>);
static_assert(not zip_withable<cannot_move>);

// An array isn't taken for the callable of a closure, or for the value it's compared to.
namespace {
  int array[3] = {1, 2, 3};
  const int const_array[3] = {1, 2, 3};
}

#define CHECK_CALL(...) static_assert(requires { __VA_ARGS__; })

CHECK_CALL(genex::views::transform(array, twice));
CHECK_CALL(genex::views::filter(array, yes));
CHECK_CALL(genex::views::remove_if(array, yes));
CHECK_CALL(genex::views::take_while(array, yes));
CHECK_CALL(genex::views::drop_while(array, yes));
CHECK_CALL(genex::views::replace_if(array, yes, 1));
CHECK_CALL(genex::views::chunk_by(array, same));
CHECK_CALL(genex::views::unique(array, same));
CHECK_CALL(genex::views::duplicates(array, same));
CHECK_CALL(genex::views::partial_sum(array, add));
CHECK_CALL(genex::views::remove(array, 1));
CHECK_CALL(genex::views::replace(array, 1, 2));
CHECK_CALL(genex::views::scan(array, 0));
CHECK_CALL(genex::views::scan(array, 0, add));
CHECK_CALL(genex::views::transform(const_array, twice));
CHECK_CALL(genex::views::remove(const_array, 1));

CHECK_CALL(genex::actions::transform(array, twice));
CHECK_CALL(genex::actions::replace_if(array, yes, 1));
CHECK_CALL(genex::actions::replace(array, 1, 2));
CHECK_CALL(genex::actions::fill(array, 1));
CHECK_CALL(genex::actions::sort(array, std::less{}));
CHECK_CALL(genex::for_each(array, [](int) {}));

// And none of that stops a pointer being a value, or a function pointer being a callable.
static_assert(genex::closure_callable<int(*)(int)>);
static_assert(not genex::closure_callable<int*>);
static_assert(not genex::closure_callable<ints>);
static_assert(genex::closure_value<int*&>);
static_assert(genex::closure_value<const char(&)[4]>);
static_assert(not genex::closure_value<int(&)[3]>);

TEST(GenexViewConcepts, ArrayIsTheRangeOfADirectCall) {
  int values[3] = {1, 2, 3};
  EXPECT_EQ(genex::views::transform(values, twice) | genex::to<std::vector>(), (ints{2, 4, 6}));
  EXPECT_EQ(genex::views::remove(values, 2) | genex::to<std::vector>(), (ints{1, 3}));
  EXPECT_EQ(genex::views::replace(values, 2, 9) | genex::to<std::vector>(), (ints{1, 9, 3}));
  EXPECT_EQ(genex::views::scan(values, 10) | genex::to<std::vector>(), (ints{11, 13, 16}));
}

TEST(GenexViewConcepts, PointersAndLiteralsAreValuesInAClosure) {
  auto first = 1;
  auto second = 2;
  auto pointers_to = std::vector<int*>{&first, &second};
  auto *target = &first;
  EXPECT_EQ(pointers_to | genex::views::remove(target) | genex::to<std::vector>(), (std::vector<int*>{&second}));

  auto words = std::vector<std::string>{"a", "b"};
  EXPECT_EQ(words | genex::views::remove("a") | genex::to<std::vector>(), (std::vector<std::string>{"b"}));
  EXPECT_EQ(words | genex::views::replace("a", "c") | genex::to<std::vector>(), (std::vector<std::string>{"c", "b"}));
}

TEST(GenexViewConcepts, ValueIsNotMovedOutOfALvalue) {
  auto kept = std::make_shared<int>(1);
  auto other = std::make_shared<int>(2);
  auto shared = std::vector{kept, other};
  EXPECT_EQ(shared | genex::views::remove(kept) | genex::to<std::vector>(), (std::vector{other}));
  EXPECT_NE(kept, nullptr);
}
