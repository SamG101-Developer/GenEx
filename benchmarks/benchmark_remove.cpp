#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/remove.hpp>

import genex.to_container;
import genex.views2.remove;
import std;


static void BM_StdRanges_Remove(benchmark::State &state) {
    std::vector<int> data(1000);
    for (size_t i = 0; i < data.size(); ++i) { data[i] = i % 2; }

    for (auto _ : state) {
        auto result = data | ranges::views::remove(0) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Remove(benchmark::State &state) {
    std::vector<int> data(1000);
    for (size_t i = 0; i < data.size(); ++i) { data[i] = i % 2; }

    for (auto _ : state) {
        auto result = data | genex::views::remove(0) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Remove);
BENCHMARK(BM_Genex_V2_Remove);
