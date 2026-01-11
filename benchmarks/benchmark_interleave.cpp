#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>

import genex.to_container;
import genex.views2.interleave;
import std;


// static void BM_StdRanges_Interleave(benchmark::State &state) {
//     std::vector<int> data1(1000);
//     std::iota(data1.begin(), data1.end(), 0);
//
//     std::vector<int> data2(1000);
//     std::iota(data2.begin(), data2.end(), 0);
//
//     for (auto _ : state) {
//         auto result = data1 | ranges::views::interleave(data2) | ranges::to<std::vector>();
//         benchmark::DoNotOptimize(result);
//     }
// }


static void BM_Genex_V2_Interleave(benchmark::State &state) {
    std::vector<int> data1(1000);
    std::iota(data1.begin(), data1.end(), 0);

    std::vector<int> data2(1000);
    std::iota(data2.begin(), data2.end(), 0);

    for (auto _ : state) {
        auto result = data1 | genex::views::interleave(data2) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


// BENCHMARK(BM_StdRanges_Interleave);
BENCHMARK(BM_Genex_V2_Interleave);
