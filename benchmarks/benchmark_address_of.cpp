#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/addressof.hpp>

import genex.to_container;
import genex.views2.address_of;
import std;


static void BM_StdRanges_AddressOf_Contiguous(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::addressof | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_AddressOf_Contiguous(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views2::address_of | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


// static void BM_StdRanges_AddressOf_Forward(benchmark::State &state) {
//     std::deque<int> data(1000);
//     std::iota(data.begin(), data.end(), 0);
//
//     for (auto _ : state) {
//         auto result = data | ranges::views::addressof | ranges::to<std::vector>();
//         benchmark::DoNotOptimize(result);
//     }
// }


// static void BM_Genex_V2_AddressOf_Forward(benchmark::State &state) {
//     std::deque<int> data(1000);
//     std::iota(data.begin(), data.end(), 0);
//
//     for (auto _ : state) {
//         auto result = data | genex::views2::address_of | genex::to<std::vector>();
//         benchmark::DoNotOptimize(result);
//     }
// }


BENCHMARK(BM_StdRanges_AddressOf_Contiguous);
BENCHMARK(BM_Genex_V2_AddressOf_Contiguous);
// BENCHMARK(BM_StdRanges_AddressOf_Forward);
// BENCHMARK(BM_Genex_V2_AddressOf_Forward);
