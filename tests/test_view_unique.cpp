#include <gtest/gtest.h>
#include <coroutine>

import genex.operations.cmp;
import genex.to_container;
import genex.views2.unique;

TEST(GenexViewsUnique, VecInput) {
  auto vec = std::vector{1, 1, 2, 2, 2, 3, 1, 1};
  const auto rng = vec
    | genex::views::unique()
    | genex::to<std::vector>();
  const auto exp = std::vector{1, 2, 3, 1};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsUnique, NoAdjacentDuplicates) {
  auto vec = std::vector{1, 2, 3};
  const auto rng = vec
    | genex::views::unique()
    | genex::to<std::vector>();
  EXPECT_EQ(rng, vec);
}

TEST(GenexViewsUnique, ProjectedInput) {
  auto vec = std::vector{std::pair{1, 'a'}, std::pair{1, 'b'}, std::pair{2, 'c'}};
  const auto rng = vec
    | genex::views::unique(genex::operations::eq{}, [](const auto &p) { return p.first; })
    | genex::to<std::vector>();
  const auto exp = std::vector{std::pair{1, 'a'}, std::pair{2, 'c'}};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsUnique, EmptyInput) {
  auto vec = std::vector<int>{};
  const auto rng = vec
    | genex::views::unique()
    | genex::to<std::vector>();
  EXPECT_TRUE(rng.empty());
}