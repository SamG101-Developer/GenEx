#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/transform.hpp>

import genex.to_container;
import genex.views2.ptr;
import std;


static void BM_StdRanges_Ptr_UniquePTr(benchmark::State &state) {
    std::vector<std::unique_ptr<int>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(std::make_unique<int>(i));
    }

    for (auto _ : state) {
        auto result = data | ranges::views::transform([](auto &&ptr) { return ptr.get(); }) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Ptr_UniquePtr(benchmark::State &state) {
    std::vector<std::unique_ptr<int>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(std::make_unique<int>(i));
    }

    for (auto _ : state) {
        auto result = data | genex::views2::ptr | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_Ptr_SharedPtr(benchmark::State &state) {
    std::vector<std::shared_ptr<int>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(std::make_shared<int>(i));
    }

    for (auto _ : state) {
        auto result = data | ranges::views::transform([](auto &&ptr) { return ptr.get(); }) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Ptr_SharedPtr(benchmark::State &state) {
    std::vector<std::shared_ptr<int>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(std::make_shared<int>(i));
    }

    for (auto _ : state) {
        auto result = data | genex::views2::ptr | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Ptr_UniquePTr);
BENCHMARK(BM_Genex_V2_Ptr_UniquePtr);
BENCHMARK(BM_StdRanges_Ptr_SharedPtr);
BENCHMARK(BM_Genex_V2_Ptr_SharedPtr);
