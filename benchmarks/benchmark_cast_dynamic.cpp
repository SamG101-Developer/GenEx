#include <benchmark/benchmark.h>

#include <range/v3/to_container.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

import genex.to_container;
import genex.views2.cast_dynamic;
import std;


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
            | genex::views::cast_dynamic<Derived*>()
            | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }

    for (const auto ptr : data) {
        delete ptr;
    }
}


BENCHMARK(BM_StdRanges_CastDynamic);
BENCHMARK(BM_Genex_V2_CastDynamic);
