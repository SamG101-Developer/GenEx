#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/empty.hpp>
#include <range/v3/view/single.hpp>

import genex.to_container;
import genex.views2.empty;
import genex.views2.single;
import std;

static void BM_StdRanges_Single(benchmark::State &state) {
  for (auto _ : state) {
    auto result = ranges::views::single(42)
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_Single(benchmark::State &state) {
  for (auto _ : state) {
    auto result = genex::views::single(42)
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_StdRanges_Empty(benchmark::State &state) {
  for (auto _ : state) {
    auto result = ranges::views::empty<int>
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_Empty(benchmark::State &state) {
  for (auto _ : state) {
    auto result = genex::views::empty<int>
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_Single);
BENCHMARK (BM_Genex_V2_Single);
BENCHMARK (BM_StdRanges_Empty);
BENCHMARK (BM_Genex_V2_Empty);
