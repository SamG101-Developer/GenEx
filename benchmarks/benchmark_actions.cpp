#include <benchmark/benchmark.h>

#include <range/v3/action/drop.hpp>
#include <range/v3/action/drop_while.hpp>
#include <range/v3/action/push_back.hpp>
#include <range/v3/action/remove.hpp>
#include <range/v3/action/remove_if.hpp>
#include <range/v3/action/reverse.hpp>
#include <range/v3/action/slice.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/action/stable_sort.hpp>
#include <range/v3/action/take.hpp>
#include <range/v3/action/take_while.hpp>
#include <range/v3/action/transform.hpp>
#include <range/v3/action/unique.hpp>

import genex.actions.concat;
import genex.actions.drop;
import genex.actions.drop_while;
import genex.actions.remove;
import genex.actions.remove_if;
import genex.actions.reverse;
import genex.actions.slice;
import genex.actions.sort;
import genex.actions.take;
import genex.actions.take_while;
import genex.actions.transform;
import genex.actions.unique;
import std;

namespace {
  constexpr auto is_even = [](const int x) { return x % 2 == 0; };
  constexpr auto is_small = [](const int x) { return x < 500; };
  constexpr auto twice = [](const int x) { return x * 2; };

  // Shuffled with a fixed seed, so a sort has real work to do and each run sees the same data.
  auto shuffled() -> std::vector<int> {
    auto data = std::vector<int>(1000);
    std::iota(data.begin(), data.end(), 0);
    auto engine = std::mt19937(42);
    std::shuffle(data.begin(), data.end(), engine);
    return data;
  }

  auto ascending() -> std::vector<int> {
    auto data = std::vector<int>(1000);
    std::iota(data.begin(), data.end(), 0);
    return data;
  }

  // Runs of ten equal values, for unique to collapse.
  // How predictable removal is decides what a branch costs, so these are the opposite cases to "shuffled": one element
  // in a hundred removed, one in a hundred kept, and only the first removed.
  auto few_even() -> std::vector<int> {
    auto data = std::vector<int>(1000);
    for (auto i = 0uz; i < data.size(); ++i) { data[i] = static_cast<int>(i % 100 == 50 ? i * 2 : i * 2 + 1); }
    return data;
  }

  auto few_odd() -> std::vector<int> {
    auto data = std::vector<int>(1000);
    for (auto i = 0uz; i < data.size(); ++i) { data[i] = static_cast<int>(i % 100 == 50 ? i * 2 + 1 : i * 2); }
    return data;
  }

  auto first_even() -> std::vector<int> {
    auto data = std::vector<int>(1000);
    for (auto i = 0uz; i < data.size(); ++i) { data[i] = static_cast<int>(i == 0 ? 0 : i * 2 + 1); }
    return data;
  }

  constexpr auto unseen_size = 1000uz;

  auto unseen_pool() -> const std::vector<int>& {
    static const auto pool = [] {
      auto data = std::vector<int>(unseen_size * 1024);
      auto engine = std::mt19937(7);
      for (auto &x : data) { x = static_cast<int>(engine()); }
      return data;
    }();
    return pool;
  }

  auto runs() -> std::vector<int> {
    auto data = std::vector<int>(1000);
    for (auto i = 0uz; i < data.size(); ++i) { data[i] = static_cast<int>(i / 10); }
    return data;
  }
}

// Each iteration acts on a fresh copy, as actions change what they're given. It reuses one buffer, as otherwise the time
// spent in malloc and free swamps the action, and depends on what else happens to be on the heap: a 4000 byte block is
// past glibc's fast path, which made unique look 60% slower depending only on which other benchmarks were registered.
#define GENEX_BENCH_ACTION(Name, Data, RangesAction, GenexAction)  \
  static void BM_StdRanges_Action##Name(benchmark::State &state) { \
    const auto data = Data();                                      \
    auto copy = data;                                              \
    for (auto _ : state) {                                         \
      copy.assign(data.begin(), data.end());                       \
      copy |= ranges::actions::RangesAction;                       \
      benchmark::DoNotOptimize(copy);                              \
    }                                                              \
  }                                                                \
  BENCHMARK(BM_StdRanges_Action##Name);                            \
  static void BM_Genex_V2_Action##Name(benchmark::State &state) {  \
    const auto data = Data();                                      \
    auto copy = data;                                              \
    for (auto _ : state) {                                         \
      copy.assign(data.begin(), data.end());                       \
      copy |= genex::actions::GenexAction;                         \
      benchmark::DoNotOptimize(copy);                              \
    }                                                              \
  }                                                                \
  BENCHMARK(BM_Genex_V2_Action##Name)

GENEX_BENCH_ACTION(Drop, ascending, drop(500), drop(500));
GENEX_BENCH_ACTION(DropWhile, ascending, drop_while(is_small), drop_while(is_small));
GENEX_BENCH_ACTION(Remove, runs, remove(50), remove(50));
GENEX_BENCH_ACTION(RemoveIf, shuffled, remove_if(is_even), remove_if(is_even));
GENEX_BENCH_ACTION(RemoveIf_FewRemoved, few_even, remove_if(is_even), remove_if(is_even));
GENEX_BENCH_ACTION(RemoveIf_FewKept, few_odd, remove_if(is_even), remove_if(is_even));
GENEX_BENCH_ACTION(RemoveIf_FirstOnly, first_even, remove_if(is_even), remove_if(is_even));

// A branch predictor can learn most of an array that's repeated every iteration, which hides what an unpredictable
// branch costs on data it hasn't seen. These go through more arrays than it can remember.
#define GENEX_BENCH_ACTION_UNSEEN(Name, RangesAction, GenexAction)                    \
  static void BM_StdRanges_Action##Name(benchmark::State &state) {                    \
    const auto &pool = unseen_pool();                                                 \
    auto copy = std::vector<int>(unseen_size);                                        \
    auto start = 0uz;                                                                 \
    for (auto _ : state) {                                                            \
      copy.assign(pool.begin() + start, pool.begin() + start + unseen_size);          \
      start = (start + unseen_size) % pool.size();                                    \
      copy |= ranges::actions::RangesAction;                                          \
      benchmark::DoNotOptimize(copy);                                                 \
    }                                                                                 \
  }                                                                                   \
  BENCHMARK(BM_StdRanges_Action##Name);                                               \
  static void BM_Genex_V2_Action##Name(benchmark::State &state) {                     \
    const auto &pool = unseen_pool();                                                 \
    auto copy = std::vector<int>(unseen_size);                                        \
    auto start = 0uz;                                                                 \
    for (auto _ : state) {                                                            \
      copy.assign(pool.begin() + start, pool.begin() + start + unseen_size);          \
      start = (start + unseen_size) % pool.size();                                    \
      copy |= genex::actions::GenexAction;                                            \
      benchmark::DoNotOptimize(copy);                                                 \
    }                                                                                 \
  }                                                                                   \
  BENCHMARK(BM_Genex_V2_Action##Name)

GENEX_BENCH_ACTION_UNSEEN(RemoveIf_Unseen, remove_if(is_even), remove_if(is_even));
GENEX_BENCH_ACTION_UNSEEN(StableSort, stable_sort, stable_sort);
GENEX_BENCH_ACTION(Reverse, ascending, reverse, reverse);
GENEX_BENCH_ACTION(Slice, ascending, slice(250, 750), slice(250, 750));
GENEX_BENCH_ACTION(Sort, shuffled, sort, sort);
GENEX_BENCH_ACTION(Take, ascending, take(500), take(500));
GENEX_BENCH_ACTION(TakeWhile, ascending, take_while(is_small), take_while(is_small));
GENEX_BENCH_ACTION(Transform, ascending, transform(twice), transform(twice));
GENEX_BENCH_ACTION(Unique, runs, unique, unique);

static void BM_StdRanges_ActionConcat(benchmark::State &state) {
  const auto lhs = ascending();
  const auto rhs = ascending();
  auto copy = lhs;
  copy.reserve(lhs.size() + rhs.size());
  for (auto _ : state) {
    copy.assign(lhs.begin(), lhs.end());
    copy |= ranges::actions::push_back(rhs);
    benchmark::DoNotOptimize(copy);
  }
}

BENCHMARK(BM_StdRanges_ActionConcat);

static void BM_Genex_V2_ActionConcat(benchmark::State &state) {
  const auto lhs = ascending();
  const auto rhs = ascending();
  auto copy = lhs;
  copy.reserve(lhs.size() + rhs.size());
  for (auto _ : state) {
    copy.assign(lhs.begin(), lhs.end());
    copy |= genex::actions::concat(rhs);
    benchmark::DoNotOptimize(copy);
  }
}

BENCHMARK(BM_Genex_V2_ActionConcat);
