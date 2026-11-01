#include <gtest/gtest.h>
#include <vector>

import genex.algorithms.for_each;

TEST(GenexAlgosForEach, RangeInput) {
  auto vec = std::vector{1, 2, 3};
  auto sum = 0;
  genex::for_each(vec, [&sum](const int x) { sum += x; });
  EXPECT_EQ(sum, 6);
}

TEST(GenexAlgosForEach, IterInput) {
  auto vec = std::vector{1, 2, 3};
  auto sum = 0;
  genex::for_each(vec.begin(), vec.end(), [&sum](const int x) { sum += x; });
  EXPECT_EQ(sum, 6);
}

TEST(GenexAlgosForEach, PipedMutatesInPlace) {
  auto vec = std::vector{1, 2, 3};
  vec | genex::for_each([](int &x) { x *= 2; });
  const auto exp = std::vector{2, 4, 6};
  EXPECT_EQ(vec, exp);
}

TEST(GenexAlgosForEach, PipedRvalue) {
  auto sum = 0;
  std::vector{1, 2, 3} | genex::for_each([&sum](const int x) { sum += x; });
  EXPECT_EQ(sum, 6);
}

TEST(GenexAlgosForEach, EmptyInput) {
  auto vec = std::vector<int>{};
  auto calls = 0;
  genex::for_each(vec, [&calls](const int) { ++calls; });
  EXPECT_EQ(calls, 0);
}
