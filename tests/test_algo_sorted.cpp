#include <gtest/gtest.h>

#include <genex/algorithms/sorted.hpp>


TEST(GenexAlgosSorted, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5};
    const auto srt = genex::algorithms::sorted(vec);
    const auto exp = std::vector{1, 2, 3, 4, 5};
    EXPECT_EQ(srt, exp);
}