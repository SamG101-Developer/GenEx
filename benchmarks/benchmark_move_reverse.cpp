#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/reverse.hpp>

import genex.to_container;
import genex.views2.reverse;
import std;


static void BM_StdRanges_Reverse(benchmark::State &state) {
    std::vector<int> data(1000);
    for (size_t i = 0; i < data.size(); ++i) { data[i] = i % 2; }

    for (auto _ : state) {
        auto result = data | ranges::views::reverse | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Reverse(benchmark::State &state) {
    std::vector<int> data(1000);
    for (size_t i = 0; i < data.size(); ++i) { data[i] = i % 2; }

    for (auto _ : state) {
        auto result = data | genex::views::reverse | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Reverse);
BENCHMARK(BM_Genex_V2_Reverse);
