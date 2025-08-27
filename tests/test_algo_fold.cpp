#include <gtest/gtest.h>

#include <genex/algorithms/fold.hpp>


TEST(GenexAlgosFoldLeft, FoldLeftWithInit) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto res = genex::algorithms::fold_left(vec, 0, [](const auto a, const auto b) { return a - b; });
    EXPECT_EQ(res, -21); // ((((0 - 1) - 2) - 3) - 4) - 5) - 6)
}


TEST(GenexAlgosFoldLeftFirst, FoldLeftFirst) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto res = genex::algorithms::fold_left_first(vec, [](const auto a, const auto b) { return a - b; });
    EXPECT_EQ(res, -19); // ((((1 - 2) - 3) - 4) - 5) - 6)
}


TEST(GenexAlgosFoldRight, FoldRightWithInit) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto res = genex::algorithms::fold_right(vec, 0, [](const auto a, const auto b) { return a - b; });
    EXPECT_EQ(res, -3); // 1 - (2 - (3 - (4 - (5 - (6 - 0)))))
}


TEST(GenexAlgosFoldRightFirst, FoldRightFirst) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto res = genex::algorithms::fold_right_first(vec, [](const auto a, const auto b) { return a - b; });
    EXPECT_EQ(res, -3); // 1 - (2 - (3 - (4 - (5 - 6))))
}


TEST(GenexAlgosFoldLeft, FoldLeftWithInit_Closure) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto fold_minus = genex::algorithms::fold_left(0, [](const auto a, const auto b) { return a - b; });
    const auto res = fold_minus(vec);
    EXPECT_EQ(res, -21); // ((((0 - 1) - 2) - 3) - 4) - 5) - 6)
}


TEST(GenexAlgosFoldLeftFirst, FoldLeftFirst_Closure) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto fold_minus = genex::algorithms::fold_left_first([](const auto a, const auto b) { return a - b; });
    const auto res = fold_minus(vec);
    EXPECT_EQ(res, -19); // ((((1 - 2) - 3) - 4) - 5) - 6)
}


TEST(GenexAlgosFoldRight, FoldRightWithInit_Closure) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto fold_minus = genex::algorithms::fold_right(0, [](const auto a, const auto b) { return a - b; });
    const auto res = fold_minus(vec);
    EXPECT_EQ(res, -3); // 1 - (2 - (3 - (4 - (5 - (6 - 0)))))
}


TEST(GenexAlgosFoldRightFirst, FoldRightFirst_Closure) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto fold_minus = genex::algorithms::fold_right_first([](const auto a, const auto b) { return a - b; });
    const auto res = fold_minus(vec);
    EXPECT_EQ(res, -3); // 1 - (2 - (3 - (4 - (5 - 6))))
}
