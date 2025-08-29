#include <gtest/gtest.h>

#include <genex/algorithms/fold.hpp>


TEST(GenexAlgosFoldLeft, FoldLeftWithInit) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto res = genex::algorithms::fold_left(vec, 0, [](const auto a, const auto b) { return a - b; });
    EXPECT_EQ(res, -21); // ((((0 - 1) - 2) - 3) - 4) - 5) - 6)
}


TEST(GenexAlgosFoldLeft, FoldLeftWithInitDiffTypes) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto res = genex::algorithms::fold_left(vec, std::string(), [](std::string a, const auto b) { return a + std::to_string(b); });
    EXPECT_EQ(res, "123456");
}


TEST(GenexAlgosFoldLeft, FoldLeftWithInit_Closure) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto fold_minus = genex::algorithms::fold_left(0, [](const auto a, const auto b) { return a - b; });
    const auto res = fold_minus(vec);
    EXPECT_EQ(res, -21); // ((((0 - 1) - 2) - 3) - 4) - 5) - 6)
}


TEST(GenexAlgosFoldLeft, FoldLeftWithInitDiffTypes_Closure) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto fold_concat = genex::algorithms::fold_left(std::string(), [](std::string a, const auto b) { return a + std::to_string(b); });
    const auto res = fold_concat(vec);
    EXPECT_EQ(res, "123456");
}


TEST(GenexAlgosFoldRight, FoldRightWithInit) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto res = genex::algorithms::fold_right(vec, 0, [](const auto a, const auto b) { return a - b; });
    EXPECT_EQ(res, -3); // 1 - (2 - (3 - (4 - (5 - (6 - 0)))))
}


TEST(GenexAlgosFoldRight, FoldRightWithInitDiffTypes) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto res = genex::algorithms::fold_right(vec, std::string(), [](const auto a, const auto b) { return std::to_string(a) + b; });
    EXPECT_EQ(res, "123456");
}


TEST(GenexAlgosFoldRight, FoldRightWithInit_Closure) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto fold_minus = genex::algorithms::fold_right(0, [](const auto a, const auto b) { return a - b; });
    const auto res = fold_minus(vec);
    EXPECT_EQ(res, -3); // 1 - (2 - (3 - (4 - (5 - (6 - 0)))))
}


TEST(GenexAlgosFoldRight, FoldRightWithInit_ClosureDiffTypes) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto fold_concat = genex::algorithms::fold_right(std::string(), [](const auto a, const auto b) { return std::to_string(a) + b; });
    const auto res = fold_concat(vec);
    EXPECT_EQ(res, "123456");
}


TEST(GenexAlgosFoldLeftFirst, FoldLeftFirst) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto res = genex::algorithms::fold_left_first(vec, [](const auto a, const auto b) { return a - b; });
    EXPECT_EQ(res, -19); // ((((1 - 2) - 3) - 4) - 5) - 6)
}


TEST(GenexAlgosFoldRightFirst, FoldRightFirst) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto res = genex::algorithms::fold_right_first(vec, [](const auto a, const auto b) { return a - b; });
    EXPECT_EQ(res, -3); // 1 - (2 - (3 - (4 - (5 - 6))))
}


TEST(GenexAlgosFoldLeftFirst, FoldLeftFirst_Closure) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto fold_minus = genex::algorithms::fold_left_first([](const auto a, const auto b) { return a - b; });
    const auto res = fold_minus(vec);
    EXPECT_EQ(res, -19); // ((((1 - 2) - 3) - 4) - 5) - 6)
}


TEST(GenexAlgosFoldRightFirst, FoldRightFirst_Closure) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto fold_minus = genex::algorithms::fold_right_first([](const auto a, const auto b) { return a - b; });
    const auto res = fold_minus(vec);
    EXPECT_EQ(res, -3); // 1 - (2 - (3 - (4 - (5 - 6))))
}
