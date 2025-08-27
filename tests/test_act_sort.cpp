#include <gtest/gtest.h>

#include <genex/actions/sort.hpp>


TEST(GenexActionsSort, VecInput) {
    auto vec = std::vector{7, 4, 5, 6, 3, 2, 1, 4, 9, 0, 8};
    vec |= genex::actions::sort;
    const auto exp = std::vector{0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(vec, exp);
}


TEST(GenexActionsSort, VecInputGreater) {
    auto vec = std::vector{7, 4, 5, 6, 3, 2, 1, 4, 9, 0, 8};
    vec |= genex::actions::sort(genex::operations::gt{});
    const auto exp = std::vector{9, 8, 7, 6, 5, 4, 4, 3, 2, 1, 0};
    EXPECT_EQ(vec, exp);
}
