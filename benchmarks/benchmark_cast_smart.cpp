#include <deque>
#include <numeric>

#include <benchmark/benchmark.h>

#include <genex/to_container.hpp>
#include <genex/views2/cast_smart.hpp>

#include <range/v3/to_container.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>


static void BM_StdRanges_CastShared(benchmark::State &state) {
    struct Base {
        virtual ~Base() = default;
    };
    struct Derived : Base {};

    std::vector<std::shared_ptr<Base>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(i % 2 == 0 ? std::make_shared<Derived>() : std::make_shared<Base>());
    }

    for (auto _ : state) {
        auto result = data
            | ranges::views::transform([](std::shared_ptr<Base> const &b) { return std::dynamic_pointer_cast<Derived>(b); })
            | ranges::views::filter([](std::shared_ptr<Derived> const &b) { return b != nullptr; })
            | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_CastShared(benchmark::State &state) {
    struct Base {
        virtual ~Base() = default;
    };
    struct Derived : Base {};

    std::vector<std::shared_ptr<Base>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(i % 2 == 0 ? std::make_shared<Derived>() : std::make_shared<Base>());
    }

    for (auto _ : state) {
        auto result = data
            | genex::views2::cast_smart<Derived>()
            | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_StdRanges_CastUnique(benchmark::State &state) {
    struct Base {
        virtual ~Base() = default;
    };
    struct Derived : Base {};

    std::vector<std::unique_ptr<Base>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(i % 2 == 0 ? std::make_unique<Derived>() : std::make_unique<Base>());
    }

    for (auto _ : state) {
        auto result = data
            | ranges::views::transform([](std::unique_ptr<Base> &b) { return std::unique_ptr<Derived>(dynamic_cast<Derived*>(b.release())); })
            | ranges::views::filter([](std::unique_ptr<Derived> const &b) { return b != nullptr; })
            | ranges::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


static void BM_Genex_V2_CastUnique(benchmark::State &state) {
    struct Base {
        virtual ~Base() = default;
    };
    struct Derived : Base {};

    std::vector<std::unique_ptr<Base>> data;
    for (auto i = 0; i < 1000; ++i) {
        data.push_back(i % 2 == 0 ? std::make_unique<Derived>() : std::make_unique<Base>());
    }

    for (auto _ : state) {
        auto result = data
            | genex::views2::cast_smart<Derived>()
            | genex::to<std::vector>();
        benchmark::DoNotOptimize(result);
    }
}


BENCHMARK(BM_StdRanges_CastShared);
BENCHMARK(BM_Genex_V2_CastShared);
BENCHMARK(BM_StdRanges_CastUnique);
BENCHMARK(BM_Genex_V2_CastUnique);
