#include <gtest/gtest.h>

#include <genex/actions/pop.hpp>


TEST(GenexActionsPopBack, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::pop_back;
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8};
    EXPECT_EQ(vec, exp);
}


TEST(GenexActionsPopFront, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::pop_front;
    const auto exp = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec, exp);
}


TEST(GenexActionsPop, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::pop(vec.begin() + 4);
    const auto exp = std::vector{0, 1, 2, 3, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec, exp);
}
