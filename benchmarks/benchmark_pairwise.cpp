#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/sliding.hpp>

import genex.to_container;
import genex.views2.pairwise;
import std;

// ranges-v3 has no pairwise view, so the closest equivalent -- a sliding window of two -- is the
// baseline. It yields subranges rather than pairs, which is the one shape difference here.
static void BM_StdRanges_Pairwise(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | ranges::views::sliding(2)
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_Pairwise(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data
      | genex::views::pairwise
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_Pairwise);
BENCHMARK (BM_Genex_V2_Pairwise);
