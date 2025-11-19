#include <iostream>
#include <numeric>
#include <benchmark/benchmark.h>

#include <genex/to_container.hpp>
#include <range/v3/to_container.hpp>

#include <genex/views2/filter.hpp>
#include <range/v3/view/filter.hpp>

#include <genex/views2/transform.hpp>
#include <range/v3/view/transform.hpp>

#include <genex/views2/drop.hpp>
#include <range/v3/view/drop.hpp>

#include <genex/views2/drop_last.hpp>
#include <range/v3/view/drop_last.hpp>

#include <genex/views2/address_of.hpp>
#include <range/v3/view/addressof.hpp>

#include <genex/views2/cast_dynamic.hpp>


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


static void BM_StdRanges_Filter(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::filter([](const int x) { return x % 2 == 0; }) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Filter(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views2::filter([](const int x) { return x % 2 == 0; }) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_Transform(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::transform([](const int x) { return x * 2; }) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Transform(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views2::transform([](const int x) { return x * 2; }) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_Drop(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::drop(500) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_Drop(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views2::drop(500) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_DropLast(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::drop_last(500) | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_DropLast(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views2::drop_last(500) | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_AddressOf(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | ranges::views::addressof | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_AddressOf(benchmark::State &state) {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);

    for (auto _ : state) {
        auto result = data | genex::views2::address_of | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_CastDynamic(benchmark::State &state) {
    struct Base {
        virtual ~Base() = default;
    };
    struct Derived : Base {};

    std::vector<Base*> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(i % 2 == 0 ? static_cast<Base*>(new Derived()) : new Base());
    }

    for (auto _ : state) {
        auto result = data
            | ranges::views::transform([](Base *b) { return dynamic_cast<Derived*>(b); })
            | ranges::views::filter([](Base *b) { return b != nullptr; })
            | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }

    for (const auto ptr : data) {
        delete ptr;
    }
}


static void BM_Genex_V2_CastDynamic(benchmark::State &state) {
    struct Base {
        virtual ~Base() = default;
    };
    struct Derived : Base {};

    std::vector<Base*> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(i % 2 == 0 ? static_cast<Base*>(new Derived()) : new Base());
    }

    for (auto _ : state) {
        auto result = data
            | genex::views2::cast_dynamic<Derived*>()
            | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }

    for (const auto ptr : data) {
        delete ptr;
    }
}


// BENCHMARK(BM_StdRanges_ToVector);
// BENCHMARK(BM_Genex_VX_ToVector);
// BENCHMARK(BM_StdRanges_Filter);
// BENCHMARK(BM_Genex_V2_Filter);
// BENCHMARK(BM_StdRanges_Transform);
// BENCHMARK(BM_Genex_V2_Transform);
// BENCHMARK(BM_StdRanges_Drop);
// BENCHMARK(BM_Genex_V2_Drop);
// BENCHMARK(BM_StdRanges_DropLast);
// BENCHMARK(BM_Genex_V2_DropLast);
// BENCHMARK(BM_StdRanges_AddressOf);
// BENCHMARK(BM_Genex_V2_AddressOf);
BENCHMARK(BM_StdRanges_CastDynamic);
BENCHMARK(BM_Genex_V2_CastDynamic);

BENCHMARK_MAIN();
