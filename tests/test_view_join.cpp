#include <gtest/gtest.h>

#include <genex/views/join.hpp>
#include <genex/views/join_with.hpp>
#include <genex/views/to.hpp>


TEST(GenexViewsJoin, VecInput) {
    auto vec = std::vector<std::vector<int>>{{1, 2, 3}, {4, 5}, {6}};

    const auto rng = vec
        | genex::views::join
        | genex::views::to<std::vector>();
    const auto exp = std::vector<int>{1, 2, 3, 4, 5, 6};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsJoin, StrInput) {
    auto vec = std::vector<std::string>{"hello", "world", "!"};
    const auto rng = vec
        | genex::views::join
        | genex::views::to<std::vector>();
    const auto exp = std::vector{'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
    EXPECT_EQ(rng, exp);

    const auto str = rng
        | genex::views::to<std::string>();

    const auto exp_str = std::string{"helloworld!"};
    EXPECT_EQ(str, exp_str);
}


TEST(GenexViewsJoinWith, VecInput) {
    auto vec = std::vector<std::vector<int>>{{1, 2, 3}, {4, 5}, {6}};

    const auto rng = vec
        | genex::views::join_with(0)
        | genex::views::to<std::vector>();
    const auto exp = std::vector{1, 2, 3, 0, 4, 5, 0, 6};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsJoinWith, StrInput) {
    auto vec = std::vector<std::string>{"hello", "world", "!"};
    const auto rng = vec
        | genex::views::join_with(' ')
        | genex::views::to<std::vector>();
    const auto exp = std::vector{'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', ' ', '!'};
    EXPECT_EQ(rng, exp);

    const auto str = rng
        | genex::views::to<std::string>();

    const auto exp_str = std::string{"hello world !"};
    EXPECT_EQ(str, exp_str);
}
