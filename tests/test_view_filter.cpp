#include <gtest/gtest.h>

import genex.to_container;
import genex.views2.filter;


struct TestStruct {
    std::string a;
    std::uint32_t b;

    auto operator==(const TestStruct &other) const -> bool {
        return a == other.a and b == other.b;
    }
};


TEST(GenexViewsFilter, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};

    const auto rng = vec
        | genex::views2::filter([](auto x) { return x % 2 == 0; })
        | genex::to<std::vector>();
    const auto exp = std::vector{2, 4, 6};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsFilter, GenInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};

    const auto rng = vec
        | genex::views2::filter([](auto x) { return x % 2 == 0; })
        | genex::views2::filter([](auto x) { return x < 5; })
        | genex::to<std::vector>();
    const auto exp = std::vector{2, 4};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsFilter, VecInputStruct) {
    auto vec = std::vector{
        TestStruct{"one", 1}, TestStruct{"two", 2}, TestStruct{"three", 3}, TestStruct{"four", 4},
        TestStruct{"five", 5}, TestStruct{"six", 6}
    };

    const auto rng = vec
        | genex::views2::filter([](const auto &x) { return x.b % 2 == 0; })
        | genex::to<std::vector>();
    const auto exp = std::vector{TestStruct{"two", 2}, TestStruct{"four", 4}, TestStruct{"six", 6}};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsFilter, VecInputPointer) {
    auto a = TestStruct{"one", 1};
    auto b = TestStruct{"two", 2};
    auto c = TestStruct{"three", 3};
    auto d = TestStruct{"four", 4};
    auto e = TestStruct{"five", 5};
    auto vec = std::vector{&a, &b, &c, &d, &e};

    const auto rng = vec
        | genex::views2::filter([](const auto *x) { return x->b % 2 == 0; })
        | genex::to<std::vector>();
    const auto exp = std::vector{&b, &d};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsFilter, VecWithProj) {
    auto vec = std::vector{
        TestStruct{"one", 1}, TestStruct{"two", 2}, TestStruct{"three", 3}, TestStruct{"four", 4},
        TestStruct{"five", 5}, TestStruct{"six", 6}
    };

    const auto rng = vec
        | genex::views2::filter([](auto x) { return x % 2 == 0; }, &TestStruct::b)
        | genex::to<std::vector>();
    const auto exp = std::vector{TestStruct{"two", 2}, TestStruct{"four", 4}, TestStruct{"six", 6}};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsFilter, IterInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6};
    const auto it_begin = vec.begin();
    const auto it_end = vec.end();
    const auto rng = genex::views2::filter(it_begin, it_end, [](auto x) { return x % 2 == 0; }) | genex::to<std::vector>();
    const auto exp = std::vector{0, 2, 4, 6};
    EXPECT_EQ(rng, exp);
}
