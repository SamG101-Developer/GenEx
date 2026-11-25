#include <concepts>
#include <cstddef>
#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <vector>

import genex.to_container;
import genex.views.enumerate;
import genex.views.transform;

TEST(GenexViewsEnumerate, VecInput) {
  auto vec1 = std::vector{8, 9};

  const auto rng = vec1
    | genex::views::enumerate
    | genex::to<std::vector>();
  EXPECT_EQ(rng[0].first, 0);
  EXPECT_EQ(rng[0].second, 8);
  EXPECT_EQ(rng[1].first, 1);
  EXPECT_EQ(rng[1].second, 9);
}

// Each element is the underlying iterator's reference, as "std::views::enumerate" gives it: an element of a container
// is referred to, not copied.
TEST(GenexViewsEnumerate, ElementIsReferred) {
  auto vec = std::vector{8, 9};
  auto rng = vec | genex::views::enumerate;
  static_assert(std::same_as<decltype(*rng.begin()), std::pair<std::size_t, int&>>);

  for (auto [i, x] : rng) { x = static_cast<int>(i) * 10; }
  EXPECT_EQ(vec, (std::vector{0, 10}));
}

TEST(GenexViewsEnumerate, ConstElementIsReferred) {
  const auto vec = std::vector{8, 9};
  auto rng = vec | genex::views::enumerate;
  static_assert(std::same_as<decltype(*rng.begin()), std::pair<std::size_t, int const&>>);
  EXPECT_EQ(&(*rng.begin()).second, &vec[0]);
}

// A move-only element enumerates, as nothing is copied.
TEST(GenexViewsEnumerate, MoveOnlyInput) {
  auto vec = std::vector<std::unique_ptr<int>>();
  vec.emplace_back(std::make_unique<int>(8));
  vec.emplace_back(std::make_unique<int>(9));

  auto sum = 0uz;
  for (auto const &[i, p] : vec | genex::views::enumerate) {
    EXPECT_EQ(p.get(), vec[i].get());
    sum += i * static_cast<std::size_t>(*p);
  }
  EXPECT_EQ(sum, 9uz);
}

// A view yielding values still yields pairs of values.
TEST(GenexViewsEnumerate, ValueInput) {
  const auto vec = std::vector{8, 9};
  auto rng = vec
    | genex::views::transform([](int x) { return x + 1; })
    | genex::views::enumerate;
  static_assert(std::same_as<decltype(*rng.begin()), std::pair<std::size_t, int>>);

  const auto out = rng | genex::to<std::vector>();
  EXPECT_EQ(out[0].second, 9);
  EXPECT_EQ(out[1].second, 10);
}
