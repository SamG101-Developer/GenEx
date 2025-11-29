#include <numeric>

#include <benchmark/benchmark.h>

#include <genex/to_container.hpp>
#include <genex/views2/chunk.hpp>

#include <range/v3/to_container.hpp>
#include <range/v3/view/chunk.hpp>


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

    const auto temp_1 = genex::views2::chunk(chunk_size)(data);
    const auto temp_2 = genex::to<std::vector>()(temp_1);

    for (auto _ : state) {
        auto result = data
            | genex::views2::chunk(chunk_size)
            | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_Chunk);
BENCHMARK(BM_Genex_V2_Chunk);
