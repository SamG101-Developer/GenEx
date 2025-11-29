#include <coroutine>
#include <gtest/gtest.h>

import genex.to_container;
import genex.views.join;
import genex.views.join_with;


TEST(GenexViewsFlatten, VecInput) {
    auto vec = std::vector<std::vector<int>>{{1, 2, 3}, {4, 5}, {6}};
    const auto rng = vec
        | genex::views::join
        | genex::to<std::vector>();
    const auto exp = std::vector<int>{1, 2, 3, 4, 5, 6};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsFlatten, StrInput) {
    auto vec = std::vector<std::string>{"hello", "world", "!"};
    const auto rng = vec
        | genex::views::join
        | genex::to<std::vector>();
    const auto exp = std::vector{'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
    EXPECT_EQ(rng, exp);

    const auto str = rng
        | genex::to<std::string>();

    const auto exp_str = std::string{"helloworld!"};
    EXPECT_EQ(str, exp_str);
}


TEST(GenexViewsFlattenWith, VecInput) {
    auto vec = std::vector<std::vector<int>>{{1, 2, 3}, {4, 5}, {6}};

    const auto rng = vec
        | genex::views::join_with(0)
        | genex::to<std::vector>();
    const auto exp = std::vector{1, 2, 3, 0, 4, 5, 0, 6};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsFlattenWith, StrInput) {
    auto vec = std::vector<std::string>{"hello", "world", "!"};
    const auto rng = vec
        | genex::views::join_with(' ')
        | genex::to<std::vector>();
    const auto exp = std::vector{'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', ' ', '!'};
    EXPECT_EQ(rng, exp);

    const auto str = rng
        | genex::to<std::string>();

    const auto exp_str = std::string{"hello world !"};
    EXPECT_EQ(str, exp_str);
}
