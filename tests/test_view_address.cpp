#include <gtest/gtest.h>

#include <genex/algorithms/all_of.hpp>
#include <genex/views/address.hpp>
#include <genex/views/map.hpp>
#include <genex/views/to.hpp>


struct TestStrut {
    int a;
};


TEST(GenexViewsAddress, VecInput) {
    auto t1 = TestStrut{1};
    auto t2 = TestStrut{2};
    auto t3 = TestStrut{3};

    auto vec = std::vector{std::move(t1), std::move(t2), std::move(t3)};
    const auto rng = vec
        | genex::views::address
        | genex::views::to<std::vector>();
    const auto exp = std::vector{&vec[0], &vec[1], &vec[2]};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsDeref, VecInput) {
    auto t1 = TestStrut{1};
    auto t2 = TestStrut{2};
    auto t3 = TestStrut{3};

    const auto p1 = &t1;
    const auto p2 = &t2;
    const auto p3 = &t3;

    auto ptrs = std::vector{p1, p2, p3};
    auto ptrs_iter = ptrs.begin();

    const auto vec = std::vector{p1, p2, p3};
    const auto res = vec
        | genex::views::deref
        | genex::algorithms::all_of([ptrs_iter](const auto &x) mutable { return &x == *ptrs_iter++; });

    EXPECT_TRUE(res);
}
