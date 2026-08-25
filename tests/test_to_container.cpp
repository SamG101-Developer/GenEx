#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.filter;
import genex.views2.move;

TEST(GenexToContainer, FilteredLvalueSourceIsNotMovedFrom) {
  auto vec = std::vector<std::shared_ptr<int>>{};
  vec.emplace_back(std::make_shared<int>(1));
  vec.emplace_back(std::make_shared<int>(2));

  const auto rng = vec
    | genex::views::filter([](auto &&x) { return *x == 2; })
    | genex::to<std::vector>();

  const auto exp = std::vector<int>{2};
  EXPECT_EQ(rng.size(), 1u);
  EXPECT_EQ(*rng[0], 2);

  // The view is an rvalue, but it refers to `vec`'s elements: `to` must leave them intact.
  ASSERT_NE(vec[0], nullptr);
  ASSERT_NE(vec[1], nullptr);
  EXPECT_EQ(*vec[1], 2);
}

TEST(GenexToContainer, ExplicitMoveStillMovesOut) {
  auto vec = std::vector<std::shared_ptr<int>>{};
  vec.emplace_back(std::make_shared<int>(1));

  const auto rng = vec
    | genex::views::move
    | genex::to<std::vector>();

  EXPECT_EQ(rng.size(), 1u);
  EXPECT_EQ(*rng[0], 1);
  EXPECT_EQ(vec[0], nullptr);
}