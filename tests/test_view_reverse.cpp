#include <gtest/gtest.h>

#include <genex/views/filter.hpp>
#include <genex/views/materialize.hpp>
#include <genex/views/reverse.hpp>
#include <genex/views/to.hpp>
#include <genex/views/transform.hpp>


TEST(GenexViewsReverse, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::reverse
        | genex::views::to<std::vector>();
    const auto exp = std::vector{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsReverse, MultliStage) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::transform([](const int i) { return i * 2; })
        | genex::views::filter([](const int i) { return i % 4 == 0; })
        | genex::views::materialize()
        | genex::views::reverse
        | genex::views::to<std::vector>();
    const auto exp = std::vector{16, 12, 8, 4, 0};
    EXPECT_EQ(rng, exp);
}
