#include <gtest/gtest.h>

#include <genex/views/intersperse.hpp>
#include <genex/views/join.hpp>
#include <genex/views/join_with.hpp>
#include <genex/views/to.hpp>
#include <genex/views/transform.hpp>


struct TestStruct {
    std::string a;
    std::uint32_t b;

    auto operator==(const TestStruct &other) const -> bool {
        return a == other.a and b == other.b;
    }
};


TEST(GenexViewsTransform, StackInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 7};

    const auto test1 = vec
        | genex::views::transform([](auto &&x) { return std::to_string(x); })
        | genex::views::to<std::vector>();
    const auto exp1 = std::vector<std::string>{"0", "1", "2", "3", "4", "5", "7"};
    EXPECT_EQ(test1, exp1);

    const auto test2 = vec
        | genex::views::transform([](auto &&x) { return std::to_string(x); })
        | genex::views::intersperse(std::string(", "))
        | genex::views::to<std::vector>();
    const auto exp2 = std::vector<std::string>{"0", ", ", "1", ", ", "2", ", ", "3", ", ", "4", ", ", "5", ", ", "7"};
    EXPECT_EQ(test2, exp2);

    const auto test3 = exp1
        | genex::views::intersperse(std::string(", "))
        | genex::views::join
        | genex::views::to<std::vector>();
    const auto exp3 = std::vector{'0', ',', ' ', '1', ',', ' ', '2', ',', ' ', '3', ',', ' ', '4', ',', ' ', '5', ',', ' ', '7'};
    EXPECT_EQ(test3, exp3);

    const auto rng = vec
        | genex::views::transform([](auto &&x) { return std::to_string(x); })
        | genex::views::intersperse(std::string(", "))
        | genex::views::join
        | genex::views::to<std::string>();
    const auto exp = std::string("0, 1, 2, 3, 4, 5, 7");
    EXPECT_EQ(rng, exp);

    const auto rng1 = vec
        | genex::views::transform([](auto &&x) { return std::to_string(x); })
        | genex::views::intersperse(std::string(", "))
        | genex::views::join_with('!')
        | genex::views::to<std::string>();
    const auto exp4 = std::string("0!, !1!, !2!, !3!, !4!, !5!, !7");
    EXPECT_EQ(rng1, exp4);
}


TEST(GenexViewsTransform, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};

    const auto rng = vec
        | genex::views::transform([](auto x) { return x * 2; })
        | genex::views::to<std::vector>();
    const auto exp = std::vector{2, 4, 6, 8, 10, 12};
    EXPECT_EQ(rng, exp);
}


// TEST(GenexViewsTransform, VecInputLambdaCapture) {
//     auto vec = std::vector{1, 2, 3, 4, 5, 6};
//     auto y = 100;
//
//     auto temp_1 = genex::views::transform([y](auto x) { return x * y; })(vec);
//     auto temp_2 = genex::views::to<std::vector>()(temp_1);
//
//     const auto rng = vec
//         | genex::views::transform([](auto x) { return x * 2; })
//         | genex::views::to<std::vector>();
//     const auto exp = std::vector{2, 4, 6, 8, 10, 12};
//     EXPECT_EQ(rng, exp);
// }


TEST(GenexViewsTransform, GenInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};

    const auto rng = vec
        | genex::views::transform([](auto x) { return x * 2; })
        | genex::views::transform([](auto x) { return x + 5; })
        | genex::views::to<std::vector>();
    const auto exp = std::vector{7, 9, 11, 13, 15, 17};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, VecInputStruct) {
    auto vec = std::vector{
        TestStruct{"one", 1}, TestStruct{"two", 2}, TestStruct{"three", 3}, TestStruct{"four", 4},
        TestStruct{"five", 5}, TestStruct{"six", 6}
    };

    const auto rng = vec
        | genex::views::transform([](auto const &x) { return TestStruct{x.a + "!!!", x.b * 2}; })
        | genex::views::to<std::vector>();
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
        | genex::views::to<std::vector>();
    const auto exp = std::vector<unsigned int>{2, 3, 4, 5, 6, 7};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, EmptyInput) {
    auto vec = std::vector<std::unique_ptr<int>>{};
    const auto rng = vec
        | genex::views::transform([](auto &&x) { return x == nullptr ? 0 : *x + 1; })
        | genex::views::to<std::vector>();
    const auto exp = std::vector<int>{};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsTransform, IterInput) {
    auto vec = std::vector{0, 1, 2, 3, 4, 5, 6};
    const auto it_begin = vec.begin();
    const auto it_end = vec.end();
    const auto rng = genex::views::transform(it_begin, it_end, [](auto x) { return x + 1; }) | genex::views::to<std::vector>();
    const auto exp = std::vector{1, 2, 3, 4, 5, 6, 7};
    EXPECT_EQ(rng, exp);
}
