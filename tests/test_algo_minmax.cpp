#include <gtest/gtest.h>

#include <genex/algorithms/minmax.hpp>


TEST(GenexAlgosMinMax, MinVec) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::algorithms::min(vec);
    EXPECT_EQ(res, 1);
}


TEST(GenexAlgosMinMax, MaxVec) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::algorithms::max(vec);
    EXPECT_EQ(res, 8);
}
