#include <benchmark/benchmark.h>

#include <range/v3/algorithm/lower_bound.hpp>
#include <range/v3/algorithm/partition.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/algorithm/rotate.hpp>
#include <range/v3/algorithm/shuffle.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/algorithm/stable_partition.hpp>
#include <range/v3/algorithm/stable_sort.hpp>
#include <range/v3/algorithm/upper_bound.hpp>

import genex.algorithms.lower_bound;
import genex.algorithms.partition;
import genex.algorithms.reverse;
import genex.algorithms.rotate;
import genex.algorithms.shuffle;
import genex.algorithms.sort;
import genex.algorithms.stable_partition;
import genex.algorithms.stable_sort;
import genex.algorithms.upper_bound;
import std;

namespace {
  // A branch predictor can memorize a short input that's sorted over and over, which makes branching code look far
  // better than it is on data it hasn't seen. So small inputs here cycle through many different versions of themselves,
  // and only inputs far too long to memorize are repeated.
  constexpr auto small = 1000uz;
  constexpr auto large = 100000uz;
  constexpr auto versions_of_small = 256uz;

  constexpr auto is_even = [](const int x) { return x % 2 == 0; };
  constexpr auto long_is_even = [](const std::string &s) { return s.size() % 2 == 0; };

  auto random_ints(const std::size_t count, const unsigned seed) -> std::vector<int> {
    auto engine = std::mt19937(seed);
    auto data = std::vector<int>(count);
    for (auto &x : data) { x = static_cast<int>(engine()); }
    return data;
  }

  auto sorted_ints(const std::size_t count, const unsigned) -> std::vector<int> {
    auto data = std::vector<int>(count);
    std::iota(data.begin(), data.end(), 0);
    return data;
  }

  auto reversed_ints(const std::size_t count, const unsigned) -> std::vector<int> {
    auto data = sorted_ints(count, 0);
    std::reverse(data.begin(), data.end());
    return data;
  }

  // A sorted range with about one percent of its elements put somewhere else.
  auto nearly_sorted_ints(const std::size_t count, const unsigned seed) -> std::vector<int> {
    auto data = sorted_ints(count, 0);
    auto engine = std::mt19937(seed);
    for (auto i = 0uz; i < count / 100; ++i) { std::swap(data[engine() % count], data[engine() % count]); }
    return data;
  }

  auto few_unique_ints(const std::size_t count, const unsigned seed) -> std::vector<int> {
    auto data = random_ints(count, seed);
    for (auto &x : data) { x = static_cast<int>(static_cast<unsigned>(x) % 16); }
    return data;
  }

  auto organ_pipe_ints(const std::size_t count, const unsigned) -> std::vector<int> {
    auto data = sorted_ints(count, 0);
    std::reverse(data.begin() + static_cast<std::ptrdiff_t>(count / 2), data.end());
    return data;
  }

  auto random_doubles(const std::size_t count, const unsigned seed) -> std::vector<double> {
    auto engine = std::mt19937(seed);
    auto dist = std::uniform_real_distribution<double>(0.0, 1.0);
    auto data = std::vector<double>(count);
    for (auto &x : data) { x = dist(engine); }
    return data;
  }

  struct record {
    int key;
    int payload[3];
  };

  auto random_records(const std::size_t count, const unsigned seed) -> std::vector<record> {
    auto engine = std::mt19937(seed);
    auto data = std::vector<record>(count);
    for (auto &x : data) { x.key = static_cast<int>(engine()); }
    return data;
  }

  using int_pair = std::pair<int, int>;

  auto random_pairs(const std::size_t count, const unsigned seed) -> std::vector<int_pair> {
    auto engine = std::mt19937(seed);
    auto data = std::vector<int_pair>(count);
    for (auto &x : data) { x = {static_cast<int>(engine() % 1000), static_cast<int>(engine())}; }
    return data;
  }

  auto random_strings(const std::size_t count, const unsigned seed) -> std::vector<std::string> {
    auto engine = std::mt19937(seed);
    auto data = std::vector<std::string>(count);
    for (auto &x : data) {
      x = std::to_string(engine());
      x += "-a string long enough to allocate";
    }
    return data;
  }

  template <typename T>
  auto versions(const std::size_t count, auto make) -> std::vector<std::vector<T>> {
    auto data = std::vector<std::vector<T>>();
    for (auto i = 0uz; i < (count > small ? 1uz : versions_of_small); ++i) {
      data.push_back(make(count, static_cast<unsigned>(i + 1)));
    }
    return data;
  }
}

// Each iteration works on a fresh copy of the next version. "copy" is what's being worked on.
#define GENEX_BENCH_ON_SIDE(Prefix, Name, Container, T, Make, Count, ...)   \
  static void Prefix##Name(benchmark::State &state) {                       \
    const auto inputs = versions<T>(Count, Make);                           \
    auto copy = Container(inputs[0].begin(), inputs[0].end());              \
    auto next = 0uz;                                                        \
    for (auto _ : state) {                                                  \
      const auto &input = inputs[next++ % inputs.size()];                   \
      copy.assign(input.begin(), input.end());                              \
      __VA_ARGS__;                                                          \
      benchmark::DoNotOptimize(copy);                                       \
      benchmark::ClobberMemory();                                           \
    }                                                                       \
  }                                                                         \
  BENCHMARK(Prefix##Name)

#define GENEX_BENCH_ON(Name, Container, T, Make, Count, RangesCall, GenexCall)                 \
  GENEX_BENCH_ON_SIDE(BM_StdRanges_, Name, Container, T, Make, Count, RangesCall);             \
  GENEX_BENCH_ON_SIDE(BM_Genex_V2_, Name, Container, T, Make, Count, GenexCall)

#define GENEX_BENCH_MUTATING(Name, T, Make, Count, RangesCall, GenexCall) \
  GENEX_BENCH_ON(Name, std::vector<T>, T, Make, Count, RangesCall, GenexCall)

#define GENEX_BENCH_SORTS(Name, T, Make, Count)                                                \
  GENEX_BENCH_MUTATING(Sort##Name, T, Make, Count, ranges::sort(copy), genex::sort(copy));     \
  GENEX_BENCH_MUTATING(StableSort##Name, T, Make, Count, ranges::stable_sort(copy), genex::stable_sort(copy))

GENEX_BENCH_SORTS(_Random_1k, int, random_ints, small);
GENEX_BENCH_SORTS(_Random_100k, int, random_ints, large);
GENEX_BENCH_SORTS(_Sorted_100k, int, sorted_ints, large);
GENEX_BENCH_SORTS(_Reversed_100k, int, reversed_ints, large);
GENEX_BENCH_SORTS(_NearlySorted_100k, int, nearly_sorted_ints, large);
GENEX_BENCH_SORTS(_FewUnique_100k, int, few_unique_ints, large);
GENEX_BENCH_SORTS(_OrganPipe_100k, int, organ_pipe_ints, large);
GENEX_BENCH_SORTS(_Doubles_100k, double, random_doubles, large);
GENEX_BENCH_SORTS(_Pairs_100k, int_pair, random_pairs, large);
GENEX_BENCH_SORTS(_Strings_10k, std::string, random_strings, large / 10);

GENEX_BENCH_MUTATING(
  Sort_Descending_100k, int, random_ints, large,
  ranges::sort(copy, std::greater{}), genex::sort(copy, std::greater{}));
GENEX_BENCH_MUTATING(
  Sort_ByKey_100k, record, random_records, large,
  ranges::sort(copy, {}, &record::key), genex::sort(copy, {}, &record::key));
GENEX_BENCH_MUTATING(
  StableSort_ByKey_100k, record, random_records, large,
  ranges::stable_sort(copy, {}, &record::key), genex::stable_sort(copy, {}, &record::key));
GENEX_BENCH_MUTATING(
  Sort_Lambda_100k, int, random_ints, large,
  ranges::sort(copy, [](const int a, const int b) { return a < b; }),
  genex::sort(copy, [](const int a, const int b) { return a < b; }));

GENEX_BENCH_MUTATING(
  Partition_Random_1k, int, random_ints, small, ranges::partition(copy, is_even), genex::partition(copy, is_even));
GENEX_BENCH_MUTATING(
  Partition_Random_100k, int, random_ints, large, ranges::partition(copy, is_even), genex::partition(copy, is_even));
GENEX_BENCH_MUTATING(
  Partition_Strings_10k, std::string, random_strings, large / 10,
  ranges::partition(copy, long_is_even), genex::partition(copy, long_is_even));
GENEX_BENCH_MUTATING(
  StablePartition_Random_1k, int, random_ints, small,
  ranges::stable_partition(copy, is_even), genex::stable_partition(copy, is_even));
GENEX_BENCH_MUTATING(
  StablePartition_Random_100k, int, random_ints, large,
  ranges::stable_partition(copy, is_even), genex::stable_partition(copy, is_even));
GENEX_BENCH_MUTATING(
  StablePartition_Strings_10k, std::string, random_strings, large / 10,
  ranges::stable_partition(copy, long_is_even), genex::stable_partition(copy, long_is_even));

GENEX_BENCH_MUTATING(
  Reverse_100k, int, random_ints, large, ranges::reverse(copy), genex::reverse(copy));
GENEX_BENCH_MUTATING(
  Rotate_ByOne_100k, int, random_ints, large,
  ranges::rotate(copy, copy.begin() + 1), genex::rotate(copy, copy.begin() + 1));
GENEX_BENCH_MUTATING(
  Rotate_ByThird_100k, int, random_ints, large,
  ranges::rotate(copy, copy.begin() + (large / 3)), genex::rotate(copy, copy.begin() + (large / 3)));
GENEX_BENCH_MUTATING(
  Rotate_ByHalf_100k, int, random_ints, large,
  ranges::rotate(copy, copy.begin() + (large / 2)), genex::rotate(copy, copy.begin() + (large / 2)));
GENEX_BENCH_MUTATING(
  Rotate_Strings_10k, std::string, random_strings, large / 10,
  ranges::rotate(copy, copy.begin() + (large / 30)), genex::rotate(copy, copy.begin() + (large / 30)));

using int_list = std::list<int>;
using int_forward_list = std::forward_list<int>;
using int_deque = std::deque<int>;

GENEX_BENCH_ON(
  Partition_List_10k, int_list, int, random_ints, large / 10,
  ranges::partition(copy, is_even), genex::partition(copy, is_even));
GENEX_BENCH_ON(
  Partition_ForwardList_10k, int_forward_list, int, random_ints, large / 10,
  ranges::partition(copy, is_even), genex::partition(copy, is_even));
GENEX_BENCH_ON(
  StablePartition_List_10k, int_list, int, random_ints, large / 10,
  ranges::stable_partition(copy, is_even), genex::stable_partition(copy, is_even));
GENEX_BENCH_ON(
  Reverse_List_10k, int_list, int, random_ints, large / 10, ranges::reverse(copy), genex::reverse(copy));
GENEX_BENCH_ON(
  Rotate_List_10k, int_list, int, random_ints, large / 10,
  ranges::rotate(copy, std::next(copy.begin(), large / 30)), genex::rotate(copy, std::next(copy.begin(), large / 30)));
GENEX_BENCH_ON(
  Rotate_ForwardList_10k, int_forward_list, int, random_ints, large / 10,
  ranges::rotate(copy, std::next(copy.begin(), large / 30)), genex::rotate(copy, std::next(copy.begin(), large / 30)));
GENEX_BENCH_ON(
  Sort_Deque_100k, int_deque, int, random_ints, large, ranges::sort(copy), genex::sort(copy));
GENEX_BENCH_ON(
  StableSort_Deque_100k, int_deque, int, random_ints, large, ranges::stable_sort(copy), genex::stable_sort(copy));

// Everything but the generator is the same, and the two generators are seeded the same, so what differs is how many
// numbers each takes from it and what it does with them.
static std::mt19937 shuffle_engine_32{1};
static std::mt19937_64 shuffle_engine_64{1};
GENEX_BENCH_MUTATING(
  Shuffle_Mt32_100k, int, sorted_ints, large,
  ranges::shuffle(copy, shuffle_engine_32), genex::shuffle(copy, shuffle_engine_32));
GENEX_BENCH_MUTATING(
  Shuffle_Mt64_100k, int, sorted_ints, large,
  ranges::shuffle(copy, shuffle_engine_64), genex::shuffle(copy, shuffle_engine_64));
GENEX_BENCH_MUTATING(
  Shuffle_Strings_10k, std::string, random_strings, large / 10,
  ranges::shuffle(copy, shuffle_engine_64), genex::shuffle(copy, shuffle_engine_64));

// Searching for a different key each time, so that the direction a search goes in can't be learned.
#define GENEX_BENCH_SEARCH(Name, Count, Call, Fn)                       \
  static void Name(benchmark::State &state) {                           \
    const auto data = [] {                                              \
      auto sorted = random_ints(Count, 1);                              \
      std::sort(sorted.begin(), sorted.end());                          \
      return sorted;                                                    \
    }();                                                                \
    const auto keys = random_ints(4096, 2);                             \
    auto next = 0uz;                                                    \
    for (auto _ : state) {                                              \
      const auto key = keys[next++ % keys.size()];                      \
      auto result = Call::Fn(data, key);                                \
      benchmark::DoNotOptimize(result);                                 \
    }                                                                   \
  }                                                                     \
  BENCHMARK(Name)

GENEX_BENCH_SEARCH(BM_StdRanges_LowerBound_1k, small, ranges, lower_bound);
GENEX_BENCH_SEARCH(BM_Genex_V2_LowerBound_1k, small, genex, lower_bound);
GENEX_BENCH_SEARCH(BM_StdRanges_LowerBound_1M, large * 10, ranges, lower_bound);
GENEX_BENCH_SEARCH(BM_Genex_V2_LowerBound_1M, large * 10, genex, lower_bound);
GENEX_BENCH_SEARCH(BM_StdRanges_UpperBound_1k, small, ranges, upper_bound);
GENEX_BENCH_SEARCH(BM_Genex_V2_UpperBound_1k, small, genex, upper_bound);
