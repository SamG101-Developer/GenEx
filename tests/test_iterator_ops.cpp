#include <gtest/gtest.h>
#include <concepts>
#include <iterator>
#include <list>
#include <utility>
#include <vector>

import genex.views.cycle;
import genex.views.enumerate;
import genex.views.iota;
import genex.views.reverse;
import genex.views.transform;
import genex.views.zip;

namespace {
  constexpr auto twice = [](const int x) { return x * 2; };

  using vec_t = std::vector<int>;
  using list_t = std::list<int>;

  using transformed_list_t = decltype(genex::views::transform(std::declval<list_t&>(), twice).begin());
  using cycled_t = decltype(genex::views::cycle(std::declval<vec_t&>()).begin());
  using reversed_t = decltype(genex::views::reverse(std::declval<vec_t&>()).begin());
  using enumerated_t = decltype(genex::views::enumerate(std::declval<vec_t&>()).begin());
  using iota_t = decltype(genex::views::iota(0, 10, 2).begin());
  using zipped_t = decltype(genex::views::zip(std::declval<vec_t&>(), std::declval<vec_t&>()).begin());

  template <typename I>
  concept addable = requires(I it) { it += 1; };

  template <typename I>
  concept decrementable = requires(I it) { --it; };
}

// An operator exists only if the iterator can really do it.
static_assert(decrementable<transformed_list_t>);
static_assert(not addable<transformed_list_t>);
static_assert(not std::sized_sentinel_for<transformed_list_t, transformed_list_t>);

// A cycle has no correct +=, as it would have to wrap around.
static_assert(std::bidirectional_iterator<cycled_t>);
static_assert(not addable<cycled_t>);

// A reverse can be measured, but only claims what it needs to.
static_assert(std::bidirectional_iterator<reversed_t>);
static_assert(std::sized_sentinel_for<reversed_t, reversed_t>);
static_assert(not addable<reversed_t>);

// These do everything a random access iterator needs, including n + it and the ordering operators.
static_assert(std::random_access_iterator<enumerated_t>);
static_assert(std::random_access_iterator<iota_t>);
static_assert(std::random_access_iterator<zipped_t>);

// Nesting doesn't add padding, which a base shared between iterators would.
using nested_t = decltype(genex::views::transform(std::declval<int*>(), std::declval<int*>(), twice).begin());
static_assert(sizeof(nested_t) == sizeof(int*));

TEST(GenexIteratorOps, EnumerateIndexFollowsArithmetic) {
  auto vec = vec_t{10, 20, 30, 40};
  auto rng = genex::views::enumerate(vec);

  auto it = rng.begin();
  it += 2;
  EXPECT_EQ((*it).first, 2uz);
  EXPECT_EQ((*it).second, 30);

  it -= 1;
  EXPECT_EQ((*it).first, 1uz);
  EXPECT_EQ((*(it + 2)).first, 3uz);
  EXPECT_EQ(it[2].first, 3uz);
}

TEST(GenexIteratorOps, IotaArithmeticIsInSteps) {
  auto rng = genex::views::iota(0, 10, 2);
  const auto first = rng.begin();

  EXPECT_EQ(*(first + 2), 4);
  EXPECT_EQ(first[3], 6);
  EXPECT_EQ(*(2 + first), 4);
  EXPECT_EQ((first + 3) - first, 3);
  EXPECT_EQ(first - (first + 3), -3);
  EXPECT_EQ(rng.end() - first, 5);
}

TEST(GenexIteratorOps, IotaDistanceRoundsUpToTheEnd) {
  auto rng = genex::views::iota(0, 9, 2);
  EXPECT_EQ(rng.end() - rng.begin(), 5);
  EXPECT_EQ(rng.size(), 5);
}

TEST(GenexIteratorOps, IotaEqualityLooksAtBothSides) {
  auto rng = genex::views::iota(0, 4);
  const auto first = rng.begin();

  EXPECT_EQ(first, rng.begin());
  EXPECT_NE(first, first + 1);
  EXPECT_NE(first, rng.end());
  EXPECT_EQ(first + 4, rng.end());
}

TEST(GenexIteratorOps, IotaStepsPastTheEndStillEnd) {
  auto rng = genex::views::iota(0, 5, 2);
  auto count = 0;
  for (auto it = rng.begin(); it != rng.end(); ++it) { ++count; }
  EXPECT_EQ(count, 3);
}

TEST(GenexIteratorOps, IotaThatCannotReachItsEndIsEmpty) {
  for (auto rng : {genex::views::iota(5, 0), genex::views::iota(0, 0), genex::views::iota(0, 5, 0), genex::views::iota(5, 0, -1)}) {
    EXPECT_EQ(rng.size(), 0uz);
    EXPECT_EQ(rng.begin(), rng.end());
  }
}

TEST(GenexIteratorOps, IotaDecrements) {
  auto rng = genex::views::iota(0, 10, 2);
  auto it = rng.begin() + 2;
  --it;
  EXPECT_EQ(*it, 2);
}

TEST(GenexIteratorOps, ZipRandomAccess) {
  auto lhs = vec_t{1, 2, 3};
  auto rhs = vec_t{4, 5, 6};
  auto rng = genex::views::zip(lhs, rhs);
  const auto first = rng.begin();

  EXPECT_EQ(std::get<1>(*(first + 2)), 6);
  EXPECT_EQ(std::get<0>(*(1 + first)), 2);
  EXPECT_EQ(std::get<0>(first[2]), 3);
  EXPECT_EQ((first + 2) - first, 2);
  EXPECT_LT(first, first + 1);
}

TEST(GenexIteratorOps, ReverseSubtractsInItsOwnDirection) {
  auto vec = vec_t{1, 2, 3};
  auto rng = genex::views::reverse(vec);
  EXPECT_EQ(rng.end() - rng.begin(), 3);
  EXPECT_LT(rng.begin(), rng.end());
}

TEST(GenexIteratorOps, PostfixReturnsThePreviousPosition) {
  auto vec = vec_t{1, 2, 3};
  auto rng = genex::views::transform(vec, twice);

  auto it = rng.begin();
  EXPECT_EQ(*it++, 2);
  EXPECT_EQ(*it, 4);
  EXPECT_EQ(*it--, 4);
  EXPECT_EQ(*it, 2);
}
