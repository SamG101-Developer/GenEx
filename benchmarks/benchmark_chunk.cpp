#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk.hpp>

import genex.to_container;
import genex.views2.chunk;
import std;


static void BM_StdRanges_Chunk(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    constexpr auto chunk_size = 10;

    for (auto _ : state) {
        auto result = data
            | ranges::views::chunk(chunk_size)
            | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Chunk(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    constexpr auto chunk_size = 10;

    for (auto _ : state) {
        auto result = data
            | genex::views2::chunk(chunk_size)
            | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Chunk);
BENCHMARK(BM_Genex_V2_Chunk);
