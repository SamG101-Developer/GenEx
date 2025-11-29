#include <coroutine>
#include <gtest/gtest.h>

import genex.actions.concat;


TEST(GenexActionsConcat, VecTwo) {
    auto vec1 = std::vector{0, 1, 2, 3, 4};
    auto vec2 = std::vector{5, 6, 7, 8, 9};
    vec1 |= genex::actions::concat(vec2);
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec1, exp);
}


TEST(GenexActionsConcat, VecThree) {
    auto vec1 = std::vector{0, 1, 2};
    auto vec2 = std::vector{3, 4, 5};
    auto vec3 = std::vector{6, 7, 8, 9};
    vec1 |= genex::actions::concat(vec2);
    vec1 |= genex::actions::concat(vec3);
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec1, exp);
}
