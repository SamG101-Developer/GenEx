#include <gtest/gtest.h>

#include <genex/views/intersperse.hpp>
#include <genex/views/to.hpp>


TEST(GenexViewsIntersperse, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto rng = vec
        | genex::views::intersperse(0)
        | genex::views::to<std::vector>();
    const auto exp = std::vector{1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6};
    EXPECT_EQ(rng, exp);
}
