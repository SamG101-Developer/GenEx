#include <coroutine>
#include <gtest/gtest.h>

import genex.to_container;
import genex.views.split;


TEST(GenexViewsChunk, VecInput) {
    auto vec = std::vector{1, 2, 3, 4, 2, 5};
    const auto rng = vec
        | genex::views::split(2)
        | genex::to<std::vector>();
    const auto exp = std::vector<std::vector<int>>{{1}, {3, 4}, {5}};
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i], exp[i]);
    }
}


TEST(GenexViewsChunk, StrInput) {
    auto str = std::string("aaa/bb/ccc/d");
    const auto rng = str
        | genex::views::split('/')
        | genex::to<std::vector>();
    const auto exp = std::vector<std::string>{"aaa", "bb", "ccc", "d"};
    for (auto i = 0; i < rng.size(); ++i) {
        EXPECT_EQ(rng[i] | genex::to<std::string>(), exp[i]);
    }
}