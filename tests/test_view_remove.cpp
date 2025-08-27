#include <gtest/gtest.h>

#include <genex/views/remove.hpp>
#include <genex/views/to.hpp>


TEST(GenexViewsRemove, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    genex::views::remove(vec, 5);

    const auto rng = vec
        | genex::views::remove(5)
        | genex::views::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 6, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsRemove, VecInputMultiple) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::remove(5)
        | genex::views::remove(6)
        | genex::views::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsRemoveIf, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::remove_if([](auto x) { return x > 5; })
        | genex::views::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 5};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsRemoveIf, VecInputMultiple) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::remove_if([](auto x) { return x > 5; })
        | genex::views::remove_if([](auto x) { return x < 3; })
        | genex::views::to<std::vector>();
    const auto exp = std::vector{3, 4, 5};
    EXPECT_EQ(rng, exp);
}
