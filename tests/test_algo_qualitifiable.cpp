#include <coroutine>
#include <gtest/gtest.h>

import genex.algorithms.all_of;
import genex.algorithms.any_of;
import genex.algorithms.none_of;


TEST(GenexAlgosAllOf, BasicTrue) {
    auto vec = std::vector{2, 4, 6, 8, 10};
    auto is_even = [](const int n) { return n % 2 == 0; };
    EXPECT_TRUE(genex::all_of(vec, is_even));
}


TEST(GenexAlgosAllOf, BasicFalse) {
    auto vec = std::vector{2, 4, 6, 7, 10};
    auto is_even = [](const int n) { return n % 2 == 0; };
    EXPECT_FALSE(genex::all_of(vec, is_even));
}


TEST(GenexAlgosAnyOf, BasicTrue) {
    auto vec = std::vector{1, 3, 5, 6, 7};
    auto is_even = [](const int n) { return n % 2 == 0; };
    EXPECT_TRUE(genex::any_of(vec, is_even));
}


TEST(GenexAlgosAnyOf, BasicFalse) {
    auto vec = std::vector{1, 3, 5, 7, 9};
    auto is_even = [](const int n) { return n % 2 == 0; };
    EXPECT_FALSE(genex::any_of(vec, is_even));
}


TEST(GenexAlgosNoneOf, BasicTrue) {
    auto vec = std::vector{1, 3, 5, 7, 9};
    auto is_even = [](const int n) { return n % 2 == 0; };
    EXPECT_TRUE(genex::none_of(vec, is_even));
}


TEST(GenexAlgosNoneOf, BasicFalse) {
    auto vec = std::vector{1, 3, 5, 6, 7};
    auto is_even = [](const int n) { return n % 2 == 0; };
    EXPECT_FALSE(genex::none_of(vec, is_even));
}
