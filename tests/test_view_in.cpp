#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views.in;


TEST(GenexViewsIn, BasicIn) {
    const auto vec = std::vector{10, 20, 30, 40, 50};
    const auto result = vec
        | genex::views::in(std::vector{20, 40, 60, 80})
        | genex::to<std::vector>();

    const auto expected = std::vector{20, 40};
    EXPECT_EQ(result, expected);
}


TEST(GenexViewsIn, NoMatches) {
    const auto vec = std::vector{1, 2, 3, 4, 5};
    const auto result = vec
        | genex::views::in(std::vector{6, 7, 8, 9})
        | genex::to<std::vector>();

    const auto expected = std::vector<int>{};
    EXPECT_EQ(result, expected);
}


TEST(GenexViewsIn, AllMatches) {
    const auto vec = std::vector{1, 2, 3};
    const auto result = vec
        | genex::views::in(std::vector{1, 2, 3, 4, 5})
        | genex::to<std::vector>();

    const auto expected = std::vector{1, 2, 3};
    EXPECT_EQ(result, expected);
}


TEST(GenexViewsNotIn, BasicNotIn) {
    const auto vec = std::vector{10, 20, 30, 40, 50};
    const auto result = vec
        | genex::views::not_in(std::vector{20, 40, 60, 80})
        | genex::to<std::vector>();

    const auto expected = std::vector{10, 30, 50};
    EXPECT_EQ(result, expected);
}


TEST(GenexViewsNotIn, NoMatches) {
    const auto vec = std::vector{1, 2, 3, 4, 5};
    const auto result = vec
        | genex::views::not_in(std::vector{6, 7, 8, 9})
        | genex::to<std::vector>();

    const auto expected = std::vector{1, 2, 3, 4, 5};
    EXPECT_EQ(result, expected);
}


TEST(GenexViewsNotIn, AllMatches) {
    const auto vec = std::vector{1, 2, 3};
    const auto result = vec
        | genex::views::not_in(std::vector{1, 2, 3, 4, 5})
        | genex::to<std::vector>();

    const auto expected = std::vector<int>{};
    EXPECT_EQ(result, expected);
}
