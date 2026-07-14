#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.views2.cycle;
import genex.views2.take;


TEST(GenexViewsCycle, VecInput) {
    auto vec = std::vector{0, 1, 2};

    const auto it = vec
        | genex::views::cycle
        | genex::views::take(9)
        | genex::to<std::vector>();
    const auto exp = std::vector{0, 1, 2, 0, 1, 2, 0, 1, 2};
    EXPECT_EQ(it, exp);
}
