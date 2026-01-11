#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/slice.hpp>

import genex.to_container;
import genex.views2.slice;
import std;


static void BM_StdRanges_Slice_RandomAccess(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::slice(200, 800) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Slice_RandomAccess(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views::slice(200, 800) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_Slice_Forward(benchmark::State &state) {
    std::list<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::slice(200, 800) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Slice_Forward(benchmark::State &state) {
    std::list<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views::slice(200, 800) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Slice_RandomAccess);
BENCHMARK(BM_Genex_V2_Slice_RandomAccess);
BENCHMARK(BM_StdRanges_Slice_Forward);
BENCHMARK(BM_Genex_V2_Slice_Forward);
