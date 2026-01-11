#include <gtest/gtest.h>

import genex.to_container;
import genex.views2.intersperse;


TEST(GenexViewsIntersperse, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto rng = vec
        | genex::views::intersperse(0)
        | genex::to<std::vector>();
    const auto exp = std::vector{1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6};
    EXPECT_EQ(rng, exp);
}
