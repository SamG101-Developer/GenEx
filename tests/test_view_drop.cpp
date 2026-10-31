#include <gtest/gtest.h>
#include <deque>
#include <list>
#include <vector>

import genex.to_container;
import genex.views.drop;
import genex.views.drop_last;
import genex.views.drop_while;

TEST(GenexViewsDrop, VecInput) {
  auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  const auto rng = vec
    | genex::views::drop(4)
    | genex::to<std::vector>();
  const auto exp = std::vector{4, 5, 6, 7, 8, 9};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsDrop, VecInputStacked) {
  auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  const auto rng = vec
    | genex::views::drop(4)
    | genex::views::drop(2)
    | genex::to<std::vector>();
  const auto exp = std::vector{6, 7, 8, 9};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsDropLast, VecInput) {
  auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  const auto rng = vec
    | genex::views::drop_last(4)
    | genex::to<std::vector>();
  const auto exp = std::vector{0, 1, 2, 3, 4, 5};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsDropWhile, VecInput) {
  auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  const auto rng = vec
    | genex::views::drop_while([](auto x) { return x < 5; })
    | genex::to<std::vector>();
  const auto exp = std::vector{5, 6, 7, 8, 9};
  EXPECT_EQ(rng, exp);
}

// A list walks back from its end, and a deque isn't contiguous but can step straight there.
template <typename Container>
static auto check_drop_last() -> void {
  auto data = Container{0, 1, 2, 3, 4};
  const auto kept = [&](const int n) { return data | genex::views::drop_last(n) | genex::to<std::vector>(); };
  EXPECT_EQ(kept(0), (std::vector{0, 1, 2, 3, 4}));
  EXPECT_EQ(kept(2), (std::vector{0, 1, 2}));
  EXPECT_EQ(kept(5), std::vector<int>{});
  EXPECT_EQ(kept(9), std::vector<int>{});

  auto empty = Container{};
  EXPECT_EQ(empty | genex::views::drop_last(1) | genex::to<std::vector>(), std::vector<int>{});
}

TEST(GenexViewsDropLast, ListInput) {
  check_drop_last<std::list<int>>();
}

TEST(GenexViewsDropLast, DequeInput) {
  check_drop_last<std::deque<int>>();
}
