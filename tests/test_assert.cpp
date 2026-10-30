#include <gtest/gtest.h>
#include <vector>

import genex.views2.stride;

// GENEX_ASSERT compiles to nothing in Release.
#ifndef NDEBUG
TEST(GenexAssert, ClosureOverloadAborts) {
  EXPECT_DEATH(static_cast<void>(genex::views::stride(0)), "Runtime assertion failed");
}

TEST(GenexAssert, RangeOverloadAborts) {
  auto vec = std::vector{1, 2, 3};
  EXPECT_DEATH(static_cast<void>(genex::views::stride(vec, 0)), "Runtime assertion failed");
}
#endif
