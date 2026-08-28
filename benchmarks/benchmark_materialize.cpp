#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/filter.hpp>

import genex.to_container;
import genex.views2.filter;
import genex.views2.materialize;
import std;

// ranges-v3 has no materialize view: caching a pipeline into a container there *is* `ranges::to`, so
// that is the baseline both on its own and in front of a second pass over the cached elements.
static void BM_StdRanges_Materialize(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_Materialize(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | genex::views::materialize;
    benchmark::DoNotOptimize(result);
  }
}

static void BM_StdRanges_MaterializeFiltered(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | ranges::views::filter([](const int x) { return x % 2 == 0; })
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_MaterializeFiltered(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | genex::views::filter([](const int x) { return x % 2 == 0; })
      | genex::views::materialize;
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_Materialize);
BENCHMARK (BM_Genex_V2_Materialize);
BENCHMARK (BM_StdRanges_MaterializeFiltered);
BENCHMARK (BM_Genex_V2_MaterializeFiltered);
