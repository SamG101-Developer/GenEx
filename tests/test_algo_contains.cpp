#include <gtest/gtest.h>

#include <genex/algorithms/contains.hpp>


TEST(GenexAlgosContains, FindElement) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const auto found = genex::algorithms::contains(vec, 2);
    EXPECT_TRUE(found);
}


TEST(GenexAlgosContains, FindElementWithProjection) {
    auto vec = std::vector{0, 2, 4, 6, 8};
    const auto found = genex::algorithms::contains(vec, 1, [](auto &&x) { return x / 2; });
    EXPECT_TRUE(found);
}
