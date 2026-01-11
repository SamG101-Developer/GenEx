#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/set_algorithm.hpp>

import genex.to_container;
import genex.views2.set_algorithms;
import std;


static void BM_StdRanges_SetSortedDifference(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    for (std::size_t i = 0; i < 500; ++i) {
        data1[i] = i * 2;
        data2[i] = i * 3;
    }

    for (auto _ : state) {
        auto result = data1 | ranges::views::set_difference(data2) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_SetSortedDifference(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    for (std::size_t i = 0; i < 500; ++i) {
        data1[i] = i * 2;
        data2[i] = i * 3;
    }

    for (auto _ : state) {
        auto result = data1 | genex::views::set_difference_sorted(data2) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_SetSortedUnion(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    for (std::size_t i = 0; i < 500; ++i) {
        data1[i] = i * 2;
        data2[i] = i * 3;
    }

    for (auto _ : state) {
        auto result = data1 | ranges::views::set_union(data2) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_SetSortedUnion(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    for (std::size_t i = 0; i < 500; ++i) {
        data1[i] = i * 2;
        data2[i] = i * 3;
    }

    for (auto _ : state) {
        auto result = data1 | genex::views::set_union_sorted(data2);
        auto v = genex::to<std::vector>()(result);
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_SetSortedIntersection(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    for (std::size_t i = 0; i < 500; ++i) {
        data1[i] = i * 2;
        data2[i] = i * 3;
    }

    for (auto _ : state) {
        auto result = data1 | ranges::views::set_intersection(data2) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_SetSortedIntersection(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    for (std::size_t i = 0; i < 500; ++i) {
        data1[i] = i * 2;
        data2[i] = i * 3;
    }

    for (auto _ : state) {
        auto result = data1 | genex::views::set_intersection_sorted(data2) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_SetSortedSymmetricDifference(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    for (std::size_t i = 0; i < 500; ++i) {
        data1[i] = i * 2;
        data2[i] = i * 3;
    }

    for (auto _ : state) {
        auto result = data1 | ranges::views::set_symmetric_difference(data2) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_SetSortedSymmetricDifference(benchmark::State &state) {
    std::vector<int> data1(500);
    std::vector<int> data2(500);
    for (std::size_t i = 0; i < 500; ++i) {
        data1[i] = i * 2;
        data2[i] = i * 3;
    }

    for (auto _ : state) {
        auto result = data1 | genex::views::set_symmetric_difference_sorted(data2) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_SetSortedDifference);
BENCHMARK(BM_Genex_V2_SetSortedDifference);
BENCHMARK(BM_StdRanges_SetSortedUnion);
BENCHMARK(BM_Genex_V2_SetSortedUnion);
BENCHMARK(BM_StdRanges_SetSortedIntersection);
BENCHMARK(BM_Genex_V2_SetSortedIntersection);
BENCHMARK(BM_StdRanges_SetSortedSymmetricDifference);
BENCHMARK(BM_Genex_V2_SetSortedSymmetricDifference);
