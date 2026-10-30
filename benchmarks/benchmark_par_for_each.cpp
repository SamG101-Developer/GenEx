#include <benchmark/benchmark.h>

import genex.parallel.for_each;
import genex.algorithms.for_each;
import std;

// Sweeps two axes: the element count (range(0)) and how much work the callable does per element (range(1), in LCG
// steps). Parallelism only pays once `elements * work` is large enough to cover the thread spawn, so the pair of
// curves is the point of this file - read off where the parallel line crosses under the serial one.
//
// Real time, not CPU time: the parallel variant burns N cores' worth of CPU by design.

namespace {
  auto step(const std::uint32_t x, const std::int64_t work) -> std::uint32_t {
    auto y = x;
    for (auto i = std::int64_t{0}; i < work; ++i) { y = y * 1664525u + 1013904223u; }
    return y;
  }
}

static void BM_Genex_ForEach_Serial(benchmark::State &state) {
  auto data = std::vector<std::uint32_t>(static_cast<std::size_t>(state.range(0)), 1u);
  const auto work = state.range(1);

  for (auto _ : state) {
    data | genex::for_each([work](std::uint32_t &x) { x = step(x, work); });
    benchmark::ClobberMemory();
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void BM_Genex_ForEach_Parallel(benchmark::State &state) {
  auto data = std::vector<std::uint32_t>(static_cast<std::size_t>(state.range(0)), 1u);
  const auto work = state.range(1);

  // threshold = 0 so the split always happens; the default threshold would hide the small-N end of the curve behind
  // the serial fallback, which is exactly the region worth seeing.
  for (auto _ : state) {
    data | genex::parallel::for_each(
      [work](std::uint32_t &x) { x = step(x, work); },
      genex::parallel::params{.threshold = 0});
    benchmark::ClobberMemory();
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}

static void sweep(benchmark::internal::Benchmark *b) {
  for (const auto elements : {1 << 10, 1 << 13, 1 << 16, 1 << 19, 1 << 22}) {
    for (const auto work : {1, 16, 256}) { b->Args({elements, work}); }
  }
  b->UseRealTime()->Unit(benchmark::kMicrosecond);
}

BENCHMARK (BM_Genex_ForEach_Serial)->Apply(sweep);
BENCHMARK (BM_Genex_ForEach_Parallel)->Apply(sweep);