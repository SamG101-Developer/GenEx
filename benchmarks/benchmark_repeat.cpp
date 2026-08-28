#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/take.hpp>

import genex.to_container;
import genex.views2.repeat;
import genex.views2.take;
import std;

static void BM_StdRanges_RepeatN(benchmark::State &state) {
  for (auto _ : state) {
    auto result = ranges::views::repeat_n(42, 1000)
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_RepeatN(benchmark::State &state) {
  for (auto _ : state) {
    auto result = genex::views::repeat(42, 1000)
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_StdRanges_RepeatUnbounded(benchmark::State &state) {
  for (auto _ : state) {
    auto result = ranges::views::repeat(42)
      | ranges::views::take(1000)
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_RepeatUnbounded(benchmark::State &state) {
  for (auto _ : state) {
    auto result = genex::views::repeat(42)
      | genex::views::take(1000)
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_RepeatN);
BENCHMARK (BM_Genex_V2_RepeatN);
BENCHMARK (BM_StdRanges_RepeatUnbounded);
BENCHMARK (BM_Genex_V2_RepeatUnbounded);
