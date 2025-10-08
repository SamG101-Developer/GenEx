#include <gtest/gtest.h>

#include <genex/views/slice.hpp>
#include <genex/to_container.hpp>


TEST(GenexViewsSlice, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto rng = vec
        | genex::views::slice(1, 6)
        | genex::to<std::vector>();
    const auto exp = std::vector{1, 2, 3, 4, 5};
    EXPECT_EQ(rng, exp);
}
