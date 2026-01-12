#include <gtest/gtest.h>

import genex.to_container;
import genex.views2.intersperse;
import genex.views2.transform;


TEST(GenexViewsIntersperse, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto rng = vec
        | genex::views::intersperse(0)
        | genex::to<std::vector>();
    const auto exp = std::vector{1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsIntersperse, VecInputTransform) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto rng = vec
        | genex::views::transform([](auto x) { return x + 1; })
        | genex::views::intersperse(0)
        | genex::to<std::vector>();
    const auto exp = std::vector{2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsIntersperse, VecInputTransformString) {
    auto vec = std::vector<std::string>{"a", "b", "c"};
    const auto rng = vec
        | genex::views::transform([](auto x) { return x + "1"; })
        | genex::views::intersperse(std::string("0"))
        | genex::to<std::vector>();
    const auto exp = std::vector<std::string>{"a1", "0", "b1", "0", "c1"};
    EXPECT_EQ(rng, exp);
}

