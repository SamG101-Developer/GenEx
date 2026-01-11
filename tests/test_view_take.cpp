#include <gtest/gtest.h>

import genex.to_container;
import genex.views2.take;
import genex.views2.take_last;
import genex.views2.take_while;


TEST(GenexViewsTake, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::take(4)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTakeLast, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::take_last(4)
        | genex::to<std::vector>();
    const auto exp = std::vector{6, 7, 8, 9};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTakeWhile, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::take_while([](auto x) { return x < 5; })
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4};
    EXPECT_EQ(rng, exp);
}
