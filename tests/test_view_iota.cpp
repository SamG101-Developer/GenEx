#include <gtest/gtest.h>

#include <genex/views/iota.hpp>
#include <genex/views/to.hpp>


TEST(GenexViewsIota, StandardRange) {
    const auto it = genex::views::iota(10, 16, 2)
        | genex::views::to<std::vector>();
    const auto exp = std::vector<std::size_t>{10, 12, 14};
    EXPECT_EQ(it, exp);
}


TEST(GenexViewsIota, StandardRangeStepJustOver) {
    const auto it = genex::views::iota(10, 17, 2)
        | genex::views::to<std::vector>();
    const auto exp = std::vector<std::size_t>{10, 12, 14, 16};
    EXPECT_EQ(it, exp);
}


TEST(GenexViewsIota, NoLo) {
    const auto it = genex::views::iota(8)
        | genex::views::to<std::vector>();
    const auto exp = std::vector<std::size_t>{0, 1, 2, 3, 4, 5, 6, 7};
    EXPECT_EQ(it, exp);
}
