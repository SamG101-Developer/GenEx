#pragma once

#include <benchmark/benchmark.h>

#include <array>
#include <cstddef>
#include <forward_list>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

// A pair of benchmarks for the same pipeline, one over each library, named so results can be compared side by side:
// BM_StdRanges_<Name> and BM_Genex_V2_<Name>. Data is a function returning what they run over, which is set up outside
// of the timed loop. Each iteration only has what's in its expression timed.
#define GENEX_BENCH_EXPR(Name, Data, RangesExpr, GenexExpr)      \
  static void BM_StdRanges_##Name(benchmark::State &state) {     \
    auto data = Data();                                          \
    for (auto _ : state) {                                       \
      auto result = RangesExpr;                                  \
      benchmark::DoNotOptimize(result);                          \
    }                                                            \
  }                                                              \
  BENCHMARK(BM_StdRanges_##Name);                                \
  static void BM_Genex_V2_##Name(benchmark::State &state) {      \
    auto data = Data();                                          \
    for (auto _ : state) {                                       \
      auto result = GenexExpr;                                   \
      benchmark::DoNotOptimize(result);                          \
    }                                                            \
  }                                                              \
  BENCHMARK(BM_Genex_V2_##Name)

// The usual case: pipe the data through a view, and collect what comes out.
#define GENEX_BENCH_VIEW(Name, Data, RangesView, GenexView) \
  GENEX_BENCH_EXPR(                                         \
    Name, Data,                                             \
    data | RangesView | ranges::to<std::vector>(),          \
    data | GenexView | genex::to<std::vector>())

namespace bench {
  // For a pipeline that makes its own data.
  struct no_data {
  };

  inline auto ascending_n(const std::size_t size, const int start = 0) -> std::vector<int> {
    auto data = std::vector<int>(size);
    std::iota(data.begin(), data.end(), start);
    return data;
  }

  inline auto ascending() -> std::vector<int> {
    return ascending_n(1000);
  }

  inline auto ascending_list() -> std::list<int> {
    auto data = std::list<int>(1000);
    std::iota(data.begin(), data.end(), 0);
    return data;
  }

  inline auto ascending_forward_list() -> std::forward_list<int> {
    auto data = std::forward_list<int>(1000);
    std::iota(data.begin(), data.end(), 0);
    return data;
  }

  // Equal neighbours and repeated values, for the views that care about them.
  template <auto Pattern>
  auto patterned() -> std::vector<int> {
    auto data = std::vector<int>(1000);
    for (auto i = 0uz; i < data.size(); ++i) { data[i] = Pattern(static_cast<int>(i)); }
    return data;
  }

  inline constexpr auto alternating = patterned<[](const int i) { return i % 2; }>;
  inline constexpr auto runs_of_ten = patterned<[](const int i) { return i / 10; }>;
  inline constexpr auto repeats_every_hundred = patterned<[](const int i) { return i % 100; }>;

  inline auto two_halves() -> std::pair<std::vector<int>, std::vector<int>> {
    return {ascending_n(500), ascending_n(500, 500)};
  }

  inline auto nested() -> std::vector<std::vector<int>> {
    auto data = std::vector<std::vector<int>>(500);
    for (auto i = 0uz; i < data.size(); ++i) { data[i] = ascending_n(20, static_cast<int>(i * 20)); }
    return data;
  }
}
