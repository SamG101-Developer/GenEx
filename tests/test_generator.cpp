#include <coroutine>
#include <gtest/gtest.h>

import genex.generator;
import std;


TEST(GenexGenerator, BasicTest) {
    auto generator_closure = []() -> genex::generator<int> {
        for (auto x = 0; x < 10; ++x) {
            co_yield x;
        }
    };
    auto gen = generator_closure();
    auto expected = 0;
    for (auto value : gen) {
        EXPECT_EQ(value, expected);
        ++expected;
    }
}
