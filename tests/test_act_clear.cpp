#include <coroutine>
#include <gtest/gtest.h>

import genex.actions.clear;


TEST(GenexActionsClear, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5};
    vec |= genex::actions::clear();
    ASSERT_TRUE(vec.empty());
}