#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/transform.hpp>

import genex.to_container;
import genex.views2.tuple_nth;
import std;


static void BM_StdRanges_TupleNth(benchmark::State &state) {
    std::vector<std::tuple<int, double, char>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.emplace_back(i, i + 0.5, 'a' + (i % 26));
    }

    for (auto _ : state) {
        auto result = data | ranges::views::transform([](auto &&tpl) { return std::get<1>(tpl); }) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_TupleNth(benchmark::State &state) {
    std::vector<std::tuple<int, double, char>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.emplace_back(i, i + 0.5, 'a' + (i % 26));
    }

    for (auto _ : state) {
        auto result = data | genex::views2::tuple_nth<1> | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_TupleNth);
BENCHMARK(BM_Genex_V2_TupleNth);
