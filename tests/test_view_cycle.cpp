#include <coroutine>
#include <gtest/gtest.h>

import genex.to_container;
import genex.views.cycle;
import genex.views.take;


TEST(GenexViewsCycle, VecInput) {
    auto vec = std::vector{0, 1, 2};

    const auto it = vec
        | genex::views::cycle
        | genex::views::take(9)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 0, 1, 2, 0, 1, 2};
    EXPECT_EQ(it, exp);
}
