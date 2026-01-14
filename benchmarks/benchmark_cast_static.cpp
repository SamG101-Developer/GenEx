#include <benchmark/benchmark.h>
#include <coroutine>

#include <range/v3/to_container.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

import genex.to_container;
import genex.views.cast_static;


static void BM_StdRanges_CastStatic(benchmark::State &state) {
    std::vector<int> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(i);
    }

    for (auto _ : state) {
        auto result = data
            | ranges::views::transform([](auto b) { return static_cast<double>(b); })
            | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_CastStatic(benchmark::State &state) {
    std::vector<int> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(i);
    }

    for (auto _ : state) {
        auto result = data
            | genex::views::cast_static<double>()
            | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_CastStatic);
BENCHMARK(BM_Genex_V2_CastStatic);
