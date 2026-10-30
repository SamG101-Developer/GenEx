#include <gtest/gtest.h>
#include <array>
#include <vector>

import genex.to_container;
import genex.views2.filter;
import genex.views2.reverse;
import genex.views2.take;
import genex.views2.transform;

namespace {
  constexpr auto is_even = [](const int x) { return x % 2 == 0; };
  constexpr auto twice = [](const int x) { return x * 2; };

  constexpr auto pipeline_over_lvalue() -> int {
    auto arr = std::array{1, 2, 3, 4};
    auto rng = arr | genex::views::filter(is_even) | genex::views::transform(twice);

    auto sum = 0;
    for (auto x : rng) { sum += x; }
    return sum;
  }

  constexpr auto to_container() -> bool {
    auto arr = std::array{1, 2, 3, 4};
    const auto out = arr | genex::views::reverse | genex::views::take(2) | genex::to<std::vector>();
    return out == std::vector{4, 3};
  }

  constexpr auto pipeline_over_rvalue() -> bool {
    auto rng = std::vector{1, 2, 3, 4} | genex::views::filter(is_even);
    return (rng | genex::to<std::vector>()) == std::vector{2, 4};
  }
}

static_assert(pipeline_over_lvalue() == 12);
static_assert(to_container());
static_assert(pipeline_over_rvalue());

TEST(GenexConstexpr, AlsoRunsAtRuntime) {
  EXPECT_EQ(pipeline_over_lvalue(), 12);
  EXPECT_TRUE(to_container());
  EXPECT_TRUE(pipeline_over_rvalue());
}
