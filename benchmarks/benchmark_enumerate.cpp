#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/enumerate.hpp>

import genex.to_container;
import genex.views.enumerate;
import std;


static void BM_StdRanges_Enumerate(benchmark::State& state) {
    std::vector<int> vec(1000);
    std::iota(vec.begin(), vec.end(), 0);

    for (auto _ : state) {
        for (auto [i, val] : vec | ranges::views::enumerate | ranges::to<std::vector>()) {
            benchmark::DoNotOptimize(i);
            benchmark::DoNotOptimize(val);
        }
    }
}


static void BM_Genex_V2_Enumerate(benchmark::State& state) {
    std::vector<int> vec(1000);
    std::iota(vec.begin(), vec.end(), 0);

    for (auto _ : state) {
        for (auto [i, val] : vec | genex::views::enumerate | genex::to<std::vector>()) {
            benchmark::DoNotOptimize(i);
            benchmark::DoNotOptimize(val);
        }
    }
}


BENCHMARK(BM_StdRanges_Enumerate);
BENCHMARK(BM_Genex_V2_Enumerate);
