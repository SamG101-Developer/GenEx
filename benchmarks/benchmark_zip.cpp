#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/zip.hpp>

import genex.to_container;
import genex.views2.zip;
import std;


static void BM_StdRanges_Zip(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    std::iota(data1.begin(), data1.end(), 0);
    std::iota(data2.begin(), data2.end(), 500);

    for (auto _ : state) {
        auto result = ranges::views::zip(data1, data2) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Zip(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    std::iota(data1.begin(), data1.end(), 0);
    std::iota(data2.begin(), data2.end(), 500);

    for (auto _ : state) {
        auto result = genex::views::zip(data1, data2) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Zip);
BENCHMARK(BM_Genex_V2_Zip);
