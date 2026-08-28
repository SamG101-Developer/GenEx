#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.as_const;

TEST(GenexViewsAsConst, VecInput) {
  auto vec = std::vector{1, 2, 3};
  const auto rng = vec
    | genex::views::as_const
    | genex::to<std::vector>();
  EXPECT_EQ(rng, vec);
}

TEST(GenexViewsAsConst, YieldsConstReferences) {
  auto vec = std::vector{1, 2, 3};
  auto view = vec | genex::views::as_const;
  static_assert(std::is_const_v<std::remove_reference_t<decltype(*view.begin())>>);
  EXPECT_EQ(*view.begin(), 1);
}
