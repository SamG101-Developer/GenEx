#include <gtest/gtest.h>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

import genex.actions.sort;
import genex.actions.unique;
import genex.to_container;
import genex.views.filter;
import genex.views.materialize;
import genex.views.reverse;
import genex.views.take;
import genex.views.transform;
import genex.views.zip;

namespace {
  constexpr auto is_even = [](const int x) { return x % 2 == 0; };
  constexpr auto twice = [](const int x) { return x * 2; };

  auto make_vec() -> std::vector<int> { return {1, 2, 3, 4, 5, 6}; }

  template <typename T, typename F>
  concept can_pipe = requires(T &&t, F &&f) { std::forward<T>(t) | std::forward<F>(f); };
}

// Only a closure can be piped into, so this doesn't take over `|` for everything callable.
static_assert(can_pipe<std::vector<int>&, decltype(genex::views::reverse)>);
static_assert(can_pipe<std::vector<int>&, decltype(genex::views::filter(is_even))>);
static_assert(not can_pipe<std::vector<int>&, decltype(twice)>);
static_assert(not can_pipe<int, decltype(twice)>);
static_assert(not can_pipe<std::vector<int>&, decltype(genex::views::filter)>);

namespace has_its_own_pipe {
  struct flags {
    int bits;
  };

  constexpr auto operator|(const flags a, const flags b) -> flags { return flags{a.bits | b.bits}; }

  // Its operator| hides any global one from here, but the pipe operators aren't found that way.
  auto pipeline_in_here() -> std::vector<int> {
    auto vec = make_vec();
    return vec | genex::views::filter(is_even) | genex::to<std::vector>();
  }

  TEST(GenexPipe, FoundInANamespaceWithItsOwnOperator) {
    EXPECT_EQ(pipeline_in_here(), (std::vector{2, 4, 6}));
    EXPECT_EQ((flags{1} | flags{2}).bits, 3);
  }
}

TEST(GenexPipeComposition, AppliesLeftToRight) {
  auto vec = make_vec();
  const auto evens_doubled = genex::views::filter(is_even) | genex::views::transform(twice);
  EXPECT_EQ(vec | evens_doubled | genex::to<std::vector>(), (std::vector{4, 8, 12}));

  const auto doubled_evens = genex::views::transform(twice) | genex::views::filter(is_even);
  EXPECT_EQ(vec | doubled_evens | genex::to<std::vector>(), (std::vector{2, 4, 6, 8, 10, 12}));
}

TEST(GenexPipeComposition, ReusableAndNested) {
  const auto step = genex::views::filter(is_even) | genex::views::transform(twice);
  const auto more = step | genex::views::take(2);

  auto first = make_vec();
  auto second = std::vector{10, 11, 12};
  EXPECT_EQ(first | more | genex::to<std::vector>(), (std::vector{4, 8}));
  EXPECT_EQ(second | more | genex::to<std::vector>(), (std::vector{20, 24}));
}

TEST(GenexPipeComposition, IncludingBareObjects) {
  auto vec = make_vec();
  const auto backwards = genex::views::reverse | genex::views::take(2);
  EXPECT_EQ(vec | backwards | genex::to<std::vector>(), (std::vector{6, 5}));
}

TEST(GenexPipeComposition, EndingInAContainer) {
  auto vec = make_vec();
  const auto collect = genex::views::filter(is_even) | genex::to<std::vector>();
  EXPECT_EQ(vec | collect, (std::vector{2, 4, 6}));
  EXPECT_EQ(make_vec() | collect, (std::vector{2, 4, 6}));
}

TEST(GenexPipeComposition, OverAnRvalueOutlivesTheStatement) {
  const auto step = genex::views::filter(is_even) | genex::views::transform(twice);
  auto rng = make_vec() | step;
  EXPECT_EQ(rng | genex::to<std::vector>(), (std::vector{4, 8, 12}));
}

TEST(GenexPipeComposition, HoldingAnRvalueRangeOutlivesTheStatement) {
  auto vec = std::vector{1, 2};
  auto rng = vec | (genex::views::zip(std::vector{3, 4}) | genex::views::take(1));
  const auto out = rng | genex::to<std::vector>();
  ASSERT_EQ(out.size(), 1uz);
  EXPECT_EQ(std::get<1>(out[0]), 3);
}

TEST(GenexPipeComposition, ThroughAContainerAndBackToAView) {
  auto vec = make_vec();
  auto rng = vec | (genex::views::filter(is_even) | genex::views::materialize | genex::views::reverse);
  EXPECT_EQ(rng | genex::to<std::vector>(), (std::vector{6, 4, 2}));
}

TEST(GenexPipeComposition, Actions) {
  auto vec = std::vector{3, 1, 3, 2, 1};
  vec |= genex::actions::sort | genex::actions::unique;
  EXPECT_EQ(vec, (std::vector{1, 2, 3}));
}

TEST(GenexTo, Sets) {
  auto vec = std::vector{3, 1, 3, 2};
  EXPECT_EQ(vec | genex::to<std::set>(), (std::set{1, 2, 3}));
  EXPECT_EQ(vec | genex::to<std::unordered_set>(), (std::unordered_set{1, 2, 3}));
  EXPECT_EQ(vec | genex::to<std::set<int>>(), (std::set{1, 2, 3}));
  EXPECT_EQ(vec | genex::views::filter(is_even) | genex::to<std::set>(), (std::set{2}));
}

TEST(GenexTo, Maps) {
  auto vec = std::vector<std::pair<int, std::string>>{{1, "a"}, {2, "b"}};
  const auto exp = std::map<int, std::string>{{1, "a"}, {2, "b"}};
  EXPECT_EQ(vec | genex::to<std::map>(), exp);
  EXPECT_EQ((vec | genex::to<std::map<int, std::string>>()), exp);
  EXPECT_EQ(exp | genex::to<std::map>(), exp);
  EXPECT_EQ((vec | genex::to<std::unordered_map>()).size(), 2uz);

  // A filter ends in a sentinel, so this can't go through the container's own constructor.
  const auto only_two = vec
    | genex::views::filter([](const auto &kv) { return kv.first == 2; })
    | genex::to<std::map>();
  EXPECT_EQ(only_two, (std::map<int, std::string>{{2, "b"}}));
}

TEST(GenexTo, OtherContainers) {
  auto vec = make_vec();
  EXPECT_EQ(vec | genex::to<std::list>(), (std::list{1, 2, 3, 4, 5, 6}));
  EXPECT_EQ(vec | genex::to<std::deque>(), (std::deque{1, 2, 3, 4, 5, 6}));
  EXPECT_EQ(vec | genex::views::transform(twice) | genex::to<std::list>(), (std::list{2, 4, 6, 8, 10, 12}));
  EXPECT_EQ(vec | genex::to<std::vector<long>>(), (std::vector<long>{1, 2, 3, 4, 5, 6}));
}
