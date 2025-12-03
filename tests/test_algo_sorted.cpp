#include <coroutine>
#include <gtest/gtest.h>

import genex.algorithms.sorted;


TEST(GenexAlgosSorted, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5};
    const auto srt = genex::sorted(vec);
    const auto exp = std::vector{1, 2, 3, 4, 5};
    EXPECT_EQ(srt, exp);
}