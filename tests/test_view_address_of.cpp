#include <gtest/gtest.h>

import genex.to_container;
import genex.views.address_of;
import genex.views.indirect;

struct TestStruct1 {
  int a;

  explicit TestStruct1(const int a) : a(a) {
  }

  TestStruct1(TestStruct1 const &) = default;
  auto operator<=>(TestStruct1 const &) const = default;
};

TEST(GenexViewsAddressOf, VecInput) {
  const auto t1 = TestStruct1{1};
  const auto t2 = TestStruct1{2};
  const auto t3 = TestStruct1{3};

  auto vec = std::vector{t1, t2, t3};
  const auto rng = vec
    | genex::views::address_of
    | genex::to<std::vector>();
  const auto exp = std::vector{&vec[0], &vec[1], &vec[2]};
  EXPECT_EQ(rng, exp);
}

TEST(GenexViewsAddressOf, VecInputTemp) {
  const auto t1 = TestStruct1{1};
  const auto t2 = TestStruct1{2};
  const auto t3 = TestStruct1{3};

  const auto rng = std::vector{t1, t2, t3}
    | genex::views::address_of
    | genex::views::indirect
    | genex::to<std::vector>();
  const auto exp = std::vector{t1, t2, t3};
  EXPECT_EQ(rng, exp);
}
