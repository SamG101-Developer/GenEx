#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/partial_sum.hpp>

import genex.to_container;
import genex.views2.scan;
import std;

static void BM_StdRanges_PartialSum(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | ranges::views::partial_sum
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_PartialSum(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | genex::views::partial_sum
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

// ranges-v3 has no seeded scan, so the baseline folds the seed in with a partial sum over the same
// data and an added first element.
static void BM_StdRanges_Scan(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | ranges::views::partial_sum
      | ranges::to<std::vector>();
    for (auto &x : result) { x += 100; }
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_Scan(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | genex::views::scan(100)
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_PartialSum);
BENCHMARK (BM_Genex_V2_PartialSum);
BENCHMARK (BM_StdRanges_Scan);
BENCHMARK (BM_Genex_V2_Scan);
