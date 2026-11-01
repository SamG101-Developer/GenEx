#include <gtest/gtest.h>
#include <vector>

import genex.algorithms.all_of;
import genex.views.indirect;

struct TestStruct1 {
  int a;

  explicit TestStruct1(const int a) : a(a) {
  }

  TestStruct1(TestStruct1 const &) = default;
  auto operator<=>(TestStruct1 const &) const = default;
};

TEST(GenexViewsIndirect, VecInput) {
  auto t1 = TestStruct1{1};
  auto t2 = TestStruct1{2};
  auto t3 = TestStruct1{3};

  const auto p1 = &t1;
  const auto p2 = &t2;
  const auto p3 = &t3;

  auto ptrs = std::vector{p1, p2, p3};
  auto ptrs_iter = ptrs.begin();

  const auto vec = std::vector{p1, p2, p3};
  const auto res = genex::all_of(
    vec | genex::views::indirect,
    [ptrs_iter](const auto &x) mutable { return &x == *ptrs_iter++; });

  EXPECT_TRUE(res);
}
