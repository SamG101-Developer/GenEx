#include <coroutine>
#include <gtest/gtest.h>

import genex.algorithms.binary_search;


TEST(GenexAlgosBinarySearch, FindElementExists) {
    auto vec = std::vector{0, 2, 4, 5, 7};
    const auto found = genex::binary_search(vec, 4);
    EXPECT_TRUE(found);
}
