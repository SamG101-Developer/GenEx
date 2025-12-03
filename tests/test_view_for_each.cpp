#include <coroutine>
#include <gtest/gtest.h>

import genex.to_container;
import genex.views.for_each;


TEST(GenexViewsForEach, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto sum = 0;

    genex::views::for_each(vec, [&sum](auto x) { sum += x; });
    constexpr auto exp = 21;
    EXPECT_EQ(sum, exp);
}


TEST(GenexViewsForEach, GenInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    auto sum = 0;

    vec
        | genex::views::for_each([&sum](auto x) { sum += x; });
    constexpr auto exp = 21;
    EXPECT_EQ(sum, exp);
}
