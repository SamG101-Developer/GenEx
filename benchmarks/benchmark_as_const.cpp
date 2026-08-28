#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/const.hpp>

import genex.to_container;
import genex.views2.as_const;
import std;

static void BM_StdRanges_AsConst(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | ranges::views::const_
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_AsConst(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | genex::views::as_const
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_AsConst);
BENCHMARK (BM_Genex_V2_AsConst);
