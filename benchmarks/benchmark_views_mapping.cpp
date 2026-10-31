#include "view_benchmarks.hpp"

#include <range/v3/to_container.hpp>
#include <range/v3/view/addressof.hpp>
#include <range/v3/view/const.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/partial_sum.hpp>
#include <range/v3/view/replace.hpp>
#include <range/v3/view/replace_if.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>

import genex.to_container;
import genex.views.address_of;
import genex.views.as_const;
import genex.views.cast_static;
import genex.views.enumerate;
import genex.views.filter;
import genex.views.indirect;
import genex.views.map;
import genex.views.materialize;
import genex.views.move;
import genex.views.move_reverse;
import genex.views.ptr;
import genex.views.replace;
import genex.views.replace_if;
import genex.views.reverse;
import genex.views.scan;
import genex.views.transform;
import genex.views.tuple_nth;

namespace {
  constexpr auto get_pointer = [](auto &&ptr) { return ptr.get(); };

  auto unique_pointers() -> std::vector<std::unique_ptr<int>> {
    auto data = std::vector<std::unique_ptr<int>>();
    for (auto i = 0; i < 1000; ++i) { data.push_back(std::make_unique<int>(i)); }
    return data;
  }

  auto shared_pointers() -> std::vector<std::shared_ptr<int>> {
    auto data = std::vector<std::shared_ptr<int>>();
    for (auto i = 0; i < 1000; ++i) { data.push_back(std::make_shared<int>(i)); }
    return data;
  }

  auto tuples() -> std::vector<std::tuple<int, double, char>> {
    auto data = std::vector<std::tuple<int, double, char>>();
    for (auto i = 0; i < 1000; ++i) { data.emplace_back(i, i + 0.5, 'a' + (i % 26)); }
    return data;
  }

  auto map_of_ints() -> std::map<int, int> {
    auto data = std::map<int, int>();
    for (auto i = 0; i < 500; ++i) { data[i] = i * 10; }
    return data;
  }

  auto strings() -> std::vector<std::string> {
    return std::vector<std::string>(1000, "a string long enough to heap allocate");
  }
}

GENEX_BENCH_EXPR(ToVector, bench::ascending, data | ranges::to<std::vector>(), data | genex::to<std::vector>());

GENEX_BENCH_VIEW(
  Transform, bench::ascending,
  ranges::views::transform([](const int x) { return x * 2; }),
  genex::views::transform([](const int x) { return x * 2; }));

GENEX_BENCH_VIEW(AddressOf_Contiguous, bench::ascending, ranges::views::addressof, genex::views::address_of);
GENEX_BENCH_VIEW(AsConst, bench::ascending, ranges::views::const_, genex::views::as_const);

GENEX_BENCH_VIEW(
  CastStatic, bench::ascending,
  ranges::views::transform([](auto b) { return static_cast<double>(b); }),
  genex::views::cast_static<double>);

GENEX_BENCH_VIEW(Indirect, unique_pointers, ranges::views::indirect, genex::views::indirect);
GENEX_BENCH_VIEW(Ptr_UniquePtr, unique_pointers, ranges::views::transform(get_pointer), genex::views::ptr);
GENEX_BENCH_VIEW(Ptr_SharedPtr, shared_pointers, ranges::views::transform(get_pointer), genex::views::ptr);

GENEX_BENCH_VIEW(Keys, map_of_ints, ranges::views::keys, genex::views::keys);
GENEX_BENCH_VIEW(Vals, map_of_ints, ranges::views::values, genex::views::vals);

GENEX_BENCH_VIEW(
  TupleNth, tuples,
  ranges::views::transform([](auto &&tpl) { return std::get<1>(tpl); }),
  genex::views::tuple_nth<1>);

GENEX_BENCH_VIEW(Replace, bench::alternating, ranges::views::replace(0, 5), genex::views::replace(0, 5));

GENEX_BENCH_VIEW(
  ReplaceIf, bench::alternating,
  ranges::views::replace_if([](const int x) { return x < 500; }, 5),
  genex::views::replace_if([](const int x) { return x < 500; }, 5));

GENEX_BENCH_VIEW(Reverse, bench::alternating, ranges::views::reverse, genex::views::reverse);

GENEX_BENCH_VIEW(PartialSum, bench::ascending, ranges::views::partial_sum, genex::views::partial_sum);

// ranges-v3 has no seeded scan, so the baseline folds the seed in with a partial sum over the same
// data and an added first element.
GENEX_BENCH_EXPR(
  Scan, bench::ascending,
  [&] {
    auto out = data | ranges::views::partial_sum | ranges::to<std::vector>();
    for (auto &x : out) { x += 100; }
    return out;
  }(),
  data | genex::views::scan(100) | genex::to<std::vector>());

// ranges-v3 has no materialize view: caching a pipeline into a container there *is* `ranges::to`, so
// that is the baseline both on its own and in front of a second pass over the cached elements.
GENEX_BENCH_EXPR(Materialize, bench::ascending, data | ranges::to<std::vector>(), data | genex::views::materialize);

GENEX_BENCH_EXPR(
  MaterializeFiltered, bench::ascending,
  data | ranges::views::filter([](const int x) { return x % 2 == 0; }) | ranges::to<std::vector>(),
  data | genex::views::filter([](const int x) { return x % 2 == 0; }) | genex::views::materialize);

// The source is refilled inside the timed region on both sides -- moving out of it is destructive,
// so every iteration needs a fresh copy for the move to have anything to steal.
#define GENEX_BENCH_MOVING_VIEW(Name, RangesView, GenexView)     \
  GENEX_BENCH_EXPR(                                              \
    Name, strings,                                               \
    [&] {                                                        \
      auto source = data;                                        \
      return source | RangesView | ranges::to<std::vector>();    \
    }(),                                                         \
    [&] {                                                        \
      auto source = data;                                        \
      return source | GenexView | genex::to<std::vector>();      \
    }())

GENEX_BENCH_MOVING_VIEW(Move, ranges::views::move, genex::views::move);
GENEX_BENCH_MOVING_VIEW(MoveReverse, ranges::views::reverse | ranges::views::move, genex::views::move_reverse);

// These read every element themselves, rather than collecting them in a benchmarked container.
static void BM_StdRanges_Enumerate(benchmark::State &state) {
  auto vec = bench::ascending();
  for (auto _ : state) {
    for (auto [i, val] : vec | ranges::views::enumerate | ranges::to<std::vector>()) {
      benchmark::DoNotOptimize(i);
      benchmark::DoNotOptimize(val);
    }
  }
}

BENCHMARK(BM_StdRanges_Enumerate);

static void BM_Genex_V2_Enumerate(benchmark::State &state) {
  auto vec = bench::ascending();
  for (auto _ : state) {
    for (auto [i, val] : vec | genex::views::enumerate | genex::to<std::vector>()) {
      benchmark::DoNotOptimize(i);
      benchmark::DoNotOptimize(val);
    }
  }
}

BENCHMARK(BM_Genex_V2_Enumerate);
