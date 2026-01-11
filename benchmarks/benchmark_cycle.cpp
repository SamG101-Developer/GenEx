#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/take.hpp>

import genex.to_container;
import genex.views2.cycle;
import genex.views2.take;
import std;


static void BM_StdRanges_Cycle(benchmark::State& state) {
    std::vector<int> vec(1000);
    std::iota(vec.begin(), vec.end(), 0);

    for (auto _ : state) {
        for (auto val : vec | ranges::views::cycle | ranges::views::take(2500) | ranges::to<std::vector>()) {
            benchmark::DoNotOptimize(val);
        }
    }
}


static void BM_Genex_V2_Cycle(benchmark::State& state) {
    std::vector<int> vec(1000);
    std::iota(vec.begin(), vec.end(), 0);

    for (auto _ : state) {
        for (auto val : vec | genex::views::cycle | genex::to_n<std::vector>(2500)) {
            benchmark::DoNotOptimize(val);
        }
    }
}


BENCHMARK(BM_StdRanges_Cycle);
BENCHMARK(BM_Genex_V2_Cycle);
