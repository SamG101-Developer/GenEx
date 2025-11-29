#include <coroutine>
#include <gtest/gtest.h>

import genex.to_container;
import genex.views.replace;
import genex.views.replace_if;


TEST(GenexViewsReplace, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    genex::views::replace(5, 9)(vec);

    const auto rng = vec
        | genex::views::replace(5, 9)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 9, 6, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsReplace, VecInputMultiple) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::replace(5, 9)
        | genex::views::replace(6, 9)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 9, 9, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsReplaceIf, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::replace_if([](auto x) { return x > 5; }, 9)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 9, 9, 9, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsReplaceIf, VecInputMultiple) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::replace_if([](auto x) { return x > 5; }, 9)
        | genex::views::replace_if([](auto x) { return x < 3; }, 9)
        | genex::to<std::vector>();
    const auto exp = std::vector{9, 9, 9, 3, 4, 5, 9, 9, 9, 9};
    EXPECT_EQ(rng, exp);
}
