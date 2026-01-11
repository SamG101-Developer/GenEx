#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/drop_last.hpp>

import genex.to_container;
import genex.views2.drop_last;
import std;


static void BM_StdRanges_DropLast_RandomAccess(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::drop_last(500) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_DropLast_RandomAccess(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views::drop_last(500) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_DropLast_Bidirectional(benchmark::State &state) {
    std::list<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::drop_last(500) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_DropLast_Bidirectional(benchmark::State &state) {
    std::list<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views::drop_last(500) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_DropLast_Forward(benchmark::State &state) {
    std::forward_list<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views::drop_last(500) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_DropLast_Forward(benchmark::State &state) {
    std::forward_list<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::drop_last(500) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_DropLast_RandomAccess);
BENCHMARK(BM_Genex_V2_DropLast_RandomAccess);
BENCHMARK(BM_StdRanges_DropLast_Bidirectional);
BENCHMARK(BM_Genex_V2_DropLast_Bidirectional);
BENCHMARK(BM_StdRanges_DropLast_Forward);
BENCHMARK(BM_Genex_V2_DropLast_Forward);
