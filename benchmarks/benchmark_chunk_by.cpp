#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk_by.hpp>

import genex.to_container;
import genex.views2.chunk_by;
import std;

static void BM_StdRanges_ChunkBy(benchmark::State &state) {
  std::vector<int> data(1000);
  for (size_t i = 0; i < data.size(); ++i) { data[i] = i / 10; }

  for (auto _ : state) {
    auto result = data
      | ranges::views::chunk_by([](const int a, const int b) { return a == b; })
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_ChunkBy(benchmark::State &state) {
  std::vector<int> data(1000);
  for (size_t i = 0; i < data.size(); ++i) { data[i] = i / 10; }

  for (auto _ : state) {
    auto result = data
      | genex::views::chunk_by([](const int a, const int b) { return a == b; })
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_ChunkBy);
BENCHMARK (BM_Genex_V2_ChunkBy);
