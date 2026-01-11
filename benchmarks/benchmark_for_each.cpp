#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/algorithm/for_each.hpp>

import genex.to_container;
import genex.views2.for_each;
import std;


static void BM_StdRanges_ForEach(benchmark::State& state) {
    std::vector<int> vec(1000);
    std::iota(vec.begin(), vec.end(), 0);

    for (auto _ : state) {
        ranges::for_each(vec, [&](auto &x) { return x += 100; });
        benchmark::DoNotOptimize(vec);
    }
}


static void BM_Genex_V2_ForEach(benchmark::State& state) {
    std::vector<int> vec(1000);
    std::iota(vec.begin(), vec.end(), 0);

    for (auto _ : state) {
        vec | genex::views::for_each([&](auto &x) { return x += 100; });
        benchmark::DoNotOptimize(vec);
    }
}


BENCHMARK(BM_StdRanges_ForEach);
BENCHMARK(BM_Genex_V2_ForEach);
