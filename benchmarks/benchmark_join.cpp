#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/join.hpp>

import genex.to_container;
import genex.views.join;
import std;


static void BM_StdRanges_Join(benchmark::State &state) {
    std::vector<std::vector<int>> data(500);
    for (std::size_t i = 0; i < 500; ++i) {
        data[i].resize(20);
        std::iota(data[i].begin(), data[i].end(), i * 20);
    }

    for (auto _ : state) {
        auto result = data | ranges::views::join | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Join(benchmark::State &state) {
    std::vector<std::vector<int>> data(500);
    for (std::size_t i = 0; i < 500; ++i) {
        data[i].resize(20);
        std::iota(data[i].begin(), data[i].end(), i * 20);
    }

    for (auto _ : state) {
        auto result = data | genex::views::join | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Join);
BENCHMARK(BM_Genex_V2_Join);
