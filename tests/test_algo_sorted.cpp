#include <gtest/gtest.h>

#include <genex/algorithms/sorted.hpp>


TEST(GenexAlgosSorted, VecInput) {
    auto vec = std::vector{5, 3, 8, 1, 4, 7, 2, 6};

    const auto res = genex::algorithms::sorted(vec);
    const auto exp = std::vector{1, 2, 3, 4, 5, 6, 7, 8};
    EXPECT_EQ(res, exp);
}