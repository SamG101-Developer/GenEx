#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

import genex.actions.sort;
import genex.algorithms.sort;
import genex.algorithms.sorted;
import genex.algorithms.stable_sort;
import genex.detail.partition_impl;
import genex.detail.projected_less;
import genex.detail.sort_impl;
import genex.detail.stable_sort_impl;
import genex.meta;

namespace {
  // Every length either side of a threshold in the sorts, and some well past them.
  const auto lengths = std::vector<std::size_t>{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 15, 16, 17, 19, 20, 21, 23, 24, 25, 31, 32, 33, 47, 48, 49, 63, 64, 65,
    127, 128, 129, 130, 255, 256, 257, 1000, 4097, 20000};

  enum class pattern {
    random, sorted, reversed, all_equal, few_unique, organ_pipe, sawtooth, alternating, nearly_sorted, nearly_reversed
  };

  const auto patterns = std::vector{
    pattern::random, pattern::sorted, pattern::reversed, pattern::all_equal, pattern::few_unique, pattern::organ_pipe,
    pattern::sawtooth, pattern::alternating, pattern::nearly_sorted, pattern::nearly_reversed};

  auto make_ints(const pattern kind, const std::size_t count) -> std::vector<int> {
    auto engine = std::mt19937(static_cast<unsigned>(count) * 31 + static_cast<unsigned>(kind));
    auto data = std::vector<int>(count);
    const auto n = static_cast<int>(count);
    for (auto i = 0; i < n; ++i) {
      switch (kind) {
        case pattern::random: data[i] = static_cast<int>(engine()); break;
        case pattern::sorted: data[i] = i; break;
        case pattern::reversed: data[i] = n - i; break;
        case pattern::all_equal: data[i] = 7; break;
        case pattern::few_unique: data[i] = static_cast<int>(engine() % 4); break;
        case pattern::organ_pipe: data[i] = i < n / 2 ? i : n - i; break;
        case pattern::sawtooth: data[i] = i % 13; break;
        case pattern::alternating: data[i] = i % 2 == 0 ? i : -i; break;
        case pattern::nearly_sorted: data[i] = i; break;
        case pattern::nearly_reversed: data[i] = n - i; break;
      }
    }
    if ((kind == pattern::nearly_sorted or kind == pattern::nearly_reversed) and count > 1) {
      for (auto i = 0uz; i < count / 50 + 1; ++i) { std::swap(data[engine() % count], data[engine() % count]); }
    }
    return data;
  }

  template <typename T>
  auto convert(const std::vector<int> &input, auto make) -> std::vector<T> {
    auto data = std::vector<T>();
    data.reserve(input.size());
    for (const auto x : input) { data.push_back(make(x)); }
    return data;
  }

  // Sorted by key alone, so the payload shows whether equal keys kept their order.
  struct keyed {
    int key;
    int payload;
    auto operator==(const keyed &) const -> bool = default;
  };

  struct keyed_large {
    int key;
    int payload;
    std::int64_t padding[4];
    auto operator==(const keyed_large &that) const -> bool { return key == that.key and payload == that.payload; }
  };

  template <typename T>
  auto keyed_data(const std::vector<int> &input) -> std::vector<T> {
    auto data = std::vector<T>();
    auto payload = 0;
    for (const auto x : input) {
      auto &added = data.emplace_back();
      added.key = x % 64;
      added.payload = payload++;
    }
    return data;
  }

  template <typename I, typename Less>
  auto call_with_less(I first, I last, Less less, auto algorithm) -> void {
    algorithm(first, last, less);
  }

  // A comparison that an adversary answers so as to make a quicksort go quadratic, from McIlroy's "A Killer Adversary
  // for Quicksort". It's still a consistent total order, just one that it decides as late as it can.
  class killer_adversary {
    std::vector<int> m_values;
    int m_solid = 0;
    int m_candidate = 0;
    int m_gas;

  public:
    long comparisons = 0;

    explicit killer_adversary(const std::size_t count) :
      m_values(count), m_gas(static_cast<int>(count) + 1) {
      std::ranges::fill(m_values, m_gas);
    }

    auto compare(const int x, const int y) -> bool {
      ++comparisons;
      if (m_values[x] == m_gas and m_values[y] == m_gas) {
        m_values[x == m_candidate ? x : y] = m_solid++;
      }
      if (m_values[x] == m_gas) { m_candidate = x; }
      else if (m_values[y] == m_gas) { m_candidate = y; }
      return m_values[x] < m_values[y];
    }
  };
}

TEST(GenexSort, IntsMatchTheStandard) {
  for (const auto kind : patterns) {
    for (const auto count : lengths) {
      auto data = make_ints(kind, count);
      auto exp = data;
      std::sort(exp.begin(), exp.end());
      const auto result = genex::sort(data);
      EXPECT_EQ(data, exp) << "pattern " << static_cast<int>(kind) << " count " << count;
      EXPECT_EQ(result, data.end());
    }
  }
}

TEST(GenexSort, OtherElementTypes) {
  for (const auto kind : patterns) {
    for (const auto count : lengths) {
      if (count > 4097) { continue; }
      const auto ints = make_ints(kind, count);

      auto doubles = convert<double>(ints, [](const int x) { return x * 0.5; });
      auto expect_doubles = doubles;
      std::sort(expect_doubles.begin(), expect_doubles.end());
      genex::sort(doubles);
      EXPECT_EQ(doubles, expect_doubles);

      auto bytes = convert<signed char>(ints, [](const int x) { return static_cast<signed char>(x); });
      auto expect_bytes = bytes;
      std::sort(expect_bytes.begin(), expect_bytes.end());
      genex::sort(bytes);
      EXPECT_EQ(bytes, expect_bytes);

      auto pairs = convert<std::pair<int, int>>(ints, [](const int x) { return std::pair{x % 32, x}; });
      auto expect_pairs = pairs;
      std::sort(expect_pairs.begin(), expect_pairs.end());
      genex::sort(pairs);
      EXPECT_EQ(pairs, expect_pairs);

      auto strings = convert<std::string>(ints, [](const int x) { return std::to_string(x) + "-padding-to-allocate-it"; });
      auto expect_strings = strings;
      std::sort(expect_strings.begin(), expect_strings.end());
      genex::sort(strings);
      EXPECT_EQ(strings, expect_strings);

      auto deque = std::deque<int>(ints.begin(), ints.end());
      auto expect_deque = deque;
      std::sort(expect_deque.begin(), expect_deque.end());
      genex::sort(deque);
      EXPECT_EQ(deque, expect_deque);
    }
  }
}

TEST(GenexSort, MoveOnlyElements) {
  for (const auto kind : patterns) {
    for (const auto count : lengths) {
      if (count > 4097) { continue; }
      auto data = convert<std::unique_ptr<int>>(make_ints(kind, count), [](const int x) { return std::make_unique<int>(x); });
      genex::sort(data, {}, [](const std::unique_ptr<int> &p) { return *p; });
      EXPECT_TRUE(std::is_sorted(data.begin(), data.end(), [](const auto &a, const auto &b) { return *a < *b; }));
    }
  }
}

TEST(GenexSort, ProxyReferences) {
  for (const auto kind : patterns) {
    for (const auto count : lengths) {
      if (count > 4097) { continue; }
      auto data = std::vector<bool>();
      for (const auto x : make_ints(kind, count)) { data.push_back(x % 2 == 0); }
      const auto trues = std::count(data.begin(), data.end(), true);
      genex::sort(data);
      EXPECT_TRUE(std::is_sorted(data.begin(), data.end()));
      EXPECT_EQ(std::count(data.begin(), data.end(), true), trues);
    }
  }
}

TEST(GenexSort, ComparatorsAndProjections) {
  auto data = make_ints(pattern::random, 5000);
  auto exp = data;
  std::sort(exp.begin(), exp.end(), std::greater{});
  genex::sort(data, std::greater{});
  EXPECT_EQ(data, exp);

  auto keys = keyed_data<keyed>(make_ints(pattern::random, 5000));
  genex::sort(keys, {}, &keyed::key);
  EXPECT_TRUE(std::is_sorted(keys.begin(), keys.end(), [](const keyed &a, const keyed &b) { return a.key < b.key; }));

  auto negated = make_ints(pattern::random, 5000);
  genex::sort(negated, {}, [](const int x) { return -static_cast<long>(x); });
  EXPECT_EQ(negated, exp);
}

TEST(GenexSort, ComparatorTakingMutableReferences) {
  auto data = make_ints(pattern::random, 1000);
  auto exp = data;
  std::sort(exp.begin(), exp.end());
  genex::sort(data, [](int &a, int &b) { return a < b; });
  EXPECT_EQ(data, exp);
  auto stable = make_ints(pattern::random, 1000);
  genex::stable_sort(stable, [](int &a, int &b) { return a < b; });
  EXPECT_EQ(stable, exp);
}

TEST(GenexSort, StatefulComparatorIsNotCopiedPerComparison) {
  auto comparisons = 0;
  auto data = make_ints(pattern::random, 1000);
  genex::sort(data, [&comparisons](const int a, const int b) { ++comparisons; return a < b; });
  EXPECT_TRUE(std::is_sorted(data.begin(), data.end()));
  EXPECT_GT(comparisons, 0);
}

TEST(GenexSort, IteratorsAndSentinels) {
  auto data = std::vector{3, 1, 2, 9, 8};
  const auto result = genex::sort(data.begin(), data.begin() + 3);
  EXPECT_EQ(result, data.begin() + 3);
  EXPECT_EQ(data, (std::vector{1, 2, 3, 9, 8}));

  struct stop_at {
    int *where;
    auto operator==(int *const it) const -> bool { return it == where; }
  };
  auto array = std::vector{5, 4, 3, 2, 1};
  const auto end = genex::sort(array.data(), stop_at{array.data() + 4});
  EXPECT_EQ(end, array.data() + 4);
  EXPECT_EQ(array, (std::vector{2, 3, 4, 5, 1}));
}

TEST(GenexSort, KillerAdversaryCannotMakeItQuadratic) {
  for (const auto count : {1000uz, 10000uz, 100000uz}) {
    auto adversary = killer_adversary(count);
    auto data = std::vector<int>(count);
    std::iota(data.begin(), data.end(), 0);
    genex::sort(data, [&adversary](const int a, const int b) { return adversary.compare(a, b); });

    // Well over what a sort does normally, as this is meant to be hard, and far under quadratic.
    const auto limit = 4.0 * static_cast<double>(count) * std::log2(static_cast<double>(count));
    EXPECT_LE(static_cast<double>(adversary.comparisons), limit) << "count " << count;
  }
}

TEST(GenexSort, ComparisonsAreNearLinearOnEasyInputs) {
  const auto count = 100000uz;
  for (const auto kind : {pattern::sorted, pattern::reversed, pattern::all_equal}) {
    auto comparisons = 0uz;
    auto data = make_ints(kind, count);
    genex::sort(data, [&comparisons](const int a, const int b) { ++comparisons; return a < b; });
    EXPECT_LE(comparisons, 3 * count) << "pattern " << static_cast<int>(kind);
  }
  auto stable_comparisons = 0uz;
  auto data = make_ints(pattern::sorted, count);
  genex::stable_sort(data, [&stable_comparisons](const int a, const int b) { ++stable_comparisons; return a < b; });
  EXPECT_LE(stable_comparisons, count);
}

TEST(GenexSortParts, HeapSort) {
  for (const auto kind : patterns) {
    for (const auto count : lengths) {
      auto data = make_ints(kind, count);
      auto exp = data;
      std::sort(exp.begin(), exp.end());
      auto less = std::less<int>{};
      genex::algorithms::detail::heap_sort(data.begin(), data.end(), less);
      EXPECT_EQ(data, exp);
    }
  }
}

TEST(GenexSortParts, SmallSortIsStable) {
  for (const auto kind : patterns) {
    for (auto count = 0uz; count <= 32; ++count) {
      auto data = keyed_data<keyed>(make_ints(kind, count));
      auto exp = data;
      std::stable_sort(exp.begin(), exp.end(), [](const keyed &a, const keyed &b) { return a.key < b.key; });
      auto less = [](const keyed &a, const keyed &b) { return a.key < b.key; };
      genex::algorithms::detail::small_sort(data.begin(), data.end(), less);
      EXPECT_EQ(data, exp) << "pattern " << static_cast<int>(kind) << " count " << count;
    }
  }
}

TEST(GenexSortParts, EveryPartitionChoiceSorts) {
  // Which partition is used depends on the input, so these go through each one on all the inputs.
  for (const auto kind : patterns) {
    for (const auto count : lengths) {
      auto exp = make_ints(kind, count);
      std::sort(exp.begin(), exp.end());
      auto less = std::less<int>{};
      const auto bits = static_cast<int>(std::bit_width(count)) + 1;

      auto branchless = make_ints(kind, count);
      genex::algorithms::detail::quicksort<true>(branchless.begin(), branchless.end(), less, bits, true);
      EXPECT_EQ(branchless, exp);

      auto branching = make_ints(kind, count);
      genex::algorithms::detail::quicksort<false>(branching.begin(), branching.end(), less, bits, true);
      EXPECT_EQ(branching, exp);
    }
  }
}

TEST(GenexSortParts, NoBadPartitionsLeftFallsBackToAHeapSort) {
  for (const auto kind : patterns) {
    auto exp = make_ints(kind, 5000);
    std::sort(exp.begin(), exp.end());
    auto less = std::less<int>{};

    auto branchless = make_ints(kind, 5000);
    genex::algorithms::detail::quicksort<true>(branchless.begin(), branchless.end(), less, 1, true);
    EXPECT_EQ(branchless, exp);

    auto branching = make_ints(kind, 5000);
    genex::algorithms::detail::quicksort<false>(branching.begin(), branching.end(), less, 1, true);
    EXPECT_EQ(branching, exp);
  }
}

TEST(GenexStableSort, MatchesTheStandardExactly) {
  const auto by_key = [](const auto &a, const auto &b) { return a.key < b.key; };
  for (const auto kind : patterns) {
    for (const auto count : lengths) {
      auto plain = keyed_data<keyed>(make_ints(kind, count));
      auto expect_plain = plain;
      std::stable_sort(expect_plain.begin(), expect_plain.end(), by_key);
      genex::stable_sort(plain, {}, &keyed::key);
      EXPECT_EQ(plain, expect_plain) << "pattern " << static_cast<int>(kind) << " count " << count;

      auto large = keyed_data<keyed_large>(make_ints(kind, count));
      auto expect_large = large;
      std::stable_sort(expect_large.begin(), expect_large.end(), by_key);
      genex::stable_sort(large, {}, &keyed_large::key);
      EXPECT_EQ(large, expect_large);
    }
  }
}

TEST(GenexStableSort, Strings) {
  const auto by_length = [](const std::string &a, const std::string &b) { return a.size() < b.size(); };
  for (const auto kind : patterns) {
    for (const auto count : lengths) {
      if (count > 4097) { continue; }
      auto data = convert<std::string>(make_ints(kind, count), [](const int x) { return std::to_string(x); });
      auto exp = data;
      std::stable_sort(exp.begin(), exp.end(), by_length);
      genex::stable_sort(data, by_length);
      EXPECT_EQ(data, exp);
    }
  }
}

TEST(GenexStableSort, MoveOnlyElements) {
  for (const auto count : lengths) {
    if (count > 4097) { continue; }
    auto data = convert<std::unique_ptr<int>>(make_ints(pattern::few_unique, count), [](const int x) { return std::make_unique<int>(x); });
    genex::stable_sort(data, {}, [](const std::unique_ptr<int> &p) { return *p; });
    EXPECT_TRUE(std::is_sorted(data.begin(), data.end(), [](const auto &a, const auto &b) { return *a < *b; }));
  }
}

TEST(GenexStableSort, ProxyReferencesAndDeques) {
  auto bits = std::vector<bool>();
  for (const auto x : make_ints(pattern::random, 5000)) { bits.push_back(x % 2 == 0); }
  genex::stable_sort(bits);
  EXPECT_TRUE(std::is_sorted(bits.begin(), bits.end()));

  auto data = make_ints(pattern::random, 5000);
  auto deque = std::deque<int>(data.begin(), data.end());
  std::stable_sort(data.begin(), data.end());
  genex::stable_sort(deque);
  EXPECT_TRUE(std::equal(deque.begin(), deque.end(), data.begin()));
}

TEST(GenexStableSort, WithoutAnyMemory) {
  const auto by_key = [](const keyed &a, const keyed &b) { return a.key < b.key; };
  for (const auto kind : patterns) {
    for (const auto count : lengths) {
      auto data = keyed_data<keyed>(make_ints(kind, count));
      auto exp = data;
      std::stable_sort(exp.begin(), exp.end(), by_key);
      auto less = by_key;
      genex::algorithms::detail::stable_sort_in_place(data.begin(), data.end(), less);
      EXPECT_EQ(data, exp) << "pattern " << static_cast<int>(kind) << " count " << count;
    }
  }
}

TEST(GenexStableSort, BranchingPathOnPlainElements) {
  const auto by_key = [](const keyed &a, const keyed &b) { return a.key < b.key; };
  for (const auto kind : patterns) {
    for (const auto count : lengths) {
      auto data = keyed_data<keyed>(make_ints(kind, count));
      auto exp = data;
      std::stable_sort(exp.begin(), exp.end(), by_key);
      auto less = by_key;
      auto buffer = std::vector<keyed>(count);
      genex::algorithms::detail::stable_sort_buffered(data.begin(), data.end(), less, buffer.data());
      EXPECT_EQ(data, exp);
    }
  }
}

namespace {
  // Allocates, so a sanitizer sees a lost, doubled, or leaked element when a comparison throws midway.
  template <auto Algorithm>
  auto survives_throwing_comparison() -> void {
    for (const auto throw_at : {0, 1, 5, 40, 200, 1000, 5000}) {
      auto data = convert<std::string>(
        make_ints(pattern::random, 2000), [](const int x) { return std::to_string(x) + "-padding-to-allocate-it"; });
      auto calls = 0;
      auto threw = false;
      try {
        Algorithm(data, [&](const std::string &a, const std::string &b) {
          if (calls++ == throw_at) { throw std::runtime_error("comparison"); }
          return a < b;
        });
      }
      catch (const std::runtime_error &) {
        threw = true;
      }
      EXPECT_TRUE(threw) << throw_at;
      EXPECT_EQ(data.size(), 2000uz);
    }
  }
}

TEST(GenexSort, SurvivesThrowingComparison) {
  survives_throwing_comparison<genex::sort>();
}

TEST(GenexStableSort, SurvivesThrowingComparison) {
  survives_throwing_comparison<genex::stable_sort>();
}

// Sorting at compile time goes through the same code, other than what needs raw storage.
namespace {
  template <std::size_t N>
  constexpr auto scrambled() -> std::array<int, N> {
    auto data = std::array<int, N>{};
    auto state = 12345u;
    for (auto &x : data) {
      state = state * 1103515245u + 12345u;
      x = static_cast<int>((state >> 16) % 1000u);
    }
    return data;
  }

  template <std::size_t N, auto Algorithm>
  constexpr auto sorts_at_compile_time() -> bool {
    auto data = scrambled<N>();
    Algorithm(data);
    for (auto i = 1uz; i < N; ++i) {
      if (data[i] < data[i - 1]) { return false; }
    }
    return true;
  }
}

static_assert(sorts_at_compile_time<0, genex::sort>());
static_assert(sorts_at_compile_time<10, genex::sort>());
static_assert(sorts_at_compile_time<100, genex::sort>());
static_assert(sorts_at_compile_time<300, genex::sort>());
static_assert(sorts_at_compile_time<10, genex::stable_sort>());
static_assert(sorts_at_compile_time<100, genex::stable_sort>());
static_assert(sorts_at_compile_time<300, genex::stable_sort>());

static_assert([] {
  auto data = std::array<std::pair<int, int>, 100>{};
  for (auto i = 0; i < 100; ++i) { data[i] = {i % 7, i}; }
  genex::stable_sort(data, {}, &std::pair<int, int>::first);
  for (auto i = 1uz; i < 100; ++i) {
    if (data[i - 1].first > data[i].first) { return false; }
    if (data[i - 1].first == data[i].first and data[i - 1].second > data[i].second) { return false; }
  }
  return true;
}());

TEST(GenexSort, ThroughTheActionsAndSorted) {
  auto data = make_ints(pattern::random, 1000);
  auto exp = data;
  std::sort(exp.begin(), exp.end());
  EXPECT_EQ(genex::sorted(data), exp);
  data |= genex::actions::sort;
  EXPECT_EQ(data, exp);

  auto stable = keyed_data<keyed>(make_ints(pattern::few_unique, 1000));
  auto expect_stable = stable;
  std::stable_sort(expect_stable.begin(), expect_stable.end(), [](const keyed &a, const keyed &b) { return a.key < b.key; });
  stable |= genex::actions::stable_sort({}, &keyed::key);
  EXPECT_EQ(stable, expect_stable);
}
