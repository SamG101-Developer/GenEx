#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.algorithms.all_of;
import genex.views.address_of;
import genex.views.indirect;


struct TestStruct1 {
    int a;

    explicit TestStruct1(const int a) : a(a) {
    }

    TestStruct1(TestStruct1 const &) = default;
    auto operator<=>(TestStruct1 const &) const = default;
};


TEST(GenexViewsAddress, VecInput) {
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


TEST(GenexViewsAddress, VecInputTemp) {
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


TEST(GenexViewsDeref, VecInput) {
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
