#include <vector>
#include <numeric>

#include <benchmark/benchmark.h>

#include <genex/to_container.hpp>

#include <range/v3/to_container.hpp>


static void BM_StdRanges_ToVector(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_VX_ToVector(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}
