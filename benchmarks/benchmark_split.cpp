#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/split.hpp>

import genex.to_container;
import genex.views2.split;
import std;


static void BM_StdRanges_Split(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);
    for (std::size_t i = 0; i < data.size() / 100; i += 10) {
        data[i] = 1;
    }

    for (auto _ : state) {
        auto result = data | ranges::views::split(1) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Split(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);
    for (std::size_t i = 0; i < data.size() / 100; i += 10) {
        data[i] = 1;
    }

    for (auto _ : state) {
        auto result = data | genex::views2::split(1) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Split);
BENCHMARK(BM_Genex_V2_Split);
