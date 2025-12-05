#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/iota.hpp>

import genex.to_container;
import genex.views2.iota;
import std;


static void BM_StdRanges_Iota_StartEnd(benchmark::State &state) {
    for (auto _ : state) {
        auto result = ranges::views::iota(0, 1000) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Iota_StartEnd(benchmark::State &state) {
    for (auto _ : state) {
        auto result = genex::views2::iota(0, 1000) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Iota_StartEnd);
BENCHMARK(BM_Genex_V2_Iota_StartEnd);
