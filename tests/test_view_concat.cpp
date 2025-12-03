#include <coroutine>
#include <gtest/gtest.h>

import genex.to_container;
import genex.views.concat;


TEST(GenexViewsConcat, VecInput) {
    auto vec1 = std::vector{1, 2, 3, 4, 5, 6};
    auto vec2 = std::vector{7, 8, 9};

    const auto rng = vec1
        | genex::views::concat(vec2)
        | genex::to<std::vector>();
    const auto exp = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsConcat, VecInputMulti) {
    auto vec1 = std::vector{1, 2, 3, 4, 5, 6};
    auto vec2 = std::vector{7, 8, 9};
    auto vec3 = std::vector{10, 11};

    const auto rng = genex::views::concat(vec1, vec2, vec3)
        | genex::to<std::vector>();
    const auto exp = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsConcat, VecInputMulti2) {
    auto vec1 = std::vector{1, 2, 3, 4, 5, 6};
    auto vec2 = std::vector{7, 8, 9};
    auto vec3 = std::vector{10, 11};

    const auto rng = vec1
        | genex::views::concat(vec2)
        | genex::views::concat(vec3)
        | genex::to<std::vector>();
    const auto exp = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    EXPECT_EQ(rng, exp);
}
