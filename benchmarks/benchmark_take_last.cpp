#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/take_last.hpp>

import genex.to_container;
import genex.views2.take_last;
import std;


static void BM_StdRanges_TakeLast_RandomAccess(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::take_last(500) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_TakeLast_RandomAccess(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views2::take_last(500) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


// static void BM_Genex_V2_TakeLast_Forward(benchmark::State &state) {
//     std::forward_list<int> data(1000);
//     std::iota(data.begin(), data.end(), 0);
//
//     for (auto _ : state) {
//         auto result = data | genex::views2::take_last(500) | genex::to<std::vector>();
//         benchmark::DoNotOptimize(result);
//     }
// }
//
//
// static void BM_StdRanges_TakeLast_Forward(benchmark::State &state) {
//     std::forward_list<int> data(1000);
//     std::iota(data.begin(), data.end(), 0);
//
//     for (auto _ : state) {
//         auto result = data | ranges::views::take_last(500) | ranges::to<std::vector>();
//         benchmark::DoNotOptimize(result);
//     }
// }


BENCHMARK(BM_StdRanges_TakeLast_RandomAccess);
BENCHMARK(BM_Genex_V2_TakeLast_RandomAccess);
// BENCHMARK(BM_StdRanges_TakeLast_Forward);
// BENCHMARK(BM_Genex_V2_TakeLast_Forward);
