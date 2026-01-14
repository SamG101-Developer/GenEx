#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/replace.hpp>

import genex.to_container;
import genex.views.replace;
import std;


static void BM_StdRanges_Replace(benchmark::State &state) {
    std::vector<int> data(1000);
    for (size_t i = 0; i < data.size(); ++i) { data[i] = i % 2; }

    for (auto _ : state) {
        auto result = data | ranges::views::replace(0, 5) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Replace(benchmark::State &state) {
    std::vector<int> data(1000);
    for (size_t i = 0; i < data.size(); ++i) { data[i] = i % 2; }

    for (auto _ : state) {
        auto result = data | genex::views::replace(0, 5) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Replace);
BENCHMARK(BM_Genex_V2_Replace);
