#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/take_while.hpp>

import genex.to_container;
import genex.views2.take_while;
import std;


static void BM_StdRanges_TakeWhile(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);
    auto func = [](const int x) { return x < 500; };

    for (auto _ : state) {
        auto result = data | ranges::views::take_while(func) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_TakeWhile(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);
    auto func = [](const int x) { return x < 500; };

    for (auto _ : state) {
        auto result = data | genex::views::take_while(func) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_TakeWhile);
BENCHMARK(BM_Genex_V2_TakeWhile);
