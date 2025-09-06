#include <gtest/gtest.h>

#include <genex/generator.hpp>


TEST(GenexGenerator, BasicTest) {
    auto generator_closure = []() -> genex::generator<int> {
        for (auto x = 0; x < 10; ++x) {
            co_yield x;
        }
    };
}
