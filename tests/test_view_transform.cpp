#include <gtest/gtest.h>
#include <coroutine>

import genex.to_container;
import genex.algorithms.tuple;
import genex.views.filter;
import genex.views.transform;
import genex.views.zip;
import genex.meta;


struct TestStruct {
    std::string a;
    std::uint32_t b;

    auto operator==(const TestStruct &other) const -> bool {
        return a == other.a and b == other.b;
    }
};


TEST(GenexViewsTransform, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};

    const auto rng = vec
        | genex::views::transform([](auto x) { return x * 2; })
        | genex::to<std::vector>();
    const auto exp = std::vector{2, 4, 6, 8, 10, 12};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, GenInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};

    const auto rng = vec
        | genex::views::transform([](auto x) { return x * 2; })
        | genex::views::transform([](auto x) { return x + 5; })
        | genex::to<std::vector>();
    const auto exp = std::vector{7, 9, 11, 13, 15, 17};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, VecInputStruct) {
    auto vec = std::vector{
        TestStruct{"one", 1}, TestStruct{"two", 2}, TestStruct{"three", 3}, TestStruct{"four", 4},
        TestStruct{"five", 5}, TestStruct{"six", 6}
    };

    const auto rng = vec
        | genex::views::transform([](const auto &x) { return TestStruct{x.a + "!!!", x.b * 2}; })
        | genex::to<std::vector>();
    const auto exp = std::vector{TestStruct{"one!!!", 2}, TestStruct{"two!!!", 4}, TestStruct{"three!!!", 6}, TestStruct{"four!!!", 8}, TestStruct{"five!!!", 10}, TestStruct{"six!!!", 12}};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, VecWithProj) {
    auto vec = std::vector{
        TestStruct{"one", 1}, TestStruct{"two", 2}, TestStruct{"three", 3}, TestStruct{"four", 4},
        TestStruct{"five", 5}, TestStruct{"six", 6}
    };

    genex::views::transform([](auto x) { return x + 1; }, &TestStruct::b)(vec);

    const auto rng = vec
        | genex::views::transform([](auto x) { return x + 1; }, &TestStruct::b)
        | genex::to<std::vector>();
    const auto exp = std::vector<unsigned int>{2, 3, 4, 5, 6, 7};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, EmptyInput) {
    auto vec = std::vector<std::unique_ptr<int>>{};
    const auto rng = vec
        | genex::views::transform([](auto &&x) { return x == nullptr ? 0 : *x + 1; })
        | genex::to<std::vector>();
    const auto exp = std::vector<int>{};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, IterInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6};
    const auto it_begin = vec.begin();
    const auto it_end = vec.end();
    const auto rng = genex::views::transform(it_begin, it_end, [](auto x) { return x + 1; }) | genex::to<std::vector>();
    const auto exp = std::vector{1, 2, 3, 4, 5, 6, 7};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, StrInput) {
    auto str = std::string("abcdef");

    const auto rng = str
        | genex::views::transform([](auto c) { return toupper(c); })
        | genex::to<std::string>();
    const auto exp = std::string("ABCDEF");
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, WithLambdaCapture) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto add_val = 5;

    const auto rng = vec
        | genex::views::transform([add_val](auto x) { return x + add_val; })
        | genex::to<std::vector>();
    const auto exp = std::vector{6, 7, 8, 9, 10, 11};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, UniquePtr) {
    auto vec = std::vector<std::unique_ptr<int>>{};
    vec.emplace_back(std::make_unique<int>(1));
    vec.emplace_back(std::make_unique<int>(2));
    vec.emplace_back(std::make_unique<int>(3));

    const auto rng = vec
        | genex::views::transform([](auto &&ptr) { return std::make_unique<int>(*ptr + 10); })
        | genex::views::transform([](auto &&ptr) { return *ptr + 1; })
        | genex::to<std::vector>();
    const auto exp = std::vector<int>{12, 13, 14};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, Stacked) {
    auto x = std::vector{0, 1, 2, 3, 4};
    auto y = std::vector{5, 6, 7, 8, 9};

    const auto rng = genex::views::zip(x, y)
        | genex::to<std::vector>()
        | genex::views::filter([](const auto &pair) { return (genex::get<0>(pair) + genex::get<1>(pair)) % 3 != 0; })
        | genex::views::transform([](const auto &pair) { return std::make_tuple(genex::get<0>(pair) * 2, genex::get<1>(pair) * 2); })
        | genex::views::transform([](const auto &pair) { return std::make_tuple(genex::get<0>(pair) * 2, genex::get<1>(pair) * 2); })
        | genex::to<std::vector>();
    const auto exp = std::vector<std::tuple<int, int>>{{0, 20}, {4, 24}, {12, 32}, {16, 36}};
    EXPECT_EQ(rng, exp);
}
