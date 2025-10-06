#include <gtest/gtest.h>

#include <genex/views/chunk.hpp>
#include <genex/views/join.hpp>
#include <genex/views/split.hpp>
#include <genex/views/to.hpp>
#include <genex/views/transform.hpp>


TEST(GenexViewsChunkN, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 5, 6};
    const auto rng = vec
        | genex::views::chunk(2)
        | genex::views::transform([](auto sub_rng) { return sub_rng | genex::views::to<std::vector>(); })
        | genex::views::to<std::vector>();
    const auto exp = std::vector<std::vector<int>>{{1, 2}, {3, 4}, {5, 6}};
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i], exp[i]);
    }
}


TEST(GenexViewsChunkN, StrInput) {
    auto str = std::string("Hello, world!");
    const auto rng = str
        | genex::views::chunk(5)
        | genex::views::transform([](auto sub_rng) { return sub_rng | genex::views::to<std::vector>(); })
        | genex::views::to<std::vector>();
    const auto exp = std::vector<std::string>{"Hello", ", wor", "ld!"};
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i] | genex::views::to<std::string>(), exp[i]);
    }
}


TEST(GenexViewsChunk, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 2, 5};

    const auto rng = vec
        | genex::views::split(2)
        | genex::views::transform([](auto sub_rng) { return sub_rng | genex::views::to<std::vector>(); })
        | genex::views::to<std::vector>();
    const auto exp = std::vector<std::vector<int>>{{1}, {3, 4}, {5}};
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i], exp[i]);
    }
}


TEST(GenexViewsChunk, StrInput) {
    auto str = std::string("aaa/bb/ccc/d");
    const auto rng = str
        | genex::views::split('/')
        | genex::views::transform([](auto sub_rng) { return sub_rng | genex::views::to<std::string>(); })
        | genex::views::to<std::vector>();
    const auto exp = std::vector<std::string>{"aaa", "bb", "ccc", "d"};
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i], exp[i]);
    }
}