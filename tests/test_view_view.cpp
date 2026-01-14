#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views.view;


TEST(GenexViewsView, VecInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const auto it = vec
        | genex::views::view
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(it, exp);
}
