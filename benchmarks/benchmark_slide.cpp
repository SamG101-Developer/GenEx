#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/sliding.hpp>

import genex.to_container;
import genex.views2.slide;
import std;

static void BM_StdRanges_Slide(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  constexpr auto window = 10;

  for (auto _ : state) {
    auto result = data
      | ranges::views::sliding(window)
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_Slide(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  constexpr auto window = 10;

  for (auto _ : state) {
    auto result = data
      | genex::views::slide(window)
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_Slide);
BENCHMARK (BM_Genex_V2_Slide);
