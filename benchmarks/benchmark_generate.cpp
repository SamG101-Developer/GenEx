#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/generate.hpp>
#include <range/v3/view/generate_n.hpp>
#include <range/v3/view/take.hpp>

import genex.to_container;
import genex.views2.generate;
import genex.views2.take;
import std;

static void BM_StdRanges_GenerateN(benchmark::State &state) {
  for (auto _ : state) {
    auto next = 0;
    auto result = ranges::views::generate_n([next]() mutable { return next++; }, 1000)
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_GenerateN(benchmark::State &state) {
  for (auto _ : state) {
    auto next = 0;
    auto result = genex::views::generate_n([next]() mutable { return next++; }, 1000)
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_StdRanges_GenerateUnbounded(benchmark::State &state) {
  for (auto _ : state) {
    auto next = 0;
    auto result = ranges::views::generate([next]() mutable { return next++; })
      | ranges::views::take(1000)
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_GenerateUnbounded(benchmark::State &state) {
  for (auto _ : state) {
    auto next = 0;
    auto result = genex::views::generate([next]() mutable { return next++; })
      | genex::views::take(1000)
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_GenerateN);
BENCHMARK (BM_Genex_V2_GenerateN);
BENCHMARK (BM_StdRanges_GenerateUnbounded);
BENCHMARK (BM_Genex_V2_GenerateUnbounded);
