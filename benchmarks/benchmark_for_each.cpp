#include <benchmark/benchmark.h>

#include <range/v3/algorithm/for_each.hpp>

import genex.views2.for_each;
import std;

static void BM_StdRanges_ForEach(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto sum = 0;
    ranges::for_each(data, [&sum](const int x) { sum += x; });
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_Genex_V2_ForEach(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto sum = 0;
    data | genex::views::for_each([&sum](const int x) { sum += x; });
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK (BM_StdRanges_ForEach);
BENCHMARK (BM_Genex_V2_ForEach);
