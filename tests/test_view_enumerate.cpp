#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views.enumerate;


TEST(GenexViewsEnumerate, VecInput) {
    auto vec1 = std::vector{8, 9};

    const auto rng = vec1
        | genex::views::enumerate
        | genex::to<std::vector>();
    EXPECT_EQ(rng[0].first, 0);
    EXPECT_EQ(rng[0].second, 8);
    EXPECT_EQ(rng[1].first, 1);
    EXPECT_EQ(rng[1].second, 9);
}
