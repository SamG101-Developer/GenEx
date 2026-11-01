#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>

import genex.actions.shuffle;
import genex.algorithms.shuffle;
import genex.detail.bounded_random;

namespace {
  // Fixed by its seed, so these can't start failing by chance.
  template <std::uint64_t Max>
  struct narrow_engine {
    using result_type = std::uint32_t;
    std::mt19937 inner{7};

    static constexpr auto min() -> result_type { return 0; }
    static constexpr auto max() -> result_type { return static_cast<result_type>(Max); }
    auto operator()() -> result_type {
      // Rejecting keeps it uniform over the range, so any bias seen is from what's being tested.
      const auto range = static_cast<std::uint32_t>(Max) + 1;
      const auto mask = range - 1;
      if ((range & mask) == 0) { return inner() & mask; }
      auto value = inner();
      while (value >= std::numeric_limits<std::uint32_t>::max() - std::numeric_limits<std::uint32_t>::max() % range) {
        value = inner();
      }
      return value % range;
    }
  };

  // Pearson's statistic. For 23 degrees of freedom, this is about one chance in 100000.
  constexpr auto limit_for_permutations_of_four = 58.0;

  template <typename Engine>
  auto check_uniform_permutations(Engine engine) -> void {
    constexpr auto rounds = 96000;
    auto counts = std::map<std::array<int, 4>, int>();
    for (auto i = 0; i < rounds; ++i) {
      auto data = std::array{0, 1, 2, 3};
      genex::shuffle(data, engine);
      ++counts[data];
    }

    EXPECT_EQ(counts.size(), 24uz);
    const auto expected = rounds / 24.0;
    auto statistic = 0.0;
    for (const auto &[_, count] : counts) {
      statistic += (count - expected) * (count - expected) / expected;
    }
    EXPECT_LT(statistic, limit_for_permutations_of_four);
  }

  template <typename Engine>
  auto check_uniform_below(Engine engine, const std::uint64_t bound) -> void {
    auto random = genex::detail::bounded_random<Engine>(engine);
    const auto rounds = bound * 2000;
    auto counts = std::vector<std::uint64_t>(bound);
    for (auto i = 0uz; i < rounds; ++i) {
      const auto value = random.below(bound);
      ASSERT_LT(value, bound);
      ++counts[value];
    }

    constexpr auto expected = 2000.0;
    auto statistic = 0.0;
    for (const auto count : counts) { statistic += (count - expected) * (count - expected) / expected; }

    // A normal approximation of the chi-squared distribution, at about five standard deviations.
    const auto freedom = static_cast<double>(bound - 1);
    EXPECT_LT(statistic, freedom + 5.0 * std::sqrt(2.0 * freedom) + 10.0) << "bound " << bound;
  }
}

TEST(GenexShuffle, IsAPermutation) {
  auto engine = std::mt19937(1);
  for (const auto count : {0uz, 1uz, 2uz, 3uz, 100uz, 10000uz}) {
    auto data = std::vector<int>(count);
    std::iota(data.begin(), data.end(), 0);
    const auto exp = data;
    EXPECT_EQ(genex::shuffle(data, engine), data.end());
    auto sorted = data;
    std::sort(sorted.begin(), sorted.end());
    EXPECT_EQ(sorted, exp);
    if (count >= 100) { EXPECT_NE(data, exp); }
  }
}

TEST(GenexShuffle, SameSeedGivesTheSameOrder) {
  auto first = std::vector<int>(1000);
  std::iota(first.begin(), first.end(), 0);
  auto second = first;
  auto first_engine = std::mt19937_64(5);
  auto second_engine = std::mt19937_64(5);
  genex::shuffle(first, first_engine);
  genex::shuffle(second, second_engine);
  EXPECT_EQ(first, second);
}

TEST(GenexShuffle, AdvancesTheCallersGenerator) {
  auto engine = std::mt19937(5);
  const auto before = engine;
  auto data = std::vector{1, 2, 3, 4};
  genex::shuffle(data, engine);
  EXPECT_NE(engine, before);
}

TEST(GenexShuffle, EveryPermutationIsEquallyLikely) {
  check_uniform_permutations(std::mt19937(1));
  check_uniform_permutations(std::mt19937_64(1));
  check_uniform_permutations(std::minstd_rand(1));
  check_uniform_permutations(std::ranlux24_base(1));
  check_uniform_permutations(narrow_engine<255>{});
  check_uniform_permutations(narrow_engine<7>{});
  check_uniform_permutations(narrow_engine<2>{});
}

TEST(GenexShuffle, OtherElementTypes) {
  auto engine = std::mt19937_64(1);
  auto strings = std::vector<std::string>{"a", "b", "c", "d", "e", "f", "g"};
  auto exp = strings;
  genex::shuffle(strings, engine);
  std::sort(strings.begin(), strings.end());
  EXPECT_EQ(strings, exp);

  auto owners = std::vector<std::unique_ptr<int>>();
  for (auto i = 0; i < 100; ++i) { owners.push_back(std::make_unique<int>(i)); }
  genex::shuffle(owners, engine);
  auto sum = 0;
  for (const auto &owner : owners) { sum += *owner; }
  EXPECT_EQ(sum, 4950);

  auto bits = std::vector<bool>(100, false);
  std::fill(bits.begin(), bits.begin() + 50, true);
  genex::shuffle(bits, engine);
  EXPECT_EQ(std::count(bits.begin(), bits.end(), true), 50);
}

TEST(GenexBoundedRandom, UniformWithEachWayOfGettingThere) {
  for (const auto bound : {1ull, 2ull, 3ull, 7ull, 10ull, 16ull, 17ull, 100ull, 1000ull}) {
    check_uniform_below(std::mt19937(1), bound);
    check_uniform_below(std::mt19937_64(1), bound);
    check_uniform_below(std::minstd_rand(1), bound);
    check_uniform_below(narrow_engine<255>{}, bound);
    check_uniform_below(narrow_engine<7>{}, bound);
    check_uniform_below(narrow_engine<2>{}, bound);
  }
}

TEST(GenexBoundedRandom, BoundsPastThirtyTwoBits) {
  const auto check = []<typename Engine>(Engine engine) {
    auto random = genex::detail::bounded_random<Engine>(engine);
    const auto bound = 1ull << 40;
    auto top_half = 0;
    for (auto i = 0; i < 10000; ++i) {
      const auto value = random.below(bound);
      ASSERT_LT(value, bound);
      top_half += value >= bound / 2;
    }
    EXPECT_GT(top_half, 4500);
    EXPECT_LT(top_half, 5500);
  };

  check(std::mt19937(1));
  check(std::mt19937_64(1));
  check(narrow_engine<255>{});
  check(narrow_engine<2>{});

  auto engine = std::mt19937_64(1);
  auto random = genex::detail::bounded_random<std::mt19937_64>(engine);
  const auto huge = std::numeric_limits<std::uint64_t>::max();
  for (auto i = 0; i < 1000; ++i) { EXPECT_LT(random.below(huge), huge); }
}

namespace {
  struct constexpr_engine {
    using result_type = std::uint32_t;
    std::uint32_t state;
    static constexpr auto min() -> result_type { return 0; }
    static constexpr auto max() -> result_type { return std::numeric_limits<result_type>::max(); }
    constexpr auto operator()() -> result_type {
      state ^= state << 13;
      state ^= state >> 17;
      state ^= state << 5;
      return state;
    }
  };
}

static_assert([] {
  auto data = std::array{0, 1, 2, 3, 4, 5, 6, 7};
  auto engine = constexpr_engine{42};
  genex::shuffle(data, engine);
  auto seen = 0;
  for (const auto x : data) { seen |= 1 << x; }
  return seen == 255 and data != std::array{0, 1, 2, 3, 4, 5, 6, 7};
}());

TEST(GenexShuffle, ThroughTheActions) {
  auto data = std::vector<int>(1000);
  std::iota(data.begin(), data.end(), 0);
  const auto exp = data;

  data |= genex::actions::shuffle;
  auto sorted = data;
  std::sort(sorted.begin(), sorted.end());
  EXPECT_EQ(sorted, exp);
  EXPECT_NE(data, exp);

  auto seeded = exp;
  seeded |= genex::actions::shuffle(std::mt19937(1));
  EXPECT_NE(seeded, exp);
}
