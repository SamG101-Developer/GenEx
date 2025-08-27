#include <gtest/gtest.h>

#include <genex/actions/push.hpp>


TEST(GenexActionsPushBack, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::push_back(10);
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    EXPECT_EQ(vec, exp);
}


TEST(GenexActionsPushFront, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::push_front(0);
    const auto exp = std::vector{0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec, exp);
}


TEST(GenexActionsPush, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::push(vec.begin() + 4, 0);
    const auto exp = std::vector{0, 1, 2, 3, 0, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec, exp);
}
