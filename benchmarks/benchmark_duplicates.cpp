#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>

import genex.to_container;
import genex.views2.duplicates;
import std;

// ranges-v3 has no duplicates view, so the baseline is the hand-rolled scan-against-seen loop that
// the view replaces.
static void BM_StdRanges_Duplicates(benchmark::State &state) {
  std::vector<int> data(1000);
  for (size_t i = 0; i < data.size(); ++i) { data[i] = i % 100; }

  for (auto _ : state) {
    auto seen = std::vector<int>();
    auto result = std::vector<int>();
    for (const auto x : data) {
      if (std::find(seen.begin(), seen.end(), x) != seen.end()) { result.push_back(x); }
      else { seen.push_back(x); }
    }
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_Duplicates(benchmark::State &state) {
  std::vector<int> data(1000);
  for (size_t i = 0; i < data.size(); ++i) { data[i] = i % 100; }

  for (auto _ : state) {
    auto result = data
      | genex::views::duplicates
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_Duplicates);
BENCHMARK (BM_Genex_V2_Duplicates);
