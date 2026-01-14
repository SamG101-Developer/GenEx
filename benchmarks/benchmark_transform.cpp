#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/transform.hpp>

import genex.to_container;
import genex.views.transform;
import std;


static void BM_StdRanges_Transform(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::transform([](const int x) { return x * 2; }) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Transform(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views::transform([](const int x) { return x * 2; }) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Transform);
BENCHMARK(BM_Genex_V2_Transform);
