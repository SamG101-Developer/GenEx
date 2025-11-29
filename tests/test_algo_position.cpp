#include <coroutine>
#include <gtest/gtest.h>

import genex.algorithms.position;
import genex.algorithms.position_last;


TEST(GenexAlgosPosition, ElementExists) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::position(vec, [](const int v) { return v == 1; });
    EXPECT_EQ(res, 3);
}


TEST(GenexAlgosPosition, ElementNotExists) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::position(vec, [](const int v) { return v == 10; });
    EXPECT_EQ(res, -1);
}


TEST(GenexAlgosPosition, ElementNotExistsDiffDefault) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::position(vec, [](const int v) { return v == 10; }, {}, 100);
    EXPECT_EQ(res, 100);
}


TEST(GenexAlgosPositionLast, ElementExists) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6, 1};

    const auto res = genex::position_last(vec, [](const int v) { return v == 1; });
    EXPECT_EQ(res, 8);
}


TEST(GenexAlgosPositionLast, ElementNotExists) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::position_last(vec, [](const int v) { return v == 10; });
    EXPECT_EQ(res, -1);
}


TEST(GenexAlgosPositionLast, ElementNotExistsDiffDefault) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::position_last(vec, [](const int v) { return v == 10; }, {}, 100);
    EXPECT_EQ(res, 100);
}
