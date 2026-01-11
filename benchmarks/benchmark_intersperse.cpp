#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/intersperse.hpp>

import genex.to_container;
import genex.views2.intersperse;
import std;


static void BM_StdRanges_Intersperse(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::intersperse(100) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Intersperse(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views::intersperse(100) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Intersperse);
BENCHMARK(BM_Genex_V2_Intersperse);
