#include <gtest/gtest.h>

#include <genex/algorithms/max_element.hpp>
#include <genex/algorithms/min_element.hpp>


TEST(GenexAlgosMinMax, MinVec) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::algorithms::min_element(vec);
    EXPECT_EQ(res, 1);
}


TEST(GenexAlgosMinMax, MinVec1Elem) {
    auto vec = std::vector{42};

    const auto res = genex::algorithms::min_element(vec);
    EXPECT_EQ(res, 42);
}


// TEST(GenexAlgosMinMax, MinVecEmptyWithDef) {
//     auto vec = std::vector<int>{};
//
//     const auto res = genex::algorithms::min_element(vec, {}, {}, 42);
//     EXPECT_EQ(res, 42);
// }


TEST(GenexAlgosMinMax, MaxVec) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::algorithms::max_element(vec);
    EXPECT_EQ(res, 8);
}


TEST(GenexAlgosMinMax, MaxVec1Elem) {
    auto vec = std::vector{42};

    const auto res = genex::algorithms::max_element(vec);
    EXPECT_EQ(res, 42);
}


// TEST(GenexAlgosMinMax, MaxVecEmptyWithDef) {
//     auto vec = std::vector<int>{};
//
//     const auto res = genex::algorithms::max_element(vec, {}, {}, 42);
//     EXPECT_EQ(res, 42);
// }
