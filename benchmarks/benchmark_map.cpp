#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/map.hpp>

import genex.to_container;
import genex.views.map;
import std;


static void BM_StdRanges_Keys(benchmark::State &state) {
    auto data = std::map<int, int>();
    for (std::size_t i = 0; i < 500; ++i) {
        data[i] = i * 10;
    }

    for (auto _ : state) {
        auto result = data | ranges::views::keys | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Keys(benchmark::State &state) {
    auto data = std::map<int, int>();
    for (std::size_t i = 0; i < 500; ++i) {
        data[i] = i * 10;
    }

    for (auto _ : state) {
        auto result = data | genex::views::keys | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_Vals(benchmark::State &state) {
    auto data = std::map<int, int>();
    for (std::size_t i = 0; i < 500; ++i) {
        data[i] = i * 10;
    }

    for (auto _ : state) {
        auto result = data | ranges::views::values | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Vals(benchmark::State &state) {
    auto data = std::map<int, int>();
    for (std::size_t i = 0; i < 500; ++i) {
        data[i] = i * 10;
    }

    for (auto _ : state) {
        auto result = data | genex::views::vals | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Keys);
BENCHMARK(BM_Genex_V2_Keys);
BENCHMARK(BM_StdRanges_Vals);
BENCHMARK(BM_Genex_V2_Vals);
