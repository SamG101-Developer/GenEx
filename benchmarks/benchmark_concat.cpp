#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/concat.hpp>

import genex.to_container;
import genex.views2.concat;
import std;


static void BM_StdRanges_Concat(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    std::iota(data1.begin(), data1.end(), 0);
    std::iota(data2.begin(), data2.end(), 500);

    for (auto _ : state) {
        auto result = ranges::views::concat(data1, data2) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Concat(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    std::iota(data1.begin(), data1.end(), 0);
    std::iota(data2.begin(), data2.end(), 500);

    for (auto _ : state) {
        auto result = genex::views2::concat(data1, data2) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Concat);
BENCHMARK(BM_Genex_V2_Concat);
