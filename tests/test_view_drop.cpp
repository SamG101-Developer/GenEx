#include <gtest/gtest.h>

#include <genex/views/drop.hpp>
#include <genex/views/drop_last.hpp>
#include <genex/views/drop_while.hpp>
#include <genex/views/to.hpp>


TEST(GenexViewsDrop, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::drop(4)
        | genex::views::to<std::vector>();
    const auto exp = std::vector{4, 5, 6, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsDrop, VecInputStacked) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::drop(4)
        | genex::views::drop(2)
        | genex::views::to<std::vector>();
    const auto exp = std::vector{6, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsDropLast, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::drop_last(4)
        | genex::views::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 5};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsDropWhile, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::drop_while([](auto x) { return x < 5; })
        | genex::views::to<std::vector>();
    const auto exp = std::vector{5, 6, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}
