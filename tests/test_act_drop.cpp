#include <gtest/gtest.h>

#include <genex/actions/drop.hpp>


TEST(GenexActionsDrop, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    vec |= genex::actions::drop(4);
    const auto exp = std::vector{4, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec, exp);
}
