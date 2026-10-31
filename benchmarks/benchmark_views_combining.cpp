#include "view_benchmarks.hpp"

#include <range/v3/to_container.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/intersperse.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/set_algorithm.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/view/zip_with.hpp>

import genex.to_container;
import genex.views.concat;
import genex.views.cycle;
import genex.views.interleave;
import genex.views.intersperse;
import genex.views.join;
import genex.views.join_with;
import genex.views.set_algorithms;
import genex.views.take;
import genex.views.zip;
import genex.views.zip_with;

namespace {
  // Sorted, with some but not all of their elements in common.
  auto overlapping_sets() -> std::pair<std::vector<int>, std::vector<int>> {
    auto data = std::pair{std::vector<int>(500), std::vector<int>(500)};
    for (auto i = 0; i < 500; ++i) {
      data.first[static_cast<std::size_t>(i)] = i * 2;
      data.second[static_cast<std::size_t>(i)] = i * 3;
    }
    return data;
  }
}

#define GENEX_BENCH_SET(Name, Algorithm)                                               \
  GENEX_BENCH_EXPR(                                                                    \
    Name, overlapping_sets,                                                            \
    data.first | ranges::views::Algorithm(data.second) | ranges::to<std::vector>(),    \
    data.first | genex::views::Algorithm(data.second) | genex::to<std::vector>())

GENEX_BENCH_SET(SetSortedDifference, set_difference);
GENEX_BENCH_SET(SetSortedUnion, set_union);
GENEX_BENCH_SET(SetSortedIntersection, set_intersection);
GENEX_BENCH_SET(SetSortedSymmetricDifference, set_symmetric_difference);

GENEX_BENCH_EXPR(
  Concat, bench::two_halves,
  ranges::views::concat(data.first, data.second) | ranges::to<std::vector>(),
  genex::views::concat(data.first, data.second) | genex::to<std::vector>());

GENEX_BENCH_EXPR(
  Zip, bench::two_halves,
  ranges::views::zip(data.first, data.second) | ranges::to<std::vector>(),
  genex::views::zip(data.first, data.second) | genex::to<std::vector>());

GENEX_BENCH_EXPR(
  ZipWith, bench::two_halves,
  ranges::views::zip_with([](const int a, const int b) { return a + b; }, data.first, data.second)
    | ranges::to<std::vector>(),
  genex::views::zip_with(data.first, data.second, [](const int a, const int b) { return a + b; })
    | genex::to<std::vector>());

GENEX_BENCH_VIEW(Join, bench::nested, ranges::views::join, genex::views::join);
GENEX_BENCH_VIEW(JoinWith, bench::nested, ranges::views::join(0), genex::views::join_with(0));
GENEX_BENCH_VIEW(Intersperse, bench::ascending, ranges::views::intersperse(100), genex::views::intersperse(100));

// ranges-v3 has no interleave view that takes a second range, so this has nothing to be compared to.
static void BM_Genex_V2_Interleave(benchmark::State &state) {
  auto data1 = bench::ascending();
  auto data2 = bench::ascending();
  for (auto _ : state) {
    auto result = data1 | genex::views::interleave(data2) | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_Genex_V2_Interleave);

// These read every element themselves, rather than collecting them in a benchmarked container.
static void BM_StdRanges_Cycle(benchmark::State &state) {
  auto vec = bench::ascending();
  for (auto _ : state) {
    for (auto val : vec | ranges::views::cycle | ranges::views::take(2500) | ranges::to<std::vector>()) {
      benchmark::DoNotOptimize(val);
    }
  }
}

BENCHMARK(BM_StdRanges_Cycle);

static void BM_Genex_V2_Cycle(benchmark::State &state) {
  auto vec = bench::ascending();
  for (auto _ : state) {
    for (auto val : vec | genex::views::cycle | genex::views::take(2500) | genex::to<std::vector>()) {
      benchmark::DoNotOptimize(val);
    }
  }
}

BENCHMARK(BM_Genex_V2_Cycle);
