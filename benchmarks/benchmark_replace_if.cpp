#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/replace_if.hpp>

import genex.to_container;
import genex.views2.replace_if;
import std;


static void BM_StdRanges_ReplaceIf(benchmark::State &state) {
    std::vector<int> data(1000);
    for (size_t i = 0; i < data.size(); ++i) { data[i] = i % 2; }

    for (auto _ : state) {
        auto result = data | ranges::views::replace_if([](const int x) { return x < 500; }, 5) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_ReplaceIf(benchmark::State &state) {
    std::vector<int> data(1000);
    for (size_t i = 0; i < data.size(); ++i) { data[i] = i % 2; }

    for (auto _ : state) {
        auto result = data | genex::views2::replace_if([](const int x) { return x < 500; }, 5) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_ReplaceIf);
BENCHMARK(BM_Genex_V2_ReplaceIf);
