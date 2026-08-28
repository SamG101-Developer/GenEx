#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/stride.hpp>

import genex.to_container;
import genex.views2.stride;
import std;

static void BM_StdRanges_Stride(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | ranges::views::stride(3)
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_Stride(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | genex::views::stride(3)
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_Stride);
BENCHMARK (BM_Genex_V2_Stride);
