#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/indirect.hpp>

import genex.to_container;
import genex.views2.indirect;
import std;


static void BM_StdRanges_Indirect(benchmark::State &state) {
    std::vector<std::unique_ptr<int>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(std::make_unique<int>(i));
    }

    for (auto _ : state) {
        auto result = data | ranges::views::indirect | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Indirect(benchmark::State &state) {
    std::vector<std::unique_ptr<int>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(std::make_unique<int>(i));
    }

    for (auto _ : state) {
        auto result = data | genex::views::indirect | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Indirect);
BENCHMARK(BM_Genex_V2_Indirect);
