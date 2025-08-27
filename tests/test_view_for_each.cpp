#include <gtest/gtest.h>

#include <genex/views/for_each.hpp>
#include <genex/views/to.hpp>


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
