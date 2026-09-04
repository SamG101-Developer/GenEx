#include <benchmark/benchmark.h>

import genex.views2.divide;
import std;

static void BM_StdRanges_Divide_RandomAccess(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    const auto n = data.size() / 4;
    const auto rem = data.size() % 4;
    auto result = std::array<std::span<int>, 4>();
    auto offset = std::size_t{0};
    for (auto i = std::size_t{0}; i < 4; ++i) {
      const auto len = n + (i < rem ? 1 : 0);
      result[i] = std::span<int>(data.data() + offset, len);
      offset += len;
    }
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_Divide_RandomAccess(benchmark::State &state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data | genex::views::divide<4>;
    benchmark::DoNotOptimize(result);
  }
}

static void BM_StdRanges_Divide_Forward(benchmark::State &state) {
  std::list<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    const auto n = data.size() / 4;
    const auto rem = data.size() % 4;
    auto result = std::array<std::pair<std::list<int>::iterator, std::list<int>::iterator>, 4>();
    auto it = data.begin();
    for (auto i = std::size_t{0}; i < 4; ++i) {
      const auto len = n + (i < rem ? 1 : 0);
      auto lo = it;
      std::advance(it, len);
      result[i] = {lo, it};
    }
    benchmark::DoNotOptimize(result);
  }
}

static void BM_Genex_V2_Divide_Forward(benchmark::State &state) {
  std::list<int> data(1000);
  std::iota(data.begin(), data.end(), 0);

  for (auto _ : state) {
    auto result = data | genex::views::divide<4>;
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK (BM_StdRanges_Divide_RandomAccess);
BENCHMARK (BM_Genex_V2_Divide_RandomAccess);
BENCHMARK (BM_StdRanges_Divide_Forward);
BENCHMARK (BM_Genex_V2_Divide_Forward);