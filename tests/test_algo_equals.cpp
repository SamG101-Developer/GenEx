#include <gtest/gtest.h>

#include <genex/algorithms/equals.hpp>


TEST(GenexAlgoEquals, VecInput) {
    std::vector<int> vec1 = {1, 2, 3, 4, 5};
    std::vector<int> vec2 = {1, 2, 3, 4, 5};
    const auto result1 = genex::algorithms::equals(vec1, vec2);
    EXPECT_TRUE(result1);
}


TEST(GenexAlgoEquals, VecInputNotEqual) {
    std::vector<int> vec1 = {1, 2, 3, 4, 5};
    std::vector<int> vec2 = {1, 2, 3, 4, 6};
    const auto result1 = genex::algorithms::equals(vec1, vec2);
    EXPECT_FALSE(result1);
}


TEST(GenexAlgoEquals, VecInputWithProjection) {
    struct Point {
        int x;
        int y;
    };

    std::vector<Point> vec1 = {{1, 2}, {3, 7}, {5, 6}};
    std::vector<Point> vec2 = {{1, 2}, {3, 4}, {5, 6}};
    const auto result1 = genex::algorithms::equals(vec1, vec2, {}, &Point::x, &Point::x);
    EXPECT_TRUE(result1);

    const auto result2 = genex::algorithms::equals(vec1, vec2, {}, &Point::y, &Point::y);
    EXPECT_FALSE(result2);
}
