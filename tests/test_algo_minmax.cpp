#include <gtest/gtest.h>

#include <genex/algorithms/min_element.hpp>
#include <genex/algorithms/max_element.hpp>


TEST(GenexAlgosMinMax, MinVec) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::algorithms::min_element(vec);
    EXPECT_EQ(res, 1);
}


TEST(GenexAlgosMinMax, MaxVec) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::algorithms::max_element(vec);
    EXPECT_EQ(res, 8);
}
