#include <gtest/gtest.h>

#include <genex/views/flatten.hpp>
#include <genex/views/to.hpp>


TEST(GenexViewsFlatten, VecInput) {
    auto vec = std::vector<std::vector<int>>{{1, 2, 3}, {4, 5}, {6}};
    const auto rng = vec
        | genex::views::flatten
        | genex::views::to<std::vector>();
    const auto exp = std::vector<int>{1, 2, 3, 4, 5, 6};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsFlatten, StrInput) {
    auto vec = std::vector<std::string>{"hello", "world", "!"};
    const auto rng = vec
        | genex::views::flatten
        | genex::views::to<std::vector>();
    const auto exp = std::vector{'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
    EXPECT_EQ(rng, exp);

    const auto str = rng
        | genex::views::to<std::string>();

    const auto exp_str = std::string{"helloworld!"};
    EXPECT_EQ(str, exp_str);
}


TEST(GenexViewsFlattenWith, DISABLED_VecInput) {
    auto vec = std::vector<std::vector<int>>{{1, 2, 3}, {4, 5}, {6}};
    const auto rng = vec
        | genex::views::flatten_with(0)
        | genex::views::to<std::vector>();
    const auto exp = std::vector{1, 2, 3, 0, 4, 5, 0, 6};
    EXPECT_EQ(rng, exp);
}


TEST(GenexViewsFlattenWith, DISABLED_StrInput) {
    auto vec = std::vector<std::string>{"hello", "world", "!"};
    const auto rng = vec
        | genex::views::flatten_with(' ')
        | genex::views::to<std::vector>();
    const auto exp = std::vector{'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', ' ', '!'};
    EXPECT_EQ(rng, exp);

    const auto str = rng
        | genex::views::to<std::string>();

    const auto exp_str = std::string{"hello world !"};
    EXPECT_EQ(str, exp_str);
}
