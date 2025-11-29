#include <coroutine>
#include <gtest/gtest.h>

import genex.actions.replace;
import genex.actions.replace_if;


TEST(GenexActionsReplace, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 3, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::replace(3, 10);
    const auto exp = std::vector{0, 1, 2, 10, 10, 10, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec, exp);
}


TEST(GenexActionsReplaceIf, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::replace_if([](const int x) { return x % 2 == 0; }, 10);
    const auto exp = std::vector{10, 1, 10, 3, 10, 5, 10, 7, 10, 9};
    EXPECT_EQ(vec, exp);
}
