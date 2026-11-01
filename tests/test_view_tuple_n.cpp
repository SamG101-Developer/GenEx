#include <concepts>
#include <gtest/gtest.h>
#include <string>
#include <tuple>
#include <vector>

import genex.to_container;
import genex.views.transform;
import genex.views.tuple_nth;
import genex.views.zip;

TEST(GenexViewsTupleN, VecInput) {
  auto vec = std::vector<std::tuple<int, int, int>>{
    {0, 10, 20},
    {1, 11, 21},
    {2, 12, 22},
    {3, 13, 23},
    {4, 14, 24},
  };

  const auto rng = vec
    | genex::views::tuple_nth<2>
    | genex::to<std::vector>();
  const auto exp = std::vector{20, 21, 22, 23, 24};
  EXPECT_EQ(rng, exp);
}

// Zipping makes a new tuple for each element, but of references to the originals.
TEST(GenexViewsTupleN, TemporaryTupleOfReferencesStillRefersToTheElements) {
  auto first = std::vector{1, 2, 3};
  auto second = std::vector{4, 5, 6};
  auto rng = genex::views::zip(first, second) | genex::views::tuple_nth<0>;
  static_assert(std::same_as<decltype(*rng.begin()), int&>);

  for (auto &element : rng) { element *= 2; }
  const auto exp = std::vector{2, 4, 6};
  EXPECT_EQ(first, exp);
}

// Long enough to be on the heap, so that a dangling reference doesn't read back as the right string by luck.
TEST(GenexViewsTupleN, TemporaryTupleInput) {
  const auto exp = std::vector<std::string>{"the first string, too long to be small", "the second string, also too long"};
  auto rng = exp
    | genex::views::transform([](const std::string &str) { return std::make_tuple(0, str); })
    | genex::views::tuple_nth<1>;
  static_assert(std::same_as<decltype(*rng.begin()), std::string>);

  const auto vals = rng | genex::to<std::vector>();
  EXPECT_EQ(vals, exp);
}
