#include <gtest/gtest.h>

import genex.to_container;
import genex.views2.join;
import genex.views2.join_with;


TEST(GenexViewsJoin, VecInput) {
    auto vec = std::vector<std::vector<int>>{{1, 2, 3}, {4, 5}, {6}};
    const auto rng = vec
        | genex::views2::join
        | genex::to<std::vector>();
    const auto exp = std::vector<int>{1, 2, 3, 4, 5, 6};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsJoin, StrInput) {
    auto vec = std::vector<std::string>{"hello", "world", "!"};
    const auto rng = vec
        | genex::views2::join
        | genex::to<std::vector>();
    const auto exp = std::vector{'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
    EXPECT_EQ(rng, exp);

    const auto str = rng
        | genex::to<std::string>();

    const auto exp_str = std::string{"helloworld!"};
    EXPECT_EQ(str, exp_str);
}


TEST(GenexViewsJoinWith, VecInput) {
    auto vec = std::vector<std::vector<int>>{{1, 2, 3}, {4, 5}, {6}};

    const auto rng = vec
        | genex::views2::join_with(0)
        | genex::to<std::vector>();
    const auto exp = std::vector{1, 2, 3, 0, 4, 5, 0, 6};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsJoinWith, StrInput) {
    auto vec = std::vector<std::string>{"hello", "world", "!"};
    const auto rng = vec
        | genex::views2::join_with(' ')
        | genex::to<std::vector>();
    const auto exp = std::vector{'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', ' ', '!'};
    EXPECT_EQ(rng, exp);

    const auto str = rng
        | genex::to<std::string>();

    const auto exp_str = std::string{"hello world !"};
    EXPECT_EQ(str, exp_str);
}
