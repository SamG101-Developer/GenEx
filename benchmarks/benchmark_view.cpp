#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/any_view.hpp>

import genex.to_container;
import genex.views2.view;
import std;


static void BM_StdRanges_TupleNth(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = ranges::any_view<int>(data) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_TupleNth(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views2::view | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_TupleNth);
BENCHMARK(BM_Genex_V2_TupleNth);
