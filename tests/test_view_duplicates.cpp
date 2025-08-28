#include <gtest/gtest.h>

#include <genex/views/duplicates.hpp>
#include <genex/views/to.hpp>


TEST(GenexViewsDuplicates, Duplicates) {
    auto vec = std::vector{1, 2, 3, 4, 1, 2};

    const auto rng = vec
        | genex::views::duplicates
        | genex::views::to<std::vector>();
    const auto exp = std::vector{1, 1};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsDuplicates, NoDuplicates) {
    auto vec = std::vector{1, 2, 3, 4};

    const auto rng = vec
        | genex::views::duplicates
        | genex::views::to<std::vector>();
    const auto exp = std::vector<int>{};
    EXPECT_EQ(rng, exp);
}
