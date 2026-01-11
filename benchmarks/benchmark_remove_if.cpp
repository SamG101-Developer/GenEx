#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/remove_if.hpp>

import genex.to_container;
import genex.views2.remove_if;
import std;


static void BM_StdRanges_RemoveIf(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::remove_if([](const int x) { return x % 2 == 0; }) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_RemoveIf(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views::remove_if([](const int x) { return x % 2 == 0; }) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_RemoveIf);
BENCHMARK(BM_Genex_V2_RemoveIf);
