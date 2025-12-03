#include <coroutine>
#include <gtest/gtest.h>

import genex.actions.reverse;


TEST(GenexActionsReverse, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::reverse;
    const auto exp = std::vector{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    EXPECT_EQ(vec, exp);
}
