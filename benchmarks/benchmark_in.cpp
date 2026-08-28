#include <benchmark/benchmark.h>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/filter.hpp>

import genex.to_container;
import genex.views2.in;
import std;

// ranges-v3 has no membership view, so the baseline is the filter-over-contains it expands to.
static void BM_StdRanges_In(benchmark::State &state) {
  std::vector<int> data(1000);
  std::vector<int> other(100);
  std::iota(data.begin(), data.end(), 0);
  std::iota(other.begin(), other.end(), 0);

  for (auto _ : state) {
    auto result = data
      | ranges::views::filter([&other](const int x) { return ranges::contains(other, x); })
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_In(benchmark::State &state) {
  std::vector<int> data(1000);
  std::vector<int> other(100);
  std::iota(data.begin(), data.end(), 0);
  std::iota(other.begin(), other.end(), 0);

  for (auto _ : state) {
    auto result = data
      | genex::views::in(other)
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_StdRanges_NotIn(benchmark::State &state) {
  std::vector<int> data(1000);
  std::vector<int> other(100);
  std::iota(data.begin(), data.end(), 0);
  std::iota(other.begin(), other.end(), 0);

  for (auto _ : state) {
    auto result = data
      | ranges::views::filter([&other](const int x) { return not ranges::contains(other, x); })
      | ranges::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_NotIn(benchmark::State &state) {
  std::vector<int> data(1000);
  std::vector<int> other(100);
  std::iota(data.begin(), data.end(), 0);
  std::iota(other.begin(), other.end(), 0);

  for (auto _ : state) {
    auto result = data
      | genex::views::not_in(other)
      | genex::to<std::vector>();
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_In);
BENCHMARK (BM_Genex_V2_In);
BENCHMARK (BM_StdRanges_NotIn);
BENCHMARK (BM_Genex_V2_NotIn);
