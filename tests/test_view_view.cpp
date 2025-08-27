#include <gtest/gtest.h>

#include <genex/views/cycle.hpp>
#include <genex/views/take.hpp>
#include <genex/views/to.hpp>
#include <genex/views/view.hpp>


TEST(GenexViewsView, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto it = vec
        | genex::views::view
        | genex::views::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(it, exp);
}
