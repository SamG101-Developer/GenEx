#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/move.hpp>

import genex.to_container;
import genex.views2.move;
import std;

// The source is refilled inside the timed region on both sides -- moving out of it is destructive,
// so every iteration needs a fresh copy for the move to have anything to steal.
static void BM_StdRanges_Move(benchmark::State &state) {
  const std::vector<std::string> data(1000, "a string long enough to heap allocate");

  for (auto _ : state) {
    auto source = data;
    auto result = source
      | ranges::views::move
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_Move(benchmark::State &state) {
  const std::vector<std::string> data(1000, "a string long enough to heap allocate");

  for (auto _ : state) {
    auto source = data;
    auto result = source
      | genex::views::move
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_Move);
BENCHMARK (BM_Genex_V2_Move);
